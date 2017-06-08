#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <marble/GeoDataLatLonBox.h>
using namespace Marble;
#include <QFontDatabase>
#include <QDebug>
#include <QFontInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->mapWidget->move(0, 0);

    // Create a Marble QWidget without a parent
    mapWidget = new MarbleWidget(ui->mapWidget);


    // Load the OpenStreetMap map
    mapWidget->setMapThemeId( "earth/bluemarble/bluemarble.dgml" );
    //mapWidget->setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    mapWidget->setProjection( Spherical);//Mercator );
    GeoDataCoordinates coord(25.97, 44.43, 0,GeoDataCoordinates::Degree);
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

    QSharedPointer<GeoDataStyle> style=QSharedPointer<GeoDataStyle>(new GeoDataStyle);
    //GeoDataStyle* style = new GeoDataStyle;
    GeoDataIconStyle iconStyle;
    iconStyle.setIconPath( ":/icon/res/gur-project/gur-project-30.png" );
    //iconStyle.setSize(QSize(32,32));
    style->setIconStyle( iconStyle );
    GeoDataLabelStyle labelStyle;
    labelStyle.setFont(mapWidget->font());
    style.data()->setLabelStyle(labelStyle);

    GeoDataPlacemark *first = new GeoDataPlacemark( tr("FIRST\uf135"));
    first->setCoordinate(116, 39, 0, GeoDataCoordinates::Degree);
    //first->style().reset(style);
    first->setStyle(style);


    GeoDataDocument *document = new GeoDataDocument;
    document->append(first);


    //mapWidget->model()->treeModel()->addDocument(document);

    mapWidget->model()->addGeoDataFile("/Users/hehao/Downloads/Bucharest.kml");

    QFont f("FontAwesome");
    f.setPointSize(30);
    ui->commandLinkButton->setFont(f);
    ui->commandLinkButton->setText("\uf021");

    qDebug()<<QFontDatabase::applicationFontFamilies(0);

    QFontInfo fi(mapWidget->font());
    qDebug()<<fi.family()<<fi.styleName()<<fi.pointSize();

    first->setName("Secomd");
    iconStyle.setIconPath( ":/icon/res/gur-project/gur-project-10.png" );
    style->setIconStyle( iconStyle );
    GeoDataBalloonStyle ballStyle;
    ballStyle.setText("asasd\uf021");
    style->setBalloonStyle(ballStyle);
    first->setStyle(style);


    // Connect the map widget to the position label.
        QObject::connect( mapWidget, SIGNAL( mouseMoveGeoPosition( QString ) ),
                          this, SLOT( updateTitle( QString ) ) );

        QObject::connect( mapWidget, SIGNAL( zoomChanged(int) ),
                              this, SLOT( setMapZoomValue(int) ) );

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
    this->setWindowTitle(tr("zoom %1-%2").arg(m_mapZoomValue).arg(t));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->mapWidget->resize(event->size());
    mapWidget->resize(ui->mapWidget->size());
    //qDebug()<<event->size()<<"  "<<ui->mapWidget->size();
}
