#ifndef APPBASE_SFMLUTIL_GUI_TEXTINFO_INCLUDED
#define APPBASE_SFMLUTIL_GUI_TEXTINFO_INCLUDED
//
// text-info.hpp
//  Code regarding the details about drawn text.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/justified-enum.hpp"
#include "sfml-util/font-manager.hpp"

#include <string>
#include <vector>
#include <memory>


namespace sfml_util
{
namespace gui
{

    //Encapsulates all required information about drawing text except for position.
    //Note:  Using all default values is not generally safe, and may cause some
    //       functions to throw exceptions.  To be safe, always provide a non-empty
    //       TEXT parameter, and a non-null FONT pointer, that will always work with
    //       all functions.  You can use the IsValid() function to check these conditions.
    class TextInfo
    {
    public:
        TextInfo(const std::string &   TEXT              = "",
                 const FontSPtr_t &    FONT              = FontSPtr_t(),
                 const unsigned int    CHAR_SIZE         = FontManager::Instance()->Size_Normal(),
                 const sf::Color &     COLOR             = sf::Color::White,
                 const sf::BlendMode   BLEND_MODE        = sf::BlendAlpha,
                 const sf::Uint32      STYLE             = sf::Text::Style::Regular,
                 const Justified::Enum JUSTIFIED         = Justified::Left,
                 const bool            IS_OUTLINE_ONLY   = false,
                 const float           OUTLINE_THICKNESS = 0.0f);

        TextInfo(const std::string &   TEXT,
                 const FontSPtr_t &    FONT,
                 const unsigned int    CHAR_SIZE,
                 const sf::Color &     COLOR,
                 const Justified::Enum JUSTIFIED);

        //returns true if there is non-empty text and a non-null font pointer
        inline bool IsValid() const { return ((false == text.empty()) && (fontSPtr.get() != nullptr)); }

        std::string     text;
        FontSPtr_t      fontSPtr;
        unsigned int    charSize;
        sf::Color       color;
        sf::BlendMode   blendMode;
        sf::Uint32      style;
        Justified::Enum justified;
        bool            isOutlineOnly;
        float           outlineThickness;
    };


    bool operator<(const TextInfo & L, const TextInfo R);

    bool operator==(const TextInfo & L, const TextInfo R);

    inline bool operator!=(const TextInfo & L, const TextInfo & R)
    {
        return ! (L == R);
    }


    using TextInfoUPtr_t = std::unique_ptr<TextInfo>;
    using TextInfoSPtr_t = std::shared_ptr<TextInfo>;
    using TextInfoSVec_t = std::vector<TextInfoSPtr_t>;
    using TextInfoVec_t  = std::vector<TextInfo>;


    //Helper function used to setup an sf::Text object
    void SetupText(sf::Text & text, const TextInfo & TEXT_INFO);

}
}
#endif //APPBASE_SFMLUTIL_GUI_TEXTINFO_INCLUDED
