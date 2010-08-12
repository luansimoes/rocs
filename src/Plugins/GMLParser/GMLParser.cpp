/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "GMLParser.h"

#include "Core/graphDocument.h"
#include <KAboutData>
#include <KGenericFactory>
#include <QFile>
#include <Core/graph.h>
#include "GMLGraphParsingHelper.h"
#include "GMLGrammar.h"

static const KAboutData aboutdata ( "rocs_gmlplugin", 0, ki18n ( "Open and Save GML files" ) , "0.1" );


extern Rocs::GMLPlugin::GMLGraphParsingHelper* phelper;

K_PLUGIN_FACTORY ( FilePLuginFactory, registerPlugin< GMLParser>(); )
K_EXPORT_PLUGIN ( FilePLuginFactory ( aboutdata ) )


GMLParser::~GMLParser() {

}

GMLParser::GMLParser ( QObject* parent, const QList< QVariant >& ) :
        FilePluginInterface ( FilePLuginFactory::componentData(), parent ) {

}

const QStringList GMLParser::extensions() const {
    return QStringList()
           << i18n ( "*.gml|Graph Markup Language Files" ) + '\n';
}


GraphDocument * GMLParser::readFile ( const QString &fileName ) {
    GraphDocument * graphDoc = new GraphDocument ( "Untitled" );
//     Graph * graph = graphDoc->addGraph();
    QList < QPair<QString, QString> > edges;
    QFile f ( fileName );
    if ( !f.open ( QFile::ReadOnly ) ) {
        setError ( i18n ( "Cannot open the file: %1. Error %2" ).arg ( fileName ).arg ( f.errorString() ) );
        delete graphDoc;
        return 0;
    }
    QString content = f.readAll();
     if (!Rocs::GMLPlugin::parse(content, graphDoc)) {
         setError(i18n("cannot parse the file %1.").arg(fileName));
         delete graphDoc;
         return 0;
    }
    return graphDoc;
}



bool GMLParser::writeFile ( GraphDocument &graph , const QString &filename ) {
    QFile file ( filename );
    QVariantList subgraphs;
    if ( file.open ( QFile::WriteOnly | QFile::Text) ) {
        QTextStream out (&file);
        Graph *g = graph.activeGraph();
        if (g) {
            out << QString("%1 %2 {\n").arg(g->directed()?"digraph":"graph").arg(g->name());
            foreach ( Node *n, g->nodes() ) {
                if (n->dynamicPropertyNames().contains("SubGraph")){
                  if (!subgraphs.contains(n->property("SubGraph"))){
                    subgraphs << n->property("SubGraph");
                    out << QString("subgraph %1 {\n").arg(n->property("SubGraph").toString());
                    foreach ( Node *nTmp, g->nodes() ) {
                        if (nTmp->property("SubGraph").isValid() && nTmp->property("SubGraph") == subgraphs.last()){
                          out << processNode(nTmp);
                        }
                    }
                    foreach ( Edge *e, g->edges() ) {
                        if (e->property("SubGraph").isValid() && e->property("SubGraph") == subgraphs.last()){ //Only edges from outer graph
                           out << processEdge(e);
                        }
                    }
                    out << "}\n";
                  }
                }else{
                  out << processNode(n);
                }
            }
            foreach ( Edge *e, g->edges() ) {
                if (!e->dynamicPropertyNames().contains("SubGraph")){ //Only edges from outer graph
                    out << processEdge(e);
                }
            }
            out << "}\n";
            return true;
        }
        setError(i18n("No active graph in this document."));
    }
    setError(i18n("Cannot open file %1 to write document. Error: %2").arg(filename).arg(file.errorString()));
    return false;
}

QString const GMLParser::processEdge(Edge*e ) const
{
    QString edge;
    edge.append(QString(" %1 -> %2 ").arg(e->from()->property("NodeName").isValid()?
                                                  e->from()->property("NodeName").toString():
                                                  e->from()->name())
                                      .arg( e->to()->property("NodeName").isValid()?
                                                  e->to()->property("NodeName").toString():
                                                  e->to()->name()));
    bool firstProperty = true;
    if(!e->name().isEmpty()){
      firstProperty = false;
      edge.append("[");
      edge.append(QString(" label = \"%2\" ").arg(e->name()));
    }
    foreach(QByteArray property, e->dynamicPropertyNames()){
      if (property != "SubGraph"){
        if(firstProperty == true){
          firstProperty = false;
          edge.append("[");
        }else{
          edge.append(", ");
        }
        edge.append(QString(" %1 = \"%2\" ").arg(QString(property)).arg(e->property(property).toString()));
      }
    }
    if (!firstProperty) //At least one property was inserted
      edge.append("]");
    edge.append(";\n");
    return edge;
}

QString const GMLParser::processNode(Node* n) const
{
        QString node;
        if (n->property("NodeName").isValid())
          node = QString("%1").arg(n->property("NodeName").toString());
        else
          node = QString("%1").arg(n->name());

        bool firstProperty = true;
        foreach(QByteArray property, n->dynamicPropertyNames()){
          if (property != "NodeName" && property != "SubGraph"){
            if(firstProperty == true){
              firstProperty = false;
              node.append("[");
            }else{
              node.append(", ");
            }
            node.append(QString(" %1 = \"%2\" ").arg(QString(property)).arg(n->property(property).toString()));
          }
        }
        //Need save X and Y??
        if (!firstProperty){ //At least one property was inserted
          node.append("]");
        }else{ //No needs os nodes definition, it doesn't have any property.
          return QString();
        }
        node.append(";\n");
        return node;
}

const QString GMLParser::lastError() {
    return _lastError;
}


void GMLParser::setError ( QString arg1 ) {
    _lastError = arg1;
}

const QString GMLParser::scriptToRun()
{
    return QString (/*"include ( arrangeNodes.js )\n"
                     "for (g = 0; g < graphs.length; ++g){\n"
                     "  nodes = graphs[g].list_nodes();\n"
                     "  for (var i = 0; i < nodes.length; i++){\n"
                     "    nodes[i].addDynamicProperty(\"NodeName\", nodes[i].name);\n"
                     "    if (nodes[i].label != undefined)\n"
                     "       nodes[i].name = nodes[i].label;\n"
                     "  }\n"
                     "}"*/
                   );
}


#include "GMLParser.moc"