/*
    This file is part of Rocs.
    Copyright 2012-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>

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

#include "testtikzfileformat.h"
#include "../tikzfileformat.h"
#include "fileformats/fileformatinterface.h"
#include "graphdocument.h"
#include "node.h"
#include "edge.h"
#include <QtTest/QtTest>

using namespace GraphTheory;

TestTikzFileFormat::TestTikzFileFormat()
{
}

void TestTikzFileFormat::serializeTest()
{
    GraphDocumentPtr document = GraphDocument::create();
    QMap<QString, NodePtr> nodes;

    // Creates a simple graph with 5 data elements and connect them with pointers.
    nodes.insert("a", Node::create(document));
    nodes["a"]->setDynamicProperty("label", "first node");
    nodes.insert("b", Node::create(document));
    nodes["b"]->setDynamicProperty("label", "b");
    nodes.insert("c", Node::create(document));
    nodes["c"]->setDynamicProperty("label", "c");
    nodes.insert("d", Node::create(document));
    nodes["d"]->setDynamicProperty("label", "d");
    nodes.insert("e", Node::create(document));
    nodes["e"]->setDynamicProperty("label", "e");

    Edge::create(nodes["a"], nodes["b"])->setDynamicProperty("label", "test value");
    Edge::create(nodes["b"], nodes["c"]);
    Edge::create(nodes["c"], nodes["d"]);
    Edge::create(nodes["d"], nodes["e"]);
    Edge::create(nodes["e"], nodes["a"]);

    // create exporter plugin
    TikzFileFormat serializer(this, QList<QVariant>());
    serializer.setFile(QUrl::fromLocalFile("test.tgf"));
    serializer.writeFile(document);
    QVERIFY(serializer.hasError() == false);
}

QTEST_MAIN(TestTikzFileFormat);
