#ifndef SFMLUTIL_GUI_COLORSET_INCLUDED
#define SFMLUTIL_GUI_COLORSET_INCLUDED
//
// color-set.hpp
//  Simple struct encapsulating a GuiEntity's color scheme.
//
//suppress warnings that are safe to ignore in boost
#include "utilz/platform.hpp"
#ifdef PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <SFML/Graphics/Color.hpp>

#ifdef PLATFORM_DETECTED_IS_APPLE
#pragma GCC diagnostic warning "-Wfloat-equal"
#endif

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    //Encapsulates the colors required to draw a GuiEntity that can
    //have focus or not have focus.  Using the default constructor
    //will use sf::Color::White when HasFocus() and a slightly darker
    //white color when ! HasFocus().
    struct ColorSet
    {
        static const sf::Uint8 DEFAULT_OFFSET_VALUE_;
        static const sf::Color DEFAULT_OFFSET_COLOR_;

        //use this constructor for default white/offwhite colors
        ColorSet();

        //use this constructor to specify all custom colors
        ColorSet(const sf::Color & FG_WITH_FOCUS,
                 const sf::Color & BG_WITH_FOCUS,
                 const sf::Color & FG_WITHOUT_FOCUS,
                 const sf::Color & BG_WITHOUT_FOCUS);

        //use this constructor when the bg is white and colors are the same with and without focus
        explicit ColorSet(const sf::Color & FG);

        //use this constructor when the without_focus colors are automatically darker than those with_focus
        ColorSet(const sf::Color & FG_WITH_FOCUS,
                 const sf::Color & BG_WITH_FOCUS);

        void ForceWithoutFocusToMatchWithFocus();

        const std::string ToString(const bool WILL_WRAP = false) const;

        sf::Color foreground_with_focus;
        sf::Color background_with_focus;
        sf::Color foreground_without_focus;
        sf::Color background_without_focus;
    };


    using ColorSetSPtr_t = std::shared_ptr<ColorSet>;
    using ColorSetSVec_t = std::vector<ColorSetSPtr_t>;


    bool operator<(const ColorSet & L, const ColorSet & R);


    bool operator==(const ColorSet & L, const ColorSet & R);


    inline bool operator!=(const ColorSet & L, const ColorSet & R)
    {
        return ! (L == R);
    }

}
}
#endif //SFMLUTIL_GUI_COLORSET_INCLUDED
