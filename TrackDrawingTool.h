#ifndef TRACKDRAWINGTOOL_H
#define TRACKDRAWINGTOOL_H

#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QMessageBox>
#include <QPoint>
#include <QUndoStack>
#include "Editor.h"
#include "Link.h"
#include "Node.h"
#include "enums.h"
#include "IEditorTool.h"

class TrackDrawingTool : public IEditorTool
{

public:
    TrackDrawingTool(Editor* editor);

    void enterMode();
    bool onMousePress(QMouseEvent* event);
    bool onMouseMove(QMouseEvent* event);
    bool onMouseDoubleClick(QMouseEvent* event);
    bool onKeyPress(QKeyEvent* event);
    bool onKeyRelease(QKeyEvent* event);
    QPointF m_lastDraggedPosition;
    void clean();

private:
    void startDrawing(QMouseEvent* event);
    void closeDrawing(QMouseEvent* event);
    void toggleHighlightSubCircuit(int graphId, bool isHighlighted);

    Editor* m_editor;
    QGraphicsItem* m_drawingLineFrom;
    QPointF m_drawingLineFromPos;
    bool m_drawing;
    QGraphicsItem* m_selectedItem;
    int m_highlighted_sub_circuit;

};

#endif // TRACKDRAWINGTOOL_H