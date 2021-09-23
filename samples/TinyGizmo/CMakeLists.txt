set(TARGET_NAME TinyGizmo)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_compile_definitions(${TARGET_NAME} PRIVATE)
target_link_libraries(${TARGET_NAME} PRIVATE app teapot tinygizmo d3d11 d3dcompiler)
