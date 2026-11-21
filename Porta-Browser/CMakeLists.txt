cmake_minimum_required(VERSION 3.21)
project(Porta-Browser VERSION 0.0.5.2 LANGUAGES CXX)

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

add_executable(Porta-Browser
        main.cpp
        MainWindow.cpp
        MainWindow.h
        DatabaseManager.cpp
        DatabaseManager.h
        resources.qrc
)

target_link_libraries(Porta-Browser PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::WebEngineWidgets
        Qt6::Sql
)

install(TARGETS Porta-Browser
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES porta-browser.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES porta-browser.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps)

set(CPACK_PACKAGE_NAME "porta-browser")
set(CPACK_PACKAGE_VENDOR "Joshi7479")
set(CPACK_PACKAGE_CONTACT "Joshi7479@proton.me")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple, fast, lightweight Browser")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_GENERATOR "DEB;RPM;TGZ")

set(CPACK_DEBIAN_PACKAGE_NAME "porta-browser")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Joshi7479 <Joshi7479@proton.me>")
set(CPACK_DEBIAN_PACKAGE_SECTION "web")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Porta Browser - a simple, fast, lightweight browser")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6sql6-sqlite")

set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)

include(CPack)

set(LINUXDEPLOY "$ENV{HOME}/tools/linuxdeploy-x86_64.AppImage"
        CACHE FILEPATH "Pfad zu linuxdeploy")

add_custom_target(appimage
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/AppDir
        COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix /usr
        DESTDIR=${CMAKE_BINARY_DIR}/AppDir
        COMMAND ${CMAKE_COMMAND} -E env
        QMAKE=${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}/qmake
        EXTRA_QT_PLUGINS=sqldrivers\;platforms\;tls\;imageformats
        LDAI_OUTPUT=Porta-Browser-x86_64.AppImage
        APPIMAGE_EXTRACT_AND_RUN=1
        ${LINUXDEPLOY}
        --appdir ${CMAKE_BINARY_DIR}/AppDir
        --plugin qt
        --output appimage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS Porta-Browser
        COMMENT "Generate AppImage"
        VERBATIM
)