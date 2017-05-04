//
// mouse-text-info.cpp
//
#include "mouse-text-info.hpp"

#include "sfml-util/font-manager.hpp"

#include "game/popup-info.hpp"

#include <tuple>


namespace sfml_util
{
namespace gui
{

    MouseTextInfo::MouseTextInfo()
    :
        up  (),
        down(),
        over()
    {}


    MouseTextInfo::MouseTextInfo(const TextInfo & TEXT_INFO)
    :
        up  (TEXT_INFO),
        down(TEXT_INFO),
        over(TEXT_INFO)
    {}


    MouseTextInfo::MouseTextInfo(const TextInfo & UP,
                                 const TextInfo & DOWN,
                                 const TextInfo & OVER)
    :
        up  (UP),
        down(DOWN),
        over(OVER)
    {}


    MouseTextInfo::MouseTextInfo(const TextInfo &  TEXT_INFO_UP,
                                 const sf::Color & COLOR_DOWN,
                                 const sf::Color & COLOR_OVER)
    :
        up  (TEXT_INFO_UP),
        down(TEXT_INFO_UP),
        over(TEXT_INFO_UP)
    {
        down.color = COLOR_DOWN;
        over.color = COLOR_OVER;
    }


    MouseTextInfo::MouseTextInfo(const std::string & TEXT,
                                 const FontSPtr_t    FONT,
                                 const unsigned int  SIZE,
                                 const sf::Color &   COLOR_UP,
                                 const sf::Color &   COLOR_DOWN,
                                 const sf::Color &   COLOR_OVER)
    :
        up  (TEXT, FONT, SIZE, COLOR_UP),
        down(TEXT, FONT, SIZE, COLOR_DOWN),
        over(TEXT, FONT, SIZE, COLOR_OVER)
    {}


    const MouseTextInfo MouseTextInfo::Make_PopupButtonSet(const std::string & NAME, const game::PopupInfo & POPUP_INFO)
    {
        return MouseTextInfo(NAME,
                             FontManager::Instance()->Font_PopupButton(),
                             FontManager::Instance()->Size_Larger(),
                             FontManager::Color_PopupButtonUp(POPUP_INFO.ButtonColor()),
                             FontManager::Color_PopupButtonDown(POPUP_INFO.ButtonColor()),
                             FontManager::Color_PopupButtonOver(POPUP_INFO.ButtonColor()));
    }


    bool operator<(const MouseTextInfo & L, const MouseTextInfo & R)
    {
        return std::tie(L.up, L.down, L.over) < std::tie(R.up, R.down, R.over);
    }


    bool operator==(const MouseTextInfo & L, const MouseTextInfo & R)
    {
        return std::tie(L.up, L.down, L.over) == std::tie(R.up, R.down, R.over);
    }

}
}
