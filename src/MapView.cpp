#include "MapView.h"
#include <QVBoxLayout>
#include <QWebChannel>

MapView::MapView(QWidget *parent) : QWidget(parent) {
    setupUI();
    loadMap();
}

void MapView::setupUI() {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_webView = new QWebEngineView(this);
    layout->addWidget(m_webView);
}

void MapView::loadMap() {
    m_webView->setUrl(QUrl("qrc:/map/index.html"));
}

void MapView::addEarthquakeMarker(double lat, double lon, double magnitude, const QString &time) {
    QString script = QString(
        "addEarthquakeMarker(%1, %2, %3, '%4');"
    ).arg(lat).arg(lon).arg(magnitude).arg(time);
    
    m_webView->page()->runJavaScript(script);
}

void MapView::clearMarkers() {
    m_webView->page()->runJavaScript("clearMarkers();");
}

void MapView::setCenter(double lat, double lon, int zoom) {
    QString script = QString(
        "map.setView([%1, %2], %3);"
    ).arg(lat).arg(lon).arg(zoom);
    
    m_webView->page()->runJavaScript(script);
}
