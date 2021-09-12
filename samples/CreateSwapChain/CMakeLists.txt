set(TARGET_NAME swapchain)
add_library(${TARGET_NAME} gorilla/swapchain.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)

set(TARGET_NAME CreateSwapChain)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} window device swapchain)
