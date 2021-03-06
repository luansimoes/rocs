/*
 *  Copyright 2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QWidget>
#include "typenames.h"

class QListView;
class QModelIndex;

namespace GraphTheory {

class NodeTypePropertyModel;
class EdgeTypePropertyModel;

class PropertiesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PropertiesWidget(QWidget *parent = 0);
    void setType(GraphTheory::NodeTypePtr type);
    void setType(GraphTheory::EdgeTypePtr type);

public Q_SLOTS:
    void addProperty();
    void deleteProperty(const QModelIndex &index);
    void renameProperty(const QModelIndex &index, const QString &name);

private:
    NodeTypePtr m_nodeType;
    NodeTypePropertyModel *m_nodeModel;
    EdgeTypePtr m_edgeType;
    EdgeTypePropertyModel *m_edgeModel;
    QListView *m_view;
};
}

#endif
