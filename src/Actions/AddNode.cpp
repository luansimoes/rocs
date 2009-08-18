
/* This file is part of Rocs,
   Copyright (C) 2008 by:
   Tomaz Canabrava <tomaz.canabrava@gmail.com>
   Ugo Sangiori <ugorox@gmail.com>

   Rocs is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Rocs is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Step; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "AddNode.h"
#include "GraphScene.h"
#include "graph.h"
#include "NodeItem.h"
#include "node.h"
#include <KLocale>
#include "graphDocument.h"
#include <KDebug>

AddNodeAction::AddNodeAction(GraphScene *scene, QObject *parent)
        : AbstractAction(scene, parent) {
    setText(i18n ( "Add Node" ));
    setToolTip ( i18n ( "Creates a new node at the click position on the drawing area." ) );
    setIcon ( KIcon ( "rocsaddnode" ) );
}

AddNodeAction::~AddNodeAction() {
    kDebug() << "Destroyed";
}

void AddNodeAction::executePress(QPointF pos) {
    if (_graph == 0) {
        kDebug() << "Error, Graph == 0";
        return;
    }

    if (pos.x() < 0) return;
    else if (pos.y() < 0) return;
    else if (pos.x() > _graphDocument->width()) return;
    else if (pos.y() > _graphDocument->height()) return;

    Node *n = _graph -> addNode(i18n("untitled"));
    n ->setX(pos.x());
    n -> setY(pos.y());
    Graph *g = qobject_cast<Graph*>(n->parent());
    g->calcRelativeCenter();

    _graphScene->createNode(_graph, n);
}