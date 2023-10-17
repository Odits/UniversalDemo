cmake_minimum_required(VERSION 3.5)

project(UniversalDemo)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
	main.cpp
	mainwindow.cpp
	mainwindow.h
	mainwindow.ui
	DynamicLib.h
	flowlayout.cpp
	flowlayout.h
	funcData_Button.h
	funcData_Button.cpp
)

add_executable(UniversalDemo ${PROJECT_SOURCES})
target_link_libraries(UniversalDemo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

add_library(TestLibrary SHARED TestLibrary/TestLibrary.h TestLibrary/TestLibrary.cpp)

IF (NOT WIN32)
	target_link_libraries(UniversalDemo PRIVATE -ldl)
	target_link_libraries(TestLibrary -ldl)
ENDIF ()