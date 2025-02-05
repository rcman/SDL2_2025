cmake_minimum_required(VERSION 3.10)
project(newstart)
find_package(SDL3 REQUIRED)

add_executable(newstart
    Examples/menu.cpp
)

target_link_libraries(newstart
    SDL3::SDL3
)

add_custom_command(TARGET newstart POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Content $<TARGET_FILE_DIR:newstart>/Content
)
