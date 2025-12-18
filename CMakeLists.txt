cmake_minimum_required(VERSION 3.16)
project(bismillah)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Install Qt6 SQL PostgreSQL driver if not available
# Run: sudo apt-get install libqt6sql6-psql

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    Sql
)

qt_standard_project_setup()

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MenuBar.cpp
    src/MapView.cpp
    src/DatabaseView.cpp
    src/FocalMechanismWidget.cpp
)

# Header files
set(HEADERS
    include/MainWindow.h
    include/MenuBar.h
    include/MapView.h
    include/DatabaseView.h
    include/FocalMechanismWidget.h
)

# Resource files
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
    Qt6::Sql
)

# Include directories
target_include_directories(bismillah PRIVATE include)

# Install (optional)
install(TARGETS bismillah
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
