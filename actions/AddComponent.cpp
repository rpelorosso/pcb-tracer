#include "AddComponent.h"
#include "../Editor.h"
#include "../Component.h"

AddComponent::AddComponent(const AddComponentMeta& meta)
    : QUndoCommand(), m_meta(meta)
{
    qDebug() << m_componentId;

    m_scene = Editor::instance()->scene();

    m_component = new Component(m_meta.m_name, Component::genComponentId());

    // Add pads
    for (size_t i = 0; i < meta.m_pads.size(); ++i) {
        m_component->addPad(meta.m_pads[i]);
    }

    qDebug() << meta.m_name;
}

void AddComponent::undo()
{
    // Delete the component
    m_component->remove(m_scene);
    CommunicationHub::instance().publish(HubEvent::COMPONENT_DELETED, m_component);

    // prevent trying to draw a line from the deleted component
    Editor *editor = Editor::instance();
    TrackDrawingTool *trackDrawingTool = editor->getTrackDrawingTool();        
    trackDrawingTool->m_drawingLineFrom = nullptr;

}

void AddComponent::redo()
{
    // Add the component to the scene    
    m_component->addToScene(m_scene);
    CommunicationHub::instance().publish(HubEvent::COMPONENT_CREATED, m_component);
}
