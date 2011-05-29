/* 
    This file is part of Rocs.
    Copyright 2011  Tomaz Canabrava <tomaz.canabrava@gmail.com>

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


#ifndef DATASTRUCTUREPLUGININTERFACE_H
#define DATASTRUCTUREPLUGININTERFACE_H

#include <kplugininfo.h>

//Qt classes
class QLayout;
class QGraphicsItem;

//Rocs Classes
class DataStructure;
class Data;
class Pointer;

class Document;

// KClasses
class KComponentData;

#include "rocslib_export.h"


class ROCSLIB_EXPORT DataStructurePluginInterface: public QObject
{
  Q_OBJECT

public:
  DataStructurePluginInterface(const KComponentData &instance, QObject* parent);

  virtual ~DataStructurePluginInterface();
  virtual DataStructure* createDataStructure(Document * parent) = 0;
  virtual DataStructure* convertToDataStructure(DataStructure*, Document* parent) = 0;

  /** @brief Check if is possíble to convert from the doc document to this data structure.
   * If is possible to convert, is returned a true value, else, before return false, is good to implement in each scritp a message saying to user that not is possible to convert and asking if the conversion shoud proceed. If the user say to carry on, then true must be returned, otherwise false.
   */
  virtual bool canConvertFrom(Document* doc) const = 0;

  QString name();

  virtual QGraphicsItem * dataItem (Data *data) const = 0;
  virtual QGraphicsItem * pointerItem (Pointer *pointer) const = 0;

  virtual QLayout* dataExtraProperties ( Data* data, QWidget* widget ) const;
  virtual QLayout* pointerExtraProperties ( Pointer* pointer, QWidget* widget )const;
  virtual QLayout* dataStructureExtraProperties ( DataStructure *dataStructure, QWidget* widget )const;

};


#endif // DataStructurePLUGININTERFACE_H
