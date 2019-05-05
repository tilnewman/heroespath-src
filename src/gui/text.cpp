// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text.cpp
//
#include "text.hpp"

#include "gui/font-manager.hpp"
#include "gui/text-info.hpp"
#include "misc/enum-util.hpp"
#include "misc/strings.hpp"
#include "sfutil/color.hpp"
#include "sfutil/common.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <ostream>

namespace heroespath
{
namespace gui
{

    Text::Text()
        : sfText_()
        , text_()
        , font_(GuiFont::Count)
    {}

    // invalid members and default parameters will not be used/set
    Text::Text(
        const TextInfo & TEXT_INFO,
        const std::string & CUSTOM_TEXT,
        const GuiFont::Enum FONT,
        const sf::Color & COLOR)
        : sfText_()
        , text_(CUSTOM_TEXT)
        , font_(GuiFont::Count)
    {
        setup(sf::Vector2f(), TEXT_INFO, CUSTOM_TEXT, FONT, COLOR);
    }

    // invalid members and default parameters will not be used/set
    Text::Text(
        const sf::Vector2f & POS_V,
        const TextInfo & TEXT_INFO,
        const std::string & CUSTOM_TEXT,
        const GuiFont::Enum FONT,
        const sf::Color & COLOR)
        : sfText_()
        , text_(CUSTOM_TEXT)
        , font_(GuiFont::Count)
    {
        setup(POS_V, TEXT_INFO, CUSTOM_TEXT, FONT, COLOR);
    }

    Text::Text(
        const std::string & TEXT,
        const GuiFont::Enum FONT,
        const unsigned int SIZE,
        const sf::Color & COLOR)
        : sfText_()
        , text_(TEXT)
        , font_(FONT)
    {
        sfText_.setCharacterSize(SIZE);
        sfText_.setFillColor(COLOR);

        if (EnumUtil<GuiFont>::IsValid(font_))
        {
            sfText_.setFont(*gui::FontManager::Instance()->GetFont(font_));
        }

        UpdateAfterChangingCachedString();
    }

    void Text::setup(
        const TextInfo & TEXT_INFO,
        const std::string & CUSTOM_TEXT,
        const GuiFont::Enum FONT,
        const sf::Color & COLOR)
    {
        setup(sf::Vector2f(), TEXT_INFO, CUSTOM_TEXT, FONT, COLOR);
    }

    void Text::setup(
        const sf::Vector2f & POS_V,
        const TextInfo & TEXT_INFO,
        const std::string & TEXT,
        const GuiFont::Enum FONT,
        const sf::Color & COLOR)
    {
        if (TEXT.empty())
        {
            text_ = TEXT_INFO.text;
        }
        else
        {
            text_ = TEXT;
        }

        if (EnumUtil<GuiFont>::IsValid(FONT))
        {
            font_ = FONT;
        }
        else if (EnumUtil<GuiFont>::IsValid(TEXT_INFO.font_letters))
        {
            font_ = TEXT_INFO.font_letters;
        }

        if (EnumUtil<GuiFont>::IsValid(font_))
        {
            sfText_.setFont(*gui::FontManager::Instance()->GetFont(font_));
        }

        if (sf::Color::Transparent == COLOR)
        {
            sfText_.setFillColor(TEXT_INFO.color);
        }
        else
        {
            sfText_.setFillColor(COLOR);
        }

        sfText_.setStyle(TEXT_INFO.style);
        sfText_.setCharacterSize(TEXT_INFO.size);

        UpdateAfterChangingCachedString();
        sfText_.setPosition(POS_V);
    }

    void Text::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(sfText_, states);
    }

    const std::string Text::getFontFamilyName() const
    {
        std::string str;

        if (sfText_.getFont() != nullptr)
        {
            str = sfText_.getFont()->getInfo().family;
        }

        return str;
    }

    float Text::getLineSpacing() const
    {
        float lineSpace = 0.0f;

        if (sfText_.getFont() != nullptr)
        {
            lineSpace = sfText_.getFont()->getLineSpacing(sfText_.getCharacterSize());
        }

        return lineSpace;
    }

    void Text::setString(const std::string & NEW_STRING)
    {
        text_ = NEW_STRING;
        UpdateAfterChangingCachedString();
    }

    void Text::setScale(const sf::Vector2f & SCALE_V)
    {
        sfText_.setScale(SCALE_V);
        CorrectForLocalOffset();
    }

    void Text::setCharacterSize(const unsigned int SIZE)
    {
        sfText_.setCharacterSize(SIZE);
        CorrectForLocalOffset();
    }

    void Text::pop_front()
    {
        if (text_.empty())
        {
            return;
        }

        text_.erase(std::begin(text_));
        UpdateAfterChangingCachedString();
    }

    void Text::pop_back()
    {
        if (text_.empty())
        {
            return;
        }

        text_.pop_back();
        UpdateAfterChangingCachedString();
    }

    void Text::clear()
    {
        text_.clear();
        UpdateAfterChangingCachedString();
    }

    void Text::push_front(const char CHAR)
    {
        text_.insert(std::begin(text_), CHAR);
        UpdateAfterChangingCachedString();
    }

    void Text::push_back(const char CHAR)
    {
        text_.push_back(CHAR);
        UpdateAfterChangingCachedString();
    }

    const std::string Text::ToString(
        const bool WILL_PREFIX, const Wrap WILL_WRAP, const std::string & SEPARATOR) const
    {
        std::string str;
        str.reserve(text_.size() + 128);

        if (WILL_WRAP == Wrap::Yes)
        {
            str += "(";
        }

        if (WillDraw() == false)
        {
            str += "INVALID" + SEPARATOR + "WONT_DRAW";
        }
        else if (IsValid() == false)
        {
            str += "INVALID";
        }

        str += SEPARATOR;

        if (text_.empty())
        {
            str += "(invalid, text is empty)";
        }
        else
        {
            str += misc::MakeLoggableString(text_);
        }

        str += SEPARATOR + GuiFont::ToString(font_) + SEPARATOR + std::to_string(size()) + SEPARATOR
            + sfutil::ColorToString(getFillColor()) + SEPARATOR;

        const auto STYLE { getStyle() };
        if (0 != STYLE)
        {
            auto separatorIfNotEmpty = [&](const std::string & S) -> std::string {
                if (S.empty())
                {
                    return "";
                }
                else
                {
                    return SEPARATOR;
                }
            };

            std::string styleStr;
            styleStr.reserve(32);

            if (STYLE & sf::Text::Style::Bold)
            {
                styleStr += "Bold";
            }

            if (STYLE & sf::Text::Style::Italic)
            {
                styleStr += separatorIfNotEmpty(styleStr) + "Italic";
            }

            if (STYLE & sf::Text::Style::Underlined)
            {
                styleStr += separatorIfNotEmpty(styleStr) + "Underlined";
            }

            if (STYLE & sf::Text::Style::StrikeThrough)
            {
                styleStr += separatorIfNotEmpty(styleStr) + "StrikeThrough";
            }

            if (styleStr.empty())
            {
                str += "(invalid, sf::Style=" + std::to_string(STYLE) + ")";
            }
            else
            {
                str += styleStr;
            }
        }

        if (WILL_WRAP == Wrap::Yes)
        {
            str += ")";
        }

        if (WILL_PREFIX)
        {
            str = std::string("Text") + ((WILL_WRAP == Wrap::Yes) ? "" : "=") + str;
        }

        return str;
    }

    void Text::UpdateAfterChangingCachedString()
    {
        sfText_.setString(text_);
        CorrectForLocalOffset();
    }

    void Text::CorrectForLocalOffset()
    {
        sfText_.setOrigin(sfutil::Position(sfText_.getLocalBounds()));
    }

} // namespace gui

std::ostream & operator<<(std::ostream & os, const gui::Text & TEXT)
{
    os << TEXT.ToString();
    return os;
}

} // namespace heroespath
