#include "FocalMechanismWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <cmath>

FocalMechanismWidget::FocalMechanismWidget(QWidget *parent)
    : QWidget(parent)
    , m_hasData(false)
{
    setMinimumSize(300, 150);
}

void FocalMechanismWidget::setEventData(const QString &eventId, double lat, double lon, 
                                       double magnitude, int strike, int dip, int slip,
                                       int depth, const QString &originTime) {
    m_eventId = eventId;
    m_latitude = lat;
    m_longitude = lon;
    m_magnitude = magnitude;
    m_strike = strike;
    m_dip = dip;
    m_slip = slip;
    m_depth = depth;
    m_originTime = originTime;
    
    // Determine location based on coordinates
    if (lat >= -6.5 && lat <= -5.5 && lon >= 105 && lon <= 107) {
        m_location = "Southern Sumatra, Indonesia";
    } else if (lat >= -8.5 && lat <= -7.0 && lon >= 109 && lon <= 111) {
        m_location = "Central Java, Indonesia";
    } else if (lat >= -9.0 && lat <= -7.5 && lon >= 114 && lon <= 116) {
        m_location = "Bali, Indonesia";
    } else if (lat >= 2.5 && lat <= 4.5 && lon >= 97 && lon <= 99) {
        m_location = "Aceh, Indonesia";
    } else if (lat >= -1.5 && lat <= 0.5 && lon >= 119 && lon <= 121) {
        m_location = "Sulawesi, Indonesia";
    } else {
        m_location = "Indonesia Region";
    }
    
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
    
    // Background
    painter.fillRect(rect(), QColor(68, 68, 78));
    
    if (!m_hasData) {
        painter.setPen(Qt::gray);
        QFont font("Arial", 10);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, "No event selected");
        return;
    }
    
    int margin = 10;
    int leftWidth = width() - 150; // Space for beach ball
    
    // Left side - Event information
    painter.setPen(Qt::white);
    
    // DateTime - large font at top
    QFont dateFont("Arial", 16, QFont::Bold);
    painter.setFont(dateFont);
    int yPos = margin + 20;
    painter.drawText(margin, yPos, m_originTime);
    
    yPos += 10;
    
    // Time ago (placeholder)
    QFont smallFont("Arial", 9);
    painter.setFont(smallFont);
    painter.setPen(QColor(255, 165, 0)); // Orange color
    painter.drawText(margin, yPos, "1h and 28m ago");
    
    yPos += 20;
    
    // Location
    painter.setPen(Qt::white);
    QFont locFont("Arial", 10);
    painter.setFont(locFont);
    painter.drawText(margin, yPos, m_location);
    
    yPos += 25;
    
    // Magnitude, Depth, Distance info
    QFont infoFont("Arial", 11, QFont::Bold);
    painter.setFont(infoFont);
    
    // FM label with circle
    painter.setPen(Qt::white);
    painter.drawText(margin, yPos, "FM");
    
    int circleX = margin + 25;
    painter.setBrush(Qt::white);
    painter.drawEllipse(circleX, yPos - 8, 10, 10);
    
    // M (Magnitude)
    int magX = circleX + 20;
    painter.setBrush(Qt::NoBrush);
    painter.drawText(magX, yPos, QString("M %1").arg(m_magnitude, 0, 'f', 1));
    
    // D (Depth)
    int depthX = magX + 60;
    painter.drawText(depthX, yPos, QString("D %1 km").arg(m_depth));
    
    // Right side - Beach ball (focal mechanism)
    int beachBallX = width() - 100;
    int beachBallY = height() / 2;
    int radius = 60;
    
    drawBeachBall(painter, beachBallX, beachBallY, radius);
}

void FocalMechanismWidget::drawBeachBall(QPainter &painter, int centerX, int centerY, int radius) {
    painter.save();
    
    // Draw outer white circle
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
    
    // Calculate angles from strike/dip/slip
    double strikeRad = m_strike * M_PI / 180.0;
    
    // Draw compressional quadrants (black)
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    
    QPainterPath path;
    path.moveTo(centerX, centerY);
    
    // First quadrant
    double startAngle = strikeRad - M_PI / 2;
    double spanAngle = M_PI;
    
    QPointF p1(centerX + radius * cos(startAngle), 
               centerY + radius * sin(startAngle));
    QPointF p2(centerX + radius * cos(startAngle + spanAngle), 
               centerY + radius * sin(startAngle + spanAngle));
    
    path.lineTo(p1);
    path.arcTo(centerX - radius, centerY - radius, 
               radius * 2, radius * 2,
               startAngle * 180 / M_PI, 
               spanAngle * 180 / M_PI);
    path.lineTo(centerX, centerY);
    
    painter.drawPath(path);
    
    // Draw nodal planes
    painter.setPen(QPen(Qt::black, 2));
    
    // Primary nodal plane
    painter.drawLine(
        centerX + radius * cos(strikeRad),
        centerY + radius * sin(strikeRad),
        centerX - radius * cos(strikeRad),
        centerY - radius * sin(strikeRad)
    );
    
    // Auxiliary plane (perpendicular)
    double auxAngle = strikeRad + M_PI / 2;
    painter.drawLine(
        centerX + radius * cos(auxAngle),
        centerY + radius * sin(auxAngle),
        centerX - radius * cos(auxAngle),
        centerY - radius * sin(auxAngle)
    );
    
    // Redraw outer circle
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::white, 2));
    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
    
    painter.restore();
}
