cmake_minimum_required(VERSION 3.16)
list(APPEND CMAKE_PREFIX_PATH "D:/opencv/opencv/build")
project(Device_gui_v2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Test)
find_package(OpenCV REQUIRED)

# ① 颗粒分析算法库(先定义,后面才能链接它)
add_library(analyzer STATIC
    src/Analyzer.cpp
    src/Analyzer.h
)
target_include_directories(analyzer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${OpenCV_INCLUDE_DIRS})
target_link_libraries(analyzer PUBLIC ${OpenCV_LIBS} Qt6::Core)

# ② 主程序(链接算法库,不再重复编译 Analyzer.cpp)
add_executable(Device_gui_v2
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    resource.qrc
)
target_link_libraries(Device_gui_v2 PRIVATE analyzer Qt6::Widgets Qt6::Network)

# ③ 单元测试(QTest + CTest)
enable_testing()
add_executable(tst_analyzer tests/tst_analyzer.cpp)
target_link_libraries(tst_analyzer PRIVATE analyzer Qt6::Test)
add_test(NAME analyzer_test COMMAND tst_analyzer)

# ④ 本地模拟设备(通信测试工具,v16)
add_subdirectory(tools/sim_device)
