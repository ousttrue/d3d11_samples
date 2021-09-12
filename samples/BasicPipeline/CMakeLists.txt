set(TARGET_NAME pipeline)
add_library(${TARGET_NAME} gorilla/pipeline.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)
target_link_libraries(${TARGET_NAME} PUBLIC shader)

set(TARGET_NAME BasicPipeline)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} window device swapchain pipeline render_target)
