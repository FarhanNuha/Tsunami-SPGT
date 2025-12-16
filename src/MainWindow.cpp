#include "MainWindow.h"
#include "MenuBar.h"

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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

void MainWindow::setupUI() {
    setWindowTitle("apakah ini my-program");
    resize(1920, 1080);

    m_menuBar = new MenuBar(this);
    setMenuBar(m_menuBar);

    connect(m_menuBar, &MenuBar::themeChanged, this, &MainWindow::onThemeChanged);

    setupStatusBar();
    setupCentralWidget();

    // Muat tema default
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

    // Tab Monitoring
    auto *monitoringTab = new QWidget();
    auto *monitorLayout = new QHBoxLayout(monitoringTab);

    // Splitter utama: kiri (8) dan kanan (3)
    m_overallSplitter = new QSplitter(Qt::Horizontal);
    m_overallSplitter->setHandleWidth(5);

    // --- PANEL KIRI (MAIN PANEL + SIDE PANEL BAWAH) ---
    m_leftPanel = new QSplitter(Qt::Vertical);
    m_leftPanel->setHandleWidth(5);

    // 1. Main Panel Atas (FIXED HEIGHT)
    m_mainPanelTopLeft = new QLabel("Network Status / Station List");
    m_mainPanelTopLeft->setAlignment(Qt::AlignCenter);
    m_mainPanelTopLeft->setStyleSheet("background-color: #44444E; border: 1px solid #715A5A;");
    m_mainPanelTopLeft->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_mainPanelTopLeft->setMinimumHeight(200);
    m_mainPanelTopLeft->setMaximumHeight(200);

    // 2. Side Panel Bawah (isi sub-tabs, bisa di-resize)
    auto *bottomLeftBox = new QTabWidget(); // Sub-tabs: peta, traces, dll
    QStringList subTabs = {"Peta", "Traces", "Arrival", "Forecast Zones", "Bulletin", "Tambahan"};
    for (const QString &tabName : subTabs) {
        auto *label = new QLabel(tabName + " Content Area");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("background-color: #37353E; color: white;");
        bottomLeftBox->addTab(label, tabName);
    }

    m_leftPanel->addWidget(m_mainPanelTopLeft);
    m_leftPanel->addWidget(bottomLeftBox);
    m_leftPanel->setStretchFactor(0, 0); // Main panel tidak stretch (fixed)
    m_leftPanel->setStretchFactor(1, 1); // Side panel bawah bisa stretch

    m_overallSplitter->addWidget(m_leftPanel);

    // --- PANEL KANAN (DATABASE + LAYER) ---
    m_rightPanel = new QSplitter(Qt::Vertical);
    m_rightPanel->setHandleWidth(5);

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

    // Bungkus panel kanan dalam scroll area
    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(m_rightPanel);
    m_overallSplitter->addWidget(scrollArea);

    // Atur rasio 8:3 (total 11)
    // Kiri = 8/11, Kanan = 3/11
    int totalWidth = 1920;
    int leftWidth = (int)(totalWidth * (8.0 / 11.0));
    int rightWidth = (int)(totalWidth * (3.0 / 11.0));
    m_overallSplitter->setSizes({leftWidth, rightWidth});

    monitorLayout->addWidget(m_overallSplitter);
    m_mainTabs->addTab(monitoringTab, "Monitoring");

    // Placeholder for other tabs
    m_mainTabs->addTab(new QLabel("Seismic Event"), "Seismic Event");
    m_mainTabs->addTab(new QLabel("Simulation"), "Simulation");

    setCentralWidget(m_mainTabs);

    // Simpan referensi sub-tab "Peta" untuk transisi
    m_mapTab = bottomLeftBox->widget(0);
}

void MainWindow::onTabChanged(int index) {
    if (index != 0) return; // Hanya tangani tab "Monitoring"

    QTabWidget *subTabs = qobject_cast<QTabWidget*>(m_leftPanel->widget(1)); // Side panel bawah
    if (!subTabs) return;

    int currentSubIndex = subTabs->currentIndex();

    if (currentSubIndex == 0) { // Tab "Peta" aktif
        m_mainPanelTopLeft->setVisible(true); // Tampilkan main panel atas
        m_rightPanel->setVisible(true);       // Tampilkan panel kanan
    } else { // Sub-tab lain aktif
        m_mainPanelTopLeft->setVisible(false); // Hilangkan main panel atas (transisi ke atas)
        m_rightPanel->setVisible(false);       // Hilangkan panel kanan (transisi ke samping/kanan)
    }
}

void MainWindow::onThemeChanged(const QString &themeName) {
    loadTheme(themeName);
}

void MainWindow::loadTheme(const QString &themeName) {
    QString fileName = ":/theme/" + themeName + ".css";
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        qApp->setStyleSheet(stream.readAll());
        file.close();
    } else {
        qApp->setStyleSheet("");
    }
}
