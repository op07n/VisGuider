#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QFont font;


    // Load the embedded font.
    QString fontPath = ":/font/fa";
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    QString fontName=QFontDatabase::applicationFontFamilies(fontId).at(0);
    if (fontId != -1)
    {
        font.setFamily(fontName);
        app.setFont(font);
        qDebug()<<"add fa font:"<<fontName;
    }

    //font.setBold(true);
    font.setPointSize(24);
    MainWindow w;
    w.setFont(font);
    w.show();

    return app.exec();
}
