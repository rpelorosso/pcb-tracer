#include "NotesTool.h"
#include "Editor.h"
#include "Config.h"
#include "CommunicationHub.h"
#include <QInputDialog>
#include <QPainter>
#include <QGraphicsScene>

int NotesTool::note_count = 0;

TextNote::TextNote(const QRectF& rect, const QColor& color, QGraphicsItem* parent)
    : QGraphicsRectItem(rect, parent), m_color(color) {
    setPen(QPen(m_color, 4, Qt::SolidLine));
    m_textItem = new QGraphicsTextItem(this);
    m_textItem->setFont(QFont("Arial", 10));
    m_textItem->setDefaultTextColor(m_color);
    setText("");
}

void TextNote::setText(const QString& text) {
    m_textItem->setPlainText(text);
    m_text = text;
    adjustTextPos();
}

void TextNote::adjustTextPos() {
    QRectF textRect = m_textItem->boundingRect();
    QRectF rect = this->rect();
    m_textItem->setPos(rect.left() + 5, rect.top() + 5);
}

void TextNote::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    QGraphicsRectItem::paint(painter, option, widget);
    adjustTextPos();
}

NotesTool::NotesTool(Editor* editor)
    : m_editor(editor), m_drawing(false), m_currentNote(nullptr), m_startPos() {}

void NotesTool::enterMode() {
    m_editor->setCursor(Qt::CrossCursor);
}

bool NotesTool::onMousePress(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startDrawing(event);
    } else if (event->button() == Qt::RightButton) {
        deleteNote(event);
    }
    return false;
}

bool NotesTool::onMouseMove(QMouseEvent* event) {
    if (m_drawing) {
       updateDrawing(event);
    }
    return false;
}

bool NotesTool::onMouseRelease(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_drawing) {
        finishDrawing(event);
    }
    return false;
}

void NotesTool::startDrawing(QMouseEvent* event) {
    m_drawing = true;
    m_startPos = m_editor->mapToScene(event->pos());
    QColor color = Config::instance()->color(Color::NOTES);
    m_currentNote = new TextNote(QRectF(m_startPos, m_startPos), color);
    m_currentNote->setParentItem(m_editor->m_layers[LinkSide::NOTES]);
}

void NotesTool::updateDrawing(QMouseEvent* event) {
    if (m_currentNote) {
        QPointF endPos = m_editor->mapToScene(event->pos());
        QRectF rect = QRectF(m_startPos, endPos).normalized();
        m_currentNote->setRect(rect);
    }
}

void NotesTool::finishDrawing(QMouseEvent* event) {
    m_drawing = false;
    if (m_currentNote) {
        bool ok;
        QString text = QInputDialog::getText(m_editor, "Add Note", "Enter note text:", QLineEdit::Normal, "", &ok);
        if (ok) {
            m_currentNote->setText(text);
            m_currentNote->m_id = genNoteId();
            m_notes.push_back(m_currentNote);
            CommunicationHub::instance().publish(HubEvent::NOTE_CREATED, m_currentNote);
        } else {
            m_editor->scene()->removeItem(m_currentNote);
            delete m_currentNote;
        }
        m_currentNote = nullptr;
    }
}

void NotesTool::deleteNote(QMouseEvent* event) {
    QGraphicsItem* item = m_editor->itemAt(event->pos());
    if (auto note = dynamic_cast<TextNote*>(item)) {
        m_editor->scene()->removeItem(note);
        m_notes.erase(std::remove(m_notes.begin(), m_notes.end(), note), m_notes.end());
        CommunicationHub::instance().publish(HubEvent::NOTE_DELETED, note);
        delete note;
    }
}

void NotesTool::cancelDrawing() {
    if (m_currentNote) {
        m_editor->scene()->removeItem(m_currentNote);
        delete m_currentNote;
        m_currentNote = nullptr;
    }
    m_drawing = false;
}