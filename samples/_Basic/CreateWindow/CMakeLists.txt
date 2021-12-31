set(TARGET_NAME CreateWindow)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla)
