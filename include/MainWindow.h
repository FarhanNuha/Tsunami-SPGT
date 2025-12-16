#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include <QLabel>

class MenuBar;
class MapView;
class SeismicEventTable;

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_PROPERTY(QList<int> splitterSizes READ splitterSizes WRITE setSplitterSizes)

public:
    explicit MainWindow(QWidget *parent = nullptr);

    QList<int> splitterSizes() const;
    void setSplitterSizes(const QList<int> &sizes);

private:
    void setupUI();
    void setupStatusBar();
    void setupCentralWidget();
    void loadTheme(const QString &themeName);
    void animatePanelVisibility(bool show);
    void animateSplitter(QSplitter *splitter, QList<int> targetSizes, int duration);

    MenuBar *m_menuBar;
    QTabWidget *m_mainTabs;
    QTabWidget *m_bottomLeftTabs;
    QSplitter *m_rightPanel;
    QSplitter *m_overallSplitter;
    QLabel *m_mainPanelTopLeft;
    MapView *m_mapView;
    SeismicEventTable *m_seismicEventTable;

private slots:
    void onTabChanged(int index);
    void onSubTabChanged(int index);
    void onThemeChanged(const QString &themeName);
};

#endif // MAINWINDOW_H
