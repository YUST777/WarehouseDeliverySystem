cmake_minimum_required(VERSION 3.16)
project(WarehouseDeliverySystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Windows-specific settings
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/core/Simulator.cpp
    src/core/Scheduler.cpp
    src/core/EventManager.cpp
    src/models/Order.cpp
    src/models/Warehouse.cpp
    src/models/Vehicle.cpp
    src/models/Event.cpp
    src/gui/MainWindow.cpp
    src/gui/OrdersPanel.cpp
    src/gui/WarehousePanel.cpp
    src/gui/VehiclePanel.cpp
    src/gui/EventLogWidget.cpp
    src/gui/StatsWidget.cpp
    src/gui/ControlBar.cpp
    src/io/InputParser.cpp
    src/io/OutputWriter.cpp
)

set(HEADERS
    src/core/Simulator.h
    src/core/Scheduler.h
    src/core/EventManager.h
    src/models/Order.h
    src/models/Warehouse.h
    src/models/Vehicle.h
    src/models/Event.h
    src/gui/MainWindow.h
    src/gui/OrdersPanel.h
    src/gui/WarehousePanel.h
    src/gui/VehiclePanel.h
    src/gui/EventLogWidget.h
    src/gui/StatsWidget.h
    src/gui/ControlBar.h
    src/io/InputParser.h
    src/io/OutputWriter.h
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
)

