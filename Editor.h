#ifndef EDITOR_H
#define EDITOR_H

#include <QGraphicsView>
#include <QUndoStack>
#include <QKeyEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <unordered_map>
#include <vector>
#include <QStatusBar>
#include "TypeChecks.h"

/*
#include "component.h"
#include "link.h"
#include "node.h"
#include "pad.h"
#include "image_layer.h"
#include "text_note.h"
#include "guide_tool.h"
*/
#include "CommunicationHub.h"
#include "IEditorTool.h"
#include "enums.h"
#include "ZoomableGraphicsView.h"
#include "QGraphicsItemLayer.h"
#include "ColorBox.h"
#include "GuideTool.h"
#include "TrackDrawingTool.h"

class ComponentDrawingTool;
class NotesTool;
class ImageTransformTool;
class ImageLayer;

class Editor : public ZoomableGraphicsView {
	Q_OBJECT

public:

    static Editor* instance(QWidget* parent = nullptr);

	QGraphicsLineItem* m_tracingIndicator;
	Editor(QWidget* parent = nullptr);
	~Editor();
	void showTracingIndicator();
	void hideTracingIndicator();
	LinkSide getCurrentSide() const { return m_currentSide; }
	void setCurrentSide(LinkSide side);
	void showStatusMessage(const QString& message);
	void zoomIn();
	void zoomOut();
	void toggleLayerVisibility(LinkSide side, bool visible);	
	void setCurrentTool(IEditorTool*);
	void enterComponentMode();
	void enterTrackMode();
	void enterNotesMode();
	void enterImageTransformMode(LinkSide side);
	void saveSceneToJson(const QString& filename);
	void loadSceneFromJson(const QString& filename);
	void setStatusBar(QStatusBar* statusBar);
	void clean();
	TrackDrawingTool *getTrackDrawingTool();

	int padSize;
	LinkSide m_currentSide;
	QUndoStack m_undoStack;
	QMap<LinkSide, QGraphicsItemLayer*> m_layers;
	QGraphicsScene *getScene() const { return m_scene; }
	DrawingState m_state;
	ColorBox* m_colorBox;
	GuideTool* m_guideTool;
	NotesTool* m_notesTool;

	template<typename T>
    T* findItemByIdAndClass(int itemId)
    {
        QList<QGraphicsItem*> items = scene()->items();
        for (QGraphicsItem* item : items)
        {
            if (isDynamicCastableToAny<T>(item))
            {
                T* castedItem = dynamic_cast<T*>(item);
                if (castedItem && castedItem->m_id == itemId)
                {
                    return castedItem;
                }
            }
        }
        return nullptr;
    }



protected:
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	QGraphicsLineItem* addTracingIndicator();

private:
	static Editor* m_instance;
	QGraphicsScene* m_scene;
	IEditorTool* m_currentTool;
	TrackDrawingTool* m_trackDrawingTool;
	ComponentDrawingTool* m_componentDrawingTool;
	ImageTransformTool* m_imageTransformTool = nullptr;

	QStatusBar* m_statusBar;
	
	/*QObject* currentEditor;

	QJsonObject getSceneElements();
	void showStatusMessage(const QString& message);
	
	*/
};

#endif // EDITOR_H

