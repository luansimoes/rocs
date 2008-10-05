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
#ifndef DINODE_H
#define DINODE_H

#include "libgraph_Node.h"
namespace libgraph{

class Edge;
class Graph;

class DiNode : public Node {
public:
  /*! Default constructor */
  DiNode(QPointF position, Graph *parent);
  
  /*! Default Destructor */
  ~DiNode();

  /*! Return the first node that this node is connected to. */
  Node *getAdjacentNode(); // TODO: Need to test

  /*! return the node connected with this using edge e */
  Node *getAdjacentNode(Edge *e); // TODO: Need to test

  /*! Return the first node that has not been visited and this node is connected to */
  Node *getFirstUnvisitedNode(); // TODO: need to test

  /*! Return the first edge on the list, does not care if it's visited or not */
  Edge *getFirstEdge(); // TODO: need to test

  /*! Return the first unvisited edge on the list */
  Edge *getFirstUnvisitedEdge(); // TODO: need to test
  
  /*! Verify if this node is connected with the To node */
  bool isConnected(Node *to);

  /*! add a new edge to this node */
  void addEdge(Edge *e);	//WARNING: CANNOT CALL IT DIRECLY.
};

}

#endif
