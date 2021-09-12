set(TARGET_NAME shader_reflection)
add_library(${TARGET_NAME} gorilla/shader_reflection.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)
target_link_libraries(${TARGET_NAME} PUBLIC shader dxguid.lib)

set(TARGET_NAME ShaderReflection)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE shader_reflection)
