#include "CompositeFigure.hpp"
#include <algorithm>

CompositeFigure::CompositeFigure() : AbstractFigure() {}

std::unique_ptr<AbstractFigure> CompositeFigure::clone() const {
    auto newComp = std::make_unique<CompositeFigure>();
    newComp->position = position;
    newComp->scaleFactor = scaleFactor;
    newComp->fillColor = fillColor;
    newComp->filled = filled;
    newComp->pivot = pivot;
    for (const auto& child : children) {
        newComp->addFigure(child.figure->clone(), child.localOffset);
    }
    return newComp;
}

void CompositeFigure::addFigure(std::unique_ptr<AbstractFigure> fig, const sf::Vector2f& localPos) {
    children.push_back({std::move(fig), localPos});
}

void CompositeFigure::removeFigure(size_t index) {
    if (index < children.size())
        children.erase(children.begin() + index);
}

std::unique_ptr<AbstractFigure> CompositeFigure::extractFigure(size_t index) {
    if (index >= children.size()) return nullptr;
    auto fig = std::move(children[index].figure);
    children.erase(children.begin() + index);
    return fig;
}

void CompositeFigure::draw(sf::RenderWindow& window) const {
    for (const auto& child : children) {
        sf::Vector2f originalPos = child.figure->getPosition();
        child.figure->setPosition(position + child.localOffset);
        child.figure->draw(window);
        child.figure->setPosition(originalPos);
    }
}

bool CompositeFigure::contains(const sf::Vector2f& point) const {
    for (const auto& child : children) {
        sf::Vector2f originalPos = child.figure->getPosition();
        child.figure->setPosition(position + child.localOffset);
        bool res = child.figure->contains(point);
        child.figure->setPosition(originalPos);
        if (res) return true;
    }
    return false;
}
/*
void CompositeFigure::serialize(std::ostream& out) const {
    AbstractFigure::serialize(out);
    out << children.size() << '\n';
    for (const auto& child : children) {
        child.figure->serialize(out);   // каждый ребёнок пишет свой тип и данные
        out << child.localOffset.x << ' ' << child.localOffset.y << '\n';
    }
}

void CompositeFigure::deserialize(std::istream& in) {
    AbstractFigure::deserialize(in);
    size_t count;
    in >> count;
    children.clear();
    for (size_t i = 0; i < count; ++i) {
        std::string childType;
        in >> childType;   // тип записан первым в serialize ребёнка
        auto child = FigureManager::instance().create(childType);
        if (child) {
            child->deserialize(in);
            sf::Vector2f offset;
            in >> offset.x >> offset.y;
            children.push_back(Child{std::move(child), offset});
        }
    }
}
*/

void CompositeFigure::serialize(std::ostream& out) const {
    AbstractFigure::serialize(out); // Записывает "Composite" и общие поля
    out << children.size() << '\n';
    for (const auto& child : children) {
        // НЕ пишем тип здесь вручную, он запишется внутри child->serialize()
        child.figure->serialize(out); 
        out << child.localOffset.x << ' ' << child.localOffset.y << '\n';
    }
}

void CompositeFigure::deserialize(std::istream& in) {
    AbstractFigure::deserialize(in);
    size_t count;
    if (!(in >> count)) return;

    children.clear();
    for (size_t i = 0; i < count; ++i) {
        std::string childType;
        if (!(in >> childType)) break; // Читаем тип, который записала AbstractFigure::serialize

        auto child = FigureManager::instance().create(childType);
        if (child) {
            child->deserialize(in); // Объект читает свои данные (уже без типа)
            sf::Vector2f offset;
            in >> offset.x >> offset.y;
            children.push_back({std::move(child), offset});
        }
    }
}

sf::FloatRect CompositeFigure::getBoundingBox() const {
    if (children.empty()) return {position.x, position.y, 0, 0};

    sf::FloatRect total;
    bool first = true;

    for (const auto& child : children) {
        // Сохраняем старую позицию
        sf::Vector2f oldPos = child.figure->getPosition();
        
        // Устанавливаем актуальную позицию для расчетов (позиция группы + смещение)
        child.figure->setPosition(this->position + child.localOffset);
        
        sf::FloatRect box = child.figure->getBoundingBox();
        
        // Возвращаем как было
        child.figure->setPosition(oldPos);

        if (first) {
            total = box;
            first = false;
        } else {
            float left = std::min(total.left, box.left);
            float top = std::min(total.top, box.top);
            float right = std::max(total.left + total.width, box.left + box.width);
            float bottom = std::max(total.top + total.height, box.top + box.height);
            total.left = left;
            total.top = top;
            total.width = right - left;
            total.height = bottom - top;
        }
    }
    return total;
}