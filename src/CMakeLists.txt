cmake_minimum_required(VERSION 3.23)

project(
  qt_desktop-gallery
  VERSION 0.7.0
  DESCRIPTION "Desktop gallery-app to show/edit Exif/IPTC and export to WebP"
  HOMEPAGE_URL "https://github.com/Zheng-Bote/qt_desktop-gallery"
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER clang++)

# set(CMAKE_CXX_CLANG_TIDY
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # set(CMAKE_CXX_CLANG_TIDY clang-tidy; -header-filter=.;
  # -checks=cppcoreguidelines-*)
endif()

set(PROG_EXEC_NAME "${PROJECT_NAME}")
set(PROG_NAME "Desktop-Gallery")
set(PROG_VERSION "${PROJECT_VERSION}")
set(PROG_HOMEPAGE "${PROJECT_HOMEPAGE_URL}")
set(PROG_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(PROG_ORGANIZATION_NAME "ZHENG Robert")
set(PROG_ORGANIZATION_DOMAIN "hase-zheng.net")

configure_file("${CMAKE_SOURCE_DIR}/configure/rz_config.h.in"
               "${CMAKE_SOURCE_DIR}/includes/rz_config.h")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)

# find_package(exiv2 REQUIRED)
find_package(exiv2lib REQUIRED CONFIG NAMES exiv2)

set(PROJECT_SOURCES main.cpp mainwindow.cpp mainwindow.h mainwindow.ui)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(
    qt_desktop-gallery
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    includes/rz_photo.cpp
    includes/rz_photo.hpp
    picture_widget.h
    picture_widget.cpp
    picture_widget.ui
    resources.qrc)
  # Define target properties for Android with Qt 6 as: set_property(TARGET
  # qt_listview APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  # ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
  # https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
  if(ANDROID)
    add_library(qt_desktop-gallery SHARED ${PROJECT_SOURCES})
    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
  else()
    add_executable(qt_desktop-gallery ${PROJECT_SOURCES})
  endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(
    qt_desktop-gallery PRIVATE Qt${QT_VERSION_MAJOR}::Widgets exiv2lib Qt6::Gui
                               Qt6::Concurrent)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  target_link_libraries(
    qt_desktop-gallery PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Exiv2::exiv2lib
                               Qt6::Concurrent)
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1. If
# you are developing for iOS or macOS you should consider setting an explicit,
# fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER
                       net.hase-zheng.qt_desktop-gallery)
endif()
set_target_properties(
  qt_desktop-gallery
  PROPERTIES ${BUNDLE_ID_OPTION} MACOSX_BUNDLE_BUNDLE_VERSION
             ${PROJECT_VERSION} MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} MACOSX_BUNDLE
             TRUE WIN32_EXECUTABLE
             TRUE)

include(GNUInstallDirs)
install(
  TARGETS qt_desktop-gallery
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(qt_desktop-gallery)
endif()
