#ifndef FOCALMECHANISMWIDGET_H
#define FOCALMECHANISMWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QLabel>

class FocalMechanismWidget : public QWidget {
    Q_OBJECT

public:
    explicit FocalMechanismWidget(QWidget *parent = nullptr);
    
    void setEventData(const QString &eventId, double lat, double lon, double magnitude,
                     int strike, int dip, int slip, int depth, const QString &originTime);
    void clearData();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawBeachBall(QPainter &painter, int centerX, int centerY, int radius);
    
    QString m_eventId;
    double m_latitude;
    double m_longitude;
    double m_magnitude;
    int m_strike;
    int m_dip;
    int m_slip;
    int m_depth;
    QString m_originTime;
    QString m_location;
    bool m_hasData;
};

#endif // FOCALMECHANISMWIDGET_H
