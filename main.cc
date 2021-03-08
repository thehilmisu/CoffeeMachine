// Bio-Hybrid Coffee Machine Example - Copyright (c) 2021 Bio-Hybrid GmbH
#include <QCoreApplication>
#include <QDebug>

#include "coffee_app.h"

int main(int argc, char** argv)
{
  // [OPTIONAL] Provide command line options to provide a recipe JSON file
  //            or to set the name of the coffee machine

  qDebug() << "Starting my coffee machine application...";

  CoffeeApp app(argc, argv);
  return app.exec();
}

