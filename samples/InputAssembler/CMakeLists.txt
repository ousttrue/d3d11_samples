set(TARGET_NAME input_assembler)
add_library(${TARGET_NAME} gorilla/input_assembler.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)

set(TARGET_NAME InputAssembler)
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
