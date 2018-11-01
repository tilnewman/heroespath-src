// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXT_HPP_INCLUDED
#define HEROESPATH_GUI_TEXT_HPP_INCLUDED
//
// text.hpp
//
#include "misc/wrap-enum.hpp"
#include "sfml-util/font-enum.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>

#include <ostream>
#include <string>

namespace heroespath
{
namespace gui
{
    struct TextInfo;

    // Wraps an sf::Text to correct the local offset problem.  This class should be used everywhere
    // instead of sf::Text, that way we can completely ignore the local offset problem.  If you are
    // wondering where all the setters are, you need to remember that you should be creating and
    // setting up all of these Text objects with TextInfo.Apply() calls.
    class Text : public sf::Drawable
    {
    public:
        Text();

        // invalid members and default parameters will not be used/set
        explicit Text(
            const TextInfo & TEXT_INFO,
            const std::string & TEXT = "",
            const GuiFont::Enum FONT = GuiFont::Count,
            const sf::Color & COLOR = sf::Color::Transparent);

        // invalid members and default parameters will not be used/set
        Text(
            const sf::Vector2f & POS_V,
            const TextInfo & TEXT_INFO,
            const std::string & TEXT = "",
            const GuiFont::Enum FONT = GuiFont::Count,
            const sf::Color & COLOR = sf::Color::Transparent);

        // this constructor mimics the sf::Text contructor
        Text(
            const std::string & TEXT,
            const GuiFont::Enum FONT,
            const unsigned int SIZE,
            const sf::Color & COLOR = sf::Color::White);

        Text(const Text &) = default;
        Text(Text &&) = default;
        Text & operator=(const Text &) = default;
        Text & operator=(Text &&) = default;

        void setup(
            const TextInfo & TEXT_INFO,
            const std::string & CUSTOM_TEXT = "",
            const GuiFont::Enum FONT = GuiFont::Count,
            const sf::Color & COLOR = sf::Color::Transparent);

        void setup(
            const sf::Vector2f & POS_V,
            const TextInfo & TEXT_INFO,
            const std::string & CUSTOM_TEXT = "",
            const GuiFont::Enum FONT = GuiFont::Count,
            const sf::Color & COLOR = sf::Color::Transparent);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const final;

        // returns true if size and font are valid and if text is not empty
        bool IsValid() const
        {
            return (!text_.empty() && GuiFont::IsValid(font_) && (getCharacterSize() > 0));
        }

        // returns true if IsValid() and color is not transparent
        bool WillDraw() const { return (IsValid() && (getFillColor().a > 0)); }

        std::size_t size() const { return text_.size(); }
        bool empty() const { return text_.empty(); }

        const sf::Vector2f findCharacterPos(const std::size_t INDEX) const
        {
            return sfText_.findCharacterPos(INDEX);
        }

        const std::string getFontFamilyName() const;
        const std::string getFontName() const { return GuiFont::ToStringNoThrow(font_); }
        const std::string getString() const { return text_; }
        GuiFont::Enum getFont() const { return font_; }
        unsigned int getCharacterSize() const { return sfText_.getCharacterSize(); }
        sf::Uint32 getStyle() const { return sfText_.getStyle(); }
        const sf::Color getFillColor() const { return sfText_.getFillColor(); }
        const sf::FloatRect getGlobalBounds() const { return sfText_.getGlobalBounds(); }
        const sf::Vector2f getScale() const { return sfText_.getScale(); }
        const sf::Vector2f getPosition() const { return sfText_.getPosition(); }

        float getLineSpacing() const;

        void setString(const std::string & NEW_STRING);
        void setFillColor(const sf::Color & COLOR) { sfText_.setFillColor(COLOR); }
        void setCharacterSize(const unsigned int SIZE);

        void setScale(const sf::Vector2f & SCALE_V);
        void setScale(const float HORIZ, const float VERT) { setScale(sf::Vector2f(HORIZ, VERT)); }

        void setPosition(const sf::Vector2f & POS_V) { sfText_.setPosition(POS_V); }

        void setPosition(const float LEFT, const float TOP)
        {
            setPosition(sf::Vector2f(LEFT, TOP));
        }

        void move(const sf::Vector2f & MOVE_V) { sfText_.move(MOVE_V); }
        void move(const float HORIZ, const float VERT) { move(sf::Vector2f(HORIZ, VERT)); }

        void clear();

        char at(const std::size_t INDEX) const { return text_.at(INDEX); }

        char operator[](const std::size_t INDEX) const { return text_[INDEX]; }

        char front() const { return text_.front(); }
        char back() const { return text_.back(); }

        void pop_front();
        void pop_back();

        void push_front(const char CHAR);
        void push_back(const char CHAR);

        std::string::const_iterator begin() const noexcept { return std::begin(text_); }
        std::string::const_iterator end() const noexcept { return std::end(text_); }

        std::string::const_iterator cbegin() const noexcept { return begin(); }
        std::string::const_iterator cend() const noexcept { return end(); }

        std::string::const_reverse_iterator rbegin() const noexcept
        {
            return std::string::const_reverse_iterator(end());
        }

        std::string::const_reverse_iterator rend() const noexcept
        {
            return std::string::const_reverse_iterator(begin());
        }

        std::string::const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        std::string::const_reverse_iterator crend() const noexcept { return rend(); }

        const std::string ToString(
            const bool WILL_PREFIX = true,
            const misc::Wrap WILL_WRAP = misc::Wrap::Yes,
            const std::string & SEPARATOR = "/") const;

    private:
        void UpdateAfterChangingCachedString();
        void CorrectForLocalOffset();

        sf::Text sfText_;
        std::string text_;
        GuiFont::Enum font_;
    };

    inline auto begin(const Text & TEXT) noexcept { return TEXT.begin(); }
    inline auto cbegin(const Text & TEXT) noexcept { return begin(TEXT); }
    inline auto rbegin(const Text & TEXT) noexcept { return TEXT.rbegin(); }
    inline auto crbegin(const Text & TEXT) noexcept { return rbegin(TEXT); }
    inline auto end(const Text & TEXT) noexcept { return TEXT.end(); }
    inline auto cend(const Text & TEXT) noexcept { return end(TEXT); }
    inline auto rend(const Text & TEXT) noexcept { return TEXT.rend(); }
    inline auto crend(const Text & TEXT) noexcept { return rend(TEXT); }

} // namespace gui

inline std::ostream & operator<<(std::ostream & os, const gui::Text & TEXT)
{
    os << TEXT.ToString();
    return os;
}

} // namespace heroespath

#endif // HEROESPATH_GUI_TEXT_HPP_INCLUDED
