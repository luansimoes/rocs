/*
 *  Copyright 2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nodetypestyle.h"
#include <QColor>
#include <QDebug>

using namespace GraphTheory;

class GraphTheory::NodeTypeStylePrivate {
public:
    NodeTypeStylePrivate()
        : m_color(77, 77, 77) // dark gray
        , m_visible(true)
    {
    }

    ~NodeTypeStylePrivate()
    {
    }

    QColor m_color;
    bool m_visible;
};

NodeTypeStyle::NodeTypeStyle()
    : QObject()
    , d(new NodeTypeStylePrivate)
{

}

NodeTypeStyle::~NodeTypeStyle()
{

}

void NodeTypeStyle::setColor(const QColor &color)
{
    if (d->m_color == color) {
        return;
    }
    d->m_color = color;
    emit colorChanged(color);
}

QColor NodeTypeStyle::color() const
{
    return d->m_color;
}

void NodeTypeStyle::setVisible(bool visible)
{
    if (d->m_visible == visible) {
        return;
    }
    d->m_visible = visible;
    emit visibilityChanged(visible);
}

bool NodeTypeStyle::isVisible() const
{
    return d->m_visible;
}