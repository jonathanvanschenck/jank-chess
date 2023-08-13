
add_executable(engine2Search engine.cpp)

target_link_libraries(engine2Search PRIVATE jankchess)

target_compile_options(engine2Search PRIVATE -O3 -Wall -Wextra)

install(
    TARGETS engine2Search
    DESTINATION bin)
