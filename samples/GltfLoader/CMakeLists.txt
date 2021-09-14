set(TARGET_NAME gltf)
add_library(${TARGET_NAME} gorilla/gltf.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)
target_link_libraries(${TARGET_NAME} PRIVATE json)

set(TARGET_NAME GltfLoader)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE gltf asset)
