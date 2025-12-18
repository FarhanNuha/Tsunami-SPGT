#include "MainWindow.h"
#include "MenuBar.h"
#include "MapView.h"
#include "DatabaseView.h"
#include "FocalMechanismWidget.h"

#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QScrollArea>
#include <QSplitter>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

void MainWindow::setupUI() {
    setWindowTitle("apakah ini my-program");
    resize(1280, 720);

    m_menuBar = new MenuBar(this);
    setMenuBar(m_menuBar);

    connect(m_menuBar, &MenuBar::themeChanged, this, &MainWindow::onThemeChanged);

    setupStatusBar();
    setupCentralWidget();

    loadTheme("dark");
}

void MainWindow::setupStatusBar() {
    auto *statusBar = new QStatusBar(this);
    statusBar->showMessage("Ready");
    setStatusBar(statusBar);
}

void MainWindow::setupCentralWidget() {
    m_mainTabs = new QTabWidget(this);
    connect(m_mainTabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    // ===== Tab Monitoring =====
    auto *monitoringTab = new QWidget();
    auto *monitorLayout = new QHBoxLayout(monitoringTab);
    monitorLayout->setContentsMargins(0, 0, 0, 0);
    monitorLayout->setSpacing(0);

    m_overallSplitter = new QSplitter(Qt::Horizontal);
    m_overallSplitter->setHandleWidth(5);
    m_overallSplitter->setChildrenCollapsible(false);

    // Panel Kiri (Main + Side Panel)
    auto *leftContainer = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // Main Panel Atas (Fixed 1/5 dari tinggi) - gunakan FocalMechanismWidget
    m_focalMechWidget = new FocalMechanismWidget();
    m_focalMechWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    // Side Panel Bawah dengan sub-tabs (4/5 dari tinggi)
    m_bottomLeftTabs = new QTabWidget();
    
    // Tab Peta dengan MapView custom
    m_mapView = new MapView();
    m_bottomLeftTabs->addTab(m_mapView, "Peta");
    
    // Tab lainnya
    QStringList subTabs = {"Traces", "Arrival", "Forecast Zones", "Bulletin", "Tambahan"};
    for (const QString &tabName : subTabs) {
        if (tabName == "Bulletin") {
            // Special handling untuk Bulletin tab
            auto *bulletinWidget = new QWidget();
            auto *bulletinLayout = new QVBoxLayout(bulletinWidget);
            
            auto *bulletinText = new QLabel(
                "INFORMASI TSUNAMI\n\n"
                "Telah terjadi gempa bumi dengan parameter sebagai berikut:\n\n"
                "Tanggal: [Akan diisi otomatis]\n"
                "Waktu: [Akan diisi otomatis]\n"
                "Lokasi: [Koordinat akan diisi]\n"
                "Magnitudo: [Akan diisi otomatis]\n"
                "Kedalaman: [Akan diisi otomatis]\n\n"
                "PERINGATAN TSUNAMI:\n"
                "Berdasarkan hasil analisis mekanisme sumber gempa dan potensi tsunami,\n"
                "diperkirakan tsunami dapat terjadi di wilayah pesisir terdekat.\n\n"
                "Masyarakat di wilayah pesisir diimbau untuk:\n"
                "1. Segera menjauhi pantai\n"
                "2. Menuju ke tempat yang lebih tinggi\n"
                "3. Tetap waspada dan ikuti informasi resmi\n\n"
                "Informasi ini akan diperbarui sesuai perkembangan situasi."
            );
            bulletinText->setWordWrap(true);
            bulletinText->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            bulletinText->setStyleSheet("background-color: #37353E; color: white; padding: 10px;");
            
            bulletinLayout->addWidget(bulletinText, 1);
            
            // Tombol Diseminasi
            auto *btnDiseminasi = new QPushButton("Diseminasi");
            btnDiseminasi->setMinimumHeight(40);
            bulletinLayout->addWidget(btnDiseminasi);
            
            connect(btnDiseminasi, &QPushButton::clicked, [this]() {
                statusBar()->showMessage("Bulletin diseminasi berhasil dikirim!", 3000);
            });
            
            m_bottomLeftTabs->addTab(bulletinWidget, tabName);
        } else {
            auto *label = new QLabel(tabName + " Content Area");
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("background-color: #37353E; color: white;");
            m_bottomLeftTabs->addTab(label, tabName);
        }
    }
    
    connect(m_bottomLeftTabs, &QTabWidget::currentChanged, this, &MainWindow::onSubTabChanged);

    leftLayout->addWidget(m_focalMechWidget, 1);
    leftLayout->addWidget(m_bottomLeftTabs, 4);

    m_overallSplitter->addWidget(leftContainer);

    // Panel Kanan (Database + Layer)
    m_rightPanel = new QSplitter(Qt::Vertical);
    m_rightPanel->setHandleWidth(5);
    m_rightPanel->setChildrenCollapsible(false);

    auto *dbBox = new QLabel("Database-Tsunami");
    dbBox->setAlignment(Qt::AlignCenter);
    dbBox->setStyleSheet("background-color: #44444E; border: 1px solid #715A5A;");
    m_rightPanel->addWidget(dbBox);

    auto *layerBox = new QLabel("Layer Controls");
    layerBox->setAlignment(Qt::AlignCenter);
    layerBox->setStyleSheet("background-color: #44444E; border: 1px solid #715A5A;");
    m_rightPanel->addWidget(layerBox);

    m_rightPanel->setStretchFactor(0, 1);
    m_rightPanel->setStretchFactor(1, 1);

    m_overallSplitter->addWidget(m_rightPanel);
    m_overallSplitter->setStretchFactor(0, 8);
    m_overallSplitter->setStretchFactor(1, 3);

    monitorLayout->addWidget(m_overallSplitter);
    m_mainTabs->addTab(monitoringTab, "Monitoring");

    // ===== Tab Seismic Event dengan Database View =====
    m_databaseView = new DatabaseView();
    m_mainTabs->addTab(m_databaseView, "Seismic Event");
    
    connect(m_databaseView, &DatabaseView::eventSelected, this, &MainWindow::onEventSelected);

    // ===== Tab Simulation =====
    m_mainTabs->addTab(new QLabel("Simulation"), "Simulation");
    
    // ===== Tab Inundation Forecast =====
    m_mainTabs->addTab(new QLabel("Inundation Forecast"), "Inundation Forecast");

    setCentralWidget(m_mainTabs);
    
    updateMainPanelHeight();
}

void MainWindow::updateMainPanelHeight() {
    int totalHeight = height() - menuBar()->height() - statusBar()->height() - 50;
    int mainPanelHeight = totalHeight * 1 / 5;
    m_focalMechWidget->setFixedHeight(mainPanelHeight);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (m_focalMechWidget && m_bottomLeftTabs->currentIndex() == 0) {
        updateMainPanelHeight();
    }
}

void MainWindow::onTabChanged(int index) {
    Q_UNUSED(index);
}

void MainWindow::onSubTabChanged(int index) {
    if (index == 0) {
        animatePanelVisibility(true);
    } else {
        animatePanelVisibility(false);
    }
}

void MainWindow::animatePanelVisibility(bool show) {
    auto *animGroup = new QParallelAnimationGroup(this);
    
    if (show) {
        m_focalMechWidget->show();
        m_rightPanel->show();
        
        auto *heightAnim = new QPropertyAnimation(m_focalMechWidget, "maximumHeight");
        heightAnim->setDuration(300);
        heightAnim->setStartValue(0);
        int targetHeight = (height() - menuBar()->height() - statusBar()->height() - 50) * 1 / 5;
        heightAnim->setEndValue(targetHeight);
        heightAnim->setEasingCurve(QEasingCurve::OutCubic);
        animGroup->addAnimation(heightAnim);
        
        QTimer::singleShot(0, this, [this]() {
            QList<int> sizes = m_overallSplitter->sizes();
            int total = sizes[0] + sizes[1];
            m_overallSplitter->setSizes({total * 8 / 11, total * 3 / 11});
        });
        
    } else {
        auto *heightAnim = new QPropertyAnimation(m_focalMechWidget, "maximumHeight");
        heightAnim->setDuration(300);
        heightAnim->setStartValue(m_focalMechWidget->height());
        heightAnim->setEndValue(0);
        heightAnim->setEasingCurve(QEasingCurve::InCubic);
        animGroup->addAnimation(heightAnim);
        
        connect(heightAnim, &QPropertyAnimation::finished, this, [this]() {
            m_focalMechWidget->hide();
        });
        
        QTimer::singleShot(100, this, [this]() {
            m_rightPanel->hide();
            QList<int> sizes = m_overallSplitter->sizes();
            int total = sizes[0] + sizes[1];
            m_overallSplitter->setSizes({total, 0});
        });
    }
    
    animGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::onEventSelected(const QString &eventId, double lat, double lon, double magnitude,
                                int strike, int dip, int slip, const QString &eventInfo) {
    // Extract origin time dari eventInfo
    QString originTime = eventInfo.split("\n")[1].replace("Magnitude ", "").split(" | ")[1];
    
    // Update focal mechanism widget
    m_focalMechWidget->setEventData(eventId, lat, lon, magnitude, strike, dip, slip, originTime);
    
    // Center map pada lokasi event
    m_mapView->centerOnCoordinate(lat, lon);
    
    // Switch ke tab Monitoring
    m_mainTabs->setCurrentIndex(0);
    
    // Switch ke sub-tab Peta
    m_bottomLeftTabs->setCurrentIndex(0);
    
    // Update status bar
    statusBar()->showMessage(QString("Event %1 selected (Mag %2)").arg(eventId).arg(magnitude, 0, 'f', 1));
}

void MainWindow::onThemeChanged(const QString &themeName) {
    loadTheme(themeName);
}

void MainWindow::loadTheme(const QString &themeName) {
    QString fileName = ":/theme/" + themeName + ".css";
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        file.close();
    } else {
        qApp->setStyleSheet("");
    }
}
