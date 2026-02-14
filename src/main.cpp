#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "Editor.hpp"
#include "Rectangle.hpp"
#include "Triangle.hpp"
#include "Trapezoid.hpp"
#include "Circle.hpp"
#include "Pentagon.hpp"
#include "Hexagon.hpp"

enum class ShapeType {
    Rectangle,
    Triangle,
    Trapezoid,
    Circle,
    Pentagon,
    Hexagon
};

std::string colorToString(const sf::Color& c) {
    std::ostringstream oss;
    oss << "R:" << (int)c.r << " G:" << (int)c.g << " B:" << (int)c.b;
    return oss.str();
}

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Simple Paint (Fullscreen)", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    Editor editor;

    ShapeType currentShape = ShapeType::Rectangle;
    float rectWidth = 150, rectHeight = 100;
    float triSide = 120;
    float trapTop = 80, trapBottom = 140, trapHeight = 100;
    float circleRadius = 70;
    float pentRadius = 80;
    float hexRadius = 80;

    sf::Color currentColor = sf::Color::White;
    std::vector<float> currentThicknesses(6, 2.0f);

    int selectedThicknessIndex = 0;
    bool colorMode = false;

    sf::Font font;
    std::vector<std::string> fontPaths = {
        "resources/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf"
    };
    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            std::cout << "Loaded font: " << path << std::endl;
            fontLoaded = true;
            break;
        } else {
            std::cout << "Failed to load: " << path << std::endl;
        }
    }
    if (!fontLoaded) {
        std::cerr << "Error: no font loaded. Text will not be displayed." << std::endl;
    }

    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(10, 10);

    sf::Text exitButton;
    exitButton.setFont(font);
    exitButton.setCharacterSize(28);
    exitButton.setFillColor(sf::Color::Red);
    exitButton.setString("X");
    exitButton.setPosition(window.getSize().x - exitButton.getLocalBounds().width - 20, 10);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                sf::FloatRect exitRect(exitButton.getPosition().x, exitButton.getPosition().y,
                                        exitButton.getLocalBounds().width, exitButton.getLocalBounds().height);
                if (exitRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    window.close();
                }
            }

            editor.handleEvent(event, window);

            if (event.type == sf::Event::MouseWheelScrolled && editor.getSelected()) {
                float delta = event.mouseWheelScroll.delta;
                editor.handleScale(delta);
            }

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Num1: currentShape = ShapeType::Rectangle; break;
                    case sf::Keyboard::Num2: currentShape = ShapeType::Triangle; break;
                    case sf::Keyboard::Num3: currentShape = ShapeType::Trapezoid; break;
                    case sf::Keyboard::Num4: currentShape = ShapeType::Circle; break;
                    case sf::Keyboard::Num5: currentShape = ShapeType::Pentagon; break;
                    case sf::Keyboard::Num6: currentShape = ShapeType::Hexagon; break;

                    case sf::Keyboard::Up:
                        switch (currentShape) {
                            case ShapeType::Rectangle: rectHeight += 5; break;
                            case ShapeType::Triangle: triSide += 5; break;
                            case ShapeType::Trapezoid: trapHeight += 5; break;
                            case ShapeType::Circle: circleRadius += 2; break;
                            case ShapeType::Pentagon: pentRadius += 2; break;
                            case ShapeType::Hexagon: hexRadius += 2; break;
                        }
                        break;
                    case sf::Keyboard::Down:
                        switch (currentShape) {
                            case ShapeType::Rectangle: rectHeight = std::max(10.0f, rectHeight - 5); break;
                            case ShapeType::Triangle: triSide = std::max(10.0f, triSide - 5); break;
                            case ShapeType::Trapezoid: trapHeight = std::max(10.0f, trapHeight - 5); break;
                            case ShapeType::Circle: circleRadius = std::max(5.0f, circleRadius - 2); break;
                            case ShapeType::Pentagon: pentRadius = std::max(5.0f, pentRadius - 2); break;
                            case ShapeType::Hexagon: hexRadius = std::max(5.0f, hexRadius - 2); break;
                        }
                        break;
                    case sf::Keyboard::Left:
                        switch (currentShape) {
                            case ShapeType::Rectangle: rectWidth = std::max(10.0f, rectWidth - 5); break;
                            case ShapeType::Trapezoid: trapTop = std::max(10.0f, trapTop - 5); break;
                            default: break;
                        }
                        break;
                    case sf::Keyboard::Right:
                        switch (currentShape) {
                            case ShapeType::Rectangle: rectWidth += 5; break;
                            case ShapeType::Trapezoid: trapTop += 5; break;
                            default: break;
                        }
                        break;

                    case sf::Keyboard::C:
                        colorMode = !colorMode;
                        break;

                    case sf::Keyboard::R: {
                        if (colorMode) {
                            if (auto* sel = editor.getSelected()) {
                                int numSides = sel->getThicknesses().size();
                                if (selectedThicknessIndex < numSides) {
                                    sf::Color col = sel->getSideColor(selectedThicknessIndex);
                                    if (event.key.shift)
                                        col.r = std::min(255, col.r + 10);
                                    else
                                        col.r = std::max(0, col.r - 10);
                                    sel->setSideColor(selectedThicknessIndex, col);
                                }
                            }
                        } else {
                            if (event.key.shift) currentColor.r = std::min(255, currentColor.r + 10);
                            else currentColor.r = std::max(0, currentColor.r - 10);
                        }
                        break;
                    }
                    case sf::Keyboard::G: {
                        if (colorMode) {
                            if (auto* sel = editor.getSelected()) {
                                int numSides = sel->getThicknesses().size();
                                if (selectedThicknessIndex < numSides) {
                                    sf::Color col = sel->getSideColor(selectedThicknessIndex);
                                    if (event.key.shift)
                                        col.g = std::min(255, col.g + 10);
                                    else
                                        col.g = std::max(0, col.g - 10);
                                    sel->setSideColor(selectedThicknessIndex, col);
                                }
                            }
                        } else {
                            if (event.key.shift) currentColor.g = std::min(255, currentColor.g + 10);
                            else currentColor.g = std::max(0, currentColor.g - 10);
                        }
                        break;
                    }
                    case sf::Keyboard::B: {
                        if (colorMode) {
                            if (auto* sel = editor.getSelected()) {
                                int numSides = sel->getThicknesses().size();
                                if (selectedThicknessIndex < numSides) {
                                    sf::Color col = sel->getSideColor(selectedThicknessIndex);
                                    if (event.key.shift)
                                        col.b = std::min(255, col.b + 10);
                                    else
                                        col.b = std::max(0, col.b - 10);
                                    sel->setSideColor(selectedThicknessIndex, col);
                                }
                            }
                        } else {
                            if (event.key.shift) currentColor.b = std::min(255, currentColor.b + 10);
                            else currentColor.b = std::max(0, currentColor.b - 10);
                        }
                        break;
                    }

                    case sf::Keyboard::T: {
                        int numSides = 0;
                        switch (currentShape) {
                            case ShapeType::Rectangle: numSides = 4; break;
                            case ShapeType::Triangle: numSides = 3; break;
                            case ShapeType::Trapezoid: numSides = 4; break;
                            case ShapeType::Circle: numSides = 1; break;
                            case ShapeType::Pentagon: numSides = 5; break;
                            case ShapeType::Hexagon: numSides = 6; break;
                        }
                        if (numSides == 0) break;
                        if (event.key.shift) {
                            currentThicknesses[selectedThicknessIndex] = std::max(0.5f, currentThicknesses[selectedThicknessIndex] - 0.5f);
                        } else {
                            currentThicknesses[selectedThicknessIndex] += 0.5f;
                        }
                        break;
                    }
                    case sf::Keyboard::Y: {
                        int numSides = 0;
                        switch (currentShape) {
                            case ShapeType::Rectangle: numSides = 4; break;
                            case ShapeType::Triangle: numSides = 3; break;
                            case ShapeType::Trapezoid: numSides = 4; break;
                            case ShapeType::Circle: numSides = 1; break;
                            case ShapeType::Pentagon: numSides = 5; break;
                            case ShapeType::Hexagon: numSides = 6; break;
                        }
                        if (numSides > 0) {
                            selectedThicknessIndex = (selectedThicknessIndex + 1) % numSides;
                        }
                        break;
                    }

                    case sf::Keyboard::Space: {
                        std::vector<float> thicknesses;
                        sf::Vector2u windowSize = window.getSize();
                        sf::Vector2f center(windowSize.x / 2.f, windowSize.y / 2.f);
                        std::unique_ptr<Figure> newFig;

                        switch (currentShape) {
                            case ShapeType::Rectangle:
                                thicknesses.assign(currentThicknesses.begin(), currentThicknesses.begin()+4);
                                newFig = std::make_unique<Rectangle>(rectWidth, rectHeight, currentColor, thicknesses);
                                break;
                            case ShapeType::Triangle:
                                thicknesses.assign(currentThicknesses.begin(), currentThicknesses.begin()+3);
                                newFig = std::make_unique<Triangle>(triSide, currentColor, thicknesses);
                                break;
                            case ShapeType::Trapezoid:
                                thicknesses.assign(currentThicknesses.begin(), currentThicknesses.begin()+4);
                                newFig = std::make_unique<Trapezoid>(trapTop, trapBottom, trapHeight, currentColor, thicknesses);
                                break;
                            case ShapeType::Circle:
                                thicknesses = {currentThicknesses[0]};
                                newFig = std::make_unique<Circle>(circleRadius, currentColor, thicknesses);
                                break;
                            case ShapeType::Pentagon:
                                thicknesses.assign(currentThicknesses.begin(), currentThicknesses.begin()+5);
                                newFig = std::make_unique<Pentagon>(pentRadius, currentColor, thicknesses);
                                break;
                            case ShapeType::Hexagon:
                                thicknesses.assign(currentThicknesses.begin(), currentThicknesses.begin()+6);
                                newFig = std::make_unique<Hexagon>(hexRadius, currentColor, thicknesses);
                                break;
                        }
                        if (newFig) {
                            newFig->setPosition(center);
                            editor.addFigure(std::move(newFig));
                        }
                        break;
                    }

                    case sf::Keyboard::Delete:
                        editor.removeSelected();
                        break;

                    default: break;
                }
            }
        }

        std::ostringstream oss;
        oss << "Current shape: ";
        switch (currentShape) {
            case ShapeType::Rectangle: oss << "Rectangle"; break;
            case ShapeType::Triangle: oss << "Triangle"; break;
            case ShapeType::Trapezoid: oss << "Trapezoid"; break;
            case ShapeType::Circle: oss << "Circle"; break;
            case ShapeType::Pentagon: oss << "Pentagon"; break;
            case ShapeType::Hexagon: oss << "Hexagon"; break;
        }
        oss << "\nColor: " << colorToString(currentColor);
        oss << "\nThicknesses: ";
        int numSides = 0;
        switch (currentShape) {
            case ShapeType::Rectangle: numSides = 4; break;
            case ShapeType::Triangle: numSides = 3; break;
            case ShapeType::Trapezoid: numSides = 4; break;
            case ShapeType::Circle: numSides = 1; break;
            case ShapeType::Pentagon: numSides = 5; break;
            case ShapeType::Hexagon: numSides = 6; break;
        }
        for (int i = 0; i < numSides; ++i) {
            if (i == selectedThicknessIndex)
                oss << " [" << currentThicknesses[i] << "]";
            else
                oss << " " << currentThicknesses[i];
        }
        oss << "\nSize: ";
        switch (currentShape) {
            case ShapeType::Rectangle: oss << rectWidth << " x " << rectHeight; break;
            case ShapeType::Triangle: oss << "side " << triSide; break;
            case ShapeType::Trapezoid: oss << "top " << trapTop << " bottom " << trapBottom << " height " << trapHeight; break;
            case ShapeType::Circle: oss << "radius " << circleRadius; break;
            case ShapeType::Pentagon: oss << "radius " << pentRadius; break;
            case ShapeType::Hexagon: oss << "radius " << hexRadius; break;
        }
        oss << "\nMode: " << (colorMode ? "COLOR" : "THICKNESS");
        if (colorMode) {
            oss << "\nCurrent side color: ";
            if (auto* sel = editor.getSelected()) {
                if (selectedThicknessIndex < sel->getSideColors().size()) {
                    sf::Color col = sel->getSideColor(selectedThicknessIndex);
                    oss << "R:" << (int)col.r << " G:" << (int)col.g << " B:" << (int)col.b;
                } else oss << "N/A";
            } else oss << " (no selection)";
        } else {
            oss << "\nCurrent thickness: " << currentThicknesses[selectedThicknessIndex];
        }
        oss << "\n\nControls:\n"
            << "1-6: select shape\n"
            << "Arrow keys: adjust size\n"
            << "R/G/B: " << (colorMode ? "change side color" : "change default color") << " (Shift+ increase)\n"
            << "C: switch mode\n"
            << "T: increase thickness, Shift+T: decrease\n"
            << "Y: next side\n"
            << "Space: add shape at center\n"
            << "Delete: remove selected\n"
            << "Mouse wheel: scale selected\n"
            << "X (top right): exit";

        infoText.setString(oss.str());

        float maxWidth = window.getSize().x - 20;
        float maxHeight = window.getSize().y - 50;
        while (infoText.getLocalBounds().width > maxWidth || infoText.getLocalBounds().height > maxHeight) {
            if (infoText.getCharacterSize() <= 8) break;
            infoText.setCharacterSize(infoText.getCharacterSize() - 1);
        }

        exitButton.setPosition(window.getSize().x - exitButton.getLocalBounds().width - 20, 10);

        window.clear(sf::Color(50,50,50));
        editor.draw(window);
        window.draw(infoText);
        window.draw(exitButton);
        window.display();
    }
    return 0;
}