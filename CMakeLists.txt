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

# Список всех исходных файлов проекта
set(SOURCES
    src/main.cpp
    src/Figure.cpp
    src/Rectangle.cpp
    src/Triangle.cpp
    src/Trapezoid.cpp
    src/Circle.cpp
    src/Pentagon.cpp
    src/Hexagon.cpp
    src/Editor.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)