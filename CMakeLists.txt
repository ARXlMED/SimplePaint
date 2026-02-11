cmake_minimum_required(VERSION 3.15)
project(MyGame)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)