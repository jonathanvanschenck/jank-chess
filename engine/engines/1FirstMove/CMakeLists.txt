
add_executable(engine1FirstMove engine.cpp)

target_link_libraries(engine1FirstMove PRIVATE jankchess)

target_compile_options(engine1FirstMove PRIVATE -O3 -Wall -Wextra)

install(
    TARGETS engine1FirstMove
    DESTINATION bin)
