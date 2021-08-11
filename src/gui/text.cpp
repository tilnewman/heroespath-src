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
#include "sfutil/position.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <sstream>

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

    bool Text::IsValid() const
    {
        return (!text_.empty() && EnumUtil<GuiFont>::IsValid(font_) && (getCharacterSize() > 0));
    }

    const std::string Text::getFontFamilyName() const
    {
        if (sfText_.getFont() == nullptr)
        {
            return "";
        }
        else
        {
            return sfText_.getFont()->getInfo().family;
        }
    }

    float Text::getLineSpacing() const
    {
        if (sfText_.getFont() == nullptr)
        {
            return 0.0f;
        }
        else
        {
            return sfText_.getFont()->getLineSpacing(sfText_.getCharacterSize());
        }
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
        std::ostringstream ss;

        if (WillDraw() == false)
        {
            ss << "INVALID" << SEPARATOR << "WONT_DRAW";
        }
        else if (IsValid() == false)
        {
            ss << "INVALID";
        }

        ss << SEPARATOR;
        if (text_.empty())
        {
            ss << "(invalid, text is empty)";
        }
        else
        {
            ss << misc::MakeLoggableString(text_);
        }

        ss << SEPARATOR << GuiFont::ToString(font_);
        ss << SEPARATOR << size();
        ss << SEPARATOR << sfutil::ToString(getFillColor(), misc::ToStringPrefix::SimpleName);

        ss << SEPARATOR;
        const auto STYLE { getStyle() };
        if (0 != STYLE)
        {
            auto separatorIfNotEmpty = [&](std::ostringstream & strstrm) -> std::string {
                if (strstrm.str().empty())
                {
                    return "";
                }
                else
                {
                    return SEPARATOR;
                }
            };

            std::ostringstream oss;

            if (STYLE & sf::Text::Style::Bold)
            {
                oss << "Bold";
            }

            if (STYLE & sf::Text::Style::Italic)
            {
                oss << separatorIfNotEmpty(oss) << "Italic";
            }

            if (STYLE & sf::Text::Style::Underlined)
            {
                oss << separatorIfNotEmpty(oss) << "Underlined";
            }

            if (STYLE & sf::Text::Style::StrikeThrough)
            {
                oss << separatorIfNotEmpty(oss) << "StrikeThrough";
            }

            if (oss.str().empty())
            {
                ss << "(invalid, sf::Style=" << STYLE << ")";
            }
            else
            {
                ss << oss.str();
            }
        }

        const auto PARTS_STR { ((WILL_WRAP == Wrap::Yes) ? ("(" + ss.str() + ")") : ss.str()) };

        if (WILL_PREFIX)
        {
            return std::string("Text").append((WILL_WRAP == Wrap::Yes) ? "" : "=") + PARTS_STR;
        }
        else
        {
            return PARTS_STR;
        }
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
} // namespace heroespath
