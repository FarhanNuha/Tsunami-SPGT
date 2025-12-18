#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>

class MapView : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapView(QWidget *parent = nullptr);
    
    void setMapDirectory(const QString &dirPath);
    void setZoomLevel(int level);
    void centerOnCoordinate(double lat, double lon);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void loadTiles();
    void updateVisibleTiles();
    QString getTilePath(int zoom, int x, int y);
    
    QGraphicsScene *m_scene;
    QString m_mapDirectory;
    int m_currentZoom;
    int m_maxZoom;
    double m_scale;
    
    bool m_isPanning;
    QPoint m_lastPanPoint;
    
    QMap<QString, QGraphicsPixmapItem*> m_tileCache;
};

#endif // MAPVIEW_H
