set(TARGET_NAME input_layout)
add_library(${TARGET_NAME} input_layout.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)
target_link_libraries(${TARGET_NAME} PUBLIC shader dxguid.lib)

set(TARGET_NAME InputLayout)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE input_layout)
