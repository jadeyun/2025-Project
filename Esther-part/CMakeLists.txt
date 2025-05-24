cmake_minimum_required(VERSION 3.16)

project(Esther-part VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    englishoverlay.cpp
    englishoverlay.h
    englishoverlay.ui
    taskitemWidget.cpp
    taskitemWidget.h
    resources.qrc
    taskrecord.cpp
    taskrecord.h
    achievementboard.cpp
    achievementboard.h
    achievementboard.ui
    graph.cpp
    graph.h
    graph.ui
    clickablelabel.cpp
    clickablelabel.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Esther-part
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        timetable.h timetable.cpp timetable.ui
        profile.h profile.cpp profile.ui
        welcomepage.h welcomepage.cpp welcomepage.ui
    )
else()
    if(ANDROID)
        add_library(Esther-part SHARED ${PROJECT_SOURCES})
    else()
        add_executable(Esther-part ${PROJECT_SOURCES})
    endif()
endif()


target_link_libraries(Esther-part PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Esther-part)
endif()
set_target_properties(Esther-part PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Esther-part
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Esther-part)
endif()
