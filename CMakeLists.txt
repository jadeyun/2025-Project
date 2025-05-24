cmake_minimum_required(VERSION 3.5)

project(MultiModeTimer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Multimedia REQUIRED)

set(SOURCES
    main.cpp
    mainwindow.cpp
    englishoverlay.cpp
    taskitemWidget.cpp
    taskrecord.cpp
    legacy/AlarmDialog.cpp
    legacy/TimerWidget.cpp
    FormDialog.cpp
)

set(HEADERS
    mainwindow.h
    englishoverlay.h
    taskitemWidget.h
    taskrecord.h
    legacy/AlarmDialog.h
    legacy/TimerWidget.h
    FormDialog.h
)

set(UIS
    mainwindow.ui
    englishoverlay.ui
    legacy/mainwindow2.ui
)

set(RESOURCES
    resources.qrc
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UIS}
    ${RESOURCES}
    historywindow.h
    historywindow.cpp
    clickablelabel.h
    clickablelabel.cpp
)

target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Multimedia)
find_package(Qt6 REQUIRED COMPONENTS Charts)

target_link_libraries(${PROJECT_NAME} Qt6::Charts)
