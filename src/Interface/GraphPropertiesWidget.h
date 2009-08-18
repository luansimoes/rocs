#ifndef SUI_GRAPHPROPERTIESWIDGET_H
#define SUI_GRAPHPROPERTIESWIDGET_H

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
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	 GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License
	 along with Step; if not, write to the Free Software
	 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA	02110-1301	USA
*/
#include <QWidget>
#include <QScrollArea>
#include "ui_GraphPropertiesWidget.h"
 
class GraphPropertiesModel;
class QGraphicsItem;
class Graph;
class Node;
class Edge;


/*! \brief Properties Area, not used yet. */
class MainWindow;
class GraphPropertiesWidget : public QScrollArea, public Ui::GraphPropertiesWidget
{
    Q_OBJECT
public:
    GraphPropertiesWidget ( MainWindow* parent = 0 );
public slots:
    void setDataSource(QGraphicsItem *o);
    
private slots:
  void nodeNameChanged(const QString& s);
  void nodeColorChanged(QColor c);
  void nodeEndChanged(bool b);
  void nodeBeginChanged(bool b);
  void nodeXChanged(int x);
  void nodeYChanged(int y);
  void nodeValueChanged(const QString& s);
  void edgeColorChanged(QColor c);
  void edgeNameChanged(const QString& s);
  void edgeValueChanged(const QString& s);
  void on__btnAddProperty_clicked();

private:
    GraphPropertiesModel *_model;
    Graph *_graph;
    Node *_node;
    Edge *_edge;
    QObject *_obj;
    QGraphicsItem *_graphicsItem;
    MainWindow *_mainWindow;
    
    void setGraph(Graph *g);
    void setNode(Node *n);
    void setEdge(Edge *e);
    void unsetNode();
    void unsetEdge();
    void unsetGraph();
    void unsetAll();
};
#endif