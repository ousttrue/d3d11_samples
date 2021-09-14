set(TARGET_NAME image)
add_library(${TARGET_NAME} gorilla/image.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)
target_link_libraries(${TARGET_NAME} PUBLIC Shlwapi.lib)

set(TARGET_NAME ImageLoader)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(
  ${TARGET_NAME}
  PRIVATE asset
          device
          window
          swapchain
          render_target
          shader
          shader_reflection
          pipeline
          texture
          image
          )
