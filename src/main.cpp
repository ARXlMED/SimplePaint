#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>

#include "Editor.hpp"
#include "Rectangle.hpp"
#include "Triangle.hpp"
#include "Trapezoid.hpp"
#include "Circle.hpp"
#include "Pentagon.hpp"
#include "Hexagon.hpp"
#include "CompositeFigure.hpp"
#include "FigureManager.hpp"
#include "TextBox.hpp"

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
    SIDE_COLOR,
    CIRCLE_THICKNESS,
    CIRCLE_COLOR,
    POLYLINE_ANGLE, 
    POLYLINE_LENGTH
};

struct ShapeListItem {
    sf::FloatRect bounds;
    AbstractFigure* figure;
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

std::string openLinuxDialog(bool saveMode, sf::RenderWindow& window) {
    char buffer[128];
    std::string result = "";
    
    // Получаем идентификатор окна X11
    sf::WindowHandle handle = window.getSystemHandle();
    std::string attachArg = " --attach=" + std::to_string(reinterpret_cast<uintptr_t>(handle)) + " --modal";
    
    std::string cmd = saveMode ?
        "zenity --file-selection --save --confirm-overwrite --file-filter='*.txt'" + attachArg :
        "zenity --file-selection --file-filter='*.txt'" + attachArg;
    
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "";
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
}


void drawEditWindow(sf::RenderWindow& window, AbstractFigure* fig, const sf::Font& font,
                    Mode currentMode, int selectedIndex,
                    std::vector<InputField>& fields, sf::FloatRect& panelBounds) {
    sf::Vector2u winSize = window.getSize();
    float panelWidth = 420;
    float panelHeight = 1100;
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
    float fieldY = currentY;

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
    scaleText.setPosition(panelX + marginLeft, currentY);
    scaleText.setString("Scale: " + std::to_string(fig->getScale()));
    window.draw(scaleText);
    currentY += lineSpacing;

    fieldY = currentY;

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

    currentY += fieldHeight + lineSpacing - 40;

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

    if (auto* poly = dynamic_cast<PolylineFigure*>(fig)) {
        sf::Text thickTitle;
        thickTitle.setFont(font);
        thickTitle.setCharacterSize(24);
        thickTitle.setFillColor(sf::Color::White);
        thickTitle.setPosition(panelX + marginLeft, currentY);
        thickTitle.setString("Thicknesses & colors:");
        window.draw(thickTitle);
        currentY += 35;

        const auto& thick = poly->getThicknesses();
        const auto& colors = poly->getSideColors();
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
    }
    else if (auto* circle = dynamic_cast<Circle*>(fig)) {
        sf::Text outlineTitle;
        outlineTitle.setFont(font);
        outlineTitle.setCharacterSize(24);
        outlineTitle.setFillColor(sf::Color::White);
        outlineTitle.setPosition(panelX + marginLeft, currentY);
        outlineTitle.setString("Outline:");
        window.draw(outlineTitle);
        currentY += 35;

        sf::RectangleShape rectThick({smallFieldWidth, fieldHeight});
        rectThick.setPosition(panelX + marginLeft, currentY);
        rectThick.setFillColor(sf::Color::White);
        rectThick.setOutlineColor(sf::Color::Black);
        rectThick.setOutlineThickness(1);
        window.draw(rectThick);
        fields.push_back({rectThick.getGlobalBounds(), EditTarget::CIRCLE_THICKNESS, 0});

        sf::Text valThick;
        valThick.setFont(font);
        valThick.setCharacterSize(24);
        valThick.setFillColor(sf::Color::Black);
        valThick.setPosition(panelX + marginLeft + 5, currentY + 5);
        valThick.setString(std::to_string((int)circle->getOutlineThickness()));
        window.draw(valThick);

        sf::RectangleShape colorRect({colorBoxSize, colorBoxSize});
        colorRect.setFillColor(circle->getOutlineColor());
        colorRect.setPosition(panelX + marginLeft + smallFieldWidth + 20, currentY);
        window.draw(colorRect);
        fields.push_back({colorRect.getGlobalBounds(), EditTarget::CIRCLE_COLOR, 0});

        currentY += fieldHeight + 8;
    }
    if (fig->getVertexCount() > 0 && currentMode == Mode::VERTEX) {
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
    hint.setString("Click white fields or color squares to edit");
    window.draw(hint);
}

void drawPolylineCreationWindow(sf::RenderWindow& window, const sf::Font& font,
                                float angle, float length,
                                std::vector<InputField>& fields, sf::FloatRect& panelBounds) {
    sf::Vector2u winSize = window.getSize();
    float panelWidth = 320;
    float panelHeight = 220;
    float panelX = winSize.x - panelWidth - 20;
    float panelY = 20;

    sf::RectangleShape panel({panelWidth, panelHeight});
    panel.setPosition(panelX, panelY);
    panel.setFillColor(sf::Color(0, 0, 0, 200));
    panel.setOutlineColor(sf::Color::Green);
    panel.setOutlineThickness(2);
    window.draw(panel);
    panelBounds = sf::FloatRect(panelX, panelY, panelWidth, panelHeight);

    fields.clear();
    float currentY = panelY + 20;
    float marginLeft = 20;

    sf::Text title;
    title.setFont(font);
    title.setCharacterSize(24);
    title.setFillColor(sf::Color::Green);
    title.setPosition(panelX + marginLeft, currentY);
    title.setString("CREATE POLYLINE");
    window.draw(title);
    currentY += 50;

    drawInputField(window, font, "Angle (deg)", std::to_string((int)angle),
                   panelX + marginLeft, currentY, 120, 40, fields, EditTarget::POLYLINE_ANGLE);
    drawInputField(window, font, "Length", std::to_string((int)length),
                   panelX + marginLeft + 140, currentY, 120, 40, fields, EditTarget::POLYLINE_LENGTH);

    currentY += 60;
    sf::Text hint;
    hint.setFont(font);
    hint.setCharacterSize(16);
    hint.setFillColor(sf::Color::White);
    hint.setPosition(panelX + marginLeft, currentY);
    hint.setString("Click to add point\nor press 'P' to draw by params.\nPress Enter to finish.");
    window.draw(hint);
}

int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Simple Paint", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    Editor editor;

    float rectWidth = 150, rectHeight = 100;
    float triSide = 120;
    float trapTop = 80, trapBottom = 140, trapHeight = 100;
    float circleRadius = 70;
    float pentRadius = 80;
    float hexRadius = 80;

    bool fileDialogActive = false;

    sf::Color currentOutlineColor = sf::Color::White;
    std::vector<float> currentThicknesses(6, 2.0f);
    int selectedIndex = 0;
    Mode currentMode = Mode::THICKNESS;
    sf::Color currentFillColor = sf::Color::White;

    // ─── Регистрация фабрик с setTypeName ─────────────────
    FigureManager::instance().registerFactory("Rectangle",
        [&]() -> std::unique_ptr<AbstractFigure> {
            std::vector<float> t(currentThicknesses.begin(), currentThicknesses.begin()+4);
            auto r = std::make_unique<Rectangle>(rectWidth, rectHeight, currentOutlineColor, t);
            r->setTypeName("Rectangle");
            return r;
        });
    FigureManager::instance().registerFactory("Triangle",
        [&]() -> std::unique_ptr<AbstractFigure> {
            std::vector<float> t(currentThicknesses.begin(), currentThicknesses.begin()+3);
            auto r = std::make_unique<Triangle>(triSide, currentOutlineColor, t);
            r->setTypeName("Triangle");
            return r;
        });
    FigureManager::instance().registerFactory("Trapezoid",
        [&]() -> std::unique_ptr<AbstractFigure> {
            std::vector<float> t(currentThicknesses.begin(), currentThicknesses.begin()+4);
            auto r = std::make_unique<Trapezoid>(trapTop, trapBottom, trapHeight, currentOutlineColor, t);
            r->setTypeName("Trapezoid");
            return r;
        });
    FigureManager::instance().registerFactory("Circle",
        [&]() -> std::unique_ptr<AbstractFigure> {
            auto r = std::make_unique<Circle>(circleRadius, currentOutlineColor, currentThicknesses[0]);
            r->setTypeName("Circle");
            return r;
        });
    FigureManager::instance().registerFactory("Pentagon",
        [&]() -> std::unique_ptr<AbstractFigure> {
            std::vector<float> t(currentThicknesses.begin(), currentThicknesses.begin()+5);
            auto r = std::make_unique<Pentagon>(pentRadius, currentOutlineColor, t);
            r->setTypeName("Pentagon");
            return r;
        });
    FigureManager::instance().registerFactory("Hexagon",
        [&]() -> std::unique_ptr<AbstractFigure> {
            std::vector<float> t(currentThicknesses.begin(), currentThicknesses.begin()+6);
            auto r = std::make_unique<Hexagon>(hexRadius, currentOutlineColor, t);
            r->setTypeName("Hexagon");
            return r;
        });
    FigureManager::instance().registerFactory("PolylineFigure",
        []() -> std::unique_ptr<AbstractFigure> {
            auto p = std::make_unique<PolylineFigure>(sf::Color::White, std::vector<float>{2.f});
            p->setTypeName("PolylineFigure");
            return p;
        });
    FigureManager::instance().registerFactory("CompositeFigure",
        []() -> std::unique_ptr<AbstractFigure> {
            auto c = std::make_unique<CompositeFigure>();
            c->setTypeName("CompositeFigure");
            return c;
        });

    std::string currentShapeName = "Rectangle";

    std::vector<AbstractFigure*> multiSelected;

    bool creatingPolyline = false;
    bool waitingForPolylineName = false;
    bool nameInputActive = false;
    std::string pendingPolylineName;

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

    TextBox inputBox(font, 24);
    TextBox nameInputBox(font, 24);
    nameInputBox.setTextMode(true);

    sf::RectangleShape saveBtnRect({100, 30});
    saveBtnRect.setFillColor(sf::Color(50, 150, 50));
    saveBtnRect.setPosition(10, 10);

    sf::Text saveBtnText("SAVE", font, 18);
    saveBtnText.setPosition(35, 14);

    sf::RectangleShape loadBtnRect({100, 30});
    loadBtnRect.setFillColor(sf::Color(50, 50, 150));
    loadBtnRect.setPosition(120, 10);

    sf::Text loadBtnText("LOAD", font, 18);
    loadBtnText.setPosition(145, 14);


    sf::Text helpText;
    helpText.setFont(font);
    helpText.setCharacterSize(16);
    helpText.setFillColor(sf::Color::White);
    helpText.setPosition(10, 10);
    std::ostringstream helpOss;
    helpOss << "F1: toggle help\n"
            << "1-6: select built-in shape\n"
            << "Click: select single\n"
            << "Shift+Click: toggle multi-select\n"
            << "Arrow keys: adjust param\n"
            << "F: switch mode\n"
            << "R/G/B: change color (Shift+ inc)\n"
            << "L: fill/unfill selected figure\n"
            << "T: inc thickness, Shift+T: dec\n"
            << "Y: next side/vertex\n"
            << "Z: group selected\n"
            << "U: ungroup selected composite\n"
            << "N: new polyline\n"
            << "P: when Polyline with parameters\n"
            << "Enter (when creating): finish polyline\n"
            << "Delete: remove selected\n"
            << "Mouse wheel: scale\n"
            << "F5: save scene to scene.txt\n"
            << "F9: load scene from scene.txt\n"
            << "Click shape name in left panel to select";
    helpText.setString(helpOss.str());

    sf::Text exitButton;
    exitButton.setFont(font);
    exitButton.setCharacterSize(28);
    exitButton.setFillColor(sf::Color::Red);
    exitButton.setString("X");
    exitButton.setPosition(window.getSize().x - exitButton.getLocalBounds().width - 20, 10);

    bool showHelp = true;

    EditTarget currentEditTarget = EditTarget::NONE;
    int editIndex = 0;
    std::vector<InputField> inputFields;
    sf::FloatRect panelBounds;

    float currentDrawAngle = 0.0f;
    float currentDrawLength = 100.0f;
    float accumulatedHeading = 0.0f;

    // ─── Начальные фигуры (10 штук, сырые указатели) ─────
    auto createInitialShapes = [&]() {
        sf::Vector2u winSize = window.getSize();
        float cx = winSize.x / 2.f;
        float cy = winSize.y / 2.f;

        // 1. Rectangle
        auto* rect = new Rectangle(150, 100, sf::Color::White, std::vector<float>{3.f, 5.f, 7.f, 2.f});
        rect->setPosition({cx - 300, cy - 200});
        rect->setFillColor(sf::Color(255, 0, 0));
        rect->setFilled(true);
        rect->setSideColor(0, sf::Color::Green);
        rect->setSideColor(1, sf::Color::Blue);
        rect->setSideColor(2, sf::Color::Yellow);
        rect->setSideColor(3, sf::Color::Magenta);
        rect->setTypeName("Rectangle");
        editor.addFigure(rect);

        // 2. Triangle
        auto* tri = new Triangle(120, sf::Color::White, std::vector<float>{2.f, 4.f, 6.f});
        tri->setPosition({cx - 150, cy - 200});
        tri->setFillColor(sf::Color(0, 0, 255));
        tri->setFilled(true);
        tri->setSideColor(0, sf::Color::Red);
        tri->setSideColor(1, sf::Color::Green);
        tri->setSideColor(2, sf::Color::Yellow);
        tri->setTypeName("Triangle");
        editor.addFigure(tri);

        // 3. Trapezoid
        auto* trap = new Trapezoid(80, 140, 100, sf::Color::White, std::vector<float>{4.f, 4.f, 8.f, 2.f});
        trap->setPosition({cx, cy - 200});
        trap->setFillColor(sf::Color(0, 255, 0));
        trap->setFilled(true);
        trap->setSideColor(0, sf::Color::Cyan);
        trap->setSideColor(1, sf::Color::Magenta);
        trap->setSideColor(2, sf::Color::White);
        trap->setSideColor(3, sf::Color::Red);
        trap->setTypeName("Trapezoid");
        editor.addFigure(trap);

        // 4. Circle
        auto* circle = new Circle(70, sf::Color::White, 5.f);
        circle->setPosition({cx - 400, cy});
        circle->setFillColor(sf::Color(255, 255, 0));
        circle->setFilled(true);
        circle->setOutlineColor(sf::Color::White);
        circle->setTypeName("Circle");
        editor.addFigure(circle);

        // 5. Pentagon
        auto* pent = new Pentagon(80, sf::Color::White, std::vector<float>{2.f, 3.f, 4.f, 5.f, 6.f});
        pent->setPosition({cx - 250, cy});
        pent->setFillColor(sf::Color(0, 255, 255));
        pent->setFilled(true);
        pent->setSideColor(0, sf::Color::Blue);
        pent->setSideColor(1, sf::Color::Red);
        pent->setSideColor(2, sf::Color::Green);
        pent->setSideColor(3, sf::Color::Yellow);
        pent->setSideColor(4, sf::Color::Magenta);
        pent->setTypeName("Pentagon");
        editor.addFigure(pent);

        // 6. Hexagon
        auto* hex = new Hexagon(80, sf::Color::White, std::vector<float>{1.f, 2.f, 3.f, 4.f, 5.f, 6.f});
        hex->setPosition({cx - 100, cy});
        hex->setFillColor(sf::Color(255, 0, 255));
        hex->setFilled(true);
        hex->setSideColor(0, sf::Color::White);
        hex->setSideColor(1, sf::Color::Red);
        hex->setSideColor(2, sf::Color::Green);
        hex->setSideColor(3, sf::Color::Blue);
        hex->setSideColor(4, sf::Color::Yellow);
        hex->setSideColor(5, sf::Color::Cyan);
        hex->setTypeName("Hexagon");
        editor.addFigure(hex);

        // 7. Polyline triangle
        auto* poly3 = new PolylineFigure(sf::Color::White, std::vector<float>{3.f, 3.f, 3.f});
        poly3->addVertex({0, -50});
        poly3->addVertex({60, 30});
        poly3->addVertex({-60, 30});
        poly3->setPosition({cx + 150, cy + 150});
        poly3->setFillColor(sf::Color(255, 165, 0));
        poly3->setFilled(true);
        poly3->setSideColor(0, sf::Color::Red);
        poly3->setSideColor(1, sf::Color::Green);
        poly3->setSideColor(2, sf::Color::Blue);
        poly3->setTypeName("PolylineFigure");
        editor.addFigure(poly3);

        // 9. Rectangle 2
        auto* rect2 = new Rectangle(130, 130, sf::Color::White, std::vector<float>{6.f, 1.f, 6.f, 1.f});
        rect2->setPosition({cx + 200, cy - 300});
        rect2->setFillColor(sf::Color(128, 128, 128));
        rect2->setFilled(true);
        rect2->setSideColor(0, sf::Color::Cyan);
        rect2->setSideColor(1, sf::Color::Magenta);
        rect2->setSideColor(2, sf::Color::Yellow);
        rect2->setSideColor(3, sf::Color::Red);
        rect2->setTypeName("Rectangle");
        editor.addFigure(rect2);

        // 10. Triangle 2
        auto* tri2 = new Triangle(100, sf::Color::White, std::vector<float>{5.f, 5.f, 5.f});
        tri2->setPosition({cx + 300, cy - 300});
        tri2->setFillColor(sf::Color(0, 255, 0));
        tri2->setFilled(true);
        tri2->setSideColor(0, sf::Color::Blue);
        tri2->setSideColor(1, sf::Color::Yellow);
        tri2->setSideColor(2, sf::Color::Red);
        tri2->setTypeName("Triangle");
        editor.addFigure(tri2);
    };
    createInitialShapes();

    sf::FloatRect shapeListBounds;
    std::vector<ShapeListItem> shapeListItems;

    sf::Clock doubleClickClock;
    AbstractFigure* lastClickedFig = nullptr;
    const float doubleClickThreshold = 0.3f;
    AbstractFigure* currentRenamingFigure = nullptr;

    sf::RectangleShape btnSave({70, 30}), btnLoad({70, 30});
    btnSave.setPosition(10, 5); btnLoad.setPosition(90, 5);
    btnSave.setFillColor(sf::Color(70, 70, 70)); btnLoad.setFillColor(sf::Color(70, 70, 70));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (fileDialogActive) {
                // Игнорируем все события, пока активен внешний диалог
                if (event.type == sf::Event::Closed) window.close();
                continue;
            }
            if (nameInputBox.isActive()) {
                nameInputBox.handleEvent(event);
                continue;
            }
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
                if (btnSave.getGlobalBounds().contains(worldPos)) {
                    fileDialogActive = true;
                    std::string path = openLinuxDialog(true, window);
                    if (!path.empty()) editor.saveToFile(path);
                    fileDialogActive = false;
                }
                if (btnLoad.getGlobalBounds().contains(worldPos)) {
                    fileDialogActive = true;
                    std::string path = openLinuxDialog(false, window);
                    if (!path.empty()) editor.loadFromFile(path);
                    fileDialogActive = false;
                }
                else if (shapeListBounds.contains(worldPos)) {
                    for (const auto& item : shapeListItems) {
                        if (item.bounds.contains(worldPos) && item.figure) {
                            multiSelected.clear();
                            editor.setSelected(item.figure);
                            break;
                        }
                    }
                }
                else if (panelBounds.contains(worldPos)) {
                    for (const auto& field : inputFields) {
                        if (field.bounds.contains(worldPos)) {
                            AbstractFigure* sel = editor.getSelected();
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
                                case EditTarget::POLYLINE_ANGLE: initialValue = currentDrawAngle; break;
                                case EditTarget::POLYLINE_LENGTH: initialValue = currentDrawLength; break;
                                case EditTarget::THICKNESS:
                                    if (auto* poly = dynamic_cast<PolylineFigure*>(sel)) {
                                        initialValue = poly->getThicknesses()[field.index];
                                        editIndex = field.index;
                                    }
                                    break;
                                case EditTarget::SIDE_COLOR:
                                    if (auto* poly = dynamic_cast<PolylineFigure*>(sel)) {
                                        initialColor = poly->getSideColors()[field.index];
                                        isColorTarget = true;
                                        editIndex = field.index;
                                    }
                                    break;
                                case EditTarget::CIRCLE_THICKNESS:
                                    if (auto* circle = dynamic_cast<Circle*>(sel)) {
                                        currentEditTarget = field.target;
                                        inputBox.setPosition(field.bounds.left, field.bounds.top);
                                        inputBox.setSize(field.bounds.width, field.bounds.height);
                                        inputBox.activate(circle->getOutlineThickness());
                                    }
                                    break;
                                case EditTarget::CIRCLE_COLOR:
                                    break;
                                default: break;
                            }

                            if (!isColorTarget) {
                                currentEditTarget = field.target;
                                inputBox.setPosition(field.bounds.left, field.bounds.top);
                                inputBox.setSize(field.bounds.width, field.bounds.height);
                                inputBox.activate(initialValue);
                            }
                            break;
                        }
                    }
                } 
                else {
                    if (creatingPolyline) {
                        AbstractFigure* sel = editor.getSelected();
                        if (sel) {
                            sel->addVertex(worldPos);
                            if (sel->getVertexCount() >= 2) {
                                sf::Vector2f v1 = sel->getLocalVertex(sel->getVertexCount() - 2);
                                sf::Vector2f v2 = sel->getLocalVertex(sel->getVertexCount() - 1);
                                accumulatedHeading = std::atan2(v2.y - v1.y, v2.x - v1.x) * 180.0f / 3.14159265f;
                            }
                        }
                    } 
                    else {
                        AbstractFigure* clickedFigure = editor.findFigureAt(worldPos);
                        bool shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
                                            sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
                        
                        if (shiftPressed) {
                            if (clickedFigure) {
                                auto it = std::find(multiSelected.begin(), multiSelected.end(), clickedFigure);
                                if (it != multiSelected.end()) multiSelected.erase(it);
                                else multiSelected.push_back(clickedFigure);
                            }
                        } else {
                            multiSelected.clear();
                            
                            if (clickedFigure) {
                                float elapsed = doubleClickClock.getElapsedTime().asSeconds();
                                
                                if (clickedFigure == lastClickedFig && elapsed < doubleClickThreshold) {
                                    nameInputBox.setPosition(worldPos.x, worldPos.y - 40);
                                    nameInputBox.setSize(200, 30);
                                    nameInputBox.setLabel("Rename figure:");
                                    nameInputBox.activate(0); 
                                    nameInputBox.setTextMode(true);
                                    nameInputBox.setString(clickedFigure->getTypeName());
                                    currentRenamingFigure = clickedFigure;
                                    nameInputActive = true;
                                } else {
                                    editor.handleEvent(event, window);
                                    lastClickedFig = clickedFigure;
                                    doubleClickClock.restart();
                                }
                            } else {
                                editor.handleEvent(event, window);
                                lastClickedFig = nullptr;
                            }
                        }
                    }
                }
            }
            else {
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

                if (event.key.code == sf::Keyboard::Num1) currentShapeName = "Rectangle";
                else if (event.key.code == sf::Keyboard::Num2) currentShapeName = "Triangle";
                else if (event.key.code == sf::Keyboard::Num3) currentShapeName = "Trapezoid";
                else if (event.key.code == sf::Keyboard::Num4) currentShapeName = "Circle";
                else if (event.key.code == sf::Keyboard::Num5) currentShapeName = "Pentagon";
                else if (event.key.code == sf::Keyboard::Num6) currentShapeName = "Hexagon";

                if (event.key.code == sf::Keyboard::F && !event.key.shift) {
                    switch (currentMode) {
                        case Mode::THICKNESS: currentMode = Mode::COLOR; break;
                        case Mode::COLOR: currentMode = Mode::FILL; break;
                        case Mode::FILL: currentMode = Mode::PIVOT; break;
                        case Mode::PIVOT: currentMode = Mode::VERTEX; break;
                        case Mode::VERTEX: currentMode = Mode::THICKNESS; break;
                    }
                }
                if (event.key.code == sf::Keyboard::L) {
                    AbstractFigure* selected = editor.getSelected();
                    if (selected) {
                        bool currentState = selected->isFilled();
                        selected->setFilled(!currentState);
                        if (selected->getFillColor() == sf::Color::Transparent || selected->getFillColor() == sf::Color{0,0,0,0}) {
                            selected->setFillColor(sf::Color::Blue);
                        }
                        std::cout << "Figure fill toggled: " << (!currentState ? "YES" : "NO") << std::endl;
                    }
                }

                if (event.key.code == sf::Keyboard::R || event.key.code == sf::Keyboard::G || event.key.code == sf::Keyboard::B) {
                    AbstractFigure* sel = editor.getSelected();
                    if (!sel) continue;

                    int step = event.key.shift ? -10 : 10;

                    if (currentMode == Mode::FILL) {
                        sf::Color c = sel->getFillColor();
                        switch (event.key.code) {
                            case sf::Keyboard::R: c.r = std::clamp((int)c.r + step, 0, 255); break;
                            case sf::Keyboard::G: c.g = std::clamp((int)c.g + step, 0, 255); break;
                            case sf::Keyboard::B: c.b = std::clamp((int)c.b + step, 0, 255); break;
                            default: break;
                        }
                        sel->setFillColor(c);
                    }
                    else if (currentMode == Mode::COLOR) {
                        if (auto* poly = dynamic_cast<PolylineFigure*>(sel)) {
                            if (selectedIndex < 0 || selectedIndex >= (int)poly->getSideColors().size()) continue;
                            sf::Color c = poly->getSideColor(selectedIndex);
                            switch (event.key.code) {
                                case sf::Keyboard::R: c.r = std::clamp((int)c.r + step, 0, 255); break;
                                case sf::Keyboard::G: c.g = std::clamp((int)c.g + step, 0, 255); break;
                                case sf::Keyboard::B: c.b = std::clamp((int)c.b + step, 0, 255); break;
                                default: break;
                            }
                            poly->setSideColor(selectedIndex, c);
                        }
                        else if (auto* circle = dynamic_cast<Circle*>(sel)) {
                            sf::Color c = circle->getOutlineColor();
                            switch (event.key.code) {
                                case sf::Keyboard::R: c.r = std::clamp((int)c.r + step, 0, 255); break;
                                case sf::Keyboard::G: c.g = std::clamp((int)c.g + step, 0, 255); break;
                                case sf::Keyboard::B: c.b = std::clamp((int)c.b + step, 0, 255); break;
                                default: break;
                            }
                            circle->setOutlineColor(c);
                        }
                    }
                }

                if (event.key.code == sf::Keyboard::P && creatingPolyline) {
                    AbstractFigure* sel = editor.getSelected();
                    if (sel) {
                        sf::Vector2f lastVert;
                        if (sel->getVertexCount() == 0) {
                            lastVert = sf::Vector2f(window.getSize().x / 2.f, window.getSize().y / 2.f);
                            sel->addVertex(lastVert);
                        } else {
                            lastVert = sel->getLocalVertex(sel->getVertexCount() - 1);
                        }
                        accumulatedHeading += currentDrawAngle;
                        float rad = accumulatedHeading * M_PI / 180.0f;
                        float dx = std::round(currentDrawLength * std::cos(rad));
                        float dy = std::round(currentDrawLength * std::sin(rad));
                        sel->addVertex(lastVert + sf::Vector2f(dx, dy));
                    }
                }

                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down ||
                    event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
                    if (editor.getSelected()) {
                        AbstractFigure* sel = editor.getSelected();
                        int dx = 0, dy = 0;
                        if (event.key.code == sf::Keyboard::Up) dy = -1;
                        else if (event.key.code == sf::Keyboard::Down) dy = 1;
                        else if (event.key.code == sf::Keyboard::Left) dx = -1;
                        else if (event.key.code == sf::Keyboard::Right) dx = 1;

                        if (currentMode == Mode::PIVOT) {
                            sel->setLocalPivot(sel->getLocalPivot() + sf::Vector2f(dx, dy));
                        }
                        else if (currentMode == Mode::VERTEX) {
                            if (selectedIndex < (int)sel->getVertexCount()) {
                                sf::Vector2f newPos = sel->getLocalVertex(selectedIndex) + sf::Vector2f(dx, dy);
                                sel->setLocalVertex(selectedIndex, newPos);
                            }
                        }
                    }
                }

                // Z – группировка (использует extractFigure)
                if (event.key.code == sf::Keyboard::Z && !event.key.shift) {
                    std::vector<AbstractFigure*> toGroup;
                    if (!multiSelected.empty()) {
                        toGroup = multiSelected;
                    } else if (AbstractFigure* sel = editor.getSelected()) {
                        toGroup.push_back(sel);
                    }
                    if (toGroup.size() >= 2) {
                        auto* composite = new CompositeFigure();
                        composite->setTypeName("CompositeFigure");
                        sf::Vector2f sumPos(0.f, 0.f);
                        std::vector<sf::Vector2f> globalPositions;
                        for (auto* fig : toGroup) {
                            globalPositions.push_back(fig->getPosition());
                            sumPos += fig->getPosition();
                        }
                        sf::Vector2f centerPos = sumPos / (float)toGroup.size();
                        composite->setPosition(centerPos);

                        for (size_t i = 0; i < toGroup.size(); ++i) {
                            auto ptr = editor.extractFigure(toGroup[i]);
                            if (ptr) {
                                sf::Vector2f localOffset = globalPositions[i] - centerPos;
                                composite->addFigure(std::move(ptr), localOffset);
                            }
                        }
                        editor.addFigure(composite);
                        editor.setSelected(composite);
                        multiSelected.clear();
                    }
                }

                // U – разгруппировка (полный её вариант)
                if (event.key.code == sf::Keyboard::U) {
                    AbstractFigure* sel = editor.getSelected();
                    if (sel) {
                        // 1. Целая группа
                        if (auto* composite = dynamic_cast<CompositeFigure*>(sel)) {
                            sf::Vector2f compPos = composite->getPosition();
                            size_t n = composite->getChildCount();
                            std::vector<std::unique_ptr<AbstractFigure>> children;
                            std::vector<sf::Vector2f> offsets;
                            for (size_t i = 0; i < n; ++i) {
                                offsets.push_back(composite->getChildOffset(i));
                            }
                            for (size_t i = 0; i < n; ++i) {
                                auto child = composite->extractFigure(0);
                                if (child) children.push_back(std::move(child));
                            }
                            editor.removeFigure(composite);
                            for (size_t i = 0; i < children.size(); ++i) {
                                children[i]->setPosition(compPos + offsets[i]);
                                editor.addFigure(children[i].release());
                            }
                            editor.setSelected(editor.getFigure(editor.getFigureCount() - 1));
                        }
                        // 2. Отдельный элемент в группе
                        else {
                            CompositeFigure* parent = nullptr;
                            size_t index = 0;
                            int total = editor.getFigureCount();
                            for (int i = 0; i < total; ++i) {
                                AbstractFigure* f = editor.getFigure(i);
                                if (auto* comp = dynamic_cast<CompositeFigure*>(f)) {
                                    for (size_t j = 0; j < comp->getChildCount(); ++j) {
                                        if (comp->getChild(j) == sel) {
                                            parent = comp;
                                            index = j;
                                            break;
                                        }
                                    }
                                    if (parent) break;
                                }
                            }
                            if (parent) {
                                sf::Vector2f offset = parent->getChildOffset(index);
                                auto child = parent->extractFigure(index);
                                if (child) {
                                    child->setPosition(parent->getPosition() + offset);
                                    editor.addFigure(child.release());
                                    if (parent->getChildCount() == 1) {
                                        auto last = parent->extractFigure(0);
                                        last->setPosition(parent->getPosition() + parent->getChildOffset(0));
                                        editor.addFigure(last.release());
                                        editor.removeFigure(parent);
                                    }
                                    editor.setSelected(sel);
                                }
                            }
                        }
                    }
                }

                if (event.key.code == sf::Keyboard::N && !creatingPolyline && !waitingForPolylineName) {
                    creatingPolyline = true;
                    accumulatedHeading = 0.0f;

                    nameInputBox.setPosition(window.getSize().x/2 - 100, window.getSize().y/2 - 15);
                    nameInputBox.setSize(200, 30);
                    nameInputBox.activate(0);
                    nameInputBox.setLabel("Enter polyline name:");
                    waitingForPolylineName = true;
                    nameInputActive = false;
                }

                if (event.key.code == sf::Keyboard::T) {
                    if (auto* sel = editor.getSelected()) {
                        if (auto* poly = dynamic_cast<PolylineFigure*>(sel)) {
                            if (selectedIndex < (int)poly->getThicknesses().size()) {
                                float newThick = poly->getThicknesses()[selectedIndex];
                                if (event.key.shift) newThick = std::max(1.0f, newThick - 1.0f);
                                else newThick += 1.0f;
                                poly->setThickness(selectedIndex, newThick);
                            }
                        }
                    } else {
                        if (selectedIndex < (int)currentThicknesses.size()) {
                            float newThick = currentThicknesses[selectedIndex];
                            if (event.key.shift) newThick = std::max(1.0f, newThick - 1.0f);
                            else newThick += 1.0f;
                            currentThicknesses[selectedIndex] = newThick;
                        }
                    }
                }

                if (event.key.code == sf::Keyboard::Y && !event.key.shift) {
                    int maxIndex = 0;
                    if (editor.getSelected()) {
                        if (currentMode == Mode::VERTEX) {
                            maxIndex = editor.getSelected()->getVertexCount();
                        } else if (auto* poly = dynamic_cast<PolylineFigure*>(editor.getSelected())) {
                            maxIndex = poly->getThicknesses().size();
                        }
                    } else {
                        maxIndex = 6;
                    }
                    if (maxIndex > 0) selectedIndex = (selectedIndex + 1) % maxIndex;
                }

                if (event.key.code == sf::Keyboard::Space && !creatingPolyline && !waitingForPolylineName) {
                    auto newFig = FigureManager::instance().create(currentShapeName);
                    if (newFig) {
                        sf::Vector2u winSize = window.getSize();
                        sf::Vector2f center(winSize.x / 2.f, winSize.y / 2.f);
                        newFig->setPosition(center);
                        newFig->setFillColor(currentFillColor);
                        newFig->setFilled(currentMode == Mode::FILL);
                        editor.addFigure(newFig.release());
                    }
                }

                if (event.key.code == sf::Keyboard::Delete) {
                    editor.removeSelected();
                }

                if (event.key.code == sf::Keyboard::Enter && creatingPolyline) {
                    AbstractFigure* sel = editor.getSelected();
                    if (sel && sel->getVertexCount() > 0) {
                        sf::Vector2f sum(0,0);
                        for (size_t i = 0; i < sel->getVertexCount(); ++i)
                            sum += sel->getLocalVertex(i);
                        sf::Vector2f center = sum / (float)sel->getVertexCount();
                        sel->setPosition(center);
                        for (size_t i = 0; i < sel->getVertexCount(); ++i) {
                            sf::Vector2f loc = sel->getLocalVertex(i) - center;
                            sel->setLocalVertex(i, loc);
                        }
                        if (!pendingPolylineName.empty()) {
                            FigureManager::instance().registerPrototype(pendingPolylineName, sel->clone());
                            currentShapeName = pendingPolylineName;
                            pendingPolylineName.clear();
                        }
                    }
                    creatingPolyline = false;
                }

                if (event.key.code == sf::Keyboard::Escape && creatingPolyline) {
                    editor.removeSelected();
                    creatingPolyline = false;
                }

                if (event.key.code == sf::Keyboard::F5) {
                    editor.saveToFile("scene.txt");
                }
                if (event.key.code == sf::Keyboard::F9) {
                    editor.loadFromFile("scene.txt");
                }
            }
        }

        if (waitingForPolylineName && !nameInputBox.isActive()) {
            std::string userEnteredName = nameInputBox.getString(); 
            if (userEnteredName.empty()) userEnteredName = "Polyline";
            auto newPoly = std::make_unique<PolylineFigure>(currentOutlineColor, std::vector<float>{2});
            newPoly->setCustomName(userEnteredName);
            editor.addFigure(newPoly.release());
            editor.setSelected(editor.getFigure(editor.getFigureCount() - 1));
            creatingPolyline = true;
            waitingForPolylineName = false;
            nameInputActive = false;
        }

        if (nameInputActive && !nameInputBox.isActive()) {
            if (currentRenamingFigure != nullptr) {
                if (!nameInputBox.getString().empty()) {
                    currentRenamingFigure->setCustomName(nameInputBox.getString());
                }
                currentRenamingFigure = nullptr;
            }
            else if (waitingForPolylineName) {
                pendingPolylineName = nameInputBox.getString();
                auto newPoly = std::make_unique<PolylineFigure>(currentOutlineColor, std::vector<float>{2});
                editor.addFigure(newPoly.release());
                editor.setSelected(editor.getFigure(editor.getFigureCount() - 1));
                creatingPolyline = true;
                waitingForPolylineName = false;
            }
            nameInputActive = false;
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
                    case EditTarget::POLYLINE_ANGLE: currentDrawAngle = val; break;
                    case EditTarget::POLYLINE_LENGTH: currentDrawLength = val; break;
                    case EditTarget::THICKNESS: {
                        if (auto* poly = dynamic_cast<PolylineFigure*>(sel)) {
                            int intVal = std::round(val);
                            poly->setThickness(editIndex, intVal);
                        }
                        break;
                    }
                    case EditTarget::CIRCLE_THICKNESS:
                        if (auto* circle = dynamic_cast<Circle*>(sel)) {
                            circle->setOutlineThickness(inputBox.getValue());
                        }
                        break;
                    default: break;
                }
            }
            currentEditTarget = EditTarget::NONE;
        }

        window.clear(sf::Color(50,50,50));
        editor.draw(window);

        for (auto* fig : multiSelected) {
            sf::FloatRect bounds = fig->getBoundingBox();
            sf::RectangleShape rect({bounds.width, bounds.height});
            rect.setPosition(bounds.left, bounds.top);
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineColor(sf::Color::Red);
            rect.setOutlineThickness(2);
            window.draw(rect);
        }

        if (showHelp) window.draw(helpText);
        if (creatingPolyline) {
            drawPolylineCreationWindow(window, font, currentDrawAngle, currentDrawLength, inputFields, panelBounds);
        } else if (editor.getSelected()) {
            drawEditWindow(window, editor.getSelected(), font, currentMode, selectedIndex, inputFields, panelBounds);
        } else {
            panelBounds = sf::FloatRect(0, 0, 0, 0);
        }

        float listWidth = 200;
        float listHeight = 450;
        float listX = 10;
        float listY = window.getSize().y - listHeight - 10;
        shapeListBounds = sf::FloatRect(listX, listY, listWidth, listHeight);

        sf::RectangleShape listBg({listWidth, listHeight});
        listBg.setPosition(listX, listY);
        listBg.setFillColor(sf::Color(0, 0, 0, 200));
        listBg.setOutlineColor(sf::Color::White);
        listBg.setOutlineThickness(2);
        window.draw(listBg);

        sf::Text listTitle;
        listTitle.setFont(font);
        listTitle.setCharacterSize(20);
        listTitle.setFillColor(sf::Color::White);
        listTitle.setPosition(listX + 5, listY + 5);
        listTitle.setString("Shapes on Scene");
        window.draw(listTitle);

        shapeListItems.clear();
        float itemY = listY + 30;
        int totalFigures = editor.getFigureCount();

        auto addListItem = [&](AbstractFigure* fig, int depth, const std::string& label) {
            std::string indent(depth * 4, ' ');
            sf::Text item;
            item.setFont(font);
            item.setCharacterSize(18);
            item.setString(indent + label);
            item.setFillColor(editor.getSelected() == fig ? sf::Color::Yellow : sf::Color::White);
            item.setPosition(listX + 5, itemY);
            window.draw(item);

            sf::FloatRect bounds = item.getGlobalBounds();
            bounds.width = listWidth - 10;
            ShapeListItem listItem;
            listItem.bounds = bounds;
            listItem.figure = fig;
            shapeListItems.push_back(listItem);
            itemY += 22;
        };

        for (int i = 0; i < totalFigures; ++i) {
            AbstractFigure* fig = editor.getFigure(i);
            if (!fig) continue;
            std::string label = fig->getCustomName() + " #" + std::to_string(i+1);
            addListItem(fig, 0, label);
            if (auto* comp = dynamic_cast<CompositeFigure*>(fig)) {
                for (size_t j = 0; j < comp->getChildCount(); ++j) {
                    addListItem(comp->getChild(j), 1, "-> " + comp->getChild(j)->getTypeName());
                }
            }
        }

        window.draw(saveBtnRect);
        window.draw(saveBtnText);
        window.draw(loadBtnRect);
        window.draw(loadBtnText);
        inputBox.update();
        inputBox.draw(window);
        nameInputBox.update();
        nameInputBox.draw(window);
        window.draw(exitButton);
        window.display();
    }
    return 0;
}