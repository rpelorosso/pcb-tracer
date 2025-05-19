#include "Sidebar.h"
#include "Editor.h"
#include <QListWidgetItem>
#include "CommunicationHub.h"
#include "NotesTool.h"

Sidebar::Sidebar(QWidget* parent)  {
    setupUi();
    setupConnections();
}

void Sidebar::setupUi() {
    m_dockTabs = new QTabWidget(this);
    setWidget(m_dockTabs);
    addTabsToDockTabs();
}

void Sidebar::addTabsToDockTabs() {
/*
    QIcon icon1 = QIcon::fromTheme("document-new");
    QIcon icon2 = QIcon::fromTheme("edit-paste");
    QIcon icon3 = QIcon::fromTheme("notepad");
*/
    m_tab1 = new QListWidget(this);
    m_dockTabs->addTab(m_tab1,  "Comps");
    m_dockTabs->setTabToolTip(0, "Components");

    m_tab2 = new QListWidget(this);
    m_dockTabs->addTab(m_tab2, "NoConn");
    m_dockTabs->setTabToolTip(1, "Isles with just one link");

    m_tab3 = new QListWidget(this);
    m_dockTabs->addTab(m_tab3, "Notes");
    m_dockTabs->setTabToolTip(2, "Notes");
}

void Sidebar::setupConnections() {
    connect(m_tab1, &QListWidget::itemDoubleClicked, this, &Sidebar::onListItemDoubleClicked);
    connect(m_tab2, &QListWidget::itemDoubleClicked, this, &Sidebar::onListItemDoubleClicked);
    connect(m_tab3, &QListWidget::itemDoubleClicked, this, &Sidebar::onListItemDoubleClicked);

    CommunicationHub::instance().subscribe(HubEvent::NODE_MADE_MULTIPLE_LINKS, [this](HubEvent event, const void* data) { this->nodeEventHandler((Node*)data, event); });
    CommunicationHub::instance().subscribe(HubEvent::NODE_MADE_SINGLE_LINK, [this](HubEvent event, const void* data) { this->nodeEventHandler((Node*)data, event); });
    CommunicationHub::instance().subscribe(HubEvent::NODE_DELETED, [this](HubEvent event, const void* data) { this->nodeEventHandler((Node*)data, event); });
    

    CommunicationHub::instance().subscribe(HubEvent::COMPONENT_CREATED, [this](HubEvent event, const void* data) { this->componentEventHandler((Component*)data, event); });
    CommunicationHub::instance().subscribe(HubEvent::COMPONENT_DELETED, [this](HubEvent event, const void* data) { this->componentEventHandler((Component*)data, event); });

    CommunicationHub::instance().subscribe(HubEvent::NOTE_CREATED, [this](HubEvent event, const void* data) { this->noteEventHandler((TextNote*)data, event); });
    CommunicationHub::instance().subscribe(HubEvent::NOTE_DELETED, [this](HubEvent event, const void* data) { this->noteEventHandler((TextNote*)data, event); });

    CommunicationHub::instance().subscribe(HubEvent::SCENE_CLEAN, [this](HubEvent event, const void* data) { this->onSceneCleanHandler((void*)data, event); });

}

void Sidebar::onListItemDoubleClicked(QListWidgetItem* item) {
    QVariant data = item->data(Qt::UserRole);
    //qDebug() << "Data type:" << data.typeName();
    if (data.canConvert<const Node*>()) {        
        //qDebug() << "Double-clicked on Node";
        const Node* node = data.value<const Node*>();
        //qDebug() << "Node ID:" << node->m_id;
        Editor::instance()->centerOn(node);
    } else if (data.canConvert<Component*>()) {
        Component* component = data.value<Component*>();
        Editor::instance()->centerOn(component->m_pads[0]);
    } else if (data.canConvert<TextNote*>()) {
        TextNote* note = data.value<TextNote*>();
        qDebug() << "Double-clicked on note";
        Editor::instance()->centerOn(note);
    }
}

std::pair<QListWidgetItem*, int> Sidebar::findItemById(QListWidget* listWidget, const int itemId) {
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem* item = listWidget->item(i);
        QVariant data = item->data(Qt::UserRole);

        if (data.canConvert<const Node*>()) {
            const Node* node = data.value<const Node*>();
            if (node->m_id == itemId) {
                return {item, i};
            }
        } else if (data.canConvert<Component*>()) {
            Component* component = data.value<Component*>();
            if (component->m_id == itemId) {
                return {item, i};
            }
        }
        else if (data.canConvert<TextNote*>()) {
            TextNote* note = data.value<TextNote*>();
            if (note->m_id == itemId) {
                return {item, i};
            }
        }
    }
    return {nullptr, -1};
}

void Sidebar::nodeEventHandler(const Node *node, HubEvent event) {
    if (event == HubEvent::NODE_MADE_SINGLE_LINK) {
        auto [existingItem, _] = findItemById(m_tab2, node->m_id);
        if (!existingItem) {
            QString item = QString("Node ID: %1").arg(node->m_id);
            QListWidgetItem* listItem = new QListWidgetItem(item);
            listItem->setData(Qt::UserRole, QVariant::fromValue(node));
            m_tab2->addItem(listItem);
        }
    } else if (event == HubEvent::NODE_MADE_MULTIPLE_LINKS || event == HubEvent::NODE_DELETED) {
        //qDebug() << "Node ID: " << node->m_id << " has been deleted";
        auto [_, index] = findItemById(m_tab2, node->m_id);
        //qDebug() << "encontro" << index;
        if (index != -1) {
            delete m_tab2->takeItem(index);
        }
    }
}

void Sidebar::componentEventHandler(Component* component, HubEvent action) {
    
    if (action == HubEvent::COMPONENT_CREATED) {
        addComponentToList(component, m_tab1);
    } else if (action == HubEvent::COMPONENT_DELETED) {
        removeComponentFromList(component, m_tab1);
    }
}

void Sidebar::noteEventHandler(TextNote* note, HubEvent action) {    
    if (action == HubEvent::NOTE_CREATED) {
        addNoteToList(note, m_tab3);
    } else if (action == HubEvent::NOTE_DELETED) {
        removeNoteFromList(note, m_tab3);
    }
}

void Sidebar::onSceneCleanHandler(void* data, HubEvent action) {    
    m_tab1->clear();
    m_tab2->clear();
    m_tab3->clear();
}

void Sidebar::addComponentToList(Component* component, QListWidget* listWidget) {
    qDebug() << "Component id:" << component->m_id << "- name:" << component->m_name << "created";
    auto [existingItem, _] = findItemById(listWidget, component->m_id);
    if (!existingItem) {
        qDebug() << "Adding it";
        QString item = QString("%1 - %2 pins").arg(component->m_name).arg(component->numberOfPads());
        QListWidgetItem* listItem = new QListWidgetItem(item);
        listItem->setData(Qt::UserRole, QVariant::fromValue(component));
        listWidget->addItem(listItem);
        listWidget->sortItems(Qt::AscendingOrder);
    } else {
        qDebug() << "Already exists";
    }
}

void Sidebar::removeComponentFromList(Component* component, QListWidget* listWidget) {    
    auto [item, index] = findItemById(listWidget, component->m_id);
    if (item) {
        delete listWidget->takeItem(index);
        qDebug() << "Removed component from list: Component ID:" << component->m_id;
    } else {
        qDebug() << "Component ID:" << component->m_id << "not found in list, ignoring deletion";
    }
}

void Sidebar::removeNoteFromList(TextNote* note, QListWidget* listWidget) {
    auto [item, index] = findItemById(listWidget, note->m_id);
    if (item) {
        delete listWidget->takeItem(index);
        qDebug() << "Removed note from list: Note ID:" << note->m_id;
    } else {
        qDebug() << "Note ID:" << note->m_id << "not found in list, ignoring deletion";
    }
}

void Sidebar::addNoteToList(TextNote* note, QListWidget* listWidget) {
    qDebug() << "Note id:" << note->m_id << "- text:" << note->m_text << "created";
    auto [existingItem, _] = findItemById(listWidget, note->m_id);
    if (!existingItem) {
        qDebug() << "Adding it";
        QString item = QString("%1").arg(note->m_text);
        QListWidgetItem* listItem = new QListWidgetItem(item);
        listItem->setData(Qt::UserRole, QVariant::fromValue(note));
        listWidget->addItem(listItem);
        listWidget->sortItems(Qt::AscendingOrder);
    } else {
        qDebug() << "Already exists";
    }
}