# 本地模拟设备(通信测试工具)
# 作为 Device_gui_v2 顶层工程的一部分构建,无独立依赖
add_executable(sim_device src/main.cpp)
target_link_libraries(sim_device PRIVATE Qt6::Core Qt6::Network)
