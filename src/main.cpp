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

enum class Mode {
    THICKNESS,
    COLOR,
    FILL,
    PIVOT,
    VERTEX
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

    sf::Color currentOutlineColor = sf::Color::White;   // цвет обводки новых фигур
    std::vector<float> currentThicknesses(6, 5.0f);
    int selectedIndex = 0; // для стороны или вершины (в зависимости от режима)

    Mode currentMode = Mode::THICKNESS;
    sf::Color currentFillColor = sf::Color::White;   // цвет заливки для новых фигур

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
                    case sf::Keyboard::Down:
                    case sf::Keyboard::Left:
                    case sf::Keyboard::Right: {
                        if (!editor.getSelected()) break; // ничего не выбрано
                        Figure* sel = editor.getSelected();
                        int dx = 0, dy = 0;
                        if (event.key.code == sf::Keyboard::Up) dy = -1;
                        else if (event.key.code == sf::Keyboard::Down) dy = 1;
                        else if (event.key.code == sf::Keyboard::Left) dx = -1;
                        else if (event.key.code == sf::Keyboard::Right) dx = 1;

                        if (currentMode == Mode::PIVOT) {
                            // Перемещаем точку привязки в локальных координатах (целочисленно)
                            sf::Vector2f newPivot = sel->getLocalPivot() + sf::Vector2f(dx, dy);
                            sel->setLocalPivot(newPivot);
                        }
                        else if (currentMode == Mode::VERTEX) {
                            size_t vertexCount = sel->getVertexCount();
                            if (vertexCount > 0 && selectedIndex < vertexCount) {
                                sf::Vector2f newPos = sel->getLocalVertex(selectedIndex) + sf::Vector2f(dx, dy);
                                sel->setLocalVertex(selectedIndex, newPos);
                            }
                        }
                        else {
                            // В остальных режимах стрелки меняют размер фигуры (как раньше)
                            switch (currentShape) {
                                case ShapeType::Rectangle:
                                    if (event.key.code == sf::Keyboard::Up) rectHeight += 5;
                                    else if (event.key.code == sf::Keyboard::Down) rectHeight = std::max(10.0f, rectHeight - 5);
                                    else if (event.key.code == sf::Keyboard::Left) rectWidth = std::max(10.0f, rectWidth - 5);
                                    else if (event.key.code == sf::Keyboard::Right) rectWidth += 5;
                                    break;
                                case ShapeType::Triangle:
                                    if (event.key.code == sf::Keyboard::Up) triSide += 5;
                                    else if (event.key.code == sf::Keyboard::Down) triSide = std::max(10.0f, triSide - 5);
                                    break;
                                case ShapeType::Trapezoid:
                                    if (event.key.code == sf::Keyboard::Up) trapHeight += 5;
                                    else if (event.key.code == sf::Keyboard::Down) trapHeight = std::max(10.0f, trapHeight - 5);
                                    else if (event.key.code == sf::Keyboard::Left) trapTop = std::max(10.0f, trapTop - 5);
                                    else if (event.key.code == sf::Keyboard::Right) trapTop += 5;
                                    break;
                                case ShapeType::Circle:
                                    if (event.key.code == sf::Keyboard::Up) circleRadius += 2;
                                    else if (event.key.code == sf::Keyboard::Down) circleRadius = std::max(5.0f, circleRadius - 2);
                                    break;
                                case ShapeType::Pentagon:
                                    if (event.key.code == sf::Keyboard::Up) pentRadius += 2;
                                    else if (event.key.code == sf::Keyboard::Down) pentRadius = std::max(5.0f, pentRadius - 2);
                                    break;
                                case ShapeType::Hexagon:
                                    if (event.key.code == sf::Keyboard::Up) hexRadius += 2;
                                    else if (event.key.code == sf::Keyboard::Down) hexRadius = std::max(5.0f, hexRadius - 2);
                                    break;
                            }
                        }
                        break;
                    }

                    case sf::Keyboard::F:
                        // Циклическое переключение режимов
                        switch (currentMode) {
                            case Mode::THICKNESS: currentMode = Mode::COLOR; break;
                            case Mode::COLOR: currentMode = Mode::FILL; break;
                            case Mode::FILL: currentMode = Mode::PIVOT; break;
                            case Mode::PIVOT: currentMode = Mode::VERTEX; break;
                            case Mode::VERTEX: currentMode = Mode::THICKNESS; break;
                        }
                        break;

                    case sf::Keyboard::R: {
                        if (currentMode == Mode::FILL) {
                            if (auto* sel = editor.getSelected()) {
                                sf::Color col = sel->getFillColor();
                                if (event.key.shift) col.r = std::min(255, col.r + 10);
                                else col.r = std::max(0, col.r - 10);
                                sel->setFillColor(col);
                                sel->setFilled(true);
                            } else {
                                if (event.key.shift) currentFillColor.r = std::min(255, currentFillColor.r + 10);
                                else currentFillColor.r = std::max(0, currentFillColor.r - 10);
                            }
                        } else if (currentMode == Mode::COLOR) {
                            if (auto* sel = editor.getSelected()) {
                                int numSides = sel->getThicknesses().size();
                                if (selectedIndex < numSides) {
                                    sf::Color col = sel->getSideColor(selectedIndex);
                                    if (event.key.shift) col.r = std::min(255, col.r + 10);
                                    else col.r = std::max(0, col.r - 10);
                                    sel->setSideColor(selectedIndex, col);
                                }
                            } else {
                                if (event.key.shift) currentOutlineColor.r = std::min(255, currentOutlineColor.r + 10);
                                else currentOutlineColor.r = std::max(0, currentOutlineColor.r - 10);
                            }
                        } else if (currentMode == Mode::THICKNESS) {
                            if (event.key.shift) currentOutlineColor.r = std::min(255, currentOutlineColor.r + 10);
                            else currentOutlineColor.r = std::max(0, currentOutlineColor.r - 10);
                        }
                        break;
                    }
                    case sf::Keyboard::G: {
                        if (currentMode == Mode::FILL) {
                            if (auto* sel = editor.getSelected()) {
                                sf::Color col = sel->getFillColor();
                                if (event.key.shift) col.g = std::min(255, col.g + 10);
                                else col.g = std::max(0, col.g - 10);
                                sel->setFillColor(col);
                                sel->setFilled(true);
                            } else {
                                if (event.key.shift) currentFillColor.g = std::min(255, currentFillColor.g + 10);
                                else currentFillColor.g = std::max(0, currentFillColor.g - 10);
                            }
                        } else if (currentMode == Mode::COLOR) {
                            if (auto* sel = editor.getSelected()) {
                                int numSides = sel->getThicknesses().size();
                                if (selectedIndex < numSides) {
                                    sf::Color col = sel->getSideColor(selectedIndex);
                                    if (event.key.shift) col.g = std::min(255, col.g + 10);
                                    else col.g = std::max(0, col.g - 10);
                                    sel->setSideColor(selectedIndex, col);
                                }
                            } else {
                                if (event.key.shift) currentOutlineColor.g = std::min(255, currentOutlineColor.g + 10);
                                else currentOutlineColor.g = std::max(0, currentOutlineColor.g - 10);
                            }
                        } else if (currentMode == Mode::THICKNESS) {
                            if (event.key.shift) currentOutlineColor.g = std::min(255, currentOutlineColor.g + 10);
                            else currentOutlineColor.g = std::max(0, currentOutlineColor.g - 10);
                        }
                        break;
                    }
                    case sf::Keyboard::B: {
                        if (currentMode == Mode::FILL) {
                            if (auto* sel = editor.getSelected()) {
                                sf::Color col = sel->getFillColor();
                                if (event.key.shift) col.b = std::min(255, col.b + 10);
                                else col.b = std::max(0, col.b - 10);
                                sel->setFillColor(col);
                                sel->setFilled(true);
                            } else {
                                if (event.key.shift) currentFillColor.b = std::min(255, currentFillColor.b + 10);
                                else currentFillColor.b = std::max(0, currentFillColor.b - 10);
                            }
                        } else if (currentMode == Mode::COLOR) {
                            if (auto* sel = editor.getSelected()) {
                                int numSides = sel->getThicknesses().size();
                                if (selectedIndex < numSides) {
                                    sf::Color col = sel->getSideColor(selectedIndex);
                                    if (event.key.shift) col.b = std::min(255, col.b + 10);
                                    else col.b = std::max(0, col.b - 10);
                                    sel->setSideColor(selectedIndex, col);
                                }
                            } else {
                                if (event.key.shift) currentOutlineColor.b = std::min(255, currentOutlineColor.b + 10);
                                else currentOutlineColor.b = std::max(0, currentOutlineColor.b - 10);
                            }
                        } else if (currentMode == Mode::THICKNESS) {
                            if (event.key.shift) currentOutlineColor.b = std::min(255, currentOutlineColor.b + 10);
                            else currentOutlineColor.b = std::max(0, currentOutlineColor.b - 10);
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
                            currentThicknesses[selectedIndex] = std::max(0.5f, currentThicknesses[selectedIndex] - 0.5f);
                        } else {
                            currentThicknesses[selectedIndex] += 0.5f;
                        }
                        break;
                    }
                    case sf::Keyboard::Y: {
                        // Переключение индекса (стороны или вершины)
                        int maxIndex = 0;
                        switch (currentShape) {
                            case ShapeType::Rectangle: maxIndex = 4; break;
                            case ShapeType::Triangle: maxIndex = 3; break;
                            case ShapeType::Trapezoid: maxIndex = 4; break;
                            case ShapeType::Circle: maxIndex = 1; break;
                            case ShapeType::Pentagon: maxIndex = 5; break;
                            case ShapeType::Hexagon: maxIndex = 6; break;
                        }
                        if (maxIndex > 0) {
                            selectedIndex = (selectedIndex + 1) % maxIndex;
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
                                newFig = std::make_unique<Rectangle>(rectWidth, rectHeight, currentOutlineColor, thicknesses);
                                break;
                            case ShapeType::Triangle:
                                thicknesses.assign(currentThicknesses.begin(), currentThicknesses.begin()+3);
                                newFig = std::make_unique<Triangle>(triSide, currentOutlineColor, thicknesses);
                                break;
                            case ShapeType::Trapezoid:
                                thicknesses.assign(currentThicknesses.begin(), currentThicknesses.begin()+4);
                                newFig = std::make_unique<Trapezoid>(trapTop, trapBottom, trapHeight, currentOutlineColor, thicknesses);
                                break;
                            case ShapeType::Circle:
                                thicknesses = {currentThicknesses[0]};
                                newFig = std::make_unique<Circle>(circleRadius, currentOutlineColor, thicknesses);
                                break;
                            case ShapeType::Pentagon:
                                thicknesses.assign(currentThicknesses.begin(), currentThicknesses.begin()+5);
                                newFig = std::make_unique<Pentagon>(pentRadius, currentOutlineColor, thicknesses);
                                break;
                            case ShapeType::Hexagon:
                                thicknesses.assign(currentThicknesses.begin(), currentThicknesses.begin()+6);
                                newFig = std::make_unique<Hexagon>(hexRadius, currentOutlineColor, thicknesses);
                                break;
                        }
                        if (newFig) {
                            newFig->setPosition(center);
                            newFig->setFillColor(currentFillColor);
                            newFig->setFilled(currentMode == Mode::FILL); // если режим fill, то заливаем
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
        oss << "\nDefault outline color: " << colorToString(currentOutlineColor);
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
            if (i == selectedIndex)
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

        oss << "\nMode: ";
        switch (currentMode) {
            case Mode::THICKNESS: oss << "THICKNESS"; break;
            case Mode::COLOR: oss << "COLOR (side)"; break;
            case Mode::FILL: oss << "FILL"; break;
            case Mode::PIVOT: oss << "PIVOT"; break;
            case Mode::VERTEX: oss << "VERTEX"; break;
        }

        if (currentMode == Mode::FILL) {
            oss << "\nFill color: ";
            if (auto* sel = editor.getSelected()) {
                oss << colorToString(sel->getFillColor());
            } else {
                oss << colorToString(currentFillColor);
            }
        } else if (currentMode == Mode::COLOR) {
            oss << "\nCurrent side color: ";
            if (auto* sel = editor.getSelected()) {
                if (selectedIndex < sel->getSideColors().size()) {
                    sf::Color col = sel->getSideColor(selectedIndex);
                    oss << colorToString(col);
                } else oss << "N/A";
            } else oss << " (no selection)";
        } else if (currentMode == Mode::THICKNESS) {
            oss << "\nCurrent thickness: " << currentThicknesses[selectedIndex];
        } else if (currentMode == Mode::PIVOT) {
            oss << "\nPivot (local): ";
            if (auto* sel = editor.getSelected()) {
                sf::Vector2f p = sel->getLocalPivot();
                oss << "(" << (int)p.x << ", " << (int)p.y << ")";
            } else {
                oss << " (no selection)";
            }
        } else if (currentMode == Mode::VERTEX) {
    oss << "\nVertex index: " << selectedIndex;
    if (auto* sel = editor.getSelected()) {
        if (selectedIndex < sel->getVertexCount()) {
            sf::Vector2f v = sel->getLocalVertex(selectedIndex);
            sf::Vector2f p = sel->getLocalPivot();
            sf::Vector2f rel = v - p;
            oss << "  relative to pivot: (" << (int)rel.x << ", " << (int)rel.y << ")";
        }
    } else {
        oss << " (no selection)";
    }
}
        

        oss << "\n\nControls:\n"
            << "1-6: select shape\n"
            << "Arrow keys: adjust size / move pivot / move vertex\n"
            << "F: switch mode (THICKNESS/COLOR/FILL/PIVOT/VERTEX)\n"
            << "R/G/B: change value (Shift+ increase)\n"
            << "T: increase thickness, Shift+T: decrease\n"
            << "Y: next side / vertex\n"
            << "Space: add shape at center (uses current fill mode)\n"
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