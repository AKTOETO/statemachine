
file(GLOB SRCS "*.cpp" "*.hpp")

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE libstate)