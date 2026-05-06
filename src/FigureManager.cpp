#include "FigureManager.hpp"

FigureManager& FigureManager::instance() {
    static FigureManager inst;
    return inst;
}

void FigureManager::registerFactory(const std::string& name, Factory factory) {
    entries[name] = {Entry::FACTORY, factory, nullptr};
}

void FigureManager::registerPrototype(const std::string& name, std::unique_ptr<AbstractFigure> prototype) {
    entries[name] = {Entry::PROTOTYPE, nullptr, std::move(prototype)};
}

bool FigureManager::hasFigure(const std::string& name) const {
    return entries.find(name) != entries.end();
}

std::unique_ptr<AbstractFigure> FigureManager::create(const std::string& name) const {
    auto it = entries.find(name);
    if (it == entries.end()) return nullptr;
    const auto& e = it->second;
    if (e.type == Entry::FACTORY) {
        return e.factory();
    } else {
        return e.prototype->clone();
    }
}

std::vector<std::string> FigureManager::getTypeNames() const {
    std::vector<std::string> names;
    for (const auto& p : entries)
        names.push_back(p.first);
    return names;
}