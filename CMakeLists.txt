cmake_minimum_required(VERSION 3.16)
project(bismillah)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(bismillah
    src/main.cpp
    src/MainWindow.cpp
    src/MenuBar.cpp
    include/MainWindow.h
    include/MenuBar.h
    app.qrc
)

target_link_libraries(bismillah PRIVATE Qt6::Core Qt6::Widgets)

target_include_directories(bismillah PRIVATE include)
