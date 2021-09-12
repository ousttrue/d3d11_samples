set(TARGET_NAME render_target)
add_library(${TARGET_NAME} gorilla/render_target.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)

set(TARGET_NAME RenderTarget)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} window device swapchain render_target)
