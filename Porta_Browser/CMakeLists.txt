cmake_minimum_required(VERSION 4.4)
project(Porta_Browser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        WebView
        WebEngineWidgets
        REQUIRED)

add_executable(Porta_Browser main.cpp
        MainWindow.cpp
        MainWindow.h)
target_link_libraries(Porta_Browser
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::WebView
        Qt6::WebEngineWidgets
)

