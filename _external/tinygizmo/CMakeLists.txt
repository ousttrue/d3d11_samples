set(TARGET_NAME tinygizmo)
add_library(${TARGET_NAME} tiny-gizmo.cpp)
target_include_directories(${TARGET_NAME} PUBLIC .)
