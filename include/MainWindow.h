#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include <QScrollArea>
#include <QLabel>

class MenuBar;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupStatusBar();
    void setupCentralWidget();
    void loadTheme(const QString &themeName);

    MenuBar *m_menuBar;
    QTabWidget *m_mainTabs;
    QSplitter *m_leftPanel, *m_rightPanel;
    QSplitter *m_overallSplitter;
    QLabel *m_mainPanelTopLeft; // Di sini diubah dari QWidget* ke QLabel*
    QWidget *m_mapTab;

private slots:
    void onTabChanged(int index);
    void onThemeChanged(const QString &themeName);
};

#endif // MAINWINDOW_H
