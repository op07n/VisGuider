#include "stdafx.h"
#include "visuallayer.h"

#include <iostream>

struct config_s {
    FILE* fp;
    json_t* root;
    json_error_t err;
};
typedef config_s* config_t;

static inline json_t* obj_get(json_t* root, const char* keys)
{
    json_t* o = root;
    char *key = NULL;
    size_t nsz = strlen(keys);
    char * nkeys = (char*)malloc(nsz+1);
    char* last=NULL;
    memcpy(nkeys, keys, nsz);
    nkeys[nsz] = '\0';

    for(key = strtok_r(nkeys, ".", &last);
        key;
        key = strtok_r(NULL, ".", &last))
    {
        /*printf("key:%s\n", key);*/
        o = json_object_get(o, key);
    }

    free(nkeys);
    return o;

}

template<class T>
T cfg_obj_get(json_t* root, const char* keys, const T& t)
{
    (void)root;
    (void)keys;
    return t;
}

template <>
QString cfg_obj_get(json_t* root, const char* keys, const QString& o)
{
    json_t* obj = obj_get(root, keys);
    if(!json_is_string(obj))
        return o;
    return QString(json_string_value(obj));
}

template <>
long cfg_obj_get(json_t* root, const char* keys, const long& o)
{
    json_t* obj = obj_get(root, keys);
    if(!json_is_integer(obj))
        return o;
    return json_integer_value(obj);;
}

template <>
double cfg_obj_get(json_t* root, const char* keys, const double& o)
{
    json_t* obj = obj_get(root, keys);
    if(!json_is_number(obj))
        return o;
    return json_number_value(obj);
}

config_t cfg_init_file(const char * name)
{
    config_s *cfg = NULL;
    if(name)
    {
        cfg = (config_s*)malloc(sizeof(config_s) );
        cfg->fp = fopen(name, "r");
        cfg->root = json_loadf(cfg->fp, 0, &cfg->err);
    }
    return cfg;

}

const char* cfg_get_string(config_s* pv, const char* keys)
{
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_string(obj))
        return NULL;
    return json_string_value(obj);
}

long cfg_get_integer(config_t cfg, const char *keys)
{
    config_s *pv = (config_s *)cfg;
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_integer(obj))
    {
        /*printf("obj is not integer %p\n", obj);*/
        return 0;
    }
    return json_integer_value(obj);
}

double cfg_get_real(config_t cfg, const char *keys)
{
    config_s *pv = (config_s *)cfg;
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_number(obj))
        return 0;
    return json_number_value(obj);
}

bool cfg_get_bool(config_t cfg, const char *keys)
{
    config_s *pv = (config_s *)cfg;
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_boolean(obj))
        return false;
    return true;
}

int cfg_get_list_size(config_t cfg, const char *keys)
{
    config_s *pv = (config_s *)cfg;
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_array(obj))
        return 0;
    return json_array_size(obj);
}

json_t* cfg_get_list_item(config_t cfg, const char *keys, int idx)
{
    config_s *pv = (config_s *)cfg;
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_array(obj))
        return 0;
    return json_array_get(obj, idx);
}

const char *cfg_get_list_string(config_t cfg, const char *keys, int idx)
{
    config_s *pv = (config_s *)cfg;
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_array(obj))
        return NULL;
    return json_string_value( json_array_get(obj, idx) );
}

long cfg_get_list_integer(config_t cfg, const char *keys, int idx)
{
    config_s *pv = (config_s *)cfg;
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_array(obj))
        return 0;
    return json_integer_value( json_array_get(obj, idx) );
}

double cfg_get_list_real(config_t cfg, const char *keys, int idx)
{
    config_s *pv = (config_s *)cfg;
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_array(obj))
        return 0.0;
    return json_real_value( json_array_get(obj, idx) );
}

bool cfg_get_list_bool(config_t cfg, const char *keys, int idx)
{
    config_s *pv = (config_s *)cfg;
    json_t* obj;
    obj = obj_get(pv->root, keys);
    if(!json_is_array(obj))
        return false;
    return json_is_boolean( json_array_get(obj, idx) );
}

void cfg_close(config_t cfg)
{
    config_s *pv = (config_s *)cfg;
    json_decref(pv->root);
    fclose(pv->fp);
}


VisualLayer::VisualLayer(MarbleWidget *parent, VisualLayer::RenderPosition RenderPosition)
    :m_widget(parent),m_index(RenderPosition)
    //,m_ballImage(300,500,QImage::Format_ARGB32)
{
    // Init Placemarker
    m_placemarkPosition=0;
    for(int i=0; i<4; ++i)
    {
        std::vector<GeoDataPlacemark*> vec;
        m_markEmpty.push_back(vec);
    }
    initPlacemarks();

    m_ball = new QWidget();
    m_ball->hide();
    m_ball->move(10,10);
    m_ball->setAttribute( Qt::WA_NoSystemBackground, true );

    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,true);
    m_ballWeb = new QWebView(m_ball);
    // allow loading of remote javascript
    QWebSettings* settings = m_ballWeb->settings();
    QWebSettings::globalSettings();
    settings->setFontFamily(QWebSettings::StandardFont, "FontAwesome");
    settings->setFontFamily(QWebSettings::FixedFont, "FontAwesome");
    settings->setAttribute(QWebSettings::PluginsEnabled, true);
    settings->setAttribute(QWebSettings::JavaEnabled, true);
    settings->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    settings->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    settings->setAttribute(QWebSettings::SpatialNavigationEnabled, true);
    settings->setAttribute(QWebSettings::LinksIncludedInFocusChain, true);
    settings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
    settings->setAttribute(QWebSettings::AutoLoadImages, true);

    m_ballWeb->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    m_ballWeb->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    QPalette palette = m_ballWeb->palette();
    palette.setBrush(QPalette::Base, Qt::transparent);
    m_ballWeb->setPalette(palette);
#ifndef MARBLE_NO_WEBKITWIDGETS
    m_ballWeb->page()->setPalette(palette);
    m_ballWeb->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
#endif
    m_ballWeb->setAttribute(Qt::WA_OpaquePaintEvent, false);
    m_ballWeb->move(0,0);

    int width=300;
    int height=600;

    m_ball->resize(width, height);
    m_ballWeb->resize(width, height);



    //qDebug()<<temp;
    connect(m_ballWeb->page()->mainFrame(),
            SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavaScriptObject()));
    //connect( m_ballWeb->page(), SIGNAL(repaintRequested(QRect)), m_ball, SLOT(update()) );

    m_ballWeb->load(QUrl("qrc:/res/temp.html"));
    //m_ball->show();

    QTimer *acttimer = new QTimer(this);
    acttimer->setInterval(100);
    QObject::connect(acttimer, &QTimer::timeout,
                     this,
                     &VisualLayer::actionEvent);
    acttimer->start();

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

    //    GeoDataCoordinates home(8.4, 48.0, 0.0, GeoDataCoordinates::Degree);
    //    QTime now = QTime::currentTime();

    //    painter->setRenderHint(QPainter::Antialiasing, true);

    //    // Large circle built by 60 small circles
    //    painter->setPen( QPen(QBrush(QColor::fromRgb(255,255,255,200)), 3.0, Qt::SolidLine, Qt::RoundCap ) );
    //    for (int i=0; i<60; ++i)
    //        painter->drawEllipse(approximate(home, M_PI * i / 30.0, 1.0), 5, 5);

    //    QFont font("FontAwesome");
    //    font.setPointSize(24);
    //    painter->setFont(font);

    //    painter->setPen( QColor( 255, 0, 0 ) );
    //    GeoDataLineString lns;
    //    // hour, minute, second hand
    //    lns.append(home);
    //    lns.append(approximate(home, M_PI * now.minute() / 30.0, 0.75));
    //    painter->drawPolyline(lns,"minute");
    //    lns.remove(1);
    //    lns.append(approximate(home, M_PI * now.hour() / 6.0, 0.5));
    //    painter->drawPolyline(lns,"hour");
    //    //painter->drawLine(home, approximate(home, M_PI * now.minute() / 30.0, 0.75));
    //    //painter->drawLine(home, approximate(home, M_PI * now.hour() / 6.0, 0.5));
    //    painter->setPen(QPen(QBrush(Qt::red), 4.0, Qt::SolidLine, Qt::RoundCap ));
    //    //painter->drawLine(home, approximate(home, M_PI * now.second() / 30.0, 1.0));
    //    lns.remove(1);
    //    lns.append(approximate(home, M_PI * now.second() / 30.0, 1.0));
    //    painter->drawPolyline(lns,"second");

    //    GeoDataCoordinates France( 2.2, 48.52, 0.0, GeoDataCoordinates::Degree );
    //    painter->setPen( QColor( 0, 0, 0 ) );
    //    painter->drawText( France, "France" );

    //    GeoDataCoordinates Canada( -77.02, 48.52, 0.0, GeoDataCoordinates::Degree );
    //    painter->setPen( QColor( 0, 0, 0 ) );
    //    painter->drawText( Canada, "Canada" );

    //    GeoDataCoordinates Beijing( 116.02, 38.52, 0.0, GeoDataCoordinates::Degree );

    //    //A line from France to Canada without tessellation

    //    GeoDataLineString shapeNoTessellation( NoTessellation );
    //    shapeNoTessellation << France << Canada<<Beijing;

    //    //painter->setPen( m_widget->oxygenSkyBlue4 );
    //    QPen p = painter->pen();
    //    p.setWidth(2);
    //    p.setColor(Qt::cyan);
    //    painter->setPen(p);
    //    painter->drawPolyline( shapeNoTessellation );

    //    //The same line, but with tessellation
    //    p.setColor(Qt::red);
    //    painter->setPen(p);
    //    GeoDataLineString shapeTessellate( Tessellate );
    //    shapeTessellate << France << Canada;

    //    //painter->setPen( oxygenBrickRed4 );
    //    painter->drawPolyline( shapeTessellate );

    //    //Now following the latitude circles
    //    p.setColor(Qt::yellow);
    //    painter->setPen(p);

    //    GeoDataLineString shapeLatitudeCircle( RespectLatitudeCircle | Tessellate );
    //    shapeLatitudeCircle << France << Canada;

    //    //painter->setPen( oxygenForestGreen4 );
    //    painter->drawPolyline( shapeLatitudeCircle );

    // Draw balloons
    //    int pos = 0;
    ////    int x = 100, y=50;
    ////    int width=200, height=50;
    ////    int border=10, ypadding = 10, xpadding=10;

    //    for(auto iter = m_balloons.begin(); iter != m_balloons.end(); ++iter)
    //    {


    //        QTime t = iter->start;
    //        t = t.addMSecs(iter->interval);
    //        if(now <= t)
    //        {
    //            // Display it
    ////            x = xpadding;
    ////            y = pos*(height+ypadding)+20;
    ////            iter->widget->resize(width, height);
    ////            iter->web->resize(width, height);
    ////            iter->widget->render(painter, QPoint(x,y),QRegion());

    //            ++pos;
    //        }
    //        //qDebug()<<now<<t;
    //    }
    QTime now = QTime::currentTime();
    int cnt = this->balloonSize();
    if(cnt > 0)
    {

        m_ball->render(painter, QPoint(10,20),QRegion());

        //QPainter pnt(&m_ballImage);
        //m_ball->render(&pnt, QPoint(xpadding,ypadding+20), QRegion());
        //emit balloon(m_ballImage);
    }

    cnt = placemarkSize();

    //placemark line show
    for(auto imark = m_marks.begin(); imark != m_marks.end(); ++imark)
    {
        const VisualPlacemark& mark = *imark;
        if(mark.drawLine && mark.points.size()>0 )
        {
            if(!m_placemarks.at(mark.id)->isVisible())
                continue;

            QPen p = painter->pen();
            p.setWidth(1);
            p.setColor(mark.color);
            painter->setPen(p);
            GeoDataLineString shapeNoTessellation( NoTessellation );
            for(size_t i=0; i<mark.points.size(); ++i)
                shapeNoTessellation<<mark.points.at(i);
            painter->drawPolyline(shapeNoTessellation);
        }
    }

    //Circle show
    QBrush brush = painter->brush();
    QPen pen = painter->pen();

    for(auto imark = m_marks.begin(); imark != m_marks.end(); ++imark)
    {
        const VisualPlacemark& mark = *imark;
        if(mark.drawCircle)
        {
            if(!m_placemarks.at(mark.id)->isVisible())
                continue;
            GeoDataPlacemark* gmark = m_placemarks.at(mark.id);

            //QPainter painter(this);
            //painter.drawEllipse(rectangle);
            GeoDataLinearRing ring;
            for(size_t i=0; i<360; ++i)
            {
                ring.append( this->approximate(gmark->coordinate(), i*M_PI/180.0, mark.circleR) );
            }
            painter->setBrush(mark.circleColor);//QColor(128,0,0,64));
            painter->setPen(mark.circleColor.lighter(150));//QColor(0,128,128,64));
            painter->drawPolygon( ring, Qt::WindingFill );
        }
    }
    painter->setBrush(brush);
    painter->setPen(pen);

    //Image show
    for(auto iact = m_actions.begin(); iact != m_actions.end(); ++iact)
    {
        VisualAction& act = *iact;
        if(act.type.compare("startimage") == 0)
        {
            QTime end = act.start.addSecs(act.begin);
            if(now >= act.start && now < end)
            {
                auto imark = std::find_if(m_marks.begin(),
                                          m_marks.end(),
                                          [=](const VisualPlacemark& mark){if(mark.index == act.markIndex) return true; return false;});
                if(imark == m_marks.end())
                    continue;

                GeoDataCoordinates pos = this->approximate( m_placemarks.at(imark->id)->coordinate(), act.angle, act.range);
                QImage img(act.sparam1);
                img = img.scaled(act.end, act.end);
                painter->drawImage(pos, img);
            }
        }
    }
    return true;
}

bool VisualLayer::eventFilter(QObject *obj, QEvent *e)
{
    MarbleWidget *widget = dynamic_cast<MarbleWidget*> ( obj );
    if ( !widget ) {
        return false;
    }
    // Adjust the current layer when '+' is pressed
    //    if (event->type() == QEvent::KeyPress)
    //    {
    ////        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    ////        if (keyEvent->key() == Qt::Key_Plus) {
    ////            ++m_index;
    ////            return true;
    ////        }
    //    }
    //qDebug()<<e->type();
    if ( e->type() == QEvent::MouseButtonDblClick
         || e->type() == QEvent::MouseMove
         || e->type() == QEvent::MouseButtonPress
         || e->type() == QEvent::MouseButtonRelease )
    {
        // Mouse events are forwarded to the underlying widget
        QMouseEvent *event = static_cast<QMouseEvent*> ( e );
        QPoint shiftedPos = event->pos() - QPoint(10,20);
        QWidget* child = m_ball->childAt(shiftedPos);
        if ( child ) {
            event->setLocalPos(shiftedPos);
            QApplication::sendEvent( child, event );
        }
    }

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

size_t VisualLayer::addPlacemark(const VisualPlacemark &mark)
{
    m_marks.push_back(mark);
    return 0;
}

size_t VisualLayer::addPlacemark(const QString &name, const PlacemarkDivision& div)
{
    // Auto increase placemark list
    if(div >= m_markEmpty.size())
        exit(0);

    initPlacemarks(m_placemarks.size());

    std::vector<GeoDataPlacemark*>& marks = m_markEmpty.at(div);

    GeoDataPlacemark* mark = marks[0];
    marks.erase(marks.begin());

    mark->setVisible(true);
    mark->setName(name);
    m_placemarks.push_back(mark);


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
    //m_placemarkPosition = 0;
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
    int pos = 0;
    double lon=1;
    double lat=1;

    for(auto imarks=m_markEmpty.begin(); imarks != m_markEmpty.end(); imarks++, ++pos)
    {
        switch(pos)
        {
        case PlacemarkDivision::NE:

            lon = 1;
            lat = 1;
            break;
        case PlacemarkDivision::SE:
            lon = 1;
            lat = -1;
            break;
        case PlacemarkDivision::NW:
            lon = -1;
            lat = 1;
            break;
        case PlacemarkDivision::SW:
            lon = -1;
            lat = -1;
            break;
        }

        std::vector<GeoDataPlacemark*>& marks = *imarks;
        if(!marks.empty())
            continue;

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
            mark->setCoordinate(lon,lat,0, GeoDataCoordinates::Degree);
            mark->setVisible(false);

            document->append(mark);

            marks.push_back(mark);
        }
        m_widget->model()->treeModel()->addDocument(document);

    }
}

int VisualLayer::placemarkSize()
{
    int cnt = 0;
    QTime now =QTime::currentTime();
    for(std::vector<VisualPlacemark>::iterator iter=m_marks.begin(); iter != m_marks.end(); ++iter)
    {
        VisualPlacemark& mark = *iter;

        if(mark.show == 2)
            continue;

        QTime t = mark.start;
        QTime t2 = t.addMSecs(mark.interval);
        if(now >= t && mark.show == 0)
        {
            mark.id = addPlacemark(mark.name, getDiv(mark.pos.longitude(), mark.pos.latitude()) );
            qDebug()<<"addMark "<<mark.name<<"index"<<mark.index<<"id"<<mark.id;
            setPlacemarkCoordinate(mark.id,
                                   mark.pos.longitude(GeoDataCoordinates::Degree),
                                   mark.pos.latitude(GeoDataCoordinates::Degree),
                                   GeoDataCoordinates::Degree);
            setPlacemarkColor(mark.id, mark.color);
            setPlacemarkFontSize(mark.id, mark.fontSize);
            if(mark.iconSize > 0 && !mark.icon.isEmpty() )
            {
                setPlacemarkIcon(mark.id, mark.icon);
                setPlacemarkIconSize(mark.id, mark.iconSize, mark.iconSize);
            }
            mark.show = 1;
            ++cnt;
        }

        if(now <= t2 || mark.interval == 0)
        {
            //qDebug()<<"Mark "<<mark.name;
            ++cnt;

        }
        else
        {
            mark.show = 2;
            qDebug()<<"reMoveMark "<<mark.name;
            removePlacemark(mark.id);
        }
    }
    return cnt;
}

VisualLayer::PlacemarkDivision VisualLayer::getDiv(double lon, double lat)
{
    if      (lon >0 && lat >0 )
        return NE;
    else if (lon >0 && lat <0 )
        return SE;
    else if (lon <0 && lat >0 )
        return NW;
    else
        return SW;
}

size_t VisualLayer::addBalloon(const VisualBalloon &ball)
{
    std::vector<VisualBalloon>::iterator iter = std::find_if(m_balloons.begin(), m_balloons.end(), [=](const VisualBalloon& iter)
    {
        if(iter.show == 2)
            return true;
        return false;
    });

    if(iter != m_balloons.end())
    {
        VisualBalloon& b = *iter;
        b.caption = ball.caption;
        b.name = ball.name;
        b.type = ball.type;
        b.interval = ball.interval;
        b.show = ball.show;
        b.start = ball.start;
        return iter - m_balloons.begin();
    }
    else
    {
        m_balloons.push_back(ball);
        return m_balloons.size()-1;
    }

}

size_t VisualLayer::addBalloon(const QString &name, int msecs, const QString &type, const QString& caption, const QTime& start)
{
    std::vector<VisualBalloon>::iterator iter = std::find_if(m_balloons.begin(), m_balloons.end(), [=](const VisualBalloon& iter)
    {
        if(iter.show == 2)
            return true;
        return false;
    });

    if(iter != m_balloons.end())
    {
        VisualBalloon &ball = *iter;
        ball.name = name;
        ball.start = start;
        ball.interval = msecs;
        ball.caption = caption;
        ball.type = type;
        ball.show = 0;
        return iter - m_balloons.begin();
    }
    else
    {
        VisualBalloon ball;
        ball.name = name;
        ball.start = start;
        ball.interval = msecs;
        ball.caption = caption;
        ball.type = type;
        ball.show = 0;
        m_balloons.push_back(ball);
        return m_balloons.size()-1;
    }




}

size_t VisualLayer::addBalloon(const QString &name, int msecs, const QString &type, const QString &caption, int start)
{
    QTime st = QTime::currentTime();
    st = st.addSecs(start);
    return addBalloon(name, msecs, type, caption, st);
}

void VisualLayer::clearBalloon()
{
    m_balloons.clear();
}

void VisualLayer::setBalloonInterval(size_t idx, int msecs)
{
    if(idx >= m_balloons.size())
        return;
    VisualBalloon& ball = m_balloons.at(idx);
    ball.interval = msecs;
}


int VisualLayer::balloonSize()
{
    int cnt = 0;
    QTime now =QTime::currentTime();
    for(std::vector<VisualBalloon>::iterator iter=m_balloons.begin(); iter != m_balloons.end(); ++iter)
    {
        VisualBalloon& ball = *iter;
        QTime t = ball.start;
        QTime t2 = t.addMSecs(iter->interval);
        if(now >= t && ball.show == 0)
        {
            m_ballWeb->page()->mainFrame()->evaluateJavaScript(QString("pushMessage('%1','%2','%3', %4);")
                                                               .arg(ball.caption)
                                                               .arg(ball.name)
                                                               .arg(ball.type)
                                                               .arg(ball.interval));
            qDebug()<<"add "<<ball.name;
            ball.show = 1;
            ++cnt;
        }

        if(now <= t2)
            ++cnt;
        else
        {
            ball.show = 2;
        }
    }
    return cnt;
}

void VisualLayer::addJavaScriptObject()
{
    qDebug()<<"js cleaned"<<m_ballWeb->url();
}

void VisualLayer::cleanVisualUnits()
{
    m_balloons.clear();
    m_marks.clear();
    m_actions.clear();
    m_ballWeb->page()->mainFrame()->evaluateJavaScript(QString("closeAllMessage();"));
    for(auto imark = m_placemarks.begin(); imark != m_placemarks.end(); ++imark)
    {
        GeoDataPlacemark* mark = *imark;
        mark->setVisible(false);
    }

    //Reload page
    m_ballWeb->load(QUrl("qrc:/res/temp.html"));

}

void VisualLayer::loadConfig(const QString &file, const double &relstart, const double &relindex)
{
    config_t cfg;
    int i;
    int cnt;
    json_t* obj;
    double posttime;
    double dstart=relstart*1000;
    double dindex=relindex;

    cfg  = cfg_init_file(file.toUtf8().data());
    posttime = cfg_get_real(cfg, "posttime") * 1000;
    qDebug()<<file<<"posttime"<<posttime<<dstart;

    // Placemark
    cnt = cfg_get_list_size(cfg, "placemark");
    qDebug()<<"placemark"<<cnt;
    for(i=0; i<cnt; ++i)
    {
        obj = cfg_get_list_item(cfg, "placemark", i);
        if(!obj)
            continue;

        VisualPlacemark mark;

        mark.name= cfg_obj_get<QString>(obj, "name",        "");
        mark.fontSize=cfg_obj_get<long>(obj, "fontsize",    24);
        mark.interval=cfg_obj_get<long>(obj, "interval",    0);

        mark.startTime = cfg_obj_get<double>(obj, "start",    0)*1000;


        // starttime must >= posttime
        if(posttime > 0 && posttime > mark.startTime)
            continue;
        mark.startTime -= posttime;
        mark.startTime += dstart;




        mark.color = cfg_obj_get<QString>(obj, "color",     "#000000");
        mark.icon = cfg_obj_get<QString>(obj, "icon",       "");
        mark.iconSize = cfg_obj_get<long>(obj, "iconsize",  0);
        mark.index = cfg_obj_get<long>(obj, "index",        0);
        mark.index+= dindex;

        double lon = cfg_obj_get<double>(obj, "lon",        0.0);
        double lat = cfg_obj_get<double>(obj, "lat",        0.0);

        int markIndex = cfg_obj_get<long>(obj, "relative",  0);

        double angle=cfg_obj_get<double>(obj,"angle",       0.0);
        double dist=cfg_obj_get<double>(obj,"distance",     0.0);

        qDebug()<<"mark "<<mark.index<<mark.name<<mark.startTime;



        mark.interval*=1000;
        mark.show = 0;
        mark.start = QTime::currentTime().addMSecs(mark.startTime);
        if(markIndex != 0)
        {
            markIndex += dindex;
            auto imark = std::find_if(m_marks.begin(), m_marks.end(),
                                      [=](const VisualPlacemark& mark)
            {
                if(mark.index == markIndex)
                    return true;
                return false;
            });
            if(imark != m_marks.end())
            {
                mark.pos = this->approximate(imark->pos, angle*180.0/M_PI, dist);
            }
        }
        else
            mark.pos = GeoDataCoordinates(lon, lat, 0,GeoDataCoordinates::Degree);

        addPlacemark(mark);
    }

    // Balloon
    cnt = cfg_get_list_size(cfg, "balloon");
    for(i=0; i<cnt; ++i)
    {
        obj = cfg_get_list_item(cfg, "balloon", i);
        if(!obj)
            continue;

        VisualBalloon ball;
        ball.caption = cfg_obj_get<QString>(obj, "caption", "Info");
        ball.type = cfg_obj_get<QString>(obj, "type", "info");
        ball.name =cfg_obj_get<QString>(obj, "content", "content");
        ball.interval = cfg_obj_get<long>(obj, "interval", 5);
        ball.interval *= 1000;
        ball.show = 0;
        long start = cfg_obj_get<double>(obj, "start", 0)*1000;

        // starttime must >= posttime
        if(posttime > 0 && posttime > start)
            continue;
        start -= posttime;
        start += dstart;

        ball.start = QTime::currentTime().addMSecs(start);
        addBalloon(ball);

    }

    // Action
    double lastActionTime = 0.0;
    cnt=cfg_get_list_size(cfg, "action");
    for(i=0; i<cnt; ++i)
    {
        obj = cfg_get_list_item(cfg, "action", i);
        if(!obj)
            continue;

        VisualAction act;
        act.show = 0;
        act.type = cfg_obj_get<QString>(obj, "type", "");
        qDebug()<<"add action "<<act.type;
        act.startTime = cfg_obj_get<double>(obj, "time", 0)*1000;

        // starttime must >= posttime
        if(posttime > 0 && posttime > act.startTime)
        {
            continue;
        }
        act.startTime -= posttime;
        act.startTime += dstart;

        if(act.startTime > lastActionTime)
            lastActionTime = act.startTime;


        act.markIndex = cfg_obj_get<long>(obj, "mark", 0);
        act.markIndex += dindex;

        act.begin = cfg_obj_get<long>(obj, "begin", 0);
        act.end = cfg_obj_get<long>(obj, "end", 0);
        if(act.type.compare("clearmark", Qt::CaseInsensitive) == 0)
        {
            act.begin += dindex;
            act.end += dindex;
        }
        act.angle = cfg_obj_get<double>(obj, "angle", 0.0)*M_PI/180.0;
        act.drawLine = cfg_obj_get<long>(obj, "line", 0);

        act.range = cfg_obj_get<double>(obj, "range", 100000.0);
        act.sparam1 = cfg_obj_get<QString>(obj, "sparam", "");

        double lon = cfg_obj_get<double>(obj, "lon",        0.0);
        double lat = cfg_obj_get<double>(obj, "lat",        0.0);

        act.start = QTime::currentTime().addMSecs(act.startTime);
        act.pos = GeoDataCoordinates(lon, lat, 0,GeoDataCoordinates::Degree);



        m_actions.push_back(act);
    }

    //Import
    cnt=cfg_get_list_size(cfg, "import");
    for(i=0; i<cnt; ++i)
    {
        obj = cfg_get_list_item(cfg, "import", i);
        if(!obj)
            continue;
        double start;
        start = cfg_obj_get<double>(obj, "relstart",0.0);
        QString name = cfg_obj_get<QString>(obj, "file", "");
        dindex = cfg_obj_get<double>(obj, "relindex", 0.0);
        //dstart -=posttime/1000.0;
        dstart =dstart + start + lastActionTime/1000.0;
        qDebug()<<"dstart"<<dstart<<"pisttime"<<posttime/1000.0<<lastActionTime/1000.0;
        loadConfig(name, dstart, dindex);
    }

}

void VisualLayer::actionEvent()
{
    QTime now = QTime::currentTime();
    for(auto iter = m_actions.begin(); iter != m_actions.end(); ++iter)
    {
        VisualAction& act = *iter;
        if(act.show == 2)
            continue;

        QTime t = act.start;
        //QTime t2 = t.addMSecs(act.interval);
        if(now >= t && act.show == 0)
        {

            if(act.type.compare("goto", Qt::CaseInsensitive) == 0)
            {
                qDebug()<<"Action "<<act.type <<"index"<<act.markIndex;
                GeoDataLookAt lookat;
                auto imark = std::find_if(m_marks.begin(), m_marks.end(),
                                          [=](const VisualPlacemark& mark)
                {

                    if(mark.index == act.markIndex)
                    {
                        qDebug()<<"find act"<<act.markIndex;
                        return true;
                    }
                    return false;
                });
                GeoDataCoordinates cd = act.pos;
                if(imark != m_marks.end())
                    cd = imark->pos;

                lookat.setCoordinates( cd );
                lookat.setRange(act.range);
                m_widget->flyTo(lookat);
            }
            else if(act.type.compare("clearBalloon", Qt::CaseInsensitive) == 0)
            {
                m_ballWeb->page()->mainFrame()->evaluateJavaScript(QString("closeAllMessage();"));
                //Reload page
                m_ballWeb->load(QUrl("qrc:/res/temp.html"));
            }
            else if(act.type.compare("clearMark", Qt::CaseInsensitive) == 0)
            {
                // Hide geo placemark
                std::for_each(m_marks.begin(), m_marks.end(), [=](VisualPlacemark& mark)
                {
                    if(mark.index >= act.begin && mark.index < act.end)
                    {
                        //m_placemarks.at(mark.id)->setName("");
                        m_placemarks.at(mark.id)->setVisible(false);
                    }

                });

                // Erase visual placemark
                for(;;)
                {
                    auto imark = std::find_if(m_marks.begin(), m_marks.end(),[=](const VisualPlacemark& mark) {
                        if(mark.index >= act.begin && mark.index < act.end)
                            return true;
                        return false;
                    });
                    if(imark != m_marks.end())
                        m_marks.erase(imark);
                    else
                        break;
                }

            }
            else if(act.type.compare("startmove", Qt::CaseInsensitive) == 0)
            {
                auto imark = std::find_if(m_marks.begin(),
                                          m_marks.end(),
                                          [=](const VisualPlacemark& mark)
                {
                    if(mark.index == act.markIndex)
                        return true;
                    return false;
                });
                if(imark != m_marks.end())
                {
                    imark->speed =act.range / 100000.0; // m/s to km/s
                    imark->angle = act.angle;
                    imark->moving = true;
                    imark->drawLine = act.drawLine;

//                    GeoDataPlacemark* mark = m_placemarks.at(imark->id);
//                    QSharedPointer<GeoDataStyle> style;
//                    style = QSharedPointer<GeoDataStyle>(new GeoDataStyle(*mark->style()));

//                    qDebug()<<"startmove index"<<imark->index<<"id"<<imark->id<<mark;
//                    if(!imark->icon.isEmpty())
//                    {
//                    GeoDataIconStyle iconStyle;//= style->iconStyle();
//                    QPixmap image(imark->icon);//iconStyle.icon());
//                    QMatrix matrix;
//                    matrix.rotate(act.angle*180/M_PI);
//                    //matrix.scale(1, 1);
//                    image = image.transformed(matrix);
//                    image = image.scaled(imark->iconSize, imark->iconSize);
//                    //iconStyle.setIconPath(":/icon/res/gur-project/gur-project-16.png");
//                    iconStyle.setIconPath("");
//                    iconStyle.setIcon(image.toImage());
//                    //qDebug()<<"icon "<<image.width()<<image.height()<<" scaleto "<<width<< height;
//                    //iconStyle.setHeading(imark->angle);

//                    style->setIconStyle(iconStyle);
//                    mark->setStyle( style );
//                    m_widget->model()->treeModel()->updateFeature(mark);
//                    }

                }
            }
            else if(act.type.compare("stopmove", Qt::CaseInsensitive) == 0)
            {
                auto imark = std::find_if(m_marks.begin(),
                                          m_marks.end(),
                                          [=](const VisualPlacemark& mark)
                {
                    if(mark.index == act.markIndex)
                        return true;
                    return false;
                });
                if(imark != m_marks.end())
                {
                    qDebug()<<"stopmove index"<<imark->index<<"id"<<imark->id;
                    imark->moving = false;
                }
            }
            else if(act.type.compare("startCircle", Qt::CaseInsensitive) == 0)
            {
                auto imark = std::find_if(m_marks.begin(),
                                          m_marks.end(),
                                          [=](const VisualPlacemark& mark)
                {
                    if(mark.index == act.markIndex)
                        return true;
                    return false;
                });
                if(imark != m_marks.end())
                {
                    qDebug()<<"circle index"<<imark->index<<"id"<<imark->id;
                    imark->drawCircle = true;
                    imark->circleR = act.range;
                    imark->circleColor = QColor(act.sparam1);
                }
            }
            else if(act.type.compare("stopCircle", Qt::CaseInsensitive) == 0)
            {
                auto imark = std::find_if(m_marks.begin(),
                                          m_marks.end(),
                                          [=](const VisualPlacemark& mark)
                {
                    if(mark.index == act.markIndex)
                        return true;
                    return false;
                });
                if(imark != m_marks.end())
                {
                    qDebug()<<"stopcircle index"<<imark->index<<"id"<<imark->id;
                    imark->drawCircle = false;
                }
            }

            act.show = 2;


        }



    }

    // Move
    for(auto imark = m_marks.begin(); imark != m_marks.end(); ++imark)
    {
        VisualPlacemark& mark = *imark;
        if(mark.moving)
        {
            mark.points.push_back(mark.pos);
            mark.pos = this->approximate(mark.pos, mark.angle, mark.speed);
            GeoDataPlacemark* gmark = m_placemarks.at(mark.id);
            gmark->setCoordinate(mark.pos);
        }
    }
}

