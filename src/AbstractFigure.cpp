#include "AbstractFigure.hpp"
#include <cmath>

AbstractFigure::AbstractFigure() : position(0,0), scaleFactor(1.f), fillColor(sf::Color::White), filled(false), pivot(0,0) {}

void AbstractFigure::move(const sf::Vector2f& offset) { position += offset; }
void AbstractFigure::scale(float factor) { scaleFactor *= factor; }
void AbstractFigure::setScale(float factor) { scaleFactor = factor; }
void AbstractFigure::setPosition(const sf::Vector2f& pos) { position = pos; }
sf::Vector2f AbstractFigure::getPosition() const { return position; }
float AbstractFigure::getScale() const { return scaleFactor; }

void AbstractFigure::setFillColor(sf::Color color) { fillColor = color; }
sf::Color AbstractFigure::getFillColor() const { return fillColor; }
void AbstractFigure::setFilled(bool f) { filled = f; }
bool AbstractFigure::isFilled() const { return filled; }

size_t AbstractFigure::getVertexCount() const { return vertices.size(); }
sf::Vector2f AbstractFigure::getLocalVertex(size_t index) const { return vertices[index]; }
sf::Vector2f AbstractFigure::getGlobalVertex(size_t index) const { return position + vertices[index] * scaleFactor; }
void AbstractFigure::setLocalVertex(size_t index, const sf::Vector2f& pos) { vertices[index] = pos; }

void AbstractFigure::addVertex(const sf::Vector2f& pos) { vertices.push_back(pos); }
void AbstractFigure::removeVertex(size_t index) { if (index < vertices.size()) vertices.erase(vertices.begin() + index); }

sf::Vector2f AbstractFigure::getLocalPivot() const { return pivot; }
sf::Vector2f AbstractFigure::getGlobalPivot() const { return position + pivot * scaleFactor; }
void AbstractFigure::setLocalPivot(const sf::Vector2f& p) { pivot = p; }
void AbstractFigure::movePivot(const sf::Vector2f& delta) { pivot += delta; }