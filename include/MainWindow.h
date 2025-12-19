#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include <QLabel>

class MenuBar;
class MapView;
class DatabaseView;
class FocalMechanismWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void setupStatusBar();
    void setupCentralWidget();
    void loadTheme(const QString &themeName);
    void animatePanelVisibility(bool show);
    void updateMainPanelHeight();

    MenuBar *m_menuBar;
    QTabWidget *m_mainTabs;
    QTabWidget *m_bottomLeftTabs;
    QSplitter *m_rightPanel;
    QSplitter *m_overallSplitter;
    FocalMechanismWidget *m_focalMechWidget;
    MapView *m_mapView;
    DatabaseView *m_databaseView;

private slots:
    void onTabChanged(int index);
    void onSubTabChanged(int index);
    void onThemeChanged(const QString &themeName);
    void onEventSelected(const QString &eventId, double lat, double lon, double magnitude,
                        int depth, int strike, int dip, int slip, const QString &eventInfo);
};

#endif // MAINWINDOW_H
