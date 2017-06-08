#include "stdafx.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "visuallayer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->mapWidget->move(0, 0);

    // Create a Marble QWidget without a parent
    mapWidget = new MarbleWidget(ui->mapWidget);


    // Load the OpenStreetMap map
    //mapWidget->setMapThemeId( "earth/plain/plain.dgml" );
    mapWidget->setMapThemeId( "earth/bluemarble/bluemarble.dgml" );
    //mapWidget->setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    mapWidget->setProjection( Spherical);//Mercator );
    GeoDataCoordinates coord(116, 39, 0,GeoDataCoordinates::Degree);
    mapWidget->centerOn(coord);

    mapWidget->setShowOverviewMap(false);
    mapWidget->setShowClouds(true);
    // Hide the FloatItems: OverviewMap, ScaleBar and Compass
    //        mapWidget->setShowOverviewMap(false);
    //        mapWidget->setShowScaleBar(false);
    //        mapWidget->setShowCompass(false);
    mapWidget->setShowTerrain(true);

    //zoom
    mapWidget->zoomView( 2200 );

    //mapWidget->setFont(this->font());
    mapWidget->setDefaultFont(mapWidget->font());

//    mapWidget->model()->addGeoDataFile("/Users/hehao/Downloads/Bucharest.kml");

    // Create and register our paint layer
    m_layer = new VisualLayer(mapWidget);
    VisualLayer* layer = m_layer;
    mapWidget->addLayer(layer);

    size_t mark = layer->addPlacemark("Q\uf135");
    layer->setPlacemarkCoordinate(mark, 116, 38);
    layer->setPlacemarkColor(mark, Qt::red);
    layer->setPlacemarkFontSize(mark, 24);
    layer->setPlacemarkIcon(mark, ":/nation/res/nation-ico/United-States.ico");
    layer->setPlacemarkIconSize(mark, 16,16);
    qDebug()<<"mark "<<mark;
    mark = layer->addPlacemark("\uf135aaabbb");
    layer->setPlacemarkCoordinate(mark, 116.5, 38);
    layer->setPlacemarkIcon(mark, ":/icon/res/gur-project/gur-project-29.png");
    layer->setPlacemarkIconSize(mark, 16,16);
    qDebug()<<"mark "<<mark;
    mark = layer->addPlacemark("\uf135aaabbb");
    layer->setPlacemarkCoordinate(mark, 117.5, 38);
    layer->setPlacemarkIcon(mark, ":/icon/res/gur-project/gur-project-21.png");
    layer->setPlacemarkIconSize(mark, 16,16);
    qDebug()<<"mark "<<mark;


    // Connect the map widget to the position label.
    QObject::connect( mapWidget, SIGNAL( mouseMoveGeoPosition( QString ) ),
                      this, SLOT( updateTitle( QString ) ) );

    QObject::connect( mapWidget, SIGNAL( zoomChanged(int) ),
                      this, SLOT( setMapZoomValue(int) ) );

    // Update each second to give the clock second resolution
    QTimer* seconds = new QTimer(this);
    seconds->setInterval(1000);
    QObject::connect(seconds, SIGNAL(timeout()), mapWidget, SLOT(update()));
    QObject::connect(seconds, SIGNAL(timeout()), this, SLOT(updateMark()));
    seconds->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setMapZoomValue(int v)
{
    m_mapZoomValue=v;
}

void MainWindow::updateTitle(const QString &t)
{
    this->setWindowTitle(tr("VisGuider %1-%2").arg(m_mapZoomValue).arg(t));
}

void MainWindow::updateMark()
{
    size_t mark = 2;
    qreal lon, lat, alt;
    m_layer->getPlacemarkCoordinate(mark, lon, lat, alt);
    //qDebug()<<lon<<lat<<alt;
    lon += 0.01;
    lat += 0.02;
    m_layer->setPlacemarkCoordinate(mark, lon, lat, alt);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->mapWidget->resize(event->size());
    mapWidget->resize(ui->mapWidget->size());
    //qDebug()<<event->size()<<"  "<<ui->mapWidget->size();
}
