#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include "visuallayer.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void setMapZoomValue(int v);
    void updateTitle(const QString& t);

    void updateMark();
protected:

    void
    resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;
    Marble::MarbleWidget *mapWidget;
    int m_mapZoomValue;

    VisualLayer* m_layer;
};

#endif // MAINWINDOW_H
