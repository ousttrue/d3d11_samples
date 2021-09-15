set(TARGET_NAME GltfScene)
add_executable(${TARGET_NAME} WIN32 main.cpp scene_renderer.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla banana)
