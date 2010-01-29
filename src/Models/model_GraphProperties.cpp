#include "model_GraphProperties.h"
#include <KLocale>
#include <KDebug>
#include <dinamicpropertieslist.h>

GraphPropertiesModel::GraphPropertiesModel( QObject *parent ) : QAbstractTableModel(parent) {
    // start all pointers to zero, so we don't crash things.
    _dataSource = 0;
    _metaObject = 0;
}

int GraphPropertiesModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    // if there's no dataSource, there's no data. return zero.
    // else return the size of properties of the object.
    if (_dataSource == 0) {
        return 0;
    }
    kDebug() << _dataSource->dynamicPropertyNames().size();
    return _dataSource->dynamicPropertyNames().size();
}

int GraphPropertiesModel::columnCount ( const QModelIndex & parent ) const {
    // should always be 2.
    Q_UNUSED(parent);
    return 3; //Name - Value - Type
}

QVariant GraphPropertiesModel::data(const QModelIndex &index, int role) const {
    // error conditions, return a default value constructed QVariant().
    if ( !index.isValid() ) {
        return QVariant();
    }
    if ( index.row() >= _dataSource->dynamicPropertyNames().size() ) {
        return QVariant();
    }
    if ( role != Qt::DisplayRole ) {
        return QVariant();
    }

    // if it's the first column, return it's name.
    // if it's the second column, return it's data.

    if (index.column() == 0) {
        return _dataSource->dynamicPropertyNames()[index.row()];
    }
    else if (index.column() == 1) {
        return _dataSource->property( _dataSource->dynamicPropertyNames()[index.row()]);
    } else if (index.column() == 2) {
        return DinamicPropertiesList::New()->typeInText(_dataSource,
                _dataSource->dynamicPropertyNames()[index.row()]);
    }

    // if it's anything else, it's an error, return a default value constructed QVariant.
    return QVariant();
}

QVariant GraphPropertiesModel::headerData(int section, Qt::Orientation orientation, int role) const {
    // if the role is not for displaying anything, return a default empty value.
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return i18n("Property");
        case 1:
            return i18n("Value");
        case 2:
            return i18n("Type");
        default:
            return QVariant();
        }
    }
    return QVariant();
}

void GraphPropertiesModel::setDataSource(QObject *dataSource) {
    // if there isn't any datasource being send, just remove everything from the model and exit.
    if (dataSource == 0) {
        int count = rowCount();
        if (count == 0) return;
        beginRemoveRows(QModelIndex(), 0, count-1);
        endRemoveRows();
        return;
    }

    // clear the model for the new data.
    if ( _dataSource) {
        beginRemoveRows(QModelIndex(), 0, _dataSource->dynamicPropertyNames().size()-1);
        endRemoveRows();
    }

    // set some needed variables.
    _dataSource = dataSource;
    _metaObject = _dataSource -> metaObject();

    // insert the information.


    beginInsertRows(QModelIndex(), 0, dataSource->dynamicPropertyNames().size()-1);
    endInsertRows();

}

Qt::ItemFlags GraphPropertiesModel::flags(const QModelIndex &index) const {
    if (index.isValid()) {
        if (index.column() != 2) {//Can't change type for now
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        } else {
            return QAbstractItemModel::flags(index);
        }
    }
    return Qt::ItemIsEnabled;


}

bool GraphPropertiesModel::setData(const QModelIndex &index, const QVariant &value,
                                   int role) {
    if (index.isValid() && role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            kDebug() << "Change name. DinamicPropertiesList take part";
            DinamicPropertiesList::New()->changePropertyName(QString(_dataSource->dynamicPropertyNames()[index.row()]), //name
                    value.toString(), //NewName
                    _dataSource); //Object
            break;
        case 1:
            kDebug() << "Just change Value";
            _dataSource->setProperty(_dataSource->dynamicPropertyNames()[index.row()],
                                     value);
            break;
        default:
            kDebug() << "shoudn't enter here ¬¬";
            return false;
            break;
        }

        emit dataChanged(index, index);
        return true;

    }
    return false;

}

void GraphPropertiesModel::addDinamicProperty(QString name, QVariant value, QObject *obj, bool isGlobal) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    if (isGlobal) {
        Edge * edge = qobject_cast<Edge*> (obj);
        if (edge) {
            edge->graph()->addEdgesDinamicProperty(name,value);
        }
        Node * node = qobject_cast<Node*> (obj);
        if (node) {
            node->graph()->addNodesDinamicProperty(name,value);
        }
        

    } else {
        Edge * edge = qobject_cast<Edge*> (obj);
        if (edge) {
            edge->addDinamicProperty(name,value);
        }
        Node * node = qobject_cast<Node*> (obj);
        if (node) {
            node->addDinamicProperty(name,value);
        }
        Graph * graph = qobject_cast<Graph*> (obj);
        if (graph) {
            graph->addDinamicProperty(name, value);
        }

    }

        endInsertRows();
  }
