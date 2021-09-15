set(TARGET_NAME GltfScene)
add_executable(${TARGET_NAME} WIN32 main.cpp resource_manager.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla banana)
