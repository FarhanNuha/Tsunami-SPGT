cmake_minimum_required(VERSION 3.16)
project(bismillah)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    WebEngineWidgets
    Sql
)

qt_standard_project_setup()

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MenuBar.cpp
    src/MapView.cpp
    src/SeismicEventTable.cpp
)

# Header files
set(HEADERS
    include/MainWindow.h
    include/MenuBar.h
    include/MapView.h
    include/SeismicEventTable.h
)

# Resources
set(RESOURCES
    app.qrc
)

# Create executable
qt_add_executable(bismillah
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link libraries
target_link_libraries(bismillah PRIVATE 
    Qt6::Core 
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Sql
)

# Include directories
target_include_directories(bismillah PRIVATE include)

# Install
install(TARGETS bismillah
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
