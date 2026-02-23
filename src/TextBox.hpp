#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class TextBox {
public:
    TextBox(const sf::Font& font, unsigned int charSize = 24);
    void handleEvent(const sf::Event& event);
    void update();
    void draw(sf::RenderWindow& window) const;
    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setLabel(const std::string& label);
    void activate(float initialValue);
    void deactivate();
    bool isActive() const;
    float getValue() const;
    std::string getString() const;

private:
    sf::Font m_font;
    sf::Text m_text;
    sf::Text m_label;
    sf::RectangleShape m_background;
    std::string m_inputString;
    bool m_active;
    float m_value;
    sf::Clock m_cursorClock;
    bool m_showCursor;
    sf::Vector2f m_position;
    sf::Vector2f m_size;
};