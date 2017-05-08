//
// text-info.cpp
//
#include "text-info.hpp"

#include "utilz/real.hpp"
#include "sfml-util/sfml-util.hpp"
#include "utilz/platform.hpp"

#include "utilz/assertlogandthrow.hpp"

#include <tuple>


namespace sfml_util
{
namespace gui
{

    TextInfo::TextInfo( const std::string &   TEXT,
                        const FontSPtr_t &    FONT,
                        const unsigned int    CHAR_SIZE,
                        const sf::Color &     COLOR,
                        const sf::BlendMode   BLEND_MODE,
                        const sf::Uint32      STYLE,
                        const Justified::Enum JUSTIFIED,
                        const bool            IS_OUTLINE_ONLY,
                        const float           OUTLINE_THICKNESS)
    :
        text	        (TEXT),
        fontSPtr	    (FONT),
        charSize	    (CHAR_SIZE),
        color	        (COLOR),
        blendMode       (BLEND_MODE),
        style	        (STYLE),
        justified       (JUSTIFIED),
        isOutlineOnly	(IS_OUTLINE_ONLY),
        outlineThickness(OUTLINE_THICKNESS)
    {}


    TextInfo::TextInfo( const std::string &   TEXT,
                        const FontSPtr_t &    FONT,
                        const unsigned int    CHAR_SIZE,
                        const sf::Color &     COLOR,
                        const Justified::Enum JUSTIFIED )
    :
        text	        (TEXT),
        fontSPtr	    (FONT),
        charSize	    (CHAR_SIZE),
        color	        (COLOR),
        blendMode       (sf::BlendAlpha),
        style	        (sf::Text::Style::Regular),
        justified       (JUSTIFIED),
        isOutlineOnly	(false),
        outlineThickness(0.0f)
    {}


    bool operator<(const TextInfo & L, const TextInfo R)
    {
        if (L.color != R.color)
            return sfml_util::ColorLess(L.color, R.color);

        if (L.blendMode != R.blendMode)
            return sfml_util::BlendModeLess(L.blendMode, R.blendMode);

        return std::tie(L.text,
                        L.fontSPtr,
                        L.charSize,
                        L.style,
                        L.justified,
                        L.isOutlineOnly,
                        L.outlineThickness)
               <
               std::tie(R.text,
                        R.fontSPtr,
                        R.charSize,
                        R.style,
                        R.justified,
                        R.isOutlineOnly,
                        R.outlineThickness);
    }


    bool operator==(const TextInfo & L, const TextInfo R)
    {
        if (utilz::IsRealClose(L.outlineThickness, R.outlineThickness) == false)
            return false;

        return std::tie(L.text,
                        L.fontSPtr,
                        L.charSize,
                        L.color,
                        L.blendMode,
                        L.style,
                        L.justified,
                        L.isOutlineOnly)
               ==
               std::tie(R.text,
                        R.fontSPtr,
                        R.charSize,
                        R.color,
                        R.blendMode,
                        R.style,
                        R.justified,
                        R.isOutlineOnly);
    }


    void SetupText(sf::Text & text, const TextInfo & TEXT_INFO)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((false == TEXT_INFO.text.empty()), "sfml_util::gui::SetupText() given a TEXT_INFO.string that was empty.");
        text.setString(TEXT_INFO.text);

        M_ASSERT_OR_LOGANDTHROW_SS((nullptr != TEXT_INFO.fontSPtr.get()), "sfml_util::gui::SetupText(\"" << TEXT_INFO.text << "\") was given a null font pointer.");
        text.setFont(*TEXT_INFO.fontSPtr);
        text.setStyle(TEXT_INFO.style);
        text.setCharacterSize(TEXT_INFO.charSize);

//linux SFML lib does not seem to support outline fonts...
#ifdef SFMLUTIL_PLATFORMDETECT__LINUX
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

}
}
