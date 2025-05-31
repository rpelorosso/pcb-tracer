#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include <QApplication>
//#include "ConfigDialog.h"
#include "Config.h"
#include "ColorBox.h"
#include "SceneLoader.h"
#include "SceneLoaderBinary.h"
#include "ConfigDialog.h"
#include "ConnectionAnalyzer.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_windowBaseTitle("PCB Tracer")
    , m_currentFilePath("")
{
    // Create widgets
    QStatusBar* statusBar = new QStatusBar(this);
    m_editor = Editor::instance();
    m_editor->setStatusBar(statusBar);
    setCurrentFilePath("");
    
    // Create toolbar
    m_toolbar = new QToolBar(this);
    m_sideToolbar = new QToolBar(this);
    createToolbarActions();

    // Create slider
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setMinimum(0);
    m_slider->setMaximum(100);
    connect(m_slider, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
    m_slider->setTickPosition(QSlider::TicksBothSides);
    m_slider->setTickInterval(50);
    m_slider->setMaximumWidth(200);

    m_colorBox = new ColorBox();
    m_colorBox->show();
    m_editor->m_colorBox = m_colorBox;

    // Create menu bar
    QMenuBar* menuBar = this->menuBar();

    // Add menus
    QMenu* fileMenu = menuBar->addMenu("File");
    QMenu* editMenu = menuBar->addMenu("Edit");
    QMenu* pcbMenu = menuBar->addMenu("PCB");
    QMenu* viewMenu = menuBar->addMenu("View");

    // Add menu actions
    QAction* setFrontSideImageAction = new QAction("Set front side image", this);
    connect(setFrontSideImageAction, &QAction::triggered, this, &MainWindow::setFrontSideImage);
    pcbMenu->addAction(setFrontSideImageAction);

    QAction* setBackSideImageAction = new QAction("Set back side image", this);
    connect(setBackSideImageAction, &QAction::triggered, this, &MainWindow::setBackSideImage);
    pcbMenu->addAction(setBackSideImageAction);

    pcbMenu->addSeparator();
    QAction* openPreferencesAction = new QAction("Preferences", this);
    connect(openPreferencesAction, &QAction::triggered, this, &MainWindow::openConfigDialog);
    pcbMenu->addAction(openPreferencesAction);

    pcbMenu->addSeparator();
    QAction* viewConnectionsAction = new QAction("View Connections", this);
    connect(viewConnectionsAction, &QAction::triggered, this, &MainWindow::viewConnections);
    pcbMenu->addAction(viewConnectionsAction);

    QAction* newAction = new QAction("New", this);
    connect(newAction, &QAction::triggered, this, &MainWindow::newProject);
    fileMenu->addAction(newAction);

    QAction* loadAction = new QAction("Load", this);
    connect(loadAction, &QAction::triggered, this, &MainWindow::handleLoadProject);
    fileMenu->addAction(loadAction);

    QAction* saveAction = new QAction("Save", this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveProject);
    fileMenu->addAction(saveAction);

    QAction* saveAsAction = new QAction("Save As", this);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveProjectAs);
    fileMenu->addAction(saveAsAction);

    fileMenu->addSeparator();

    QAction* exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exit);
    fileMenu->addAction(exitAction);


    QAction* undoAction = new QAction("Undo", this);
    undoAction->setShortcut(Qt::Key_Z);
    connect(undoAction, &QAction::triggered, &(m_editor->m_undoStack), &QUndoStack::undo);
    editMenu->addAction(undoAction);

    QAction* redoAction = new QAction("Redo", this);
    redoAction->setShortcut(Qt::Key_Y);
    connect(redoAction, &QAction::triggered, &(m_editor->m_undoStack), &QUndoStack::redo);
    editMenu->addAction(redoAction);

    editMenu->addSeparator();

    QAction* resetLinkWidthAction = new QAction("Reset Track Width", this);
    connect(resetLinkWidthAction, &QAction::triggered, this, &MainWindow::resetLinkWidth);
    editMenu->addAction(resetLinkWidthAction);

    // Create layout and add widgets
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout* topLayout = new QHBoxLayout();
    QHBoxLayout* bottomLayout = new QHBoxLayout();

    topLayout->addWidget(m_toolbar);
    topLayout->addStretch();
    topLayout->addWidget(m_sideToolbar);
    mainLayout->addLayout(topLayout);

    topLayout->addWidget(m_slider);
    mainLayout->addWidget(m_editor);
    mainLayout->addLayout(bottomLayout);
    bottomLayout->addWidget(statusBar);
    bottomLayout->addWidget(m_colorBox);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(0);

    m_sidebar = new Sidebar(this);
    m_sidebar->setWindowTitle("Elements");
    addDockWidget(Qt::RightDockWidgetArea, m_sidebar);

    setCentralWidget(centralWidget);

    statusBar->showMessage("Ready");

    resize(800, 600);

    QAction* toggleAction = m_sidebar->toggleViewAction();
    viewMenu->addAction(toggleAction);
    viewMenu->addSeparator();
    // Add the About action to the View menu
    QAction* aboutAction = new QAction("About", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);
    viewMenu->addAction(aboutAction);


}

MainWindow::~MainWindow() {
    qDebug() << "MainWindow::~MainWindow()";
    delete m_editor;
    delete m_sidebar;
    delete m_colorBox;
    delete m_slider;
    delete m_toolbar;
    delete m_sideToolbar;    
}

void MainWindow::sliderValueChanged(int value)
{
    m_editor->m_guideTool->setLayerOpacity(LinkSide::BACK, (100-value)/100.0);
}

void MainWindow::exit()
{
    close();
}

void MainWindow::viewConnections() {
    ConnectionAnalyzer::getConnections();
}

void MainWindow::cleanProject() {
    Editor::instance()->clean();
    setCurrentFilePath("");
}

void MainWindow::newProject() {

    if (!Editor::instance()->m_undoStack.isClean()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Unsaved Changes",
                                                                  "You have unsaved changes. Do you want to discard them?",
                                                                  QMessageBox::Yes | QMessageBox::No,
                                                                  QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            cleanProject();
        }
    } else {
        cleanProject();
    }
}

void MainWindow::saveProject()
{
    if (!m_currentFilePath.isEmpty()) {
        if (m_currentFilePath.endsWith(".jpcb", Qt::CaseInsensitive)) {
            SceneLoader::saveSceneToJson(m_currentFilePath);
        } else if (m_currentFilePath.endsWith(".pcb", Qt::CaseInsensitive)) {
            SceneLoaderBinary::saveSceneToBinary(m_currentFilePath);        
        }
    } else {
        saveProjectAs();
    }
}

void MainWindow::saveProjectAs()
{
        QString fileFilter;
        fileFilter = "PCB JSON Files (*.jpcb);;PCB Binary Files (*.pcb);;PCB Files (*.pcb *.jpcb)";
        QString filePath = QFileDialog::getSaveFileName(this, "Save PCB work", "", fileFilter);
        if (!filePath.isEmpty()) {
            bool success = false;
            if (filePath.endsWith(".jpcb", Qt::CaseInsensitive)) {
                success = SceneLoader::saveSceneToJson(filePath);
            } else if (filePath.endsWith(".pcb", Qt::CaseInsensitive)) {
            success = SceneLoaderBinary::saveSceneToBinary(filePath);
        } else {
            // If no extension was provided, default to .pcb
            filePath += ".pcb";
            success = SceneLoaderBinary::saveSceneToBinary(filePath);
        }

        if (success) {
            setCurrentFilePath(filePath);
            m_editor->showStatusMessage("Project saved successfully.");
            // QMessageBox::information(this, "Save Successful", "The project was saved successfully.");
        } else {
            QMessageBox::critical(this, "Save Failed", "Failed to save the project. Please try again.");
        }
    }
}

void MainWindow::handleLoadProject() {
    if (!Editor::instance()->m_undoStack.isClean()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Unsaved Changes",
                                                                  "You have unsaved changes. Do you want to discard them?",
                                                                  QMessageBox::Yes | QMessageBox::No,
                                                                  QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            loadProject();
        } else {
            loadProject();
        }
    } else {
        loadProject();
    }
}

void MainWindow::loadProject() {
        QString fileFilter;
        fileFilter = "PCB JSON Files (*.jpcb);;PCB Binary Files (*.pcb);;PCB Files (*.pcb *.jpcb)";
        QString filePath = QFileDialog::getOpenFileName(this, "Load PCB work", "", fileFilter);
        if (!filePath.isEmpty()) {
            bool success = false;
            Editor::instance()->clean();
            if (filePath.endsWith(".jpcb", Qt::CaseInsensitive)) {
                success = SceneLoader::loadSceneFromJson(filePath);
            } else
            if (filePath.endsWith(".pcb", Qt::CaseInsensitive)) {
            success = SceneLoaderBinary::loadSceneFromBinary(filePath);
        } else {
            QMessageBox::warning(this, "Unsupported File", "The selected file format is not supported.");
            return;
        }

        if (success) {
            setCurrentFilePath(filePath);
            m_editor->showStatusMessage("Project loaded successfully.");
            // QMessageBox::information(this, "Load Successful", "The project was loaded successfully.");
        } else {
            QMessageBox::critical(this, "Load Failed", "Failed to load the project. Please check the file and try again.");
        }
    }        
}

void MainWindow::setFrontSideImage()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Image");
    if (!filePath.isEmpty()) {
        m_editor->m_guideTool->setImageLayer(LinkSide::FRONT, filePath);
    }

}

void MainWindow::setBackSideImage()
{    
    QString filePath = QFileDialog::getOpenFileName(this, "Open Image");
    if (!filePath.isEmpty()) {
        m_editor->m_guideTool->setImageLayer(LinkSide::BACK, filePath);
    }

}

void MainWindow::createToolbarActions()
{
    QActionGroup* actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);

    m_addTrackAction = actionGroup->addAction("Tracks");
    m_addTrackAction->setCheckable(true);
    m_addTrackAction->setChecked(true);
    connect(m_addTrackAction, &QAction::toggled, this, &MainWindow::addTrackButtonAction);
    m_toolbar->addAction(m_addTrackAction);

    m_addComponentAction = actionGroup->addAction("Components");
    m_addComponentAction->setCheckable(true);
    connect(m_addComponentAction, &QAction::toggled, this, &MainWindow::addComponentButtonAction);
    m_toolbar->addAction(m_addComponentAction);

    m_addNotesAction = actionGroup->addAction("Notes");
    m_addNotesAction->setCheckable(true);
    connect(m_addNotesAction, &QAction::toggled, this, &MainWindow::addNotesButtonAction);
    m_toolbar->addAction(m_addNotesAction);

    QAction* zoomInAction = new QAction(QIcon::fromTheme("zoom-in"), "Zoom In", this);
    connect(zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);
    m_toolbar->addAction(zoomInAction);

    QAction* zoomOutAction = new QAction(QIcon::fromTheme("zoom-out"), "Zoom Out", this);
    connect(zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);
    m_toolbar->addAction(zoomOutAction);

    m_flipHAction = new QAction(QIcon::fromTheme("object-flip-horizontal", QIcon(":/icons/icons/flip-horizontal.svg")), "Flip Horizontally", this);
    m_flipHAction->setCheckable(true);
    connect(m_flipHAction, &QAction::triggered, m_editor, &Editor::flipHorizontal);
    m_toolbar->addAction(m_flipHAction);

    m_flipVAction = new QAction(QIcon::fromTheme("object-flip-vertical", QIcon(":/icons/icons/flip-vertical.svg")), "Flip Vertically", this);
    m_flipVAction->setCheckable(true);
    connect(m_flipVAction, &QAction::triggered, m_editor, &Editor::flipVertical);
    m_toolbar->addAction(m_flipVAction);

    QActionGroup* sideGroup = new QActionGroup(this);
    sideGroup->setExclusive(false);

    m_frontSideAction = sideGroup->addAction("Front");
    m_frontSideAction->setCheckable(true);
    m_frontSideAction->setChecked(true);
    connect(m_frontSideAction, &QAction::toggled, this, &MainWindow::frontSideToggleButtonAction);
    m_sideToolbar->addAction(m_frontSideAction);

    m_backSideAction = sideGroup->addAction("Back");
    m_backSideAction->setCheckable(true);
    m_backSideAction->setChecked(true);
    connect(m_backSideAction, &QAction::toggled, this, &MainWindow::backSideToggleButtonAction);
    m_sideToolbar->addAction(m_backSideAction);
}

void MainWindow::resetLinkWidth()
{
    //m_editor->resetLinkWidth();
}

void MainWindow::backSideToggleButtonAction(bool checked)
{
    m_editor->toggleLayerVisibility(LinkSide::BACK, checked);
}

void MainWindow::frontSideToggleButtonAction(bool checked)
{
    m_editor->toggleLayerVisibility(LinkSide::FRONT, checked);
}

void MainWindow::addTrackButtonAction(bool checked)
{
    if (checked) {
        qDebug() << "Add Track mode activated";
        m_editor->enterTrackMode();
    }
}

void MainWindow::addComponentButtonAction(bool checked)
{
    if (checked) {
        qDebug() << "Add Component mode activated";
        m_editor->enterComponentMode();
    }
}

void MainWindow::addNotesButtonAction(bool checked)
{
    if (checked) {
        qDebug() << "Add Notes mode activated";
        m_editor->enterNotesMode();
    }
}

void MainWindow::zoomIn()
{
   m_editor->zoomIn();
}

void MainWindow::zoomOut()
{
    m_editor->zoomOut();
}

void MainWindow::openConfigDialog()
{
        ConfigDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            QVariantMap newConfig = dialog.getConfigData();
            Config::instance()->updateFromConfigDialog(newConfig);
            Config::instance()->apply();
        }
}

void MainWindow::setCurrentFilePath(const QString& filePath)
{
    m_currentFilePath = filePath;
    if(m_currentFilePath.isEmpty()) {
        setWindowTitle(m_windowBaseTitle);
    } else {
        setWindowTitle(m_windowBaseTitle + " - " + m_currentFilePath);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // If the undo stack is not clean (changes were made), prompt the user
    if (!m_editor->m_undoStack.isClean()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Unsaved Changes",
                                                                  "You have unsaved changes. Do you want to discard them?",
                                                                  QMessageBox::Yes | QMessageBox::No,
                                                                  QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_E && event->modifiers() & Qt::ControlModifier) {
        ConnectionAnalyzer::getConnections();
    }
    else if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier) {
        Editor::instance()->enterComponentMode();
        m_addComponentAction->toggle();
    } else if (event->key() == Qt::Key_T && event->modifiers() & Qt::ControlModifier) {
        Editor::instance()->enterTrackMode();
        m_addTrackAction->toggle();
    } else if (event->key() == Qt::Key_N && event->modifiers() & Qt::ControlModifier) {
        Editor::instance()->enterNotesMode();
        m_addNotesAction->toggle();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
   // m_editor->keyReleaseEvent(event);
}

void MainWindow::showAboutDialog()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle("About PCB Tracer");
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.setText("<h2>PCB Tracer</h2> \
                     <p>This tool is designed to aid in reverse engineering, repair, and documentation of printed circuit boards.</p> \
                     <p>Version 1.0</p> \
                     <p>Made with with <a href='https://www.qt.io'>QT</a>.</p> \
                     <p>Pull requests and issues are welcome! <a href='https://github.com/rpelorosso/pcb-tracer'>GitHub Repository</a></p>");
    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.exec();
}
