set(TARGET_NAME ImGuiDockspaceView)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla app imgui)
