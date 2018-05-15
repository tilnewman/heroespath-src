// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SLIDERBARSYLE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SLIDERBARSYLE_HPP_INCLUDED
//
// sliderbar-style.hpp
//  A type that encapsulates the style of a SliderBar.
//
#include "sfml-util/brightness-enum.hpp"
#include "sfml-util/orientation-enum.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // encapsulates all the the style elements of a SliderBar
        struct SliderStyle
        {
            explicit SliderStyle(
                const Orientation::Enum ORIENTATION = Orientation::Vert,
                const Brightness::Enum BRIGHTNESS = Brightness::Bright,
                const bool WILL_LABEL_ARROWS = true,
                const bool IS_LINE_LARGE = true)
                : orientation(ORIENTATION)
                , brightness(BRIGHTNESS)
                , willLabelArrows(WILL_LABEL_ARROWS)
                , isLineLarge(IS_LINE_LARGE)
            {}

            Orientation::Enum orientation;
            Brightness::Enum brightness;
            bool willLabelArrows;
            bool isLineLarge;
        };

        inline bool operator==(const SliderStyle & L, const SliderStyle & R)
        {
            return (
                (L.brightness == R.brightness) && (L.isLineLarge == R.isLineLarge)
                && (L.orientation == R.orientation) && (L.willLabelArrows == R.willLabelArrows));
        }

        inline bool operator!=(const SliderStyle & L, const SliderStyle & R) { return !(L == R); }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SLIDERBARSYLE_HPP_INCLUDED
