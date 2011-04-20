/*
    Plugin that implements a list structure in Rocs
    Copyright (C) 2011 Wagner Reck


    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef LISTSTRUCTURE_H
#define LISTSTRUCTURE_H

#include "DataStructure.h"

class ListNode;
namespace Rocs{
class ListStructure : public DataStructure {
  Q_OBJECT
  public:
    using DataStructure::remove;
    using DataStructure::addPointer;
    using DataStructure::addData;

    ListStructure ( Document* parent = 0 );

    ListStructure(DataStructure& other, Document* parent);

    virtual ~ListStructure();

  public slots:

    virtual Data* addData ( QString name );

    virtual void remove(Data* n);

    virtual Pointer* addPointer ( Data* from, Data* to );

    void arrangeNodes();

    virtual void remove(Pointer* e);

    QScriptValue begin();
    void setBegin(ListNode* node);
    QScriptValue createNode(const QString &name);


private:
    void init();
    void createFront();

    ListNode * m_begin;
    QParallelAnimationGroup* m_animationGroup;

    bool m_building;
};
}
#endif // LISTSTRUCTURE_H
