#include "MapView.h"
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QtMath>
#include <QDebug>

MapView::MapView(QWidget *parent) 
    : QWidget(parent), 
      m_currentZoom(2),
      m_centerLat(-2.5),
      m_centerLon(118.0),
      m_isPanning(false) {
    setupUI();
    loadTiles();
}

MapView::~MapView() {
    qDeleteAll(m_markers);
}

void MapView::setupUI() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_scene = new QGraphicsScene(this);
    m_scene->setBackgroundBrush(QBrush(QColor(170, 211, 223))); // Ocean color

    m_view = new QGraphicsView(m_scene);
    m_view->setDragMode(QGraphicsView::NoDrag);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform);
    m_view->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    
    layout->addWidget(m_view);
}

void MapView::loadTiles() {
    // Tile naming: world{z}{x}{y}.png
    // z: zoom level (0-3)
    // x: tile X coordinate (0-3 depending on zoom)
    // y: tile Y coordinate (0-3 depending on zoom)
    
    // Untuk zoom 2, kita load tiles 4x4 grid
    int tilesPerSide = qPow(2, m_currentZoom);
    int tileSize = 256;
    
    m_scene->clear();
    m_tileItems.clear();
    
    for (int x = 0; x < tilesPerSide; x++) {
        for (int y = 0; y < tilesPerSide; y++) {
            QPixmap tile = loadTile(m_currentZoom, x, y);
            if (!tile.isNull()) {
                QGraphicsPixmapItem *item = m_scene->addPixmap(tile);
                item->setPos(x * tileSize, y * tileSize);
                item->setZValue(-1); // Tiles di belakang
                
                QString key = QString("%1_%2_%3").arg(m_currentZoom).arg(x).arg(y);
                m_tileItems[key] = item;
            }
        }
    }
    
    // Set scene size
    int sceneSize = tilesPerSide * tileSize;
    m_scene->setSceneRect(0, 0, sceneSize, sceneSize);
    
    // Center view pada Indonesia
    QPointF center = latLonToPixel(m_centerLat, m_centerLon, m_currentZoom);
    m_view->centerOn(center);
    
    updateMarkerPositions();
}

QPixmap MapView::loadTile(int z, int x, int y) {
    // Format: world{z}{x}{y}.png
    // Contoh: world000.png, world001.png, world010.png, dll
    QString filename = QString(":/maps/world%1%2%3.png")
                           .arg(z)
                           .arg(x)
                           .arg(y);
    
    QPixmap tile(filename);
    
    if (tile.isNull()) {
        // Fallback: buat tile placeholder
        tile = QPixmap(256, 256);
        tile.fill(QColor(200, 200, 200, 100));
    }
    
    return tile;
}

QPointF MapView::latLonToPixel(double lat, double lon, int zoom) {
    // Web Mercator projection
    // Convert lat/lon to pixel coordinates at given zoom level
    
    int tileSize = 256;
    int mapSize = tileSize * qPow(2, zoom);
    
    // Longitude: -180 to 180 → 0 to mapSize
    double x = (lon + 180.0) * mapSize / 360.0;
    
    // Latitude: Web Mercator projection
    double latRad = lat * M_PI / 180.0;
    double mercN = qLn(qTan(M_PI / 4.0 + latRad / 2.0));
    double y = mapSize / 2.0 - (mapSize * mercN / (2.0 * M_PI));
    
    return QPointF(x, y);
}

void MapView::addEarthquakeMarker(int id, double lat, double lon, double magnitude, const QString &time) {
    QPointF pos = latLonToPixel(lat, lon, m_currentZoom);
    
    // Ukuran marker berdasarkan magnitude
    double radius = qPow(2, magnitude) * 0.8;
    if (radius < 5) radius = 5;
    if (radius > 30) radius = 30;
    
    // Warna berdasarkan magnitude
    QColor color = getMagnitudeColor(magnitude);
    
    // Buat marker
    QGraphicsEllipseItem *circle = m_scene->addEllipse(
        pos.x() - radius, pos.y() - radius,
        radius * 2, radius * 2,
        QPen(Qt::white, 2),
        QBrush(color)
    );
    circle->setZValue(10);
    circle->setToolTip(QString("M%1 - %2\nLat: %3, Lon: %4\n%5")
                           .arg(magnitude, 0, 'f', 1)
                           .arg("Earthquake")
                           .arg(lat, 0, 'f', 4)
                           .arg(lon, 0, 'f', 4)
                           .arg(time));
    
    // Label magnitude
    QGraphicsTextItem *label = m_scene->addText(QString("M%1").arg(magnitude, 0, 'f', 1));
    label->setPos(pos.x() - 15, pos.y() - radius - 20);
    label->setDefaultTextColor(Qt::black);
    label->setZValue(11);
    QFont font = label->font();
    font.setBold(true);
    font.setPointSize(8);
    label->setFont(font);
    
    // Simpan marker
    EarthquakeMarker *marker = new EarthquakeMarker{id, lat, lon, magnitude, time, circle};
    m_markers.append(marker);
}

void MapView::clearMarkers() {
    for (EarthquakeMarker *marker : m_markers) {
        if (marker->item) {
            m_scene->removeItem(marker->item);
            delete marker->item;
        }
        delete marker;
    }
    m_markers.clear();
}

void MapView::setCenter(double lat, double lon) {
    m_centerLat = lat;
    m_centerLon = lon;
    QPointF center = latLonToPixel(lat, lon, m_currentZoom);
    m_view->centerOn(center);
}

void MapView::setZoom(int zoom) {
    if (zoom < 0) zoom = 0;
    if (zoom > 3) zoom = 3; // Max zoom level dari tiles kamu
    
    if (zoom != m_currentZoom) {
        m_currentZoom = zoom;
        loadTiles();
    }
}

void MapView::updateMarkerPositions() {
    for (EarthquakeMarker *marker : m_markers) {
        QPointF pos = latLonToPixel(marker->lat, marker->lon, m_currentZoom);
        
        double radius = qPow(2, marker->magnitude) * 0.8;
        if (radius < 5) radius = 5;
        if (radius > 30) radius = 30;
        
        marker->item->setRect(pos.x() - radius, pos.y() - radius, radius * 2, radius * 2);
    }
}

QColor MapView::getMagnitudeColor(double magnitude) {
    if (magnitude >= 7.0) return QColor(139, 0, 0, 200);     // Dark red
    if (magnitude >= 6.0) return QColor(255, 0, 0, 200);     // Red
    if (magnitude >= 5.0) return QColor(255, 140, 0, 200);   // Dark orange
    if (magnitude >= 4.0) return QColor(255, 165, 0, 200);   // Orange
    if (magnitude >= 3.0) return QColor(255, 215, 0, 200);   // Gold
    return QColor(144, 238, 144, 200);                        // Light green
}

void MapView::wheelEvent(QWheelEvent *event) {
    // Zoom in/out
    int delta = event->angleDelta().y();
    if (delta > 0) {
        setZoom(m_currentZoom + 1);
    } else {
        setZoom(m_currentZoom - 1);
    }
}

void MapView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void MapView::mouseMoveEvent(QMouseEvent *event) {
    if (m_isPanning) {
        QPointF delta = m_view->mapToScene(event->pos()) - m_view->mapToScene(m_lastPanPoint);
        m_view->setSceneRect(m_view->sceneRect().translated(-delta.x(), -delta.y()));
        m_lastPanPoint = event->pos();
    }
}

void MapView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
    }
}
