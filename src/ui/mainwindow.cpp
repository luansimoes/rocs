/*
    This file is part of Rocs.
    Copyright 2008-2011  Tomaz Canabrava <tomaz.canabrava@gmail.com>
    Copyright 2008       Ugo Sangiori <ugorox@gmail.com>
    Copyright 2010-2011  Wagner Reck <wagner.reck@gmail.com>
    Copyright 2011-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "rocsversion.h"
#include "settings.h"

#include "libgraphtheory/editor.h"
#include "libgraphtheory/editorplugins/editorpluginmanager.h"
#include "libgraphtheory/kernel/kernel.h"
#include "libgraphtheory/view.h"

#include <QApplication>
#include <QCloseEvent>
#include <QGraphicsView>
#include <QLabel>
#include <QLayout>
#include <QSplitter>
#include <QToolButton>
#include <QGridLayout>
#include <QDebug>
#include <QIcon>
#include <QPushButton>
#include <QInputDialog>
#include <QActionGroup>
#include <QFileDialog>
#include <QQuickWidget>
#include <QPointer>

#include <KActionCollection>
#include <KRecentFilesAction>
#include <KActionMenu>
#include <KTar>
#include <KMessageBox>
#include <KLocalizedString>
#include <KConfigDialog>
#include <KToolBar>
#include <ktexteditor/view.h>
#include <ktexteditor/editor.h>
#include <ktexteditor/document.h>

#include "ui/documenttypeswidget.h"
#include "ui/configuredefaultproperties.h"
#include "ui/codeeditorwidget.h"
#include "ui/scriptoutputwidget.h"
#include "ui/sidedockwidget.h"
#include "ui/documentationwidget.h"
#include "ui/fileformatdialog.h"
#include "ui/journalwidget.h"
#include "grapheditorwidget.h"
#include "plugins/ApiDoc/ApiDocWidget.h"
#include "project/project.h"

using namespace GraphTheory;

MainWindow::MainWindow()
    : KXmlGuiWindow()
    , m_currentProject(0)
    , m_kernel(new Kernel)
    , m_codeEditorWidget(new CodeEditorWidget(this))
    , m_graphEditorWidget(new GraphEditorWidget(this))
    , m_outputWidget(new ScriptOutputWidget(this))
    , m_scriptDbg(0)
{
    setObjectName("RocsMainWindow");
    m_graphEditor = new GraphTheory::Editor();

    setupWidgets();
    setupActions();
    setupGUI(ToolBar | Keys | Save | Create);

    setupToolbars();
    setupToolsPluginsAction();

    // setup kernel
    connect(m_kernel, &Kernel::message,
        m_outputWidget, &ScriptOutputWidget::processMessage);

    // TODO: use welcome widget instead of creating default empty project
    createProject();
    updateCaption();

    // update rocs config version
    Settings::setVersion(ROCS_VERSION_STRING);

    // disable save action from kpart, since we take care for the editor by global save action
    // here "file_save" is the action identifier from katepartui.rc
    // note that we may not use that name for our own actions
    foreach(KActionCollection* ac, KActionCollection::allCollections()) {
        if (ac->action("file_save")) {
            ac->removeAction(ac->action("file_save"));
            break; // we only expect that action once
        }
    }
}

MainWindow::~MainWindow()
{
    Settings::setVSplitterSizeTop(m_vSplitter->sizes() [0]);
    Settings::setVSplitterSizeBottom(m_vSplitter->sizes() [1]);
    Settings::setHSplitterSizeLeft(m_hSplitter->sizes() [0]);
    Settings::setHSplitterSizeRight(m_hSplitter->sizes() [1]);
    Settings::setHScriptSplitterSizeLeft(m_hScriptSplitter->sizes() [0]);
    Settings::setHScriptSplitterSizeRight(m_hScriptSplitter->sizes() [1]);
    m_recentProjects->saveEntries(Settings::self()->config()->group("RecentFiles"));

    Settings::self()->save();

    m_graphEditor->deleteLater();
    m_kernel->deleteLater();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (queryClose() == true) {
        event->accept();
        return;
    } else {
        event->ignore();
        return;
    }
}

void MainWindow::setupWidgets()
{
    // setup main widgets
    QWidget *sidePanel = setupSidePanel();
    QWidget *scriptPanel = setupScriptPanel();

    // splits the main window horizontal
    m_vSplitter = new QSplitter(this);
    m_vSplitter->setOrientation(Qt::Vertical);
    m_vSplitter->addWidget(m_graphEditorWidget);
    m_vSplitter->addWidget(scriptPanel);

    // horizontal arrangement
    m_hSplitter = new QSplitter(this);
    m_hSplitter->setOrientation(Qt::Horizontal);
    m_hSplitter->addWidget(m_vSplitter);
    m_hSplitter->addWidget(sidePanel);

    // set sizes for script panel
    m_hScriptSplitter->setSizes(QList<int>() << Settings::hScriptSplitterSizeLeft() << Settings::hScriptSplitterSizeRight() << 80);

    // set sizes for vertical splitter
    m_vSplitter->setSizes(QList<int>() << Settings::vSplitterSizeTop() << Settings::vSplitterSizeBottom());

    // set sizes for side panel
    // the following solves the setting of the panel width if it was closed at previous session
    int panelWidth = Settings::hSplitterSizeRight();
    if (panelWidth == 0) {
        //FIXME this is only a workaround
        // that fixes the wrong saving of hSplitterSizeRight
        panelWidth = 400;
    }
    m_hSplitter->setSizes(QList<int>() << Settings::hSplitterSizeLeft() << panelWidth);

    setCentralWidget(m_hSplitter);
}

void MainWindow::setupToolbars()
{
    // If current version in settings file is less than demanded version
    // perform operations.
    QString configVersion = Settings::version();
    if (configVersion.compare(QString("1.7.70")) < 0) {
        qDebug() << "Apply new default settings for toolbars";
        KToolBar* bar;

        bar = toolBar("main");
        bar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        bar->setOrientation(Qt::Vertical);
        addToolBar(Qt::LeftToolBarArea, bar);

        bar = toolBar("align");
        bar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        bar->setOrientation(Qt::Vertical);
        addToolBar(Qt::LeftToolBarArea, bar);
    }
}

QWidget* MainWindow::setupScriptPanel()
{
    m_hScriptSplitter = new QSplitter(this);
    m_hScriptSplitter->setOrientation(Qt::Horizontal);

    KToolBar *executeCommands = new KToolBar(this);
    executeCommands->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    executeCommands->setOrientation(Qt::Vertical);
    m_runScript = new QAction(QIcon::fromTheme("media-playback-start"), i18nc("@action:intoolbar Script Execution", "Run"), this);
    m_stepRunScript = new QAction(QIcon::fromTheme("go-next"), i18nc("@action:intoolbar Script Execution", "One Step"), this);
    m_stopScript = new QAction(QIcon::fromTheme("process-stop"), i18nc("@action:intoolbar Script Execution", "Stop"), this);
    m_stopScript->setEnabled(false);
    executeCommands->addAction(m_runScript);
    executeCommands->addAction(m_stepRunScript);
    // add actions to action collection to be able to set shortcuts on them in the ui
    actionCollection()->addAction("_runScript", m_runScript);
    actionCollection()->addAction("_stepRunScript", m_stepRunScript);
    actionCollection()->addAction("_stopScript", m_stopScript);

    // debug controls submenu
    m_debugMenu = new KActionMenu(QIcon::fromTheme("debug-run"), i18nc("@title:menu Debug execution", "Debug"), this);
    m_debugScript = new QAction(QIcon::fromTheme("debug-run"), i18nc("@action:inmenu Debug execution", "Debug run"), m_debugMenu);
    m_interruptScript = new QAction(QIcon::fromTheme("debug-run-cursor"), i18nc("@action:inmenu Debug execution", "Interrupt at first line"), m_debugMenu);
    m_debugMenu->addAction(m_debugScript);
    m_debugMenu->addAction(m_interruptScript);
    executeCommands->addWidget(m_debugMenu->createWidget(executeCommands));
    executeCommands->addAction(m_stopScript);
    actionCollection()->addAction("_debugScript", m_debugScript);
    actionCollection()->addAction("_interruptScript", m_interruptScript);

    // set toolbar visibility defaults
    showExecutionButtonDebug(Settings::executionModeDebugVisible());
    showExecutionButtonOneStep(Settings::executionModeOneStepVisible());

    connect(m_runScript, &QAction::triggered,
        this, &MainWindow::executeScript);
    connect(m_stepRunScript, &QAction::triggered,
        this, &MainWindow::executeScriptOneStep);
    connect(m_stopScript, &QAction::triggered,
        this, &MainWindow::stopScript);

    m_hScriptSplitter->addWidget(m_codeEditorWidget);
    m_hScriptSplitter->addWidget(m_outputWidget);

    QWidget *scriptInterface = new QWidget(this);
    scriptInterface->setLayout(new QHBoxLayout);
    scriptInterface->layout()->addWidget(m_hScriptSplitter);
    scriptInterface->layout()->addWidget(executeCommands);

    return scriptInterface;
}

QWidget* MainWindow::setupSidePanel()
{
    QWidget *panel = new QWidget(this);
    panel->setLayout(new QVBoxLayout);
    panel->setVisible(false);

    // add sidebar
    SidedockWidget* sideDock = new SidedockWidget(panel);
    addToolBar(Qt::RightToolBarArea, sideDock->toolbar());
    panel->layout()->addWidget(sideDock);

    // add widgets to dock
    // document property widgets
    DocumentTypesWidget *documentTypesWidget = new DocumentTypesWidget(panel);
    connect(this, &MainWindow::graphDocumentChanged,
        documentTypesWidget, &DocumentTypesWidget::setDocument);
    sideDock->addDock(documentTypesWidget, i18n("Element Types"), QIcon::fromTheme("document-properties"));
    if (m_currentProject && m_currentProject->activeGraphDocument()) {
        documentTypesWidget->setDocument(m_currentProject->activeGraphDocument());
    }

    // Project Journal
    m_journalWidget = new JournalEditorWidget(panel);
    sideDock->addDock(m_journalWidget, i18nc("@title", "Journal"), QIcon::fromTheme("story-editor"));

    // Rocs handbook
    DocumentationWidget* documentation = new DocumentationWidget(panel);
    sideDock->addDock(documentation, i18nc("@title", "Handbook"), QIcon::fromTheme("help-contents"));

    // Rocs scripting API documentation
//FIXME commented out until Grantlee is ported
//     ApiDocWidget* apiDoc = new ApiDocWidget(panel);
//     sideDock->addDock(apiDoc, i18nc("@title", "Scripting API"), QIcon::fromTheme("documentation"));

    return panel;
}

void MainWindow::setProject(Project *project)
{
    m_codeEditorWidget->setProject(project);
    m_graphEditorWidget->setProject(project);
    m_journalWidget->openJournal(project);
    updateCaption();

    if (m_currentProject) {
        m_currentProject->disconnect(this);
        m_currentProject->deleteLater();
    }

    connect(project, static_cast<void (Project::*)(GraphTheory::GraphDocumentPtr)>(&Project::activeGraphDocumentChanged),
        this, &MainWindow::graphDocumentChanged);
    m_currentProject = project;
    emit graphDocumentChanged(m_currentProject->activeGraphDocument());
}

void MainWindow::setupActions()
{
    qDebug() << "create and connect actions";
    KStandardAction::quit(this, SLOT(quit()), actionCollection());
    KStandardAction::preferences(this, SLOT(showSettings()), actionCollection());

    // setup graph visual editor actions and add them to mainwindow action collection
//     m_graphEditor->setupActions(actionCollection()); //FIXME add editor actions to main action collection

    // Menu actions
    createAction("document-new",        i18nc("@action:inmenu", "New Project"),        "new-project", QKeySequence::New, SLOT(createProject()), this);
    createAction("document-save",       i18nc("@action:inmenu", "Save Project"),       "save-project", QKeySequence::Save, SLOT(saveProject()), this);
    createAction("document-open",       i18nc("@action:inmenu", "Open Project"),       "open-project", QKeySequence::Open, SLOT(openProject()), this);

    m_recentProjects = new KRecentFilesAction(QIcon ("document-open"), i18nc("@action:inmenu","Recent Projects"), this);
    connect(m_recentProjects, &KRecentFilesAction::urlSelected,
        this, &MainWindow::openProject);
    actionCollection()->addAction("recent-project", m_recentProjects);

    m_recentProjects->loadEntries(Settings::self()->config()->group("RecentFiles"));
    createAction("document-save-as",     i18nc("@action:inmenu", "Save Project as"),   "save-project-as",    SLOT(saveProjectAs()), this);
    createAction("document-new",        i18nc("@action:inmenu", "New Graph Document"), "new-graph",         SLOT(createGraphDocument()), this);
    createAction("document-new",        i18nc("@action:inmenu", "New Script File"),    "new-script",        SLOT(createCodeDocument()),    this);
    createAction("document-import",     i18nc("@action:inmenu", "Import Graph"),       "import-graph",      SLOT(importGraphDocument()),   this);
    createAction("document-export",     i18nc("@action:inmenu", "Export Graph as"),    "export-graph-as",      SLOT(exportGraphDocument()), this);

    createAction("document-import",  i18nc("@action:inmenu", "Import Script"),       "add-script",          SLOT(importCodeDocument()),   this);
    createAction("document-export", i18nc("@action:inmenu", "Export Script"),      "export-script",      SLOT(exportCodeDocument()), this);
    createAction("",  i18nc("@action:inmenu", "Configure Code Editor..."),      "config-code-editor",      SLOT(showCodeEditorConfig()), this);
}

void MainWindow::createAction(const QByteArray& iconName, const QString& actionTitle, const QString& actionName,
                              const QKeySequence& shortcut, const char* slot, QObject *parent)
{
    QAction* action = new QAction(QIcon::fromTheme(iconName), actionTitle, parent);
    action->setShortcutContext(Qt::ApplicationShortcut);
    actionCollection()->addAction(actionName, action);
    actionCollection()->setDefaultShortcut(action, shortcut);
    connect(action, SIGNAL(triggered(bool)), parent, slot);
}

void MainWindow::createAction(const QByteArray& iconName, const QString& actionTitle, const QString& actionName,
                              const char* slot, QObject *parent)
{
    QAction* action = new QAction(QIcon::fromTheme(iconName), actionTitle, parent);
    actionCollection()->addAction(actionName, action);
    connect(action, SIGNAL(triggered(bool)), parent, slot);
}

void MainWindow::showSettings()
{
    QPointer<KConfigDialog> dialog = new KConfigDialog(this, "settings", Settings::self());
    ConfigureDefaultProperties * defaultProperties = new ConfigureDefaultProperties(dialog);

    dialog->addPage(defaultProperties, i18nc("@title:tab", "Default Settings"), QString(), i18nc("@title:tab", "Default Settings"), true);

    connect(defaultProperties, &ConfigureDefaultProperties::showExecuteModeDebugChanged,
        this, &MainWindow::showExecutionButtonDebug);
    connect(defaultProperties, &ConfigureDefaultProperties::showExecuteModeOneStepChanged,
        this, &MainWindow::showExecutionButtonOneStep);
    dialog->exec();
}

void MainWindow::setupToolsPluginsAction()
{
    QAction *action = 0;
    QList<EditorPluginInterface*> availablePlugins =  m_graphEditorPluginManager.plugins();
    QList<QAction*> actions;
    int count = 0;
    foreach(EditorPluginInterface * plugin, availablePlugins) {
        action = new QAction(plugin->displayName(), this);
        action->setData(count++);
        connect(action, &QAction::triggered,
            this, &MainWindow::showEditorPluginDialog);
        actions << action;
    }
    unplugActionList("tools_plugins");
    plugActionList("tools_plugins", actions);
}

void MainWindow::importCodeDocument()
{
    QString startDirectory = Settings::lastOpenedDirectory();
    QUrl fileUrl = QUrl::fromLocalFile(QFileDialog::getOpenFileName(this,
        i18nc("@title:window", "Import Script into Project"),
        startDirectory));

    if (fileUrl.isEmpty()) {
        return;
    }

    m_currentProject->importCodeDocument(fileUrl);
    Settings::setLastOpenedDirectory(startDirectory);
}

void MainWindow::exportCodeDocument()
{
    QString startDirectory = Settings::lastOpenedDirectory();
    QUrl fileUrl = QUrl::fromLocalFile(QFileDialog::getSaveFileName(this,
        i18nc("@title:window", "Export Script"),
        startDirectory,
        i18n("JavaScript (*.js)")));
    m_codeEditorWidget->activeDocument()->saveAs(fileUrl);
}

void MainWindow::createProject()
{
    if (!queryClose()) {
        return;
    }

    Project *project = new Project(m_graphEditor);
    project->addCodeDocument(KTextEditor::Editor::instance()->createDocument(0));
    project->addGraphDocument(m_graphEditor->createDocument());
    project->setModified(false);

    setProject(project);
}

void MainWindow::saveProject()
{
    if (m_currentProject->projectUrl().isEmpty()) {
        QString startDirectory = Settings::lastOpenedDirectory();
        QString file = QFileDialog::getSaveFileName(this,
                            i18nc("@title:window", "Save Project"),
                            startDirectory,
                            i18n("Rocs Projects (*.rocs)"));

        if (file.isEmpty()) {
            qCritical() << "Filename is empty and no script file was created.";
            return;
        }
        Settings::setLastOpenedDirectory(m_currentProject->projectUrl().path());
        m_currentProject->setProjectUrl(QUrl::fromLocalFile(file));
    }
    m_currentProject->projectSave();
    updateCaption();
}

void MainWindow::saveProjectAs()
{
    QString startDirectory = Settings::lastOpenedDirectory();
    QString file = QFileDialog::getSaveFileName(this,
                        i18nc("@title:window", "Save Project As"),
                        startDirectory,
                        i18n("Rocs Projects (*.rocs)"));

    if (file.isEmpty()) {
        qCritical() << "Filename is empty and no script file was created.";
        return;
    }
    QFileInfo fi(file);
    if (fi.exists()) {
        const int btnCode = KMessageBox::warningContinueCancel(
            this,
            i18nc("@info", "A file named \"%1\" already exists. Are you sure you want to overwrite it?", fi.fileName()),
            i18nc("@title:window", "Overwrite File?"),
            KStandardGuiItem::overwrite());
        if (btnCode == KMessageBox::Cancel) {
            return; // cancel saving
        }
    }
    Settings::setLastOpenedDirectory(m_currentProject->projectUrl().path());
    m_currentProject->projectSaveAs(QUrl::fromLocalFile(file));
    updateCaption();
}

void MainWindow::openProject(const QUrl &fileName)
{
    if (!queryClose()) {
        return;
    }

    QString startDirectory = Settings::lastOpenedDirectory();
    QUrl file = fileName;
    if (file.isEmpty()){
    // show open dialog
         file = QUrl::fromLocalFile(QFileDialog::getOpenFileName(this,
                    i18nc("@title:window", "Open Project Files"),
                    startDirectory,
                    i18n("Rocs projects (*.rocs)")));

        if (file.isEmpty()) {
            return;
        }
    }

    Project *project = new Project(file, m_graphEditor);
    setProject(project);

    m_recentProjects->addUrl(file);
    Settings::setLastOpenedDirectory(file.path());
}

void MainWindow::updateCaption()
{
    if (!m_currentProject) {
        return;
    }

    if (m_currentProject->projectUrl().isEmpty()) {
        setCaption(i18nc("caption text for temporary project", "[ untitled ]"));
    } else {
        setCaption(QString("[ %1 ]").arg(m_currentProject->projectUrl().toLocalFile()));
    }
}

QString MainWindow::uniqueFilename(const QString &basePrefix, const QString &suffix) {
    QFile targetFile;
    QString basePath = m_currentProject->projectUrl().path();
    QString fullSuffix = "." + suffix;
    QString fullPrefix = basePrefix;

    if (fullPrefix.isNull()) {
        fullPrefix = m_currentProject->projectUrl().fileName().remove(QRegExp(".rocs*$"));
    } else if (fullPrefix.endsWith(fullSuffix)) {
        fullPrefix.remove(QRegExp(fullSuffix + "$"));
    }

    targetFile.setFileName(basePath + fullPrefix + fullSuffix);
    for(int i = 1; targetFile.exists(); i++) {
        targetFile.setFileName(basePath + fullPrefix + QString::number(i) + fullSuffix);
    }

    return targetFile.fileName();
}

void MainWindow::createCodeDocument()
{
    QString basePrefix = QInputDialog::getText(this,
                            i18n("ScriptName"),
                            i18n("Enter the name of your new script"));
    if (basePrefix.isNull()) {
        qDebug() << "Filename is empty and no script file was created.";
    } else {
        QString fileName = uniqueFilename(basePrefix, "js"); //TODO this does nothing
        KTextEditor::Document *document = KTextEditor::Editor::instance()->createDocument(0);
        m_currentProject->addCodeDocument(document);
    }
}

void MainWindow::createGraphDocument()
{
    QString file = QInputDialog::getText(this,
                        i18n("Graph name"),
                        i18n("Enter the name of the Graph"));
    if (file.isEmpty()) {
        qDebug() << "Filename is empty and no script file was created.";
        return;
    }
    if (!file.endsWith(QLatin1String(".graph"))){
        file.append(".graph");
    }

    GraphDocumentPtr document = m_graphEditor->createDocument();
    m_currentProject->addGraphDocument(document);
    document->setDocumentName(file);
}

bool MainWindow::queryClose()
{
    if (!m_currentProject) {
        return true;
    }
    if (m_currentProject->isModified()) {
        const int btnCode = KMessageBox::warningYesNoCancel(this, i18nc(
                                "@info",
                                "Changes on your project are unsaved. Do you want to save your changes?"));
        if (btnCode == KMessageBox::Yes) {
            saveProject();
            return true;
        }
        if (btnCode == KMessageBox::No) {
            return true;
        }
        return false; // do not close
    }
    return true; // save to close project: no changes
}

void MainWindow::quit()
{
    if (queryClose()) {
        QApplication::quit();
    }
}

void MainWindow::importGraphDocument()
{
    FileFormatDialog importer(this);
    GraphDocumentPtr document = importer.importFile();
    if (!document) {
        qWarning() << "No graph document was imported.";
        return;
    }
    m_currentProject->addGraphDocument(document);
}

void MainWindow::exportGraphDocument()
{
    FileFormatDialog exporter(this);
    exporter.exportFile(m_currentProject->activeGraphDocument());
}

void MainWindow::showCodeEditorConfig()
{
    KTextEditor::Editor *editor = KTextEditor::Editor::instance();
    editor->configDialog(this);
}

void MainWindow::showEditorPluginDialog()
{
    QAction *action = qobject_cast<QAction *> (sender());

    if (!action) {
        return;
    }
    if (EditorPluginInterface *plugin =  m_graphEditorPluginManager.plugins().value(action->data().toInt())) {
        plugin->showDialog(m_currentProject->activeGraphDocument());
    }
}

void MainWindow::executeScript()
{
    if (m_outputWidget->isOutputClearEnabled()) {
        m_outputWidget->clear();
    }
    QString script = m_codeEditorWidget->activeDocument()->text();
    enableStopAction();
    m_kernel->execute(m_currentProject->activeGraphDocument(), script);
}

void MainWindow::executeScriptOneStep()
{
    //FIXME implement scripting interfaces for GraphTheory
    qCritical() << "Scripting engine currently disalbed";
#if 0
    Q_ASSERT(m_outputWidget);

    //FIXME implement scripting interfaces for GraphTheory
    qCritical() << "Scripting engine currently disalbed";
//     QtScriptBackend *engine = DocumentManager::self().activeDocument()->engineBackend();

    //TODO disable start action
    enableStopAction();
    if (!engine->isRunning()) {
        if (m_outputWidget->isOutputClearEnabled()) {
            m_outputWidget->clear();
        }
        QString script = text.isEmpty() ? m_codeEditorWidget->text() : text;
        QString scriptPath = m_codeEditorWidget->document()->url().path();
        IncludeManager inc;

        script = inc.include(script,
                             scriptPath.isEmpty() ? scriptPath : scriptPath.section('/', 0, -2),
                             m_codeEditorWidget->document()->documentName());

        //FIXME implement scripting interfaces for GraphTheory
        qCritical() << "Scripting engine currently disalbed";
//         engine->setScript(script, DocumentManager::self().activeDocument());
        engine->executeStep();
        return;
    }
    engine->continueExecutionStep();
#endif
}

void MainWindow::stopScript()
{
    m_kernel->stop();
    disableStopAction();
}

void MainWindow::enableStopAction()
{
    m_stopScript->setEnabled(true);
}

void MainWindow::disableStopAction()
{
    m_stopScript->setEnabled(false);
}

void MainWindow::showExecutionButtonDebug(bool visible)
{
    m_debugMenu->setVisible(visible);
}

void MainWindow::showExecutionButtonOneStep(bool visible)
{
    m_stepRunScript->setVisible(visible);
}
