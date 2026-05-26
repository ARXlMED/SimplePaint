#include "TextBox.hpp"
#include <sstream>
#include <cctype>
#include <SFML/System/String.hpp>   // для корректного преобразования Unicode

TextBox::TextBox(const sf::Font& font, unsigned int charSize)
    : m_font(font), m_active(false), m_value(0.f), m_showCursor(false), m_textMode(false)
{
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
    if (m_textMode) {
        // В текстовом режиме поле начинается пустым, значение не показываем
        m_inputString.clear();
    } else {
        std::ostringstream oss;
        oss << initialValue;
        m_inputString = oss.str();
    }
    m_text.setString(m_inputString);
    m_cursorClock.restart();
    m_showCursor = true;
}

void TextBox::deactivate() {
    m_active = false;
    if (!m_inputString.empty() && !m_textMode) {
        try {
            m_value = std::stof(m_inputString);
        } catch (...) {
            // Оставляем предыдущее значение, если парсинг не удался
        }
    }
    // В текстовом режиме m_value не обновляем (оно не используется)
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
        // Получаем UTF-32 код символа
        sf::Uint32 unicode = event.text.unicode;

        // Обработка Backspace (код 8)
        if (unicode == 8) {
            if (!m_inputString.empty()) {
                m_inputString.pop_back();
                m_text.setString(m_inputString);
            }
            return;
        }

        if (m_textMode) {
            // В текстовом режиме принимаем любые не-управляющие символы
            if (unicode >= 32 && unicode != 127) {
                // Преобразуем UTF-32 в UTF-8 строку
                sf::String sfstr(unicode);
                // toUtf8() возвращает std::basic_string<sf::Uint8>, преобразуем в std::string
                auto utf8tmp = sfstr.toUtf8();
                std::string utf8char(reinterpret_cast<const char*>(utf8tmp.c_str()), utf8tmp.size());
                m_inputString += utf8char;
                m_text.setString(m_inputString);
            }
        } else {
            // Числовой режим: только цифры, '-' и '.'
            if (unicode == '-' || unicode == '.' || (unicode >= '0' && unicode <= '9')) {
                m_inputString += static_cast<char>(unicode);
                m_text.setString(m_inputString);
            }
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
    if (m_cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        m_showCursor = !m_showCursor;
        m_cursorClock.restart();
    }
}

void TextBox::draw(sf::RenderWindow& window) const {
    if (!m_active) return;
    window.draw(m_background);
    window.draw(m_text);
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

void TextBox::setTextMode(bool textMode) {
    m_textMode = textMode;
}

void TextBox::setString(const std::string& text) {
    m_inputString = text;
    m_text.setString(m_inputString);
}