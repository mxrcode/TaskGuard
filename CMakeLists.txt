cmake_minimum_required(VERSION 3.5)

project(TaskGuard VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Sql Network)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

set(TS_FILES
    translations/TaskGuard_en_US.ts
    translations/TaskGuard_uk_UA.ts
    translations/TaskGuard_ru_RU.ts
    translations/TaskGuard_es_ES.ts
)

set(PROJECT_SOURCES
        src/main.cpp
        src/loginwindow.h src/loginwindow.cpp src/loginwindow.ui
        src/mainwindow.cpp src/mainwindow.h src/mainwindow.ui
        src/textindentdelegate.h src/textindentdelegate.cpp
        src/about_author.h src/about_author.cpp src/about_author.ui
        src/about_qt_window.h src/about_qt_window.cpp src/about_qt_window.ui
        src/encryption.h src/encryption.cpp src/encryption.ui
        src/settingswindow.h src/settingswindow.cpp src/settingswindow.ui
        src/version.h
        src/string_encryption.h src/string_encryption.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TaskGuard
        MANUAL_FINALIZATION
        resources/application.qrc
        resources/recources.rc
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET TaskGuard APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(TaskGuard SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(TaskGuard
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(TaskGuard PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql  Qt${QT_VERSION_MAJOR}::Network ${OPENSSL_LIBRARIES})

set_target_properties(TaskGuard PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS TaskGuard
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TaskGuard)
endif()
