//
// color-set.cpp
//
#include "color-set.hpp"

#include "sfml-util/sfml-util.hpp"

#include <sstream>
#include <tuple>


namespace sfml_util
{
namespace gui
{

    const sf::Uint8 ColorSet::DEFAULT_OFFSET_VALUE_(35);
    const sf::Color ColorSet::DEFAULT_OFFSET_COLOR_(DEFAULT_OFFSET_VALUE_, DEFAULT_OFFSET_VALUE_, DEFAULT_OFFSET_VALUE_, 0);



    ColorSet::ColorSet()
    :
        foreground_with_focus   (sf::Color::White),
        background_with_focus   (sf::Color::White),
        foreground_without_focus(sf::Color::White - DEFAULT_OFFSET_COLOR_),
        background_without_focus(sf::Color::White - DEFAULT_OFFSET_COLOR_)
    {}


    ColorSet::ColorSet(const sf::Color & FG_WITH_FOCUS,
                       const sf::Color & BG_WITH_FOCUS,
                       const sf::Color & FG_WITHOUT_FOCUS,
                       const sf::Color & BG_WITHOUT_FOCUS)
    :
        foreground_with_focus   (FG_WITH_FOCUS),
        background_with_focus   (BG_WITH_FOCUS),
        foreground_without_focus(FG_WITHOUT_FOCUS),
        background_without_focus(BG_WITHOUT_FOCUS)
    {}


    ColorSet::ColorSet(const sf::Color & FG)
    :
        foreground_with_focus   (FG),
        background_with_focus   (sf::Color::White),
        foreground_without_focus(FG),
        background_without_focus(sf::Color::White)
    {}


    ColorSet::ColorSet(const sf::Color & FG_WITH_FOCUS,
                       const sf::Color & BG_WITH_FOCUS)
    :
        foreground_with_focus   (FG_WITH_FOCUS),
        background_with_focus   (BG_WITH_FOCUS),
        foreground_without_focus(FG_WITH_FOCUS),
        background_without_focus(BG_WITH_FOCUS)
    {
        foreground_without_focus -= DEFAULT_OFFSET_COLOR_;
        background_without_focus -= DEFAULT_OFFSET_COLOR_;
    }


    void ColorSet::ForceWithoutFocusToMatchWithFocus()
    {
        foreground_without_focus = foreground_with_focus;
        background_without_focus = background_with_focus;
    }


    const std::string ColorSet::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        if (WILL_WRAP)
            ss << "(";

        ss  << "fg_wf="      << sfml_util::ColorToString(foreground_with_focus)
            << ", fg_wof="   << sfml_util::ColorToString(foreground_without_focus)
            << ", bg_wf="    << sfml_util::ColorToString(background_with_focus)
            << ", bg_wof="   << sfml_util::ColorToString(background_without_focus);

        if (WILL_WRAP)
            ss << ")";

        return ss.str();
    }


    bool operator<(const ColorSet & L, const ColorSet & R)
    {
        if (L.foreground_with_focus != R.foreground_with_focus)
            return sfml_util::ColorLess(L.foreground_with_focus, R.foreground_with_focus);

        if (L.background_with_focus != R.background_with_focus)
            return sfml_util::ColorLess(L.background_with_focus, R.background_with_focus);

        if (L.foreground_without_focus != R.foreground_without_focus)
            return sfml_util::ColorLess(L.foreground_without_focus, R.foreground_without_focus);

        return sfml_util::ColorLess(L.background_without_focus, R.background_without_focus);
    }


    bool operator==(const ColorSet & L, const ColorSet & R)
    {
        return std::tie(L.foreground_with_focus,
                        L.background_with_focus,
                        L.foreground_without_focus,
                        L.background_without_focus)
               ==
               std::tie(R.foreground_with_focus,
                        R.background_with_focus,
                        R.foreground_without_focus,
                        R.background_without_focus);
    }

}
}
