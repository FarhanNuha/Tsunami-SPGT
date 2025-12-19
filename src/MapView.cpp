#include "MapView.h"
#include <QDir>
#include <QPixmap>
#include <QScrollBar>
#include <QPainter>
#include <QFont>
#include <cmath>

MapView::MapView(QWidget *parent) 
    : QGraphicsView(parent)
    , m_currentZoom(0)
    , m_maxZoom(4)
    , m_scale(1.0)
    , m_isPanning(false)
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    
    setMapDirectory("maps");
}

void MapView::setMapDirectory(const QString &dirPath) {
    m_mapDirectory = dirPath;
    loadTiles();
}

void MapView::setZoomLevel(int level) {
    if (level < 0 || level > m_maxZoom) return;
    
    m_currentZoom = level;
    loadTiles();
}

void MapView::loadTiles() {
    m_scene->clear();
    m_tileCache.clear();
    
    // Load tiles based on current zoom level
    QString basePath = m_mapDirectory + "/world.png";
    QPixmap basePixmap(basePath);
    
    if (basePixmap.isNull()) {
        // Create placeholder
        basePixmap = QPixmap(512, 256);
        basePixmap.fill(Qt::darkGray);
        
        QPainter painter(&basePixmap);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(basePixmap.rect(), Qt::AlignCenter, 
                        QString("Map tiles not found\nPlace world.png in '%1' directory").arg(m_mapDirectory));
    }
    
    if (m_currentZoom == 0) {
        // Zoom level 0: single tile
        QGraphicsPixmapItem *item = m_scene->addPixmap(basePixmap);
        item->setPos(0, 0);
        m_tileCache["world"] = item;
    } else {
        // Higher zoom levels: load quadtree tiles
        int tilesPerSide = std::pow(2, m_currentZoom);
        int baseTileWidth = basePixmap.width();
        int baseTileHeight = basePixmap.height();
        
        for (int y = 0; y < tilesPerSide; y++) {
            for (int x = 0; x < tilesPerSide; x++) {
                QString tilePath = getTilePath(m_currentZoom, x, y);
                QPixmap tilePixmap(tilePath);
                
                if (tilePixmap.isNull()) {
                    // Use scaled portion of base map as fallback
                    int srcX = x * baseTileWidth / tilesPerSide;
                    int srcY = y * baseTileHeight / tilesPerSide;
                    int srcW = baseTileWidth / tilesPerSide;
                    int srcH = baseTileHeight / tilesPerSide;
                    
                    tilePixmap = basePixmap.copy(srcX, srcY, srcW, srcH);
                }
                
                QGraphicsPixmapItem *item = m_scene->addPixmap(tilePixmap);
                item->setPos(x * tilePixmap.width(), y * tilePixmap.height());
                
                QString key = QString("%1_%2_%3").arg(m_currentZoom).arg(x).arg(y);
                m_tileCache[key] = item;
            }
        }
    }
    
    m_scene->setSceneRect(m_scene->itemsBoundingRect());
    
    // Fit in view on first load
    if (m_currentZoom == 0) {
        fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

QString MapView::getTilePath(int zoom, int x, int y) {
    // Convert x,y coordinates to quadtree key
    QString quadKey;
    
    for (int i = zoom - 1; i >= 0; i--) {
        int digit = 0;
        int mask = 1 << i;
        
        if (x & mask) digit += 1;
        if (y & mask) digit += 2;
        
        quadKey += QString::number(digit);
    }
    
    return QString("%1/world%2.png").arg(m_mapDirectory).arg(quadKey);
}

void MapView::centerOnCoordinate(double lat, double lon) {
    // Web Mercator projection
    double x = (lon + 180.0) / 360.0;
    double latRad = lat * M_PI / 180.0;
    double mercN = std::log(std::tan((M_PI / 4.0) + (latRad / 2.0)));
    double y = (1.0 - mercN / M_PI) / 2.0;
    
    // Convert to scene coordinates
    QRectF sceneRect = m_scene->sceneRect();
    double sceneX = x * sceneRect.width();
    double sceneY = y * sceneRect.height();
    
    // Add marker at location
    QGraphicsEllipseItem *marker = m_scene->addEllipse(
        sceneX - 5, sceneY - 5, 10, 10,
        QPen(Qt::red, 2),
        QBrush(Qt::red)
    );
    marker->setZValue(1000); // Always on top
    
    centerOn(sceneX, sceneY);
}

void MapView::wheelEvent(QWheelEvent *event) {
    // Simple smooth zoom tanpa dynamic tile loading
    const double scaleFactor = 1.15;
    
    if (event->angleDelta().y() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
        m_scale *= scaleFactor;
    } else {
        // Zoom out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        m_scale /= scaleFactor;
    }
    
    event->accept();
}

void MapView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void MapView::mouseMoveEvent(QMouseEvent *event) {
    if (m_isPanning) {
        QPoint delta = event->pos() - m_lastPanPoint;
        m_lastPanPoint = event->pos();
        
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MapView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MapView::updateVisibleTiles() {
    // TODO: Implement tile culling for performance
}
