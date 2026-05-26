#include "Editor.hpp"
#include "CompositeFigure.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cmath>

Editor::Editor() {
    for (int i = 0; i < MAX_FIGURES; ++i) figures[i] = nullptr;
}

Editor::~Editor() {
    for (int i = 0; i < figureCount; ++i) delete figures[i];
}

void Editor::addFigure(AbstractFigure* fig) {
    if (figureCount < MAX_FIGURES) {
        figures[figureCount++] = fig;
    }
}

bool Editor::removeFigure(AbstractFigure* fig) {
    for (int i = 0; i < figureCount; ++i) {
        if (figures[i] == fig) {
            delete figures[i];
            for (int j = i; j < figureCount - 1; ++j) {
                figures[j] = figures[j + 1];
            }
            figures[--figureCount] = nullptr;
            if (selectedFigure == fig) selectedFigure = nullptr;
            return true;
        }
    }
    return false;
}

void Editor::removeSelected() {
    if (selectedFigure) {
        removeFigure(selectedFigure);
        selectedFigure = nullptr;
        dragging = false;
    }
}

AbstractFigure* Editor::getSelected() const {
    return selectedFigure;
}

void Editor::setSelected(AbstractFigure* fig) {
    selectedFigure = fig;
}

bool Editor::isSelectedValid() const {
    if (!selectedFigure) return false;
    for (int i = 0; i < figureCount; ++i)
        if (figures[i] == selectedFigure) return true;
    return false;
}

AbstractFigure* Editor::findFigureAt(const sf::Vector2f& point) {
    for (int i = figureCount - 1; i >= 0; --i) {
        if (figures[i]->contains(point))
            return figures[i];
    }
    return nullptr;
}

size_t Editor::getFigureCount() const { return figureCount; }

AbstractFigure* Editor::getFigure(size_t index) {
    return (index < (size_t)figureCount) ? figures[index] : nullptr;
}

const AbstractFigure* const* Editor::getFigures() const { return figures; }

void Editor::handleEvent(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
        selectedFigure = nullptr;
        // Поиск фигуры под курсором
        for (int i = figureCount - 1; i >= 0; --i) {
            if (figures[i]->contains(mouse)) {
                selectedFigure = figures[i];
                dragOffset = selectedFigure->getPosition() - mouse;
                dragging = true;
                break;
            }
        }
    }
    else if (event.type == sf::Event::MouseMoved && dragging && selectedFigure) {
        sf::Vector2f mouse = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
        selectedFigure->setPosition(mouse + dragOffset);
    }
    else if (event.type == sf::Event::MouseButtonReleased &&
             event.mouseButton.button == sf::Mouse::Left) {
        dragging = false;
    }
}

void Editor::draw(sf::RenderWindow& window) {
    // 1. Рисуем все фигуры из массива
    for (int i = 0; i < figureCount; ++i) {
        figures[i]->draw(window);
    }

    // 2. Рисуем выделение (рамка и пивот)
    if (selectedFigure) {
        CompositeFigure* parent = nullptr;
        sf::Vector2f offset(0.f, 0.f);

        // Проверяем, не находится ли selectedFigure внутри группы
        for (int i = 0; i < figureCount; ++i) {
            if (auto* comp = dynamic_cast<CompositeFigure*>(figures[i])) {
                for (size_t j = 0; j < comp->getChildCount(); ++j) {
                    if (comp->getChild(j) == selectedFigure) {
                        parent = comp;
                        offset = comp->getChildOffset(j);
                        break;
                    }
                }
            }
            if (parent) break;
        }

        sf::Vector2f originalPos = selectedFigure->getPosition();
        if (parent) selectedFigure->setPosition(parent->getPosition() + offset);

        sf::FloatRect bounds = selectedFigure->getBoundingBox();
        sf::RectangleShape rect({bounds.width, bounds.height});
        rect.setPosition(bounds.left, bounds.top);
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color::Red);
        rect.setOutlineThickness(2);
        window.draw(rect);

        // Пивот
        sf::Vector2f pivotPos = selectedFigure->getGlobalPivot();
        float radius = 5.f;
        sf::CircleShape pivotMarker(radius);
        pivotMarker.setOrigin(radius, radius);
        pivotMarker.setPosition(std::round(pivotPos.x), std::round(pivotPos.y));
        pivotMarker.setFillColor(sf::Color::Yellow);
        pivotMarker.setOutlineColor(sf::Color::Black);
        pivotMarker.setOutlineThickness(1.f);
        window.draw(pivotMarker);

        sf::RectangleShape lineH({radius * 2, 1});
        lineH.setOrigin(radius, 0.5f);
        lineH.setPosition(pivotMarker.getPosition());
        lineH.setFillColor(sf::Color::Black);
        window.draw(lineH);

        sf::RectangleShape lineV({1, radius * 2});
        lineV.setOrigin(0.5f, radius);
        lineV.setPosition(pivotMarker.getPosition());
        lineV.setFillColor(sf::Color::Black);
        window.draw(lineV);

        if (parent) selectedFigure->setPosition(originalPos);
    }
}

void Editor::handleScale(float delta) {
    if (selectedFigure) {
        float newScale = selectedFigure->getScale() * (1.0f + delta * 0.1f);
        if (newScale > 0.1f && newScale < 5.0f)
            selectedFigure->setScale(newScale);
    }
}

void Editor::saveToFile(const std::string& filename) {
    std::ofstream out(filename);
    if (!out) return;
    out << figureCount << '\n';
    for (int i = 0; i < figureCount; ++i) {
        figures[i]->serialize(out);
    }
}

void Editor::loadFromFile(const std::string& filename) {
    // Очищаем текущую сцену
    for (int i = 0; i < figureCount; ++i) delete figures[i];
    figureCount = 0;
    selectedFigure = nullptr;

    std::ifstream in(filename);
    if (!in) return;
    int count;
    in >> count;
    for (int i = 0; i < count; ++i) {
        std::string type;
        in >> type;   // тип фигуры
        auto figPtr = FigureManager::instance().create(type);
        if (figPtr) {
            figPtr->deserialize(in);
            addFigure(figPtr.release());  // передаём владение
        } else {
            // пропустить строки этой фигуры? Лучше выйти или попытаться пропустить.
            std::cerr << "Unknown figure type: " << type << std::endl;
            // Пропускаем до конца фигуры? Сложно, для простоты прерываем загрузку.
            break;
        }
    }
}

std::unique_ptr<AbstractFigure> Editor::extractFigure(AbstractFigure* fig) {
    for (int i = 0; i < figureCount; ++i) {
        if (figures[i] == fig) {
            std::unique_ptr<AbstractFigure> ptr(figures[i]);
            for (int j = i; j < figureCount - 1; ++j) {
                figures[j] = figures[j + 1];
            }
            figures[--figureCount] = nullptr;
            if (selectedFigure == fig) selectedFigure = nullptr;
            return ptr;
        }
    }
    return nullptr;
}