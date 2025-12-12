//
// Created by kiro3 on 12/11/2025.
//
// main.cpp
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include "mainwindow.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
