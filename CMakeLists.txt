cmake_minimum_required(VERSION 3.16)
list(APPEND CMAKE_PREFIX_PATH "D:/opencv/opencv/build")
project(Device_gui_v2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

add_executable(Device_gui_v2
    main.cpp
    mainwindow.cpp
    Analyzer.cpp
)

target_include_directories(Device_gui_v2 PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(Device_gui_v2 PRIVATE Qt6::Widgets ${OpenCV_LIBS})