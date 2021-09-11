set(TARGET_NAME MinTriangle)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE window device swapchain pipeline input_layout render_target)
