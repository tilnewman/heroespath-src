// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-info.cpp
//
#include "text-info.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/platform.hpp"
#include "misc/real.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sfml-util-blend-mode.hpp"

#include <iomanip>
#include <sstream>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        TextInfo::TextInfo()
            : text("")
            , font_ptr_opt(boost::none)
            , char_size(FontManager::Instance()->Size_Normal())
            , color(sf::Color::White)
            , blend_mode(sf::BlendAlpha)
            , style(sf::Text::Style::Regular)
            , justified(Justified::Left)
            , is_outline(false)
            , outline_thickness(0.0f)
        {}

        TextInfo::TextInfo(
            const std::string & TEXT,
            const FontPtrOpt_t & FONT_PTR_OPT,
            const unsigned int CHAR_SIZE,
            const sf::Color & COLOR,
            const sf::BlendMode & BLEND_MODE,
            const sf::Uint32 STYLE,
            const Justified::Enum JUSTIFIED,
            const bool IS_OUTLINE_ONLY,
            const float OUTLINE_THICKNESS)
            : text(TEXT)
            , font_ptr_opt(FONT_PTR_OPT)
            , char_size(CHAR_SIZE)
            , color(COLOR)
            , blend_mode(BLEND_MODE)
            , style(STYLE)
            , justified(JUSTIFIED)
            , is_outline(IS_OUTLINE_ONLY)
            , outline_thickness(OUTLINE_THICKNESS)
        {}

        TextInfo::TextInfo(
            const std::string & TEXT,
            const FontPtr_t FONT_PTR,
            const unsigned int CHAR_SIZE,
            const sf::Color & COLOR,
            const Justified::Enum JUSTIFIED)
            : text(TEXT)
            , font_ptr_opt(FONT_PTR)
            , char_size(CHAR_SIZE)
            , color(COLOR)
            , blend_mode(sf::BlendAlpha)
            , style(sf::Text::Style::Regular)
            , justified(JUSTIFIED)
            , is_outline(false)
            , outline_thickness(0.0f)
        {}

        TextInfo::TextInfo(
            const TextInfo & TEXT_INFO_TO_COPY,
            const std::string & TEXT,
            const ColorOpt_t & COLOR_OPT,
            const unsigned CHAR_SIZE,
            const FontPtrOpt_t NUMBERS_FONT_PTR_OPT)
            : TextInfo(TEXT_INFO_TO_COPY)
        {
            if (TEXT.empty() == false)
            {
                text = TEXT;
            }

            if (COLOR_OPT)
            {
                color = COLOR_OPT.value();
            }

            if (CHAR_SIZE != 0)
            {
                char_size = CHAR_SIZE;
            }

            if (NUMBERS_FONT_PTR_OPT)
            {
                font_ptr_opt = NUMBERS_FONT_PTR_OPT;
            }
        }

        TextInfo::TextInfo(const TextInfo & TEXT_INFO_TO_COPY, const FontPtr_t NEW_FONT_PTR)
            : TextInfo(TEXT_INFO_TO_COPY, TEXT_INFO_TO_COPY.text, boost::none, 0, NEW_FONT_PTR)
        {}

        void TextInfo::Apply(sf::Text & sfText, const FontPtrOpt_t & CUSTOM_FONT_PTR_OPT) const
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (text.empty() == false),
                "sfml_util::gui::TextInfo::Apply() but text was empty.  " << *this);

            M_ASSERT_OR_LOGANDTHROW_SS(
                (char_size != 0),
                "sfml_util::gui::TextInfo::Apply() but char_size was zero.  " << *this);

            FontPtr_t fontPtrToUse = [&]() {
                if (CUSTOM_FONT_PTR_OPT)
                {
                    return CUSTOM_FONT_PTR_OPT.value();
                }
                else
                {
                    M_ASSERT_OR_LOGANDTHROW_SS(
                        (!!font_ptr_opt),
                        "sfml_util::gui::TextInfo::Apply() but font_ptr_opt was boost::none.  "
                            << *this);

                    return font_ptr_opt.value();
                }
            }();

            sfText.setString(text);
            sfText.setFont(*fontPtrToUse);
            sfText.setStyle(style);
            sfText.setCharacterSize(char_size);

            // linux SFML lib does not seem to support outline fonts...
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_LINUX
            text.setColor(TEXT_INFO.color);
#else
            if (is_outline)
            {
                sfText.setFillColor(sf::Color::Transparent);
                sfText.setOutlineColor(color);
                sfText.setOutlineThickness(outline_thickness);
            }
            else
            {
                sfText.setFillColor(color);
            }
#endif
        }

        const sf::Text TextInfo::Make() const
        {
            sf::Text t;
            Apply(t);
            return t;
        }

        const std::string TextInfo::ToString(
            const bool WILL_PREFIX, const misc::Wrap WILL_WRAP, const std::string & SEPARATOR) const
        {
            std::ostringstream ss;

            if (IsValid() == false)
            {
                ss << "INVALID" << SEPARATOR;
            }

            ss << std::quoted(text) << SEPARATOR;

            if (font_ptr_opt)
            {
                ss << font_ptr_opt.value()->getInfo().family;
            }
            else
            {
                ss << "NoFont";
            }

            ss << SEPARATOR << "Size" << char_size
               << sfml_util::ToString(color, misc::ToStringPrefix::SimpleName) << SEPARATOR
               << sfml_util::ToString(blend_mode, misc::ToStringPrefix::SimpleName) << SEPARATOR;

            auto styleToString = [&SEPARATOR](const sf::Uint32 STYLE) {
                std::ostringstream oss;

                if (STYLE & sf::Text::Style::Regular)
                    oss << ((oss.str().empty()) ? "" : SEPARATOR) << "Regular";

                if (STYLE & sf::Text::Style::Bold)
                    oss << ((oss.str().empty()) ? "" : SEPARATOR) << "Bold";

                if (STYLE & sf::Text::Style::Italic)
                    oss << ((oss.str().empty()) ? "" : SEPARATOR) << "Italic";

                if (STYLE & sf::Text::Style::Underlined)
                    oss << ((oss.str().empty()) ? "" : SEPARATOR) << "Underlined";

                if (STYLE & sf::Text::Style::StrikeThrough)
                    oss << ((oss.str().empty()) ? "" : SEPARATOR) << "StrikeThrough";

                return oss.str();
            };

            ss << styleToString(style) << SEPARATOR << sfml_util::Justified::ToString(justified)
               << SEPARATOR;

            if (is_outline)
            {
                ss << "Outline" << outline_thickness << SEPARATOR;
            }

            auto const PARTS_STR { (
                (WILL_WRAP == misc::Wrap::Yes) ? ("(" + ss.str() + ")") : ss.str()) };

            if (WILL_PREFIX)
            {
                return std::string("TextInfo").append((WILL_WRAP == misc::Wrap::Yes) ? "" : "=")
                    + PARTS_STR;
            }
            else
            {
                return PARTS_STR;
            }
        }

        bool operator<(const TextInfo & L, const TextInfo & R)
        {
            return std::tie(
                       L.font_ptr_opt,
                       L.char_size,
                       L.style,
                       L.justified,
                       L.is_outline,
                       L.outline_thickness,
                       L.color,
                       L.blend_mode,
                       L.text)
                < std::tie(
                       R.font_ptr_opt,
                       R.char_size,
                       R.style,
                       R.justified,
                       R.is_outline,
                       R.outline_thickness,
                       R.color,
                       R.blend_mode,
                       R.text);
        }

        bool operator==(const TextInfo & L, const TextInfo & R)
        {
            if (misc::IsRealClose(L.outline_thickness, R.outline_thickness) == false)
            {
                return false;
            }

            return std::tie(
                       L.font_ptr_opt,
                       L.char_size,
                       L.color,
                       L.blend_mode,
                       L.style,
                       L.justified,
                       L.is_outline,
                       L.text)
                == std::tie(
                       R.font_ptr_opt,
                       R.char_size,
                       R.color,
                       R.blend_mode,
                       R.style,
                       R.justified,
                       R.is_outline,
                       R.text);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
