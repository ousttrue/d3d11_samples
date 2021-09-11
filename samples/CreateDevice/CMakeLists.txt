set(TARGET_NAME device)
add_library(${TARGET_NAME} device.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)
target_link_libraries(${TARGET_NAME} PUBLIC d3d11.lib)

set(TARGET_NAME CreateDevice)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE device)
