
# Build apps (-DJC_BUILD_PERFT=ON)
if(JC_BUILD_PERFT)
    add_executable(perft perft.cpp)
    target_link_libraries(perft PRIVATE jankchess)
    target_compile_options(perft PRIVATE -O3 -Wall -Wformat=0 -Wextra)
    install(
        TARGETS perft
        DESTINATION bin)
endif()

# Build apps (-DJC_BUILD_DEV=ON)
if(JC_BUILD_DEV)
    add_executable(dev dev.cpp)
    target_link_libraries(dev PRIVATE jankchess)
    target_compile_options(dev PRIVATE -O3 -Wall -Wextra)
    install(
        TARGETS dev
        DESTINATION bin)
endif()

