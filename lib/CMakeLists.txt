set(TARGET_NAME gorilla)
add_library(
  ${TARGET_NAME}
  gorilla/asset.cpp
  gorilla/constant_buffer.cpp
  gorilla/device.cpp
  gorilla/gltf.cpp
  gorilla/image.cpp
  gorilla/input_assembler.cpp
  gorilla/pipeline.cpp
  gorilla/render_target.cpp
  gorilla/shader.cpp
  gorilla/shader_reflection.cpp
  gorilla/swapchain.cpp
  gorilla/texture.cpp
  gorilla/window.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)
target_link_libraries(
  ${TARGET_NAME}
  PRIVATE json
  PUBLIC d3d11.lib d3dcompiler.lib dxguid.lib Shlwapi.lib)
