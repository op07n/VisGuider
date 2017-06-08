#include "stdafx.h"
#include "visuallayer.h"



VisualLayer::VisualLayer(MarbleWidget *parent, int index)
    :m_widget(parent),m_index(index)
{

    // Init Placemarker
    m_placemarkPosition=0;
    initPlacemarks();


}

QStringList VisualLayer::renderPosition() const
{
    // Here will paint in exactly one of the following layers.
    // The current one can be changed by pressing the '+' key
    QStringList layers = QStringList() << "SURFACE" << "HOVERS_ABOVE_SURFACE";
    layers << "ORBIT" << "USER_TOOLS" << "STARS";

    int index = m_index % layers.size();
    return QStringList() << layers.at(index);

}

bool VisualLayer::render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer)
{
    // Have window title reflect the current paint layer

    GeoDataCoordinates home(8.4, 48.0, 0.0, GeoDataCoordinates::Degree);
    QTime now = QTime::currentTime();

    painter->setRenderHint(QPainter::Antialiasing, true);

    // Large circle built by 60 small circles
    painter->setPen( QPen(QBrush(QColor::fromRgb(255,255,255,200)), 3.0, Qt::SolidLine, Qt::RoundCap ) );
    for (int i=0; i<60; ++i)
        painter->drawEllipse(approximate(home, M_PI * i / 30.0, 1.0), 5, 5);

    QFont font("FontAwesome");
    font.setPointSize(24);
    painter->setFont(font);

    painter->setPen( QColor( 255, 0, 0 ) );
    GeoDataLineString lns;
    // hour, minute, second hand
    lns.append(home);
    lns.append(approximate(home, M_PI * now.minute() / 30.0, 0.75));
    painter->drawPolyline(lns,"minute");
    lns.remove(1);
    lns.append(approximate(home, M_PI * now.hour() / 6.0, 0.5));
    painter->drawPolyline(lns,"hour");
    //painter->drawLine(home, approximate(home, M_PI * now.minute() / 30.0, 0.75));
    //painter->drawLine(home, approximate(home, M_PI * now.hour() / 6.0, 0.5));
    painter->setPen(QPen(QBrush(Qt::red), 4.0, Qt::SolidLine, Qt::RoundCap ));
    //painter->drawLine(home, approximate(home, M_PI * now.second() / 30.0, 1.0));
    lns.remove(1);
    lns.append(approximate(home, M_PI * now.second() / 30.0, 1.0));
    painter->drawPolyline(lns,"second");

    GeoDataCoordinates France( 2.2, 48.52, 0.0, GeoDataCoordinates::Degree );
        painter->setPen( QColor( 0, 0, 0 ) );
        painter->drawText( France, "France" );

        GeoDataCoordinates Canada( -77.02, 48.52, 0.0, GeoDataCoordinates::Degree );
        painter->setPen( QColor( 0, 0, 0 ) );
        painter->drawText( Canada, "Canada" );

        //A line from France to Canada without tessellation

        GeoDataLineString shapeNoTessellation( NoTessellation );
        shapeNoTessellation << France << Canada;

        //painter->setPen( m_widget->oxygenSkyBlue4 );
        QPen p = painter->pen();
        p.setWidth(2);
        p.setColor(Qt::cyan);
        painter->setPen(p);
        painter->drawPolyline( shapeNoTessellation );

        //The same line, but with tessellation
        p.setColor(Qt::red);
        painter->setPen(p);
        GeoDataLineString shapeTessellate( Tessellate );
        shapeTessellate << France << Canada;

        //painter->setPen( oxygenBrickRed4 );
        painter->drawPolyline( shapeTessellate );

        //Now following the latitude circles
        p.setColor(Qt::yellow);
        painter->setPen(p);

        GeoDataLineString shapeLatitudeCircle( RespectLatitudeCircle | Tessellate );
        shapeLatitudeCircle << France << Canada;

        //painter->setPen( oxygenForestGreen4 );
        painter->drawPolyline( shapeLatitudeCircle );


    return true;
}

bool VisualLayer::eventFilter(QObject *obj, QEvent *event)
{
    // Adjust the current layer when '+' is pressed
    //    if (event->type() == QEvent::KeyPress)
    //    {
    ////        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    ////        if (keyEvent->key() == Qt::Key_Plus) {
    ////            ++m_index;
    ////            return true;
    ////        }
    //    }

    return false;
}

GeoDataCoordinates VisualLayer::approximate(const GeoDataCoordinates &base, qreal angle, qreal dist) const
{
    // This is just a rough estimation that ignores projections.
    // It only works for short distances. Don't use in real code.
    GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
    return GeoDataCoordinates ( base.longitude(deg) + 1.5 * dist * sin(angle),
                                base.latitude(deg) + dist * cos(angle), 0.0, deg);
}

size_t VisualLayer::addPlacemark(const QString &name)
{
    // Auto increase placemark list
    if(m_placemarkPosition >= m_placemarks.size() )
    {
        initPlacemarks(m_placemarks.size());
    }

    GeoDataPlacemark* mark = m_placemarks.at(m_placemarkPosition);
    mark->setName(name);
    mark->setVisible(true);


    return m_placemarkPosition++;

}

void VisualLayer::removePlacemark(size_t idx)
{
    if(idx >= m_placemarks.size())
        return;

    GeoDataPlacemark* mark = m_placemarks.at(idx);
    mark->setVisible(false);
}

void VisualLayer::clearPlacemark()
{
    size_t i=0;
    for(i=0; i<m_placemarks.size(); ++i)
    {
        m_placemarks.at(i)->setVisible(false);
    }
    m_placemarkPosition = 0;
}

void VisualLayer::setPlacemarkName(size_t idx, const QString &name)
{
    if(idx >= m_placemarks.size())
        return;

    GeoDataPlacemark* mark = m_placemarks.at(idx);
    mark->setName(name);
    QSharedPointer<GeoDataStyle> style;

    style = QSharedPointer<GeoDataStyle>(new GeoDataStyle);//(*mark->customStyle()));
    GeoDataLabelStyle& labelStyle = style->labelStyle();
    labelStyle.setFont(m_widget->font());

    mark->setStyle(style);
}

void VisualLayer::setPlacemarkIcon(size_t idx, const QString &path, int width, int height)
{
    if(idx >= m_placemarks.size())
        return;

    GeoDataPlacemark* mark = m_placemarks.at(idx);
    QSharedPointer<GeoDataStyle> style;
    style = QSharedPointer<GeoDataStyle>(new GeoDataStyle(*mark->customStyle()));

    GeoDataIconStyle &iconStyle = style->iconStyle();

    iconStyle.setIconPath(path);
    QImage image = iconStyle.icon();
    //qDebug()<<"icon "<<image.width()<<image.height()<<" scaleto "<<width<< height;
    iconStyle.setIcon(image.scaled(width,height));

    mark->setStyle( style );

}

void VisualLayer::setPlacemarkIconSize(size_t idx, int width, int height)
{
    if(idx >= m_placemarks.size())
        return;

    GeoDataPlacemark* mark = m_placemarks.at(idx);
    QSharedPointer<GeoDataStyle> style;
    style = QSharedPointer<GeoDataStyle>(new GeoDataStyle(*mark->customStyle()));
    GeoDataIconStyle& iconStyle = style->iconStyle();
    QImage image=iconStyle.icon();

    iconStyle.setIcon(image.scaled(width,height));
    mark->setStyle(style);

    //qDebug()<<idx<<(style.data());

}

void VisualLayer::setPlacemarkFontSize(size_t idx, int pt)
{
    if(idx >= m_placemarks.size())
        return;

    GeoDataPlacemark* mark = m_placemarks.at(idx);
    QSharedPointer<GeoDataStyle> style=
            QSharedPointer<GeoDataStyle>(new GeoDataStyle(*mark->customStyle()));
    GeoDataLabelStyle& labelStyle = style->labelStyle();
    QFont font = labelStyle.font();
    font.setPointSize(pt);
    labelStyle.setFont(font);

    mark->setStyle(style);
}

void VisualLayer::setPlacemarkColor(size_t idx, const QColor &color)
{
    if(idx >= m_placemarks.size())
        return;

    GeoDataPlacemark* mark = m_placemarks.at(idx);
    QSharedPointer<GeoDataStyle> style=
            QSharedPointer<GeoDataStyle>(new GeoDataStyle(*mark->customStyle()));
    GeoDataLabelStyle& labelStyle = style->labelStyle();
    labelStyle.setColor(color);

    mark->setStyle(style);
}

void VisualLayer::setPlacemarkCoordinate(size_t idx, qreal longitude, qreal latitude, qreal altitude, GeoDataCoordinates::Unit _unit)
{
    if(idx >= m_placemarks.size())
        return;

    GeoDataPlacemark* mark = m_placemarks.at(idx);
    mark->setCoordinate(longitude, latitude, altitude, _unit);
    //qDebug()<<longitude<< latitude<< altitude<< _unit;
}

void VisualLayer::getPlacemarkCoordinate(size_t idx, qreal &longitude, qreal &latitude, qreal &altitude)
{
    if(idx >= m_placemarks.size())
        return;

    GeoDataPlacemark* mark = m_placemarks.at(idx);
    qreal lon, lat;
    mark->coordinate(lon, lat, altitude);
    GeoDataCoordinates c(lon,  lat);
    c.geoCoordinates(longitude, latitude, GeoDataCoordinates::Degree);

}

void VisualLayer::initPlacemarks(size_t size)
{
    size_t i;

    if(size == 0 || size > 1024)
        size = 100;

    //Document
    GeoDataDocument *document = new GeoDataDocument;
    for(i=0; i<size; ++i)
    {
        GeoDataPlacemark *mark = new GeoDataPlacemark("");

        QSharedPointer<GeoDataStyle> style=
                QSharedPointer<GeoDataStyle>(new GeoDataStyle);
        GeoDataLabelStyle& labelStyle = style->labelStyle();
        labelStyle.setFont(m_widget->font());
        mark->setStyle(style);

        mark->setVisualCategory(GeoDataPlacemark::Nation);
        mark->setCoordinate(1,1,0, GeoDataCoordinates::Degree);
        mark->setVisible(false);

        document->append(mark);

        m_placemarks.push_back(mark);
    }
    m_widget->model()->treeModel()->addDocument(document);
}
