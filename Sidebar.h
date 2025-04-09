#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QDockWidget>
#include <QTabWidget>
#include <QListWidget>
#include <QIcon>
#include "Node.h"
#include "NotesTool.h"
#include "Component.h"

//#include "notes_tool.h"
#include "CommunicationHub.h"

class Sidebar : public QDockWidget {
    

public:
    Sidebar(QWidget* parent = nullptr);

private:
    void setupUi();
    void addTabsToDockTabs();
    void setupConnections();
    void onListItemDoubleClicked(QListWidgetItem* item);
    std::pair<QListWidgetItem*, int> findItemById(QListWidget* listWidget, const int itemId);
    void nodeEventHandler(const Node* node, HubEvent action);
    void noteEventHandler(TextNote* note, HubEvent action);
    void onSceneCleanHandler(void* data, HubEvent action);
    void componentEventHandler(Component* component, HubEvent action);
    void addComponentToList(Component* component, QListWidget* listWidget);
    void removeComponentFromList(Component* component, QListWidget* listWidget);
    void addNoteToList(TextNote* note, QListWidget* listWidget);
    void removeNoteFromList(TextNote* note, QListWidget* listWidget);

    QTabWidget* m_dockTabs;
    QListWidget* m_tab1;
    QListWidget* m_tab2;
    QListWidget* m_tab3;
};

#endif // SIDEBAR_H