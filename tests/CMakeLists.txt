set(TARGET_NAME hlslparse_test)
add_executable(${TARGET_NAME} hlslparse_test.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE catch2 banana gorilla)
