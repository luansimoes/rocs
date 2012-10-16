/*
    This file is part of Rocs.
    Copyright 2009-2011  Tomaz Canabrava <tomaz.canabrava@gmail.com>
    Copyright 2012       Andreas Cord-Landwehr <cola@uni-paderborn.de>

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

#include "DataPropertiesWidget.h"
#include "Data.h"
#include <KDebug>
#include "Scene/DataItem.h"
#include "DataStructure.h"
#include "Actions/PropertiesDialogAction.h"
#include "model_GraphProperties.h"
#include <QPainter>
#include <DataStructureBackendManager.h>
#include <DataStructurePluginInterface.h>

DataPropertiesWidget::DataPropertiesWidget(DataPtr data, QWidget* parent)
    : KDialog(parent)
{
    ui = new Ui::DataPropertiesWidget;
    ui->setupUi(mainWidget());

    // edit data types by separate dialog
    QPointer<PropertiesDialogAction> dataTypePropertiesAction = new PropertiesDialogAction(
            i18n("Edit Data Types"), data->dataStructure()->document()->dataType(data->dataType()), this);
    ui->_editType->setDefaultAction(dataTypePropertiesAction);
    ui->_editType->setIcon(KIcon("document-properties"));
    connect(data->dataStructure()->document(), SIGNAL(dataTypeCreated(int)), this, SLOT(updateDataTypes()));
    connect(data->dataStructure()->document(), SIGNAL(dataTypeRemoved(int)), this, SLOT(updateDataTypes()));

    setCaption(i18nc("@title:window", "Data Element Properties"));
    setButtons(Close);
    setAttribute(Qt::WA_DeleteOnClose);
    setData(data);
}

void DataPropertiesWidget::setData(DataPtr data)
{
    DataTypePtr dataType;
    if (_data == data) {
        return;
    }
    if (_data) {
         dataType = _data->dataStructure()->document()->dataType(_data->dataType());
         dataType->disconnect(this);
        _data->disconnect(this);
        ui->_dataType->clear();
    }
    _data = data;

    updateDataTypes();

    delete ui->extraItems->layout();
    ui->extraItems->setLayout(DataStructureBackendManager::self()->dataExtraProperties(_data, this));
    reflectAttributes();

    // listen to ui
    connect(ui->_dataType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setDataType(int)));
    connect(ui->_color, SIGNAL(activated(QColor)),
            this, SLOT(colorChanged(QColor)));

    dataType = _data->dataStructure()->document()->dataType(_data->dataType());
    connect(dataType.get(), SIGNAL(propertyAdded(QString,QVariant)), this, SLOT(updateProperties()));
    connect(dataType.get(), SIGNAL(propertyRemoved(QString)), this, SLOT(updateProperties()));
    connect(dataType.get(), SIGNAL(propertyRenamed(QString,QString)), this, SLOT(updateProperties()));

    GraphPropertiesModel *model = new GraphPropertiesModel();
    model->setDataSource(_data.get());

    ui->_propertiesTable->setModel(model);
    ui->_propertiesTable->horizontalHeader()->setProperty("stretchLastSection", true);
}

void DataPropertiesWidget::setPosition(QPointF screenPosition)
{
    move(screenPosition.x() + 10, screenPosition.y() + 10);
}

void DataPropertiesWidget::reflectAttributes()
{
    if (!ui->extraItems->layout()) {
        _oldDataStructurePlugin = DataStructureBackendManager::self()->activeBackend()->internalName();
    }

    if (_oldDataStructurePlugin != DataStructureBackendManager::self()->activeBackend()->internalName()) {
        ui->extraItems->layout()->deleteLater();
    }

    if (!ui->extraItems->layout()) {
        ui->extraItems->setLayout(DataStructureBackendManager::self()->dataExtraProperties(_data, this));
    }

    ui->_color->setColor(_data->color().value<QColor>());

    DataTypePtr dataType = _data->dataStructure()->document()->dataType(_data->dataType());
    ui->_dataType->setCurrentIndex(ui->_dataType->findData(QVariant(_data->dataType())));
}

void DataPropertiesWidget::colorChanged(const QColor& c)
{
    _data->setColor(QColor(c));
}

void DataPropertiesWidget::setDataType(int dataTypeIndex)
{
    _data->setDataType(ui->_dataType->itemData(dataTypeIndex).toInt());
}

void DataPropertiesWidget::updateDataTypes()
{
    ui->_dataType->clear();
    // setup data types combobox
    foreach (int dataType, _data->dataStructure()->document()->dataTypeList()) {
        QString dataTypeString = _data->dataStructure()->document()->dataType(dataType)->name();
        ui->_dataType->addItem(dataTypeString, QVariant(dataType));
    }
    if (_data) {
        ui->_dataType->setCurrentIndex(ui->_dataType->findData(QVariant(_data->dataType())));
    }
}

void DataPropertiesWidget::updateProperties()
{
    // TODO the following can be solved much nicer by updating the model
    GraphPropertiesModel *model = new GraphPropertiesModel();
    model->setDataSource(_data.get());
    ui->_propertiesTable->model()->deleteLater();
    ui->_propertiesTable->setModel(model);
}