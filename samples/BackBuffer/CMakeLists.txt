set(TARGET_NAME BackBuffer)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} window device swapchain)
