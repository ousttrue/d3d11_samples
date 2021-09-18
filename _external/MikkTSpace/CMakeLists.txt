set(TARGET_NAME mikktspace)
add_library(${TARGET_NAME} mikktspace.c)
target_include_directories(${TARGET_NAME} PUBLIC .)
