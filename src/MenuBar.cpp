#include "MenuBar.h"
#include <QMenu>
#include <QActionGroup>
#include <QMessageBox>

MenuBar::MenuBar(QWidget *parent) : QMenuBar(parent) {
    setupMenu();
}

void MenuBar::setupMenu() {
    // --- File ---
    auto *fileMenu = addMenu("&File");
    actionOpenFile = fileMenu->addAction("Open File...");
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", parentWidget(), &QWidget::close);

    // --- Settings ---
    auto *settingsMenu = addMenu("&Settings");
    actionSaveSettings = settingsMenu->addAction("Save Settings...");

    // --- View ---
    auto *viewMenu = addMenu("&View");
    actionToggleFullscreen = viewMenu->addAction("Toggle Fullscreen");
    actionToggleFullscreen->setCheckable(true);

    auto *themeMenu = viewMenu->addMenu("Theme");
    themeGroup = new QActionGroup(this);
    themeGroup->setExclusive(true);

    actionThemeDark = themeMenu->addAction("Dark");
    actionThemeDark->setCheckable(true);
    actionThemeDark->setChecked(true);
    themeGroup->addAction(actionThemeDark);

    actionThemeLight = themeMenu->addAction("Light");
    actionThemeLight->setCheckable(true);
    themeGroup->addAction(actionThemeLight);

    actionThemeDarkTrans = themeMenu->addAction("Dark (Transparent)");
    actionThemeDarkTrans->setCheckable(true);
    themeGroup->addAction(actionThemeDarkTrans);

    actionThemeLightTrans = themeMenu->addAction("Light (Transparent)");
    actionThemeLightTrans->setCheckable(true);
    themeGroup->addAction(actionThemeLightTrans);

    connect(themeGroup, &QActionGroup::triggered, this, &MenuBar::onThemeSelected);

    // --- Help ---
    auto *helpMenu = addMenu("&Help");
    actionAbout = helpMenu->addAction("About");
    connect(actionAbout, &QAction::triggered, this, []() {
        QMessageBox::about(nullptr, "About", "apakah ini my-program\nVersion 1.0");
    });
}

void MenuBar::onThemeSelected(QAction *action) {
    QString themeName;
    if (action == actionThemeDark) themeName = "dark";
    else if (action == actionThemeLight) themeName = "light";
    else if (action == actionThemeDarkTrans) themeName = "dark-transparent";
    else if (action == actionThemeLightTrans) themeName = "light-transparent";

    emit themeChanged(themeName);
}
