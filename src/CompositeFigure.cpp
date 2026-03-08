#include "CompositeFigure.hpp"
#include <algorithm>

CompositeFigure::CompositeFigure() : AbstractFigure() {}

void CompositeFigure::addFigure(std::unique_ptr<AbstractFigure> fig, const sf::Vector2f& localPos) {
    children.push_back({std::move(fig), localPos});
}

void CompositeFigure::removeFigure(size_t index) {
    if (index < children.size())
        children.erase(children.begin() + index);
}

void CompositeFigure::draw(sf::RenderWindow& window) const {
    for (const auto& child : children) {
        sf::Vector2f originalPos = child.figure->getPosition();
        child.figure->setPosition(position + child.localOffset * scaleFactor);
        child.figure->draw(window);
        child.figure->setPosition(originalPos);
    }
}

bool CompositeFigure::contains(const sf::Vector2f& point) const {
    for (const auto& child : children) {
        sf::Vector2f originalPos = child.figure->getPosition();
        child.figure->setPosition(position + child.localOffset * scaleFactor);
        bool res = child.figure->contains(point);
        child.figure->setPosition(originalPos);
        if (res) return true;
    }
    return false;
}

sf::FloatRect CompositeFigure::getBoundingBox() const {
    if (children.empty()) return {0,0,0,0};
    sf::FloatRect total;
    for (const auto& child : children) {
        sf::Vector2f originalPos = child.figure->getPosition();
        child.figure->setPosition(position + child.localOffset * scaleFactor);
        sf::FloatRect box = child.figure->getBoundingBox();
        child.figure->setPosition(originalPos);
        if (total.width == 0) total = box;
        else {
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