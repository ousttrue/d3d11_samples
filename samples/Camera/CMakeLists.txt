set(TARGET_NAME orbit_camera)
add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE .)

set(TARGET_NAME Camera)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(
  ${TARGET_NAME}
  window
  device
  swapchain
  pipeline
  render_target
  constant_buffer
  orbit_camera
  asset
  )
