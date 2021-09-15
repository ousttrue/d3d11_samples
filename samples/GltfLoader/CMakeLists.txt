set(TARGET_NAME GltfLoader)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla banana)
