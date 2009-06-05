
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

#include "action_Select.h"
#include "SUI_GraphScene.h"
#include "graph.h"
#include "node.h"
#include "edge.h"
#include "graphicsitem_Node.h"
#include "graphicsitem_Edge.h"
#include "graphicsitem_MultiEdge.h"
#include "graphicsitem_OrientedEdge.h"

#include <KLocale>
#include <KDebug>
#include <QDebug>

SelectAction::SelectAction(GraphScene *scene, QObject *parent)
        : AbstractAction(scene, parent) {
    setText(i18n ( "Select" ));
    setToolTip ( i18n ( "Select Items by clicking on them." ) );
    setIcon ( KIcon ( "single-select" ) );
    _selectionRect = 0;
}

SelectAction::~SelectAction() {
}

void SelectAction::executePress(QPointF pos){
  _p1 = pos;
  _selectionRect = new QGraphicsRectItem();
  _selectionRect->setFlag(QGraphicsItem::ItemIsSelectable, false);
  _graphScene->addItem(_selectionRect);
}

void SelectAction::executeMove(QPointF pos){
  if (_selectionRect == 0) return;
  _selectionRect->setRect(QRectF(_p1, pos));
}

void SelectAction::executeRelease(QPointF pos) {
    if ( _selectionRect == 0) return;
    if ( !_graph ) return;    
    _graphScene->removeItem(_selectionRect);
    
    delete _selectionRect;
    _selectionRect = 0;
    
    QList<QGraphicsItem*> currentSelection = _graphScene->selectedItems();
    foreach(QGraphicsItem *i, currentSelection) {
        i->setSelected(false);
        i->update();
    }
    
    if (pos == _p1){
      qDebug() << "Selecionando um item apenas.";
      singleSelect(pos);
    }
    else{
      qDebug() << "Selecionando um monte de items";
      multiSelect(pos);
    }
    
}

void SelectAction::multiSelect(QPointF pos){
  QList<QGraphicsItem*> items = _graphScene->items(QRectF(_p1, pos));
  foreach(QGraphicsItem *i, items){
    if (!( qgraphicsitem_cast<NodeItem*>(i) 
	|| qgraphicsitem_cast<EdgeItem*>(i)
	|| qgraphicsitem_cast<OrientedEdgeItem*>(i))){
      items.removeAll(i);
    }
  }
  if (items.size() == 1){
    singleSelect(items[0]->pos());
    return;
  }
  
  foreach(QGraphicsItem *i, items){
    i->setSelected(true);
    i->update();
  }
  
}

void SelectAction::singleSelect(QPointF pos){
    QGraphicsItem * item = _graphScene->itemAt(pos);
    if ( !qgraphicsitem_cast<NodeItem*>(item) && !qgraphicsitem_cast<EdgeItem*>(item) && !qgraphicsitem_cast<OrientedEdgeItem*>(item)){
        emit ItemSelectedChanged(0);
        return;
    }

    item->setSelected(true);
    emit ItemSelectedChanged(item);
    item->update();
    qDebug() << "Item Selected!";
}
#include "action_Select.moc"