// Bio-Hybrid Coffee Machine Example - Copyright (c) 2021 Bio-Hybrid GmbH
#include "coffee_app.h"

#include <coffeemaker/coffeemaker.h>
#include <coffeeweb/coffeeweb.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QDebug>

// -------------------------------------------------------------------------------------------------
CoffeeApp::CoffeeApp(int& argc, char** argv)
    : QGuiApplication(argc, argv)
    , m_coffeeMaker(new CoffeeMaker(this))
    , m_coffeeWeb(new CoffeeWeb(this))
{
    const auto engine = new QQmlApplicationEngine(this);
    qmlRegisterUncreatableType<CoffeeMaker>("CoffeeMaker", 1, 0, "CoffeeMaker", "Uncreatable type");
    qmlRegisterUncreatableMetaObject(
      SCREENLIST_NAMESPACE::staticMetaObject, // static meta object
      "screenlistEnum",                // import statement (can be any string)
      1, 0,                          // major and minor version of the import
      "ScreenList",                 // name in QML (does not have to match C++ name)
      "Error: only enums"            // error in case someone tries to create a MyNamespace object
    );


    // Register the application's coffeemaker object with the engine so it is available in Qml.
    // it is registered with the name : 'maker'
    _coffeeList.clear();
    const auto rootContext = engine->rootContext();
    rootContext->setContextProperty("maker", m_coffeeMaker);
    rootContext->setContextProperty("coffee", this);
    rootContext->setContextProperty("applicationDirPath", QGuiApplication::applicationDirPath());
    // Load our main qml file
    engine->addImportPath("qrc:/");
    engine->load(QUrl(QStringLiteral("qrc:/main.qml")));



    quint32 old_id = 0;
    connect(m_coffeeWeb, &CoffeeWeb::recipesRequestReply, this,
    [&](quint32 id, const QString& json) {

        //if(id != old_id)
        {
            _coffeeList = json;
            //qDebug() << "#### Received JSON:" << id << "\n" << qPrintable(json);
            emit receipesReceived(_coffeeList);

            old_id = id;
        }


    });
    m_coffeeWeb->requestRecipes();
}

QString CoffeeApp::coffeeList() {
    return _coffeeList;
}


// -------------------------------------------------------------------------------------------------
