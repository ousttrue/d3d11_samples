set(TARGET_NAME CompileShader)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla)
