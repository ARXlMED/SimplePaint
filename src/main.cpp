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
#include "TextBox.hpp"

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

enum class EditTarget {
    NONE,
    GLOBAL_X,
    GLOBAL_Y,
    PIVOT_X,
    PIVOT_Y,
    VERTEX_X,
    VERTEX_Y,
    THICKNESS,
    SIDE_COLOR
};

std::string colorToString(const sf::Color& c) {
    std::ostringstream oss;
    oss << "R:" << (int)c.r << " G:" << (int)c.g << " B:" << (int)c.b;
    return oss.str();
}

std::string modeToString(Mode mode) {
    switch (mode) {
        case Mode::THICKNESS: return "THICKNESS";
        case Mode::COLOR: return "COLOR (side)";
        case Mode::FILL: return "FILL";
        case Mode::PIVOT: return "PIVOT";
        case Mode::VERTEX: return "VERTEX";
        default: return "UNKNOWN";
    }
}

struct InputField {
    sf::FloatRect bounds;
    EditTarget target;
    int index;
};

void drawInputField(sf::RenderWindow& window, const sf::Font& font,
                    const std::string& label, const std::string& value,
                    float x, float y, float width, float height,
                    std::vector<InputField>& fields, EditTarget target, int index = -1) {
    sf::RectangleShape rect({width, height});
    rect.setPosition(x, y);
    rect.setFillColor(sf::Color::White);
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(1);
    window.draw(rect);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Black);
    text.setPosition(x + 5, y + 5);
    text.setString(value);
    window.draw(text);

    fields.push_back({rect.getGlobalBounds(), target, index});

    sf::Text labelText;
    labelText.setFont(font);
    labelText.setCharacterSize(18);
    labelText.setFillColor(sf::Color::White);
    labelText.setPosition(x, y - 22);
    labelText.setString(label);
    window.draw(labelText);
}

void drawEditWindow(sf::RenderWindow& window, Figure* fig, const sf::Font& font,
                    Mode currentMode, int selectedIndex,
                    std::vector<InputField>& fields, sf::FloatRect& panelBounds) {
    sf::Vector2u winSize = window.getSize();
    float panelWidth = 420;
    float panelHeight = 1050;
    float panelX = winSize.x - panelWidth - 20;
    float panelY = 20;

    sf::RectangleShape panel({panelWidth, panelHeight});
    panel.setPosition(panelX, panelY);
    panel.setFillColor(sf::Color(0, 0, 0, 200));
    panel.setOutlineColor(sf::Color::White);
    panel.setOutlineThickness(2);
    window.draw(panel);
    panelBounds = sf::FloatRect(panelX, panelY, panelWidth, panelHeight);

    fields.clear();

    float marginLeft = 20;
    float marginTop = 20;
    float currentY = panelY + marginTop;
    float lineSpacing = 45;
    float fieldWidth = 140;
    float fieldHeight = 40;
    float smallFieldWidth = 100;
    float colorBoxSize = 40;

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(30);
    title.setFillColor(sf::Color::White);
    title.setPosition(panelX + marginLeft, currentY);
    title.setString("EDIT FIGURE");
    window.draw(title);
    currentY += 50;

    sf::Text modeText;
    modeText.setFont(font);
    modeText.setCharacterSize(24);
    modeText.setFillColor(sf::Color::White);
    modeText.setPosition(panelX + marginLeft, currentY);
    modeText.setString("Mode: " + modeToString(currentMode));
    window.draw(modeText);
    currentY += lineSpacing;

    float fieldX1 = panelX + marginLeft;
    float fieldX2 = fieldX1 + fieldWidth + 20;
    float fieldY = currentY + 10;

    // Global X
    sf::Text labelX;
    labelX.setFont(font);
    labelX.setCharacterSize(18);
    labelX.setFillColor(sf::Color::White);
    labelX.setPosition(fieldX1, fieldY - 22);
    labelX.setString("Global X");
    window.draw(labelX);

    sf::RectangleShape rectX({fieldWidth, fieldHeight});
    rectX.setPosition(fieldX1, fieldY);
    rectX.setFillColor(sf::Color::White);
    rectX.setOutlineColor(sf::Color::Black);
    rectX.setOutlineThickness(1);
    window.draw(rectX);
    fields.push_back({rectX.getGlobalBounds(), EditTarget::GLOBAL_X, 0});

    sf::Text valX;
    valX.setFont(font);
    valX.setCharacterSize(24);
    valX.setFillColor(sf::Color::Black);
    valX.setPosition(fieldX1 + 5, fieldY + 5);
    valX.setString(std::to_string((int)fig->getPosition().x));
    window.draw(valX);

    // Global Y
    sf::Text labelY;
    labelY.setFont(font);
    labelY.setCharacterSize(18);
    labelY.setFillColor(sf::Color::White);
    labelY.setPosition(fieldX2, fieldY - 22);
    labelY.setString("Global Y");
    window.draw(labelY);

    sf::RectangleShape rectY({fieldWidth, fieldHeight});
    rectY.setPosition(fieldX2, fieldY);
    rectY.setFillColor(sf::Color::White);
    rectY.setOutlineColor(sf::Color::Black);
    rectY.setOutlineThickness(1);
    window.draw(rectY);
    fields.push_back({rectY.getGlobalBounds(), EditTarget::GLOBAL_Y, 0});

    sf::Text valY;
    valY.setFont(font);
    valY.setCharacterSize(24);
    valY.setFillColor(sf::Color::Black);
    valY.setPosition(fieldX2 + 5, fieldY + 5);
    valY.setString(std::to_string((int)fig->getPosition().y));
    window.draw(valY);

    currentY += fieldHeight + lineSpacing - 40;

    // Scale
    sf::Text scaleText;
    scaleText.setFont(font);
    scaleText.setCharacterSize(24);
    scaleText.setFillColor(sf::Color::White);
    scaleText.setPosition(panelX + marginLeft, currentY + 10);
    scaleText.setString("Scale: " + std::to_string(fig->getScale()));
    window.draw(scaleText);
    currentY += lineSpacing;

    fieldY = currentY + 20;

    // Pivot X
    sf::Text labelPivotX;
    labelPivotX.setFont(font);
    labelPivotX.setCharacterSize(18);
    labelPivotX.setFillColor(sf::Color::White);
    labelPivotX.setPosition(fieldX1, fieldY - 22);
    labelPivotX.setString("Pivot X");
    window.draw(labelPivotX);

    sf::RectangleShape rectPX({fieldWidth, fieldHeight});
    rectPX.setPosition(fieldX1, fieldY);
    rectPX.setFillColor(sf::Color::White);
    rectPX.setOutlineColor(sf::Color::Black);
    rectPX.setOutlineThickness(1);
    window.draw(rectPX);
    fields.push_back({rectPX.getGlobalBounds(), EditTarget::PIVOT_X, 0});

    sf::Text valPX;
    valPX.setFont(font);
    valPX.setCharacterSize(24);
    valPX.setFillColor(sf::Color::Black);
    valPX.setPosition(fieldX1 + 5, fieldY + 5);
    valPX.setString(std::to_string((int)fig->getLocalPivot().x));
    window.draw(valPX);

    // Pivot Y
    sf::Text labelPivotY;
    labelPivotY.setFont(font);
    labelPivotY.setCharacterSize(18);
    labelPivotY.setFillColor(sf::Color::White);
    labelPivotY.setPosition(fieldX2, fieldY - 22);
    labelPivotY.setString("Pivot Y");
    window.draw(labelPivotY);

    sf::RectangleShape rectPY({fieldWidth, fieldHeight});
    rectPY.setPosition(fieldX2, fieldY);
    rectPY.setFillColor(sf::Color::White);
    rectPY.setOutlineColor(sf::Color::Black);
    rectPY.setOutlineThickness(1);
    window.draw(rectPY);
    fields.push_back({rectPY.getGlobalBounds(), EditTarget::PIVOT_Y, 0});

    sf::Text valPY;
    valPY.setFont(font);
    valPY.setCharacterSize(24);
    valPY.setFillColor(sf::Color::Black);
    valPY.setPosition(fieldX2 + 5, fieldY + 5);
    valPY.setString(std::to_string((int)fig->getLocalPivot().y));
    window.draw(valPY);

    currentY += fieldHeight + lineSpacing - 20;

    // Filled info
    sf::Text fillText;
    fillText.setFont(font);
    fillText.setCharacterSize(24);
    fillText.setFillColor(sf::Color::White);
    fillText.setPosition(panelX + marginLeft, currentY);
    fillText.setString("Filled: " + std::string(fig->isFilled() ? "yes" : "no"));
    window.draw(fillText);
    currentY += 30;

    sf::Text fillColorText;
    fillColorText.setFont(font);
    fillColorText.setCharacterSize(24);
    fillColorText.setFillColor(sf::Color::White);
    fillColorText.setPosition(panelX + marginLeft, currentY);
    fillColorText.setString("Fill color: " + colorToString(fig->getFillColor()));
    window.draw(fillColorText);
    currentY += lineSpacing;

    // Thicknesses & colors
    sf::Text thickTitle;
    thickTitle.setFont(font);
    thickTitle.setCharacterSize(24);
    thickTitle.setFillColor(sf::Color::White);
    thickTitle.setPosition(panelX + marginLeft, currentY);
    thickTitle.setString("Thicknesses & colors:");
    window.draw(thickTitle);
    currentY += 35;

    const auto& thick = fig->getThicknesses();
    const auto& colors = fig->getSideColors();
    for (size_t i = 0; i < thick.size(); ++i) {
        sf::Text sideLabel;
        sideLabel.setFont(font);
        sideLabel.setCharacterSize(20);
        sideLabel.setFillColor(sf::Color::White);
        sideLabel.setPosition(panelX + marginLeft, currentY + 5);
        sideLabel.setString("Side " + std::to_string(i) + ":");
        window.draw(sideLabel);

        sf::RectangleShape rectThick({smallFieldWidth, fieldHeight});
        rectThick.setPosition(panelX + marginLeft + 80, currentY);
        rectThick.setFillColor(sf::Color::White);
        rectThick.setOutlineColor(sf::Color::Black);
        rectThick.setOutlineThickness(1);
        window.draw(rectThick);
        fields.push_back({rectThick.getGlobalBounds(), EditTarget::THICKNESS, (int)i});

        sf::Text valThick;
        valThick.setFont(font);
        valThick.setCharacterSize(24);
        valThick.setFillColor(sf::Color::Black);
        valThick.setPosition(panelX + marginLeft + 85, currentY + 5);
        valThick.setString(std::to_string((int)thick[i]));
        window.draw(valThick);

        sf::RectangleShape colorRect({colorBoxSize, colorBoxSize});
        colorRect.setFillColor(colors[i]);
        colorRect.setPosition(panelX + marginLeft + 80 + smallFieldWidth + 20, currentY);
        window.draw(colorRect);
        fields.push_back({colorRect.getGlobalBounds(), EditTarget::SIDE_COLOR, (int)i});

        if ((currentMode == Mode::THICKNESS || currentMode == Mode::COLOR) && i == (size_t)selectedIndex) {
            sf::Text marker;
            marker.setFont(font);
            marker.setCharacterSize(28);
            marker.setFillColor(sf::Color::Yellow);
            marker.setPosition(panelX + marginLeft + 80 + smallFieldWidth + 20 + colorBoxSize + 10, currentY);
            marker.setString("<--");
            window.draw(marker);
        }

        currentY += fieldHeight + 8;
    }
    currentY += 15;

    // Vertices (compact view)
    if (currentMode == Mode::VERTEX) {
        sf::Text vertexTitle;
        vertexTitle.setFont(font);
        vertexTitle.setCharacterSize(24);
        vertexTitle.setFillColor(sf::Color::White);
        vertexTitle.setPosition(panelX + marginLeft, currentY);
        vertexTitle.setString("Vertices (relative to pivot):");
        window.draw(vertexTitle);
        currentY += 35;

        for (size_t i = 0; i < fig->getVertexCount(); ++i) {
            sf::Vector2f v = fig->getLocalVertex(i);
            sf::Vector2f p = fig->getLocalPivot();
            sf::Vector2f rel = v - p;

            float baseX = panelX + marginLeft + 30;

            sf::Text vertexLabel;
            vertexLabel.setFont(font);
            vertexLabel.setCharacterSize(22);
            vertexLabel.setFillColor(sf::Color::White);
            vertexLabel.setPosition(panelX + marginLeft, currentY + 5);
            vertexLabel.setString("V" + std::to_string(i) + ":");
            window.draw(vertexLabel);

            sf::RectangleShape rectVX({fieldWidth, fieldHeight});
            rectVX.setPosition(baseX, currentY);
            rectVX.setFillColor(sf::Color::White);
            rectVX.setOutlineColor(sf::Color::Black);
            rectVX.setOutlineThickness(1);
            window.draw(rectVX);
            fields.push_back({rectVX.getGlobalBounds(), EditTarget::VERTEX_X, (int)i});

            sf::Text valVX;
            valVX.setFont(font);
            valVX.setCharacterSize(24);
            valVX.setFillColor(sf::Color::Black);
            valVX.setPosition(baseX + 5, currentY + 5);
            valVX.setString(std::to_string((int)rel.x));
            window.draw(valVX);

            float baseY = baseX + fieldWidth + 10;
            sf::RectangleShape rectVY({fieldWidth, fieldHeight});
            rectVY.setPosition(baseY, currentY);
            rectVY.setFillColor(sf::Color::White);
            rectVY.setOutlineColor(sf::Color::Black);
            rectVY.setOutlineThickness(1);
            window.draw(rectVY);
            fields.push_back({rectVY.getGlobalBounds(), EditTarget::VERTEX_Y, (int)i});

            sf::Text valVY;
            valVY.setFont(font);
            valVY.setCharacterSize(24);
            valVY.setFillColor(sf::Color::Black);
            valVY.setPosition(baseY + 5, currentY + 5);
            valVY.setString(std::to_string((int)rel.y));
            window.draw(valVY);

            currentY += fieldHeight + 8;
        }
    }

    sf::Text hint;
    hint.setFont(font);
    hint.setCharacterSize(18);
    hint.setFillColor(sf::Color::Yellow);
    hint.setPosition(panelX + marginLeft, currentY + 10);
    hint.setString("Click white fields to edit");
    window.draw(hint);
}

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Simple Paint", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    Editor editor;

    ShapeType currentShape = ShapeType::Rectangle;
    float rectWidth = 150, rectHeight = 100;
    float triSide = 120;
    float trapTop = 80, trapBottom = 140, trapHeight = 100;
    float circleRadius = 70;
    float pentRadius = 80;
    float hexRadius = 80;

    sf::Color currentOutlineColor = sf::Color::White;
    std::vector<int> currentThicknesses(6, 2); // целые
    int selectedIndex = 0;
    Mode currentMode = Mode::THICKNESS;
    sf::Color currentFillColor = sf::Color::White;

    auto createInitialShapes = [&]() {
        sf::Vector2u winSize = window.getSize();
        float cx = winSize.x / 2.f;
        float cy = winSize.y / 2.f;

        /*auto rect = std::make_unique<Rectangle>(150, 100, sf::Color::Red, std::vector<float>{2,2,2,2});
        rect->setPosition({cx - 250, cy - 150});
        rect->setFillColor(sf::Color::Yellow);
        rect->setFilled(true);
        editor.addFigure(std::move(rect));

        auto tri = std::make_unique<Triangle>(120, sf::Color::Green, std::vector<float>{3,3,3});
        tri->setPosition({cx + 250, cy - 200});
        tri->setFillColor(sf::Color::Cyan);
        tri->setFilled(true);
        editor.addFigure(std::move(tri));

        auto trap = std::make_unique<Trapezoid>(80, 140, 100, sf::Color::Blue, std::vector<float>{2,2,2,2});
        trap->setPosition({cx, cy});
        trap->setFillColor(sf::Color::Magenta);
        trap->setFilled(true);
        editor.addFigure(std::move(trap));

        auto pent = std::make_unique<Pentagon>(80, sf::Color::Yellow, std::vector<float>{2,2,2,2,2});
        pent->setPosition({cx - 250, cy + 200});
        pent->setFillColor(sf::Color::White);
        pent->setFilled(true);
        editor.addFigure(std::move(pent));

        auto hex = std::make_unique<Hexagon>(80, sf::Color::Cyan, std::vector<float>{2,2,2,2,2,2});
        hex->setPosition({cx + 250, cy + 200});
        hex->setFillColor(sf::Color::Red);
        hex->setFilled(true);
        editor.addFigure(std::move(hex));*/
    };
    createInitialShapes();

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

    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(16);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(10, 10);
    std::ostringstream helpOss;
    helpOss << "F1: toggle help\n"
            << "1-6: select shape\n"
            << "Click figure to edit\n"
            << "Arrow keys: adjust param\n"
            << "F: switch mode\n"
            << "R/G/B: change color (Shift+ inc)\n"
            << "T: inc thickness, Shift+T: dec\n"
            << "Y: next side/vertex\n"
            << "Delete: remove\n"
            << "Mouse wheel: scale";
    helpText.setString(helpOss.str());

    sf::Text exitButton;
    exitButton.setFont(font);
    exitButton.setCharacterSize(28);
    exitButton.setFillColor(sf::Color::Red);
    exitButton.setString("X");
    exitButton.setPosition(window.getSize().x - exitButton.getLocalBounds().width - 20, 10);

    bool showHelp = true;

    TextBox inputBox(font, 24);
    EditTarget currentEditTarget = EditTarget::NONE;
    int editIndex = 0;
    std::vector<InputField> inputFields;
    sf::FloatRect panelBounds;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (inputBox.isActive()) {
                inputBox.handleEvent(event);
                continue;
            }

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);

                sf::FloatRect exitRect(exitButton.getPosition().x, exitButton.getPosition().y,
                                        exitButton.getLocalBounds().width, exitButton.getLocalBounds().height);
                if (exitRect.contains(worldPos)) {
                    window.close();
                }

                if (panelBounds.contains(worldPos)) {
                    for (const auto& field : inputFields) {
                        if (field.bounds.contains(worldPos)) {
                            Figure* sel = editor.getSelected();
                            if (!sel) break;

                            float initialValue = 0;
                            sf::Color initialColor;
                            bool isColorTarget = false;
                            switch (field.target) {
                                case EditTarget::GLOBAL_X: initialValue = sel->getPosition().x; break;
                                case EditTarget::GLOBAL_Y: initialValue = sel->getPosition().y; break;
                                case EditTarget::PIVOT_X: initialValue = sel->getLocalPivot().x; break;
                                case EditTarget::PIVOT_Y: initialValue = sel->getLocalPivot().y; break;
                                case EditTarget::VERTEX_X: initialValue = sel->getLocalVertex(field.index).x; break;
                                case EditTarget::VERTEX_Y: initialValue = sel->getLocalVertex(field.index).y; break;
                                case EditTarget::THICKNESS:
                                    initialValue = sel->getThicknesses()[field.index];
                                    editIndex = field.index;
                                    break;
                                case EditTarget::SIDE_COLOR:
                                    initialColor = sel->getSideColor(field.index);
                                    isColorTarget = true;
                                    editIndex = field.index;
                                    break;
                                default: break;
                            }

                            if (isColorTarget) {
                                // Здесь можно добавить диалог выбора цвета, пока игнорируем
                            } else {
                                currentEditTarget = field.target;
                                inputBox.setPosition(field.bounds.left, field.bounds.top);
                                inputBox.setSize(field.bounds.width, field.bounds.height);
                                inputBox.activate(initialValue);
                            }
                            break;
                        }
                    }
                } else {
                    editor.handleEvent(event, window);
                }
            } else {
                editor.handleEvent(event, window);
            }

            if (event.type == sf::Event::MouseWheelScrolled && editor.getSelected()) {
                float delta = event.mouseWheelScroll.delta;
                editor.handleScale(delta);
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::F1) {
                    showHelp = !showHelp;
                }

                // Выбор типа фигуры цифрами 1-6
                if (event.key.code == sf::Keyboard::Num1) currentShape = ShapeType::Rectangle;
                else if (event.key.code == sf::Keyboard::Num2) currentShape = ShapeType::Triangle;
                else if (event.key.code == sf::Keyboard::Num3) currentShape = ShapeType::Trapezoid;
                else if (event.key.code == sf::Keyboard::Num4) currentShape = ShapeType::Circle;
                else if (event.key.code == sf::Keyboard::Num5) currentShape = ShapeType::Pentagon;
                else if (event.key.code == sf::Keyboard::Num6) currentShape = ShapeType::Hexagon;

                // Переключение режимов по F
                if (event.key.code == sf::Keyboard::F && !event.key.shift) {
                    switch (currentMode) {
                        case Mode::THICKNESS: currentMode = Mode::COLOR; break;
                        case Mode::COLOR: currentMode = Mode::FILL; break;
                        case Mode::FILL: currentMode = Mode::PIVOT; break;
                        case Mode::PIVOT: currentMode = Mode::VERTEX; break;
                        case Mode::VERTEX: currentMode = Mode::THICKNESS; break;
                    }
                }

                // Стрелки для пивота и вершин
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down ||
                    event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
                    if (editor.getSelected()) {
                        Figure* sel = editor.getSelected();
                        int dx = 0, dy = 0;
                        if (event.key.code == sf::Keyboard::Up) dy = -1;
                        else if (event.key.code == sf::Keyboard::Down) dy = 1;
                        else if (event.key.code == sf::Keyboard::Left) dx = -1;
                        else if (event.key.code == sf::Keyboard::Right) dx = 1;

                        if (currentMode == Mode::PIVOT) {
                            sf::Vector2f newPivot = sel->getLocalPivot() + sf::Vector2f(dx, dy);
                            sel->setLocalPivot(newPivot);
                        }
                        else if (currentMode == Mode::VERTEX) {
                            if (selectedIndex < (int)sel->getVertexCount()) {
                                sf::Vector2f newPos = sel->getLocalVertex(selectedIndex) + sf::Vector2f(dx, dy);
                                sel->setLocalVertex(selectedIndex, newPos);
                            }
                        }
                    } else {
                        // Стрелки меняют параметры будущих фигур
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
                }

                // Обработка R, G, B
                if (event.key.code == sf::Keyboard::R) {
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
                }
                if (event.key.code == sf::Keyboard::G) {
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
                }
                if (event.key.code == sf::Keyboard::B) {
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
                }

                // Обработка T для толщины
                if (event.key.code == sf::Keyboard::T) {
                    if (auto* sel = editor.getSelected()) {
                        int numSides = sel->getThicknesses().size();
                        if (selectedIndex < numSides) {
                            int newThick = (int)sel->getThicknesses()[selectedIndex];
                            if (event.key.shift) newThick = std::max(1, newThick - 1);
                            else newThick += 1;
                            sel->setThickness(selectedIndex, newThick);
                        }
                    } else {
                        int maxIndex = 0;
                        switch (currentShape) {
                            case ShapeType::Rectangle: maxIndex = 4; break;
                            case ShapeType::Triangle: maxIndex = 3; break;
                            case ShapeType::Trapezoid: maxIndex = 4; break;
                            case ShapeType::Circle: maxIndex = 1; break;
                            case ShapeType::Pentagon: maxIndex = 5; break;
                            case ShapeType::Hexagon: maxIndex = 6; break;
                        }
                        if (maxIndex > 0 && selectedIndex < maxIndex) {
                            int newThick = currentThicknesses[selectedIndex];
                            if (event.key.shift) newThick = std::max(1, newThick - 1);
                            else newThick += 1;
                            currentThicknesses[selectedIndex] = newThick;
                        }
                    }
                }

                // Обработка Y для переключения индекса
                if (event.key.code == sf::Keyboard::Y && !event.key.shift) {
                    int maxIndex = 0;
                    if (editor.getSelected()) {
                        if (currentMode == Mode::VERTEX) {
                            maxIndex = editor.getSelected()->getVertexCount();
                        } else {
                            maxIndex = editor.getSelected()->getThicknesses().size();
                        }
                    } else {
                        switch (currentShape) {
                            case ShapeType::Rectangle: maxIndex = 4; break;
                            case ShapeType::Triangle: maxIndex = 3; break;
                            case ShapeType::Trapezoid: maxIndex = 4; break;
                            case ShapeType::Circle: maxIndex = 1; break;
                            case ShapeType::Pentagon: maxIndex = 5; break;
                            case ShapeType::Hexagon: maxIndex = 6; break;
                        }
                    }
                    if (maxIndex > 0) {
                        selectedIndex = (selectedIndex + 1) % maxIndex;
                    }
                }

                // Добавление новой фигуры (Space)
                if (event.key.code == sf::Keyboard::Space) {
                    std::vector<float> thicknesses;
                    sf::Vector2u winSize = window.getSize();
                    sf::Vector2f center(winSize.x / 2.f, winSize.y / 2.f);
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
                            thicknesses = {(float)currentThicknesses[0]};
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
                        newFig->setFilled(currentMode == Mode::FILL);
                        editor.addFigure(std::move(newFig));
                    }
                }

                // Удаление выделенной фигуры
                if (event.key.code == sf::Keyboard::Delete) {
                    editor.removeSelected();
                }
            }
        }

        if (!inputBox.isActive() && currentEditTarget != EditTarget::NONE) {
            float val = inputBox.getValue();
            if (auto* sel = editor.getSelected()) {
                switch (currentEditTarget) {
                    case EditTarget::GLOBAL_X: {
                        sf::Vector2f pos = sel->getPosition();
                        pos.x = val;
                        sel->setPosition(pos);
                        break;
                    }
                    case EditTarget::GLOBAL_Y: {
                        sf::Vector2f pos = sel->getPosition();
                        pos.y = val;
                        sel->setPosition(pos);
                        break;
                    }
                    case EditTarget::PIVOT_X: {
                        sf::Vector2f p = sel->getLocalPivot();
                        p.x = val;
                        sel->setLocalPivot(p);
                        break;
                    }
                    case EditTarget::PIVOT_Y: {
                        sf::Vector2f p = sel->getLocalPivot();
                        p.y = val;
                        sel->setLocalPivot(p);
                        break;
                    }
                    case EditTarget::VERTEX_X: {
                        sf::Vector2f v = sel->getLocalVertex(editIndex);
                        v.x = val;
                        sel->setLocalVertex(editIndex, v);
                        break;
                    }
                    case EditTarget::VERTEX_Y: {
                        sf::Vector2f v = sel->getLocalVertex(editIndex);
                        v.y = val;
                        sel->setLocalVertex(editIndex, v);
                        break;
                    }
                    case EditTarget::THICKNESS: {
                        int intVal = std::round(val);
                        sel->setThickness(editIndex, intVal);
                        break;
                    }
                    default: break;
                }
            }
            currentEditTarget = EditTarget::NONE;
        }

        window.clear(sf::Color(50,50,50));
        editor.draw(window);
        if (showHelp) {
            window.draw(helpText);
        }
        if (editor.getSelected()) {
            drawEditWindow(window, editor.getSelected(), font, currentMode, selectedIndex, inputFields, panelBounds);
        }
        inputBox.update();
        inputBox.draw(window);
        window.draw(exitButton);
        window.display();
    }
    return 0;
}