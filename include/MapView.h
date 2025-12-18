#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMap>
#include <QPoint>
#include <QPixmap>

struct TileCoord {
    int z, x, y;
    
    bool operator<(const TileCoord &other) const {
        if (z != other.z) return z < other.z;
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

struct EarthquakeMarker {
    int id;
    double lat, lon, magnitude;
    QString time;
    QGraphicsEllipseItem *item;
};

class MapView : public QWidget {
    Q_OBJECT

public:
    explicit MapView(QWidget *parent = nullptr);
    ~MapView();

    void addEarthquakeMarker(int id, double lat, double lon, double magnitude, const QString &time);
    void clearMarkers();
    void setCenter(double lat, double lon);
    void setZoom(int zoom);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void setupUI();
    void loadTiles();
    void updateVisibleTiles();
    QPixmap loadTile(int z, int x, int y);
    QPointF latLonToPixel(double lat, double lon, int zoom);
    void updateMarkerPositions();
    QColor getMagnitudeColor(double magnitude);
    
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    
    int m_currentZoom;
    double m_centerLat, m_centerLon;
    QMap<QString, QGraphicsPixmapItem*> m_tileItems;
    QList<EarthquakeMarker*> m_markers;
    
    // Pan/drag
    bool m_isPanning;
    QPoint m_lastPanPoint;
    QPointF m_viewCenter;

signals:
    void markerClicked(int id, double lat, double lon, double magnitude);
};

#endif // MAPVIEW_H
