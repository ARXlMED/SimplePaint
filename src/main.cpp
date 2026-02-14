#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <sstream>
#include <iomanip>
#include "Editor.hpp"
#include "Rectangle.hpp"
#include "Triangle.hpp"
#include "Trapezoid.hpp"
#include "Circle.hpp"
#include "Pentagon.hpp"
#include "Hexagon.hpp"

// Очередь команд из консольного потока
std::queue<std::string> commandQueue;
std::mutex queueMutex;

// Функция консольного ввода (работает в отдельном потоке)
void consoleInputThread() {
    std::string line;
    while (true) {
        std::cout << "Enter command (add <type> ... or quit): ";
        std::getline(std::cin, line);
        if (line == "quit") break;
        std::lock_guard<std::mutex> lock(queueMutex);
        commandQueue.push(line);
    }
}

// Разбор команды и создание фигуры
std::unique_ptr<Figure> parseCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string command;
    iss >> command;
    if (command != "add") return nullptr;

    std::string type;
    iss >> type;

    float r, g, b;
    int numThick = 0;
    std::vector<float> thicknesses;

    if (type == "rectangle") {
        float w, h;
        iss >> w >> h >> r >> g >> b;
        for (int i = 0; i < 4; ++i) { float t; iss >> t; thicknesses.push_back(t); }
        if (iss.fail()) return nullptr;
        return std::make_unique<Rectangle>(w, h, sf::Color(r*255, g*255, b*255), thicknesses);
    }
    else if (type == "triangle") {
        float side;
        iss >> side >> r >> g >> b;
        for (int i = 0; i < 3; ++i) { float t; iss >> t; thicknesses.push_back(t); }
        if (iss.fail()) return nullptr;
        return std::make_unique<Triangle>(side, sf::Color(r*255, g*255, b*255), thicknesses);
    }
    else if (type == "trapezoid") {
        float top, bottom, height;
        iss >> top >> bottom >> height >> r >> g >> b;
        for (int i = 0; i < 4; ++i) { float t; iss >> t; thicknesses.push_back(t); }
        if (iss.fail()) return nullptr;
        return std::make_unique<Trapezoid>(top, bottom, height, sf::Color(r*255, g*255, b*255), thicknesses);
    }
    else if (type == "circle") {
        float radius;
        iss >> radius >> r >> g >> b;
        float t; iss >> t; thicknesses.push_back(t);
        if (iss.fail()) return nullptr;
        return std::make_unique<Circle>(radius, sf::Color(r*255, g*255, b*255), thicknesses);
    }
    else if (type == "pentagon") {
        float radius;
        iss >> radius >> r >> g >> b;
        for (int i = 0; i < 5; ++i) { float t; iss >> t; thicknesses.push_back(t); }
        if (iss.fail()) return nullptr;
        return std::make_unique<Pentagon>(radius, sf::Color(r*255, g*255, b*255), thicknesses);
    }
    else if (type == "hexagon") {
        float radius;
        iss >> radius >> r >> g >> b;
        for (int i = 0; i < 6; ++i) { float t; iss >> t; thicknesses.push_back(t); }
        if (iss.fail()) return nullptr;
        return std::make_unique<Hexagon>(radius, sf::Color(r*255, g*255, b*255), thicknesses);
    }
    return nullptr;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Simple Paint (Console)");
    window.setFramerateLimit(60);
    Editor editor;

    // Запускаем поток консольного ввода
    std::thread inputThread(consoleInputThread);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Обработка событий редактора (перемещение, выделение)
            editor.handleEvent(event, window);

            // Масштабирование колёсиком мыши
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    editor.handleScale(event.mouseWheelScroll.delta);
                }
            }

            // Горячие клавиши: Del удаляет выделенную фигуру
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete) {
                editor.removeSelected();
            }
        }

        // Обрабатываем команды из консоли
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            while (!commandQueue.empty()) {
                std::string cmd = commandQueue.front();
                commandQueue.pop();
                if (cmd == "quit") {
                    window.close();
                    break;
                }
                auto fig = parseCommand(cmd);
                if (fig) {
                    fig->setPosition({600, 400}); // центр окна
                    editor.addFigure(std::move(fig));
                    std::cout << "Figure added.\n";
                } else {
                    std::cout << "Invalid command. Examples:\n";
                    std::cout << "add rectangle 200 150 1 0 0 2 2 2 2\n";
                    std::cout << "add triangle 100 0 1 0 3 3 3\n";
                    std::cout << "add trapezoid 80 120 100 0 0 1 2 2 2 2\n";
                    std::cout << "add circle 60 1 1 0 3\n";
                    std::cout << "add pentagon 70 0 1 0 2 2 2 2 2\n";
                    std::cout << "add hexagon 70 0 1 1 1 2 3 4 5 6\n";
                }
            }
        }

        // Обновление заголовка окна с подсказкой
        std::string title = "Simple Paint - Commands in console";
        window.setTitle(title);

        window.clear(sf::Color(50, 50, 50));
        editor.draw(window);
        window.display();
    }

    inputThread.join(); // дожидаемся завершения потока ввода
    return 0;
}