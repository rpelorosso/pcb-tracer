#ifndef IEDITOR_TOOL_H
#define IEDITOR_TOOL_H

#include <QPoint>
#include <QMouseEvent>
#include <QKeyEvent>

class Editor;

class IEditorTool {
public:
    virtual ~IEditorTool() = default;
    virtual void enterMode() {}
    virtual void exitMode() {}
    virtual bool onMousePress(QMouseEvent* event) { return false; }
    virtual bool onMouseMove(QMouseEvent* event) { return false; }
    virtual bool onMouseDoubleClick(QMouseEvent* event) { return false; };
    virtual bool onKeyPress(QKeyEvent* event) { return false; };
    virtual bool onKeyRelease(QKeyEvent* event) { return false; };
    virtual bool onMouseRelease(QMouseEvent* event) { return false; };
    

protected:
    Editor* m_editor = nullptr;
};

#endif // IEDITOR_TOOL_H