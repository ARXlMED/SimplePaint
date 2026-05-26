#pragma once
#include "AbstractFigure.hpp"
#include <functional>
#include <map>
#include <memory>

class FigureManager {
public:
    using Factory = std::function<std::unique_ptr<AbstractFigure>()>;

    static FigureManager& instance();

    void registerFactory(const std::string& name, Factory factory);
    void registerPrototype(const std::string& name, std::unique_ptr<AbstractFigure> prototype);
    bool hasFigure(const std::string& name) const;
    std::unique_ptr<AbstractFigure> create(const std::string& name) const;
    std::vector<std::string> getTypeNames() const;

private:
    FigureManager() = default;
    struct Entry {
        enum Type { FACTORY, PROTOTYPE } type;
        Factory factory;
        std::unique_ptr<AbstractFigure> prototype;
    };
    std::map<std::string, Entry> entries;
};