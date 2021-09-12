set(TARGET_NAME ConstantBuffer)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE window device swapchain pipeline
                                             render_target shader_reflection)
