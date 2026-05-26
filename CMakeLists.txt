cmake_minimum_required(VERSION 3.15)
project(MyGame)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sfml/lib/cmake)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/AbstractFigure.cpp
    src/PolylineFigure.cpp
    src/CompositeFigure.cpp
    src/FigureManager.cpp
    src/Rectangle.cpp
    src/Triangle.cpp
    src/Trapezoid.cpp
    src/Circle.cpp
    src/Pentagon.cpp
    src/Hexagon.cpp
    src/Editor.cpp
    src/TextBox.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)