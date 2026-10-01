#include "mainwindow.h"

#include <QApplication>
#include <QFile>

QString loadQss(){
    QFile file(":/style.qss");

    file.open(QFile::ReadOnly);

    QString style = file.readAll();

    file.close();
    return style;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    const QString style=loadQss();
    a.setStyleSheet(style);

    MainWindow w;
    w.show();

    return QApplication::exec();
}
