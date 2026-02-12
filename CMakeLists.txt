cmake_minimum_required(VERSION 3.16)
project(pcb_tracer)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets SvgWidgets REQUIRED)

option(USE_OPENGL "Enable OpenGL support" OFF)

if(USE_OPENGL)
    add_definitions(-DUSE_OPENGL)
    find_package(Qt6 COMPONENTS OpenGL REQUIRED)
    find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
endif()

add_executable(pcb_tracer
	main.cpp
	Sidebar.cpp
	Sidebar.h
	MainWindow.cpp
	MainWindow.h
    CommunicationHub.h
	Editor.cpp
	Editor.h
	TrackDrawingTool.cpp
	TrackDrawingTool.h
	ComponentDrawingTool.cpp
	ComponentDrawingTool.h
	NotesTool.cpp
	NotesTool.h
	IEditorTool.h
	ImageLayer.cpp
	ImageLayer.h
	Node.cpp
	Node.h
	Link.cpp
	Link.h
	Component.cpp
	Component.h
	Config.cpp
	Config.h
	ColorBox.cpp
	ColorBox.h
	GuideTool.cpp
    GuideTool.h
    SceneLoader.cpp
    SceneLoader.h
	ZoomableGraphicsView.cpp
	ZoomableGraphicsView.h
	QGraphicsItemLayer.cpp
	QGraphicsItemLayer.h
	SceneLoaderBinary.cpp
	SceneLoaderBinary.h
	ConfigDialog.cpp
	ConfigDialog.h
	ConnectionAnalyzer.cpp
	ConnectionAnalyzer.h
	actions/AddTrack.cpp
	actions/AddTrack.h
	actions/MoveNode.cpp
	actions/MoveNode.h
	actions/DeleteTrack.cpp
	actions/DeleteTrack.h
	actions/AssignSideToTrack.cpp
	actions/AssignSideToTrack.h
	actions/AddComponent.cpp
	actions/AddComponent.h
	resources.qrc
)

target_link_libraries(pcb_tracer PRIVATE Qt6::Widgets Qt6::SvgWidgets)

if(USE_OPENGL)
    target_link_libraries(pcb_tracer PRIVATE Qt6::OpenGLWidgets)
endif()

install(TARGETS pcb_tracer DESTINATION bin)
