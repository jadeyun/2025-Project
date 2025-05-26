<<<<<<< Updated upstream
cmake_minimum_required(VERSION 3.5)

project(MultiModeTimer)

set(CMAKE_CXX_STANDARD 14)
=======
cmake_minimum_required(VERSION 3.16)
project(MultiModeTimer VERSION 1.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Qt自动化工具
>>>>>>> Stashed changes
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

<<<<<<< Updated upstream
find_package(Qt6 COMPONENTS Widgets Multimedia REQUIRED)

=======
# 查找Qt包
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts Multimedia)

# 设置源文件
>>>>>>> Stashed changes
set(SOURCES
    main.cpp
    mainwindow.cpp
    englishoverlay.cpp
    taskitemWidget.cpp
    taskrecord.cpp
<<<<<<< Updated upstream
=======
    achievementboard.cpp
    graph.cpp
    profile.cpp
    timetable.cpp
    welcomepage.cpp
    clickablelabel.cpp
    historywindow.cpp
>>>>>>> Stashed changes
    legacy/AlarmDialog.cpp
    legacy/TimerWidget.cpp
    FormDialog.cpp
)

<<<<<<< Updated upstream
=======
# 设置头文件
>>>>>>> Stashed changes
set(HEADERS
    mainwindow.h
    englishoverlay.h
    taskitemWidget.h
    taskrecord.h
<<<<<<< Updated upstream
=======
    achievementboard.h
    graph.h
    profile.h
    timetable.h
    welcomepage.h
    clickablelabel.h
    historywindow.h
>>>>>>> Stashed changes
    legacy/AlarmDialog.h
    legacy/TimerWidget.h
    FormDialog.h
)

<<<<<<< Updated upstream
set(UIS
    mainwindow.ui
    englishoverlay.ui
    legacy/mainwindow2.ui
)

set(RESOURCES
    resources.qrc
)

add_executable(${PROJECT_NAME}
=======
# 设置UI文件
set(UIS
    mainwindow.ui
    englishoverlay.ui
    achievementboard.ui
    graph.ui
    profile.ui
    timetable.ui
    welcomepage.ui
    legacy/mainwindow2.ui
)

# 设置资源文件
set(RESOURCES
    resources.qrc
    resource.qrc
)

# 合并所有项目源文件
set(PROJECT_SOURCES
>>>>>>> Stashed changes
    ${SOURCES}
    ${HEADERS}
    ${UIS}
    ${RESOURCES}
<<<<<<< Updated upstream
    historywindow.h
    historywindow.cpp
    clickablelabel.h
    clickablelabel.cpp
)

target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Multimedia)
find_package(Qt6 REQUIRED COMPONENTS Charts)

target_link_libraries(${PROJECT_NAME} Qt6::Charts)
=======
)

# 设置包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 创建可执行文件
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MultiModeTimer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(MultiModeTimer SHARED ${PROJECT_SOURCES})
    else()
        add_executable(MultiModeTimer ${PROJECT_SOURCES})
    endif()
endif()

# 链接Qt库
target_link_libraries(MultiModeTimer PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Multimedia
)

# iOS/macOS Bundle设置
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MultiModeTimer)
endif()

set_target_properties(MultiModeTimer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 安装配置
include(GNUInstallDirs)
install(TARGETS MultiModeTimer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Qt6终结处理
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MultiModeTimer)
endif()
>>>>>>> Stashed changes
