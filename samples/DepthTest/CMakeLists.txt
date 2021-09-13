set(TARGET_NAME DepthTest)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(
  ${TARGET_NAME}
  PRIVATE window
          device
          swapchain
          pipeline
          shader_reflection
          render_target
          orbit_camera
          constant_buffer
          input_assembler
          asset)
