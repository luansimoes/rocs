/* 
    This file is part of Rocs,
    Copyright 2008-2011  Tomaz Canabrava <tomaz.canabrava@gmail.com>
    Copyright 2008       Ugo Sangiori <ugorox@gmail.com>

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

#include "GraphPropertiesWidget.h"
#include "model_GraphProperties.h"
#include <KDebug>
#include <QGraphicsItem>
#include "DataStructure.h"
#include "Document.h"
#include "Data.h"
#include "Pointer.h"
#include "MainWindow.h"
#include "DataItem.h"
#include "PointerItem.h"
#include "GraphScene.h"
#include <KLocale>
#include <QRadioButton>
#include <DataStructurePluginManager.h>
#include "DocumentManager.h"


GraphPropertiesWidget::GraphPropertiesWidget (DataStructurePtr g, MainWindow* parent )
        : KButtonGroup ( parent ) {
    setupUi(this);
    _mainWindow = parent;

    _graph = g;
    _graphName->setText(_graph->name());
    _graphEdgeColor->setColor(_graph->pointerDefaultColor());
    _graphNodeColor->setColor(_graph->dataDefaultColor());
    _graphVisible->setChecked( ! _graph->readOnly());
    _activateGraph->setChecked(true);
    _showEdgeNames->setChecked( _graph->pointerNameVisibility() );
    _showEdgeValues->setChecked(_graph->pointerValueVisibility());
    _showNodeNames->setChecked( _graph->dataNameVisibility() );
    _showNodeValues->setChecked(_graph->dataValueVisibility());

    _editWidget->setVisible(_activateGraph->isChecked());
    
    if (!_extraProperties->layout()){
        QLayout * lay = DataStructurePluginManager::self()->dataStructureExtraProperties(g, _extraProperties);
        _extraProperties->setLayout(lay);
    }

    Document *gDocument = qobject_cast<Document*>(g->parent());
    connect(this, SIGNAL(addGraph(QString)), gDocument, SLOT(addDataStructure(QString)));
    connect(this, SIGNAL(removeGraph(DataStructurePtr)), g.get(), SLOT(remove()));
    
    
    connect( _graphEdgeColor, SIGNAL(activated(QColor)), this, SLOT(setPointerDefaultColor(QColor)));
    connect( _graphNodeColor, SIGNAL(activated(QColor)), this, SLOT(setDatumDefaultColor(QColor)));

    connect( _showEdgeNames,  SIGNAL(toggled(bool)), g.get(), SLOT(setPointerNameVisibility(bool)));
    connect( _showEdgeValues, SIGNAL(toggled(bool)), g.get(), SLOT(setPointerValueVisibility(bool)));
    connect( _showNodeNames,  SIGNAL(toggled(bool)), g.get(), SLOT(setDataNameVisibility(bool)  ));
    connect( _showNodeValues, SIGNAL(toggled(bool)), g.get(), SLOT(setDataValueVisibility(bool) ));

    connect( _graphName,      SIGNAL(textChanged(QString)), g.get(), SLOT(setName(QString)));
}

GraphPropertiesWidget::~GraphPropertiesWidget(){
}

void GraphPropertiesWidget::setPointerDefaultColor(QColor c){ 
    _graph->setPointerDefaultColor(c); 
}

void GraphPropertiesWidget::setDatumDefaultColor(QColor c){
    _graph->setDataDefaultColor(c);
}

void GraphPropertiesWidget::on__graphPointerColorApplyNow_clicked() {  
    _graph->setPointersColor(_graphEdgeColor->color()); 
}

void GraphPropertiesWidget::on__graphDatumColorApplyNow_clicked() { 
    _graph->setDataColor(_graphNodeColor->color()); 
}

void GraphPropertiesWidget::on__graphVisible_toggled(bool b){
  _graph->setReadOnly( !b );
  _mainWindow->scene()->hideGraph( _graph, b );
}

QRadioButton *GraphPropertiesWidget::radio()const {
    return _activateGraph;
}

void GraphPropertiesWidget::on__activateGraph_toggled(bool b) {
    _editWidget->setVisible( b );
    if (b) {
       DocumentManager::self()->activeDocument()->setActiveDataStructure(_graph);
    }
}

void GraphPropertiesWidget::on__graphDelete_clicked() {
    emit removeGraph(_graph);
}

void GraphPropertiesWidget::on__graphName_textChanged(const QString& s){
    _activateGraph->setText(s);
}
