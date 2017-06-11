#ifndef VISUALLAYER_H
#define VISUALLAYER_H

#include <QObject>

struct VisualBalloon {
    QString name;
    QColor color;
    int interval;
    QTime start;
};


class VisualLayer : public QObject, public LayerInterface
{
    Q_OBJECT
public:
    explicit VisualLayer(MarbleWidget *parent = 0, int index=0);

    // Implemented from LayerInterface
    virtual QStringList renderPosition() const;

    // Implemented from LayerInterface
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    // Overriding QObject
    virtual bool eventFilter(QObject *obj, QEvent *event);

    GeoDataCoordinates approximate(const GeoDataCoordinates &base, qreal angle, qreal dist) const;

    /// Placemark
    size_t addPlacemark(const QString& name="");
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

    /// Balloon
    size_t addBalloon(const QString& name="", int msecs=5000, const QColor& color=QColor(Qt::red));
    void clearBalloon();
    void setBalloonInterval(size_t idx, int msecs);
    void setBalloonColor(size_t idx, const QColor& color);
signals:

public slots:


private:
    MarbleWidget* m_widget;
    int m_index;
    std::vector<GeoDataPlacemark*> m_placemarks;
    size_t m_placemarkPosition;
    std::vector<VisualBalloon> m_balloons;


};

#endif // VISUALLAYER_H
