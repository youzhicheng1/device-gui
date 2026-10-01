cmake_minimum_required(VERSION 3.16)
list(APPEND CMAKE_PREFIX_PATH "D:/opencv/opencv/build")
project(Device_gui_v2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)
find_package(OpenCV REQUIRED)

# ① 库(必须先定义)
add_library(analyzer STATIC Analyzer.cpp)
target_include_directories(analyzer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(analyzer PUBLIC ${OpenCV_LIBS} Qt6::Core)
target_link_libraries(analyzer PUBLIC ${OpenCV_LIBS})

# ② 主程序(不再编 Analyzer.cpp,改为链接库)
add_executable(Device_gui_v2 main.cpp mainwindow.cpp)
target_link_libraries(Device_gui_v2 PRIVATE analyzer Qt6::Widgets)

# ③ 测试
enable_testing()
add_executable(tst_analyzer tests/tst_analyzer.cpp)
target_link_libraries(tst_analyzer PRIVATE analyzer Qt6::Test)
add_test(NAME analyzer_test COMMAND tst_analyzer)