#include "kross_rocsengine.h"
#include "libgraph_GraphDocument.h"
#include "libgraph_Graph.h"
#include <QList>
#include <KDebug>
#include <KTextBrowser>

kross_rocsengine::kross_rocsengine(KTextBrowser *debugArea){
  _debugArea = debugArea;
}

void kross_rocsengine::setDocument(libgraph::GraphDocument *gd){
  _graphDocument = gd;
}

void kross_rocsengine::setEngine(const QString& engine){
  _engine = engine;
}


void kross_rocsengine::execute(const QString& script){
  Kross::Action action(0, "myScript");

  action.setInterpreter(_engine);

  QList<libgraph::Graph*> graphList = _graphDocument->graphs();

 // action.addObject( graphList, "graphs");
  foreach(libgraph::Graph* g, graphList){
    action.addObject( g, g->name());
  }

  foreach(QString s, Kross::Manager::self().interpreters()){
    kDebug() << "BACKEND FOUND" << s;
  }
  
  action.addObject( _debugArea, "debug");

  QString codeToBeExecuted = "";
  if(_engine == "python"){
    foreach(libgraph::Graph* g, graphList){
      codeToBeExecuted += "import " + g->name() + " \n";
    }
    kDebug() << "Python imports : " << codeToBeExecuted;
  }
  else if(_engine == "ruby"){
    foreach(libgraph::Graph* g, graphList){
      codeToBeExecuted += "require '" + g->name() + "' \n";
    }
    kDebug() << "ruby requires : " << codeToBeExecuted;
  }
    
  codeToBeExecuted += script;
  action.setCode( codeToBeExecuted.toAscii() );

  action.trigger();
  kDebug()  << "Should Have Worked";
}
