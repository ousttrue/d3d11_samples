set(TARGET_NAME ImGuiDockspace)
add_executable(${TARGET_NAME} WIN32 main.cpp dockspace.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla banana app imgui teapot)
