#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QWebEngineView>

class MapView : public QWidget {
    Q_OBJECT

public:
    explicit MapView(QWidget *parent = nullptr);

    void addEarthquakeMarker(double lat, double lon, double magnitude, const QString &time);
    void clearMarkers();
    void setCenter(double lat, double lon, int zoom = 10);

private:
    void setupUI();
    void loadMap();

    QWebEngineView *m_webView;

signals:
    void markerClicked(double lat, double lon);
};

#endif // MAPVIEW_H
