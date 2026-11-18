cmake_minimum_required(VERSION 3.21)
project(Porta_Browser VERSION 0.0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        WebEngineWidgets
        Sql
)

add_executable(Porta_Browser
        main.cpp
        MainWindow.cpp
        MainWindow.h
        DatabaseManager.cpp
        DatabaseManager.h
        resources.qrc
)

target_link_libraries(Porta_Browser PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::WebEngineWidgets
        Qt6::Sql
)

install(TARGETS Porta_Browser
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# CPack
set(CPACK_PACKAGE_NAME "Porta-Browser")
set(CPACK_PACKAGE_VENDOR "Joshi7479")
set(CPACK_PACKAGE_CONTACT "Joshi7479@proton.me")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple, fast, lightweight Browser")
set(CPACK_GENERATOR "DEB;RPM;TGZ")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Joshi7479 <Joshi7479@proton.me>")
set(CPACK_DEBIAN_PACKAGE_SECTION "web")
set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")


install(FILES porta-browser.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES icon.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps)


include(CPack)
