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
    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::NoDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    
    // Set default map directory
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
    
    // Load root tile
    QString tilePath = m_mapDirectory + "/world.png";
    QPixmap pixmap(tilePath);
    
    if (pixmap.isNull()) {
        // Create placeholder jika file tidak ada
        pixmap = QPixmap(512, 256);
        pixmap.fill(Qt::darkGray);
        
        // Add text
        QPainter painter(&pixmap);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(pixmap.rect(), Qt::AlignCenter, 
                        QString("Map tiles not found\nPlace world.png in '%1' directory").arg(m_mapDirectory));
    }
    
    QGraphicsPixmapItem *item = m_scene->addPixmap(pixmap);
    item->setPos(0, 0);
    m_tileCache["world"] = item;
    
    m_scene->setSceneRect(m_scene->itemsBoundingRect());
    fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

QString MapView::getTilePath(int zoom, int x, int y) {
    // Konversi x,y ke format quadtree string
    // Untuk simplifikasi, gunakan format world{zoom}_{x}_{y}.png
    // atau ikuti pattern world[0-3]+ yang ada
    
    if (zoom == 1) {
        // Level 1: world0.png, world1.png, world2.png, world3.png
        int index = y * 2 + x;
        return QString("%1/world%2.png").arg(m_mapDirectory).arg(index);
    } else if (zoom == 2) {
        // Level 2: world00.png, world01.png, ..., world33.png
        int firstDigit = (y / 2) * 2 + (x / 2);
        int secondDigit = (y % 2) * 2 + (x % 2);
        return QString("%1/world%2%3.png").arg(m_mapDirectory).arg(firstDigit).arg(secondDigit);
    } else if (zoom == 3) {
        // Level 3: world000.png, ..., world333.png
        QString quadKey;
        for (int i = 0; i < zoom; i++) {
            int mask = 1 << (zoom - i - 1);
            int digit = 0;
            if (x & mask) digit += 1;
            if (y & mask) digit += 2;
            quadKey += QString::number(digit);
        }
        return QString("%1/world%2.png").arg(m_mapDirectory).arg(quadKey);
    } else if (zoom == 4) {
        // Level 4: world0000.png, ..., world3333.png
        QString quadKey;
        for (int i = 0; i < zoom; i++) {
            int mask = 1 << (zoom - i - 1);
            int digit = 0;
            if (x & mask) digit += 1;
            if (y & mask) digit += 2;
            quadKey += QString::number(digit);
        }
        return QString("%1/world%2.png").arg(m_mapDirectory).arg(quadKey);
    }
    
    return QString("%1/world.png").arg(m_mapDirectory);
}

void MapView::centerOnCoordinate(double lat, double lon) {
    // Konversi lat/lon ke koordinat scene
    // Implementasi Web Mercator projection
    double x = (lon + 180.0) / 360.0 * m_scene->width();
    double latRad = lat * M_PI / 180.0;
    double mercN = std::log(std::tan((M_PI / 4.0) + (latRad / 2.0)));
    double y = (1.0 - mercN / M_PI) / 2.0 * m_scene->height();
    
    centerOn(x, y);
}

void MapView::wheelEvent(QWheelEvent *event) {
    // Zoom dengan scroll wheel menggunakan scale transform
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
        
        // Pan view
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
    // Optimisasi: hanya load tiles yang visible
    // Implementasi di sini jika diperlukan untuk performa
}
