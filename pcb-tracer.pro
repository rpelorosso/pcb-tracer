# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = pcb-tracer

QT = core gui widgets

HEADERS = \
   $$PWD/actions/AddComponent.h \
   $$PWD/actions/AddTrack.h \
   $$PWD/actions/AssignSideToTrack.h \
   $$PWD/actions/DeleteTrack.h \
   $$PWD/actions/MoveNode.h \
   $$PWD/ColorBox.h \
   $$PWD/CommunicationHub.h \
   $$PWD/Component.h \
   $$PWD/ComponentDrawingTool.h \
   $$PWD/Config.h \
   $$PWD/ConfigDialog.h \
   $$PWD/ConnectionAnalyzer.h \
   $$PWD/Editor.h \
   $$PWD/enums.h \
   $$PWD/GuideTool.h \
   $$PWD/IEditorTool.h \
   $$PWD/ImageLayer.h \
   $$PWD/Link.h \
   $$PWD/MainWindow.h \
   $$PWD/Node.h \
   $$PWD/NotesTool.h \
   $$PWD/QGraphicsItemLayer.h \
   $$PWD/SceneLoader.h \
   $$PWD/SceneLoaderBinary.h \
   $$PWD/Sidebar.h \
   $$PWD/TrackDrawingTool.h \
   $$PWD/TypeChecks.h \
   $$PWD/ZoomableGraphicsView.h

SOURCES = \
   $$PWD/actions/AddComponent.cpp \
   $$PWD/actions/AddTrack.cpp \
   $$PWD/actions/AssignSideToTrack.cpp \
   $$PWD/actions/DeleteTrack.cpp \
   $$PWD/actions/MoveNode.cpp \
   $$PWD/ColorBox.cpp \
   $$PWD/Component.cpp \
   $$PWD/ComponentDrawingTool.cpp \
   $$PWD/Config.cpp \
   $$PWD/ConfigDialog.cpp \
   $$PWD/ConnectionAnalyzer.cpp \
   $$PWD/Editor.cpp \
   $$PWD/GuideTool.cpp \
   $$PWD/ImageLayer.cpp \
   $$PWD/Link.cpp \
   $$PWD/main.cpp \
   $$PWD/MainWindow.cpp \
   $$PWD/Node.cpp \
   $$PWD/NotesTool.cpp \
   $$PWD/QGraphicsItemLayer.cpp \
   $$PWD/SceneLoader.cpp \
   $$PWD/SceneLoaderBinary.cpp \
   $$PWD/Sidebar.cpp \
   $$PWD/TrackDrawingTool.cpp \
   $$PWD/ZoomableGraphicsView.cpp

INCLUDEPATH = \
    $$PWD/. \
    $$PWD/actions

#DEFINES = 

RESOURCES += resources.qrc
