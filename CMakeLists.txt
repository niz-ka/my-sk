cmake_minimum_required(VERSION 3.0...3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.17)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT LTO_SUPPORT)

project(Kahoot VERSION 0.1.20220106 LANGUAGES CXX)

add_library(kahoot-shared src/client.hpp src/client.cpp src/game.hpp src/game.cpp src/helpers.hpp src/message.hpp src/question.hpp src/question.cpp src/server.hpp src/server.cpp)
# target_include_directories(kahoot-shared PRIVATE vendor/toml vendor/spdlog/include vendor/doctest vendor/expected/include/tl/)
target_compile_features(kahoot-shared PUBLIC cxx_std_20)

add_executable(kahoot-server src/main.cpp)

set_target_properties(kahoot-server PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(kahoot-server PUBLIC cxx_std_20)
target_compile_options(kahoot-server PRIVATE -Wall -Wextra -Wpedantic)
if(LTO_SUPPORT)
    set_target_properties(kahoot-server PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
endif()

target_include_directories(kahoot-server PRIVATE vendor/toml vendor/spdlog/include vendor/doctest vendor/expected/include/tl/)
target_link_libraries(kahoot-server PRIVATE pthread)
target_link_libraries(kahoot-server PRIVATE kahoot-shared)

# add_executable(kahoot-tests tests/main.cpp)

# set_target_properties(kahoot-tests PROPERTIES CXX_EXTENSIONS OFF)
# target_compile_features(kahoot-tests PUBLIC cxx_std_20)
# target_compile_options(kahoot-tests PRIVATE -Wall -Wextra -Wpedantic)
# if(LTO_SUPPORT)
    # set_target_properties(kahoot-tests PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
# endif()

# target_include_directories(kahoot-tests PRIVATE vendor/toml vendor/spdlog/include vendor/doctest vendor/expected/include/tl/)
# target_link_libraries(kahoot-tests PRIVATE pthread kahoot-shared)