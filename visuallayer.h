#ifndef VISUALLAYER_H
#define VISUALLAYER_H

#include <QObject>

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
    size_t addPlacemark(const QString& name);
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
signals:

public slots:
private:
    // Placemark
    void initPlacemarks(size_t size=100);
private:
    MarbleWidget* m_widget;
    int m_index;
    std::vector<GeoDataPlacemark*> m_placemarks;
    size_t m_placemarkPosition;

};

#endif // VISUALLAYER_H
