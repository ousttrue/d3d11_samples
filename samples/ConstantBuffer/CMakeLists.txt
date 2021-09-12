set(TARGET_NAME constant_buffer)
add_library(${TARGET_NAME} gorilla/constant_buffer.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)

set(TARGET_NAME ConstantBuffer)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(
  ${TARGET_NAME}
  PRIVATE window
          device
          swapchain
          pipeline
          render_target
          shader_reflection
          constant_buffer)
