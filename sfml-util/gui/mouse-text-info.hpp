#ifndef SFMLUTIL_GUI_MOUSETEXTINFO_INCLUDED
#define SFMLUTIL_GUI_MOUSETEXTINFO_INCLUDED
//
// mouse-text-info.hpp
//  A handy wrapper struct that holds three colors relating to mouse position.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/text-info.hpp"

#include <string>
#include <vector>
#include <memory>


namespace game
{
    class PopupInfo;
}
namespace sfml_util
{
namespace gui
{

    //handy wrapper struct
    class MouseTextInfo
    {
    public:
        MouseTextInfo();

        MouseTextInfo(const TextInfo & TEXT_INFO);

        MouseTextInfo(const TextInfo & UP,
                      const TextInfo & DOWN,
                      const TextInfo & OVER);

        MouseTextInfo(const TextInfo &  TEXT_INFO_UP,
                      const sf::Color & COLOR_DOWN,
                      const sf::Color & COLOR_OVER);

        MouseTextInfo(const std::string & TEXT,
                      const FontSPtr_t    FONT,
                      const unsigned int  SIZE,
                      const sf::Color &   COLOR_UP   = sf::Color(220,220,220),
                      const sf::Color &   COLOR_DOWN = sf::Color::White,
                      const sf::Color &   COLOR_OVER = sf::Color::White);

        //MouseTextInfo generating helper functions
        static const MouseTextInfo Make_PopupButtonSet(const std::string & NAME, const game::PopupInfo &);

        TextInfo up;
        TextInfo down;
        TextInfo over;
    };


    using MouseTextInfoUPtr_t = std::unique_ptr<MouseTextInfo>;
    using MouseTextInfoSPtr_t = std::shared_ptr<MouseTextInfo>;
    using MouseTextInfoSVec_t = std::vector<MouseTextInfoSPtr_t>;
    using MouseTextInfoVec_t  = std::vector<MouseTextInfo>;


    bool operator<(const MouseTextInfo & L, const MouseTextInfo & R);

    bool operator==(const MouseTextInfo & L, const MouseTextInfo & R);

    inline bool operator!=(const MouseTextInfo & L, const MouseTextInfo & R)
    {
        return ! (L == R);
    }

}
}
#endif //SFMLUTIL_GUI_MOUSETEXTINFO_INCLUDED
