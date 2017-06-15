#ifndef VISUALLAYER_H
#define VISUALLAYER_H

#include <QObject>

struct VisualBalloon {
    QString caption;
    QString type;
    QString name;
    int interval;
    QTime start;
    int show;

};

struct VisualPlacemark {
    QString color;
    QString name;
    QString icon;
    int iconSize;
    int fontSize;
    int interval; // 0 -forever
    int startTime;
    int show;
    int index;
    QTime start;
    GeoDataCoordinates pos;
    std::vector<GeoDataCoordinates> points;
    size_t id;

    double speed;
    double angle;
    bool moving;
    bool drawLine;

    bool drawCircle;
    double circleR;
    QColor circleColor;

    VisualPlacemark()
        :iconSize(0), fontSize(14),interval(0),show(0),start(QTime::currentTime()),id(0)
        ,moving(false)
        ,drawCircle(false)
    {}
};

struct VisualAction {
    int markIndex;
    QString type;
    int startTime;
    int interval;
    QTime start;
    GeoDataCoordinates pos;
    int show;
    double range;
    double angle;
    int begin;
    int end;
    int drawLine;
    QString sparam1;

};


class VisualLayer : public QObject, public LayerInterface
{
    Q_OBJECT
public:
    enum PlacemarkDivision {
        NE = 0, // 1,  1
        SE,     // 1, -1
        NW,     //-1,  1
        SW      //-1, -1

    };
    enum RenderPosition {
        SURFACE,
        HOVERS_ABOVE_SURFACE,
        ORBIT,
        USER_TOOLS,
        STARS
    };
    explicit VisualLayer(MarbleWidget *parent = 0, RenderPosition RenderPosition=SURFACE);

    // Implemented from LayerInterface
    virtual QStringList renderPosition() const;

    // Implemented from LayerInterface
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    // Overriding QObject
    virtual bool eventFilter(QObject *obj, QEvent *event);

    GeoDataCoordinates approximate(const GeoDataCoordinates &base, qreal angle, qreal dist) const;

    /// Placemark
    size_t addPlacemark(const VisualPlacemark& mark);
    size_t addPlacemark(const QString& name="", const PlacemarkDivision& div= PlacemarkDivision::NE);
    void removePlacemark(size_t idx);
    void clearPlacemark();
    void setPlacemarkName(size_t idx, const QString& name);
    void setPlacemarkIcon(size_t idx, const QString& path, int width=32, int height=32);
    void setPlacemarkIconSize(size_t idx, int width=32, int height=32);
    void setPlacemarkFontSize(size_t idx, int pt);
    void setPlacemarkColor(size_t idx, const QColor& color);
    void setPlacemarkCoordinate(size_t idx, qreal longitude, qreal latitude, qreal altitude = 0,
                                GeoDataCoordinates::Unit _unit = GeoDataCoordinates::Degree);
    void getPlacemarkCoordinate(size_t idx, qreal& longitude,  qreal& latitude, qreal& altitude);
    void initPlacemarks(size_t size=100);
    int placemarkSize();
    PlacemarkDivision getDiv(double lon, double lat);

    /// Balloon
    size_t addBalloon(const VisualBalloon& ball);
    size_t addBalloon(const QString& name="", int msecs=5000, const QString &type="default", const QString &caption="Info", const QTime &start=QTime::currentTime());
    size_t addBalloon(const QString& name="", int msecs=5000, const QString &type="default", const QString &caption="Info", int start=0);
    void clearBalloon();
    void setBalloonInterval(size_t idx, int msecs);
    int balloonSize();
signals:
    void balloon(const QImage&);

public slots:

    void addJavaScriptObject();

    void cleanVisualUnits();
    void loadConfig(const QString& file, const double&relstart=0, const double&relindex=0);

    void actionEvent();

private:
    MarbleWidget* m_widget;
    RenderPosition m_index;
    std::vector<GeoDataPlacemark*> m_placemarks;
    std::vector<VisualPlacemark> m_marks;
    size_t m_placemarkPosition;
    std::vector<VisualBalloon> m_balloons;
    QWidget *m_ball;
    QWebView* m_ballWeb;
    //QImage m_ballImage;
    std::vector<VisualAction> m_actions;
    std::vector<std::vector<GeoDataPlacemark*> > m_markEmpty;


};

#endif // VISUALLAYER_H
