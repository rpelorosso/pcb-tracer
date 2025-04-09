#ifndef NOTESTOOL_H
#define NOTESTOOL_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QColor>
#include <QUuid>
#include <QPointF>
#include <vector>
#include "IEditorTool.h"

class Editor;

class TextNote : public QGraphicsRectItem {
public:
    TextNote(const QRectF& rect, const QColor& color, QGraphicsItem* parent = nullptr);
    void setText(const QString& text);
    void adjustTextPos();
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    QVariantMap toDict() const;
    static TextNote* fromDict(const QVariantMap& data);

    int m_id;
    QColor m_color;
    QGraphicsTextItem* m_textItem;
    QString m_text;
};

class NotesTool : public IEditorTool {
public:
    static int note_count;
    NotesTool(Editor* editor);
    void enterMode();
    bool onMousePress(QMouseEvent* event);
    bool onMouseMove(QMouseEvent* event);
    bool onMouseRelease(QMouseEvent* event);
    void startDrawing(QMouseEvent* event);
    void updateDrawing(QMouseEvent* event);
    void finishDrawing(QMouseEvent* event);
    void deleteNote(QMouseEvent* event);
    void cancelDrawing();

    Editor* m_editor;
    bool m_drawing;
    TextNote* m_currentNote;
    QPointF m_startPos;
    std::vector<TextNote*> m_notes;
    
    static int genNoteId() {
        return ++note_count;
    }

    static int getLastNoteId() {
        return note_count;
    }
    
    static void setNoteCount(int count) {
        note_count = count;
    }
};

#endif // NOTESTOOL_H