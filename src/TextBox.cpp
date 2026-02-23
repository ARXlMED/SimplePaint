#include "TextBox.hpp"
#include <sstream>
#include <cctype>

TextBox::TextBox(const sf::Font& font, unsigned int charSize)
    : m_font(font), m_active(false), m_value(0.f), m_showCursor(false) {
    m_text.setFont(m_font);
    m_text.setCharacterSize(charSize);
    m_text.setFillColor(sf::Color::Black);

    m_label.setFont(m_font);
    m_label.setCharacterSize(charSize - 4);
    m_label.setFillColor(sf::Color::White);

    m_background.setFillColor(sf::Color::White);
    m_background.setOutlineColor(sf::Color::Black);
    m_background.setOutlineThickness(2);
}

void TextBox::setPosition(float x, float y) {
    m_position = {x, y};
    m_background.setPosition(x, y);
    m_text.setPosition(x + 5, y + 2);
    m_label.setPosition(x, y - m_label.getCharacterSize() - 2);
}

void TextBox::setSize(float width, float height) {
    m_size = {width, height};
    m_background.setSize({width, height});
}

void TextBox::setLabel(const std::string& label) {
    m_label.setString(label);
}

void TextBox::activate(float initialValue) {
    m_active = true;
    m_value = initialValue;
    std::ostringstream oss;
    oss << initialValue;
    m_inputString = oss.str();
    m_text.setString(m_inputString);
    m_cursorClock.restart();
    m_showCursor = true;
}

void TextBox::deactivate() {
    m_active = false;
    if (!m_inputString.empty()) {
        try {
            m_value = std::stof(m_inputString);
        } catch (...) {
            // оставляем старое значение
        }
    }
    m_inputString.clear();
}

bool TextBox::isActive() const {
    return m_active;
}

float TextBox::getValue() const {
    return m_value;
}

std::string TextBox::getString() const {
    return m_inputString;
}

void TextBox::handleEvent(const sf::Event& event) {
    if (!m_active) return;
    if (event.type == sf::Event::TextEntered) {
        char c = static_cast<char>(event.text.unicode);
        if (std::isdigit(c) || c == '-' || c == '.') {
            m_inputString += c;
            m_text.setString(m_inputString);
        } else if (c == '\b' && !m_inputString.empty()) {
            m_inputString.pop_back();
            m_text.setString(m_inputString);
        }
    } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter) {
            deactivate();
        } else if (event.key.code == sf::Keyboard::Escape) {
            m_inputString.clear();
            deactivate();
        }
    }
}

void TextBox::update() {
    if (!m_active) return;
    // Мигание курсора каждые 0.5 секунды
    if (m_cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        m_showCursor = !m_showCursor;
        m_cursorClock.restart();
    }
}

void TextBox::draw(sf::RenderWindow& window) const {
    if (!m_active) return;
    window.draw(m_background);
    window.draw(m_text);
    // Рисуем курсор
    if (m_showCursor) {
        sf::FloatRect textBounds = m_text.getLocalBounds();
        float cursorX = m_text.getPosition().x + textBounds.width + 2;
        float cursorY = m_text.getPosition().y;
        float cursorHeight = m_text.getCharacterSize();
        sf::RectangleShape cursor({2, cursorHeight});
        cursor.setFillColor(sf::Color::Black);
        cursor.setPosition(cursorX, cursorY);
        window.draw(cursor);
    }
    window.draw(m_label);
}