set(TARGET_NAME window)
add_library(${TARGET_NAME} window.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)

set(TARGET_NAME CreateWindow)
add_executable(${TARGET_NAME} WIN32 main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE window)
