#include "MainWindow.h"
#include "MenuBar.h"
#include "MapView.h"
#include "SeismicEventTable.h"

#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QSplitter>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QVariantAnimation>
#include <QParallelAnimationGroup>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

void MainWindow::setupUI() {
    setWindowTitle("apakah ini my-program");
    resize(1280, 720); // 720p untuk testing

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

    // ========== TAB MONITORING ==========
    auto *monitoringTab = new QWidget();
    auto *monitorLayout = new QHBoxLayout(monitoringTab);
    monitorLayout->setContentsMargins(0, 0, 0, 0);
    monitorLayout->setSpacing(0);

    // Splitter utama: kiri (8) dan kanan (3)
    m_overallSplitter = new QSplitter(Qt::Horizontal);
    m_overallSplitter->setHandleWidth(0); // Hilangkan resize handle
    m_overallSplitter->setChildrenCollapsible(false);

    // --- PANEL KIRI ---
    auto *leftContainer = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // Main Panel Atas
    m_mainPanelTopLeft = new QLabel("Network Status / Station List");
    m_mainPanelTopLeft->setAlignment(Qt::AlignCenter);
    m_mainPanelTopLeft->setStyleSheet("background-color: #44444E; border: 1px solid #715A5A;");
    m_mainPanelTopLeft->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // Side Panel Bawah dengan sub-tabs
    m_bottomLeftTabs = new QTabWidget();
    
    // Tab Peta dengan MapView
    m_mapView = new MapView();
    m_bottomLeftTabs->addTab(m_mapView, "Peta");
    
    // Tab lainnya
    QStringList subTabs = {"Traces", "Arrival", "Forecast Zones", "Bulletin", "Tambahan"};
    for (const QString &tabName : subTabs) {
        auto *label = new QLabel(tabName + " Content Area");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("background-color: #37353E; color: white;");
        m_bottomLeftTabs->addTab(label, tabName);
    }
    
    connect(m_bottomLeftTabs, &QTabWidget::currentChanged, this, &MainWindow::onSubTabChanged);

    // Set ratio 3:8 untuk main panel : side panel
    leftLayout->addWidget(m_mainPanelTopLeft, 3);
    leftLayout->addWidget(m_bottomLeftTabs, 8);

    m_overallSplitter->addWidget(leftContainer);

    // --- PANEL KANAN ---
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

    // Set ratio 8:3 awal
    m_overallSplitter->setStretchFactor(0, 8);
    m_overallSplitter->setStretchFactor(1, 3);

    monitorLayout->addWidget(m_overallSplitter);
    m_mainTabs->addTab(monitoringTab, "Monitoring");

    // ========== TAB SEISMIC EVENT ==========
    m_seismicEventTable = new SeismicEventTable();
    m_mainTabs->addTab(m_seismicEventTable, "Seismic Event");
    
    // Connect seismic table dengan map view
    connect(m_seismicEventTable, &SeismicEventTable::eventSelected,
            this, [this](int id, double lat, double lon, double magnitude) {
        // Switch ke tab Monitoring
        m_mainTabs->setCurrentIndex(0);
        
        // Switch ke sub-tab Peta
        m_bottomLeftTabs->setCurrentIndex(0);
        
        // Center map ke lokasi event
        m_mapView->setCenter(lat, lon);
        
        // Highlight atau zoom ke marker
        statusBar()->showMessage(
            QString("Event ID %1: M%2 at (%3, %4)")
                .arg(id).arg(magnitude, 0, 'f', 1).arg(lat, 0, 'f', 4).arg(lon, 0, 'f', 4)
        );
    });

    // ========== TAB SIMULATION ==========
    m_mainTabs->addTab(new QLabel("Simulation"), "Simulation");

    setCentralWidget(m_mainTabs);
}

void MainWindow::onTabChanged(int index) {
    // Bisa digunakan untuk logic tab-level
}

void MainWindow::onSubTabChanged(int index) {
    if (index == 0) { // Tab "Peta" aktif
        animatePanelVisibility(true);
    } else { // Sub-tab lain aktif
        animatePanelVisibility(false);
    }
}

void MainWindow::animatePanelVisibility(bool show) {
    // Dapatkan ukuran sekarang
    QList<int> sizes = m_overallSplitter->sizes();
    int totalWidth = sizes[0] + sizes[1];
    
    if (show) {
        // Kembalikan ke ratio 8:3
        int leftTarget = totalWidth * 8 / 11;
        int rightTarget = totalWidth * 3 / 11;
        
        // Animasi smooth dengan QPropertyAnimation
        animateSplitter(m_overallSplitter, {leftTarget, rightTarget}, 300);
        
        // Tampilkan main panel dengan fade-in
        m_mainPanelTopLeft->show();
        m_rightPanel->show();
        
    } else {
        // Sembunyikan panel kanan dan expand panel kiri
        animateSplitter(m_overallSplitter, {totalWidth, 0}, 300);
        
        // Sembunyikan main panel atas
        m_mainPanelTopLeft->hide();
    }
}

void MainWindow::animateSplitter(QSplitter *splitter, QList<int> targetSizes, int duration) {
    QList<int> currentSizes = splitter->sizes();
    
    // QPropertyAnimation tidak support QList<int> secara langsung
    // Kita gunakan QVariantAnimation sebagai alternatif
    auto *animation = new QVariantAnimation(this);
    animation->setDuration(duration);
    animation->setStartValue(QVariant::fromValue(currentSizes));
    animation->setEndValue(QVariant::fromValue(targetSizes));
    animation->setEasingCurve(QEasingCurve::InOutCubic);
    
    connect(animation, &QVariantAnimation::valueChanged, this, [splitter](const QVariant &value) {
        splitter->setSizes(value.value<QList<int>>());
    });
    
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

QList<int> MainWindow::splitterSizes() const {
    return m_overallSplitter->sizes();
}

void MainWindow::setSplitterSizes(const QList<int> &sizes) {
    m_overallSplitter->setSizes(sizes);
}

void MainWindow::onThemeChanged(const QString &themeName) {
    loadTheme(themeName);
}

void MainWindow::loadTheme(const QString &themeName) {
    QString fileName = ":/theme/" + themeName + ".css";
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        file.close();
    } else {
        qApp->setStyleSheet("");
    }
}
