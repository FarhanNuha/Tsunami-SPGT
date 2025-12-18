#include "FocalMechanismWidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <cmath>

FocalMechanismWidget::FocalMechanismWidget(QWidget *parent)
    : QWidget(parent)
    , m_hasData(false)
{
    setMinimumSize(300, 200);
}

void FocalMechanismWidget::setEventData(const QString &eventId, double lat, double lon, 
                                       double magnitude, int strike, int dip, int slip,
                                       const QString &originTime) {
    m_eventId = eventId;
    m_latitude = lat;
    m_longitude = lon;
    m_magnitude = magnitude;
    m_strike = strike;
    m_dip = dip;
    m_slip = slip;
    m_originTime = originTime;
    m_hasData = true;
    
    update();
}

void FocalMechanismWidget::clearData() {
    m_hasData = false;
    update();
}

void FocalMechanismWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (!m_hasData) {
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, "No event selected");
        return;
    }
    
    // Background
    painter.fillRect(rect(), QColor(68, 68, 78));
    
    // Draw info text di kiri
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);
    
    int textX = 10;
    int textY = 20;
    int lineHeight = 18;
    
    painter.drawText(textX, textY, QString("Event ID: %1").arg(m_eventId));
    textY += lineHeight;
    painter.drawText(textX, textY, QString("Time: %1").arg(m_originTime));
    textY += lineHeight;
    painter.drawText(textX, textY, QString("Magnitude: %1").arg(m_magnitude, 0, 'f', 1));
    textY += lineHeight;
    painter.drawText(textX, textY, QString("Lat: %1°").arg(m_latitude, 0, 'f', 4));
    textY += lineHeight;
    painter.drawText(textX, textY, QString("Lon: %1°").arg(m_longitude, 0, 'f', 4));
    textY += lineHeight * 2;
    
    painter.drawText(textX, textY, "Focal Mechanism:");
    textY += lineHeight;
    painter.drawText(textX, textY, QString("Strike: %1°").arg(m_strike));
    textY += lineHeight;
    painter.drawText(textX, textY, QString("Dip: %1°").arg(m_dip));
    textY += lineHeight;
    painter.drawText(textX, textY, QString("Slip: %1°").arg(m_slip));
    
    // Draw beach ball di kanan
    int beachBallX = width() - 120;
    int beachBallY = height() / 2;
    int radius = 80;
    
    drawBeachBall(painter, beachBallX, beachBallY, radius);
}

void FocalMechanismWidget::drawBeachBall(QPainter &painter, int centerX, int centerY, int radius) {
    // Simplified beach ball representation
    // Untuk implementasi yang akurat, perlu kalkulasi kompleks dari strike/dip/slip
    
    painter.save();
    
    // Draw outer circle
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
    
    // Calculate nodal planes based on strike, dip, slip
    // Simplified visualization - untuk demo purposes
    double strikeRad = m_strike * M_PI / 180.0;
    double dipRad = m_dip * M_PI / 180.0;
    
    // Draw first nodal plane (simplified)
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    
    QPainterPath path;
    path.moveTo(centerX, centerY);
    
    // Create arc untuk quadrant yang terkompresi
    double startAngle = strikeRad - M_PI / 2;
    double endAngle = strikeRad + M_PI / 2;
    
    QPointF p1(centerX + radius * cos(startAngle), centerY + radius * sin(startAngle));
    QPointF p2(centerX + radius * cos(endAngle), centerY + radius * sin(endAngle));
    
    path.lineTo(p1);
    path.arcTo(centerX - radius, centerY - radius, radius * 2, radius * 2,
               startAngle * 180 / M_PI, 180);
    path.lineTo(centerX, centerY);
    
    painter.drawPath(path);
    
    // Draw nodal plane line
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(
        centerX + radius * cos(strikeRad),
        centerY + radius * sin(strikeRad),
        centerX - radius * cos(strikeRad),
        centerY - radius * sin(strikeRad)
    );
    
    // Draw perpendicular plane
    painter.drawLine(
        centerX + radius * cos(strikeRad + M_PI/2),
        centerY + radius * sin(strikeRad + M_PI/2),
        centerX - radius * cos(strikeRad + M_PI/2),
        centerY - radius * sin(strikeRad + M_PI/2)
    );
    
    // Draw outer circle again
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
    
    painter.restore();
}
