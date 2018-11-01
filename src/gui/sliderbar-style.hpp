// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SLIDERBARSYLE_HPP_INCLUDED
#define HEROESPATH_GUI_SLIDERBARSYLE_HPP_INCLUDED
//
// sliderbar-style.hpp
//
#include "gui/brightness-enum.hpp"
#include "gui/orientation-enum.hpp"

namespace heroespath
{
namespace gui
{

    // Responsible for wrapping all the information about the style of a SliderBar
    struct SliderStyle
    {
        explicit SliderStyle(
            const Orientation::Enum ORIENTATION = Orientation::Vert,
            const Brightness::Enum BRIGHTNESS = Brightness::Bright,
            const bool WILL_LABEL_ARROWS = true,
            const bool IS_LINE_LARGE = true)
            : orientation(ORIENTATION)
            , brightness(BRIGHTNESS)
            , will_label_arrows(WILL_LABEL_ARROWS)
            , is_line_large(IS_LINE_LARGE)
        {}

        SliderStyle(const SliderStyle &) = default;
        SliderStyle(SliderStyle &&) = default;
        SliderStyle & operator=(const SliderStyle &) = default;
        SliderStyle & operator=(SliderStyle &&) = default;

        Orientation::Enum orientation;
        Brightness::Enum brightness;
        bool will_label_arrows;
        bool is_line_large;
    };

    inline bool operator==(const SliderStyle & L, const SliderStyle & R)
    {
        return (
            (L.brightness == R.brightness) && (L.is_line_large == R.is_line_large)
            && (L.orientation == R.orientation) && (L.will_label_arrows == R.will_label_arrows));
    }

    inline bool operator!=(const SliderStyle & L, const SliderStyle & R) { return !(L == R); }

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SLIDERBARSYLE_HPP_INCLUDED
