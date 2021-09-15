set(TARGET_NAME CreateDevice)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gorilla)
