set(TARGET_NAME shader)
add_library(${TARGET_NAME} shader.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)
target_link_libraries(${TARGET_NAME} PUBLIC d3dcompiler.lib)

set(TARGET_NAME CompileShader)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE shader)
