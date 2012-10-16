/*
    This file is part of Rocs.
    Copyright 2012  Andreas Cord-Landwehr <cola@uni-paderborn.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef POINTERTYPE_H
#define POINTERTYPE_H

#include <QString>
#include <QColor>

#include <boost/scoped_ptr.hpp>

#include "RocsCoreExport.h"
#include "CoreTypes.h"

class Data;
class PointerTypePrivate;
class DataStructure;
class Document;

/**
* \class PointerType
*
* A pointer type groups a set of pointers and gives them a common appearance. Each pointer type is
* associated to a document and must get an unique identifier on creation. To create a pointer type,
* use the static \see create(document, identifier) method.
*/
class ROCSLIB_EXPORT PointerType : public QObject
{
    Q_OBJECT

public:
    enum Direction {
        Unidirectional,
        Bidirectional
    };

    /**
     * Create pointer type objects.
     *
     * \param document is graph document for that the pointer type is created
     * \param identifier is unique identifier of the pointer type within the document
     * \return the created pointer type
     */
    static PointerTypePtr create(Document* parent, int identifier);

    /**
     * Default destructor.
     * DO NOT CALL IT, let the shared pointer take care for deletion.
     */
    virtual ~PointerType();

    /**
     * \return name of pointer type
     */
    const QString& name() const;

    /**
     * \return unique identifier of pointer type
     */
    int identifier() const;

    /**
     * \return direction for pointers of this type
     */
    Direction direction() const;

    /**
     * \return line style for pointers of this type
     */
    Qt::PenStyle lineStyle() const;

    /**
     * \return default color for pointers of this type
     */
    const QColor& defaultColor() const;


    /**
     * Add a new dynamic property to all data elements of this pointer type.
     *
     * \param name is the unique name of the property
     * \param defaultValue is the default value that is set initially
     */
    void addProperty(QString name, QString defaultValue = "");

    /**
     * Remove dynamic property from pointer type.
     */
    void removeProperty(QString name);

    /**
     * Rename property from \p oldName to \p newName. The old
     * property name at all pointers is changed with this change.
     * The property is not changed, if \p newName already exists at this
     * data type.
     *
     * \param oldName is the current property name
     * \param newName is the new property name
     */
    void renameProperty(QString oldName, QString newName);

    /**
     * \return list of all properties, ordered in display order
     */
    QList<QString> properties() const;

    /**
     * Set default value of property \p name to \p value.
     */
    void setPropertyDefaultValue(QString name, QVariant value);

    /**
     * \return default value for property \p name
     */
    QVariant propertyDefaultValue(QString name) const;

    /**
     * Set default value of property \p name to \p visible.
     */
    void setPropertyVisible(QString name, bool visible);

    /**
     * \return true if this property is visible, otherwise false.
     */
    bool isPropertyVisible(QString name) const;


public slots:
    /**
     * Set name of the pointer type.
     *
     * \param name of the pointer type
     */
    void setName(QString name);

    /**
     * Set default color value for this pointer type.
     *
     * \param color is the default color to be set
     */
    void setDefaultColor(QColor color);

    /**
     * Set direction for all pointers of this type.
     *
     * \param direction is the new direction for the pointers
     */
    void setDirection(PointerType::Direction direction);

    /**
     * Set line style for all pointers of this type.
     *
     * \param lineStyle is the new line style
     */
    void setLineStyle(Qt::PenStyle lineStyle);

    /**
     * Unregister pointer type from graph document.
     */
    void remove();

signals:
    /**
     * Emitted when data type is removed.
     */
    void removed();

    /**
     * Emitted when settings for the visual representation changed.
     */
    void styleChanged();

    /**
     * Emitted when direction was changed.
     */
    void directionChanged(PointerType::Direction direction);

    /**
     * Emitted when name was changed.
     */
    void nameChanged(const QString& name);

    /**
     * Emitted when new property was created.
     */
    void propertyAdded(const QString& name, const QVariant& defaultValue);

    /**
     * Emitted when property was removed.
     */
    void propertyRemoved(const QString& name);

    /**
     * Emitted when default value of property is changed.
     */
    void propertyDefaultValueChanged(const QString& name);

    /**
     * Emitted when the visibility was changed.
     */
    void propertyVisibilityChanged(const QString& name);

    /**
     * Emitted when a property was renamed.
     */
    void propertyRenamed(QString oldName, QString newName);

protected:
    /**
     * \internal
     * Default constructor. To create PointerType elements use \see PointerType::create(...).
     *
     * \param document is graph document for that the pointer type is created
     * \param identifier is unique identifier of the pointer type within the document
     */
    PointerType(Document* document, int identifier);

private:
    boost::scoped_ptr<PointerTypePrivate> d;
    PointerType(const PointerType &);
    PointerType & operator=(const PointerType &);
};

#endif // POINTERTYPE_H
