#pragma once

#include <QMainWindow>
#include <QStatusBar>
#include <QToolBar>
#include <QSlider>
#include <QAction>
#include <QActionGroup>
#include <QUndoStack>
#include "Editor.h"
#include "ColorBox.h"
#include "Sidebar.h"
#include "enums.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent* event) override;    


private slots:
    void sliderValueChanged(int value);
    void exit();
    void saveProject();
    void saveProjectAs();
    void handleLoadProject();
    void loadProject();
    void setFrontSideImage();
    void setBackSideImage();
    void viewConnections();
    void addTrackButtonAction(bool checked);
    void addComponentButtonAction(bool checked);
    void addNotesButtonAction(bool checked);
    void showAboutDialog();
    void frontSideToggleButtonAction(bool checked);
    void backSideToggleButtonAction(bool checked);
    void zoomIn();
    void zoomOut();
    void resetLinkWidth();
    void openConfigDialog();
    void cleanProject();
    void newProject();

    void autoSaveProject();

private:
    QString m_windowBaseTitle;
    QString m_currentFilePath;
    Editor* m_editor;
    QToolBar* m_toolbar;
    QToolBar* m_sideToolbar;
    QSlider* m_slider;
    ColorBox* m_colorBox;
    Sidebar* m_sidebar;
    QTimer* m_autoSaveTimer;
    bool m_changesSinceLastAutosave;
    bool m_wasJustAutosaved;

    QAction* m_addTrackAction;
    QAction* m_addComponentAction;
    QAction* m_addNotesAction;
    QAction* m_frontSideAction;
    QAction* m_backSideAction;
    QAction* m_flipHAction;
    QAction* m_flipVAction;

    bool checkAndLoadAutosave(QString originalFilePath);
    void loadProjectFromFile(const QString& filePath, bool isAutoLoad = false);    
    void createToolbarActions();
    void setCurrentFilePath(const QString& filePath);
    void saveToFile(const QString& filePath, bool isAutoLoad = false);
    QString getAutosaveFilePath(QString filePath);
    bool promptForUnsavedChanges();
    void removeAutosaveFile();
    void renameToAutosaveFile(QString filePath);
};