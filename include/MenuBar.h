#ifndef MENUBAR_H
#define MENUBAR_H

#include <QMenuBar>
#include <QActionGroup>

class MenuBar : public QMenuBar {
    Q_OBJECT

public:
    explicit MenuBar(QWidget *parent = nullptr);

    QAction *actionOpenFile;
    QAction *actionSaveSettings;
    QAction *actionToggleFullscreen;
    QAction *actionAbout;

    QActionGroup *themeGroup;
    QAction *actionThemeDark;
    QAction *actionThemeLight;
    QAction *actionThemeDarkTrans;
    QAction *actionThemeLightTrans;

signals:
    void themeChanged(const QString &themeName);

private:
    void setupMenu();

private slots:
    void onThemeSelected(QAction *action);
};

#endif // MENUBAR_H
