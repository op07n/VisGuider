#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <marble/MarbleWidget.h>
#include <marble/MarbleGlobal.h>
#include <marble/GeoDataDocument.h>
#include <marble/GeoDataPlacemark.h>
#include <marble/GeoDataLineString.h>
#include <marble/GeoDataTreeModel.h>
#include <marble/MarbleModel.h>
///style
#include <marble/GeoDataStyle.h>
#include <marble/GeoDataIconStyle.h>
#include <marble/GeoDataLabelStyle.h>
#include <marble/GeoDataLineStyle.h>
#include <marble/GeoDataPolyStyle.h>
#include <marble/GeoDataBalloonStyle.h>

#include <QMainWindow>
#include <QResizeEvent>
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
protected:

    void
    resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;
    Marble::MarbleWidget *mapWidget;
    int m_mapZoomValue;
};

#endif // MAINWINDOW_H
