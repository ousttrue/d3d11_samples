set(TARGET_NAME FontMesh)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla banana stb)
