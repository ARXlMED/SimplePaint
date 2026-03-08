#pragma once
#include "AbstractFigure.hpp"
#include <functional>
#include <map>

class FigureManager {
public:
    using Creator = std::function<std::unique_ptr<AbstractFigure>(const sf::Color&, const std::vector<float>&)>;

    static FigureManager& instance();

    void registerType(const std::string& name, Creator creator);
    std::vector<std::string> getTypeNames() const;
    std::unique_ptr<AbstractFigure> create(const std::string& name, const sf::Color& color, const std::vector<float>& thicknesses) const;

private:
    FigureManager() = default;
    std::map<std::string, Creator> creators;
};