#include "FigureManager.hpp"

FigureManager& FigureManager::instance() {
    static FigureManager inst;
    return inst;
}

void FigureManager::registerType(const std::string& name, Creator creator) {
    creators[name] = creator;
}

std::vector<std::string> FigureManager::getTypeNames() const {
    std::vector<std::string> names;
    for (const auto& pair : creators)
        names.push_back(pair.first);
    return names;
}

std::unique_ptr<AbstractFigure> FigureManager::create(const std::string& name, const sf::Color& color, const std::vector<float>& thicknesses) const {
    auto it = creators.find(name);
    if (it != creators.end())
        return it->second(color, thicknesses);
    return nullptr;
}