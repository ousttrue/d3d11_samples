set(TARGET_NAME ImGui)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla banana app imgui teapot)
