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
#include "sfml-util/sfml-util.hpp"

#include <tuple>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        TextInfo::TextInfo(
            const std::string & TEXT,
            const FontPtrOpt_t FONT_PTR_OPT,
            const unsigned int CHAR_SIZE,
            const sf::Color & COLOR,
            const sf::BlendMode & BLEND_MODE,
            const sf::Uint32 STYLE,
            const Justified::Enum JUSTIFIED,
            const bool IS_OUTLINE_ONLY,
            const float OUTLINE_THICKNESS)
            : text(TEXT)
            , fontPtrOpt(FONT_PTR_OPT)
            , charSize(CHAR_SIZE)
            , color(COLOR)
            , blendMode(BLEND_MODE)
            , style(STYLE)
            , justified(JUSTIFIED)
            , isOutlineOnly(IS_OUTLINE_ONLY)
            , outlineThickness(OUTLINE_THICKNESS)
        {}

        TextInfo::TextInfo(
            const std::string & TEXT,
            const FontPtr_t FONT_PTR,
            const unsigned int CHAR_SIZE,
            const sf::Color & COLOR,
            const Justified::Enum JUSTIFIED)
            : text(TEXT)
            , fontPtrOpt(FONT_PTR)
            , charSize(CHAR_SIZE)
            , color(COLOR)
            , blendMode(sf::BlendAlpha)
            , style(sf::Text::Style::Regular)
            , justified(JUSTIFIED)
            , isOutlineOnly(false)
            , outlineThickness(0.0f)
        {}

        bool operator<(const TextInfo & L, const TextInfo & R)
        {
            if (L.color != R.color)
            {
                return sfml_util::color::ColorLess(L.color, R.color);
            }

            if (L.blendMode != R.blendMode)
            {
                return sfml_util::color::BlendModeLess(L.blendMode, R.blendMode);
            }

            return std::tie(
                       L.text,
                       L.fontPtrOpt,
                       L.charSize,
                       L.style,
                       L.justified,
                       L.isOutlineOnly,
                       L.outlineThickness)
                < std::tie(
                       R.text,
                       R.fontPtrOpt,
                       R.charSize,
                       R.style,
                       R.justified,
                       R.isOutlineOnly,
                       R.outlineThickness);
        }

        bool operator==(const TextInfo & L, const TextInfo & R)
        {
            if (misc::IsRealClose(L.outlineThickness, R.outlineThickness) == false)
            {
                return false;
            }

            return std::tie(
                       L.text,
                       L.fontPtrOpt,
                       L.charSize,
                       L.color,
                       L.blendMode,
                       L.style,
                       L.justified,
                       L.isOutlineOnly)
                == std::tie(
                       R.text,
                       R.fontPtrOpt,
                       R.charSize,
                       R.color,
                       R.blendMode,
                       R.style,
                       R.justified,
                       R.isOutlineOnly);
        }

        void SetupText(sf::Text & text, const TextInfo & TEXT_INFO)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (false == TEXT_INFO.text.empty()),
                "sfml_util::gui::SetupText() given a TEXT_INFO.string that was empty.");

            text.setString(TEXT_INFO.text);

            M_ASSERT_OR_LOGANDTHROW_SS(
                (!!TEXT_INFO.fontPtrOpt),
                "sfml_util::gui::SetupText(\"" << TEXT_INFO.text
                                               << "\") was given an uninitialized font pointer.");

            text.setFont(*TEXT_INFO.fontPtrOpt.value());
            text.setStyle(TEXT_INFO.style);
            text.setCharacterSize(TEXT_INFO.charSize);

// linux SFML lib does not seem to support outline fonts...
#ifdef HEROESPATH_PLATFORM_DETECTED_IS_LINUX
            text.setColor(TEXT_INFO.color);
#else
            if (TEXT_INFO.isOutlineOnly)
            {
                text.setFillColor(sf::Color::Transparent);
                text.setOutlineColor(TEXT_INFO.color);
                text.setOutlineThickness(TEXT_INFO.outlineThickness);
            }
            else
            {
                text.setFillColor(TEXT_INFO.color);
            }
#endif
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
