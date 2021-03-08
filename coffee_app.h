// Bio-Hybrid Coffee Machine Example - Copyright (c) 2021 Bio-Hybrid GmbH
#pragma once

#include <QGuiApplication>

class CoffeeMaker;
class CoffeeWeb;


namespace SCREENLIST_NAMESPACE {

    Q_NAMESPACE
    enum SCREEN_LIST{
        standbyScreen = 0,

    };
    Q_ENUM_NS(SCREEN_LIST)

}


class CoffeeApp : public QGuiApplication
{
    Q_OBJECT
    Q_PROPERTY( QString coffeeList READ coffeeList NOTIFY receipesReceived )
public:
    CoffeeApp(int& argc, char** argv);
    QString coffeeList();


private:
    QString _coffeeList;
    CoffeeMaker* m_coffeeMaker;
    CoffeeWeb* m_coffeeWeb;
signals:
    void receipesReceived(QString data);

};
