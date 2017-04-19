#ifndef APPBASE_SFMLUTIL_SLIDERBARSYLE_INCLUDED
#define APPBASE_SFMLUTIL_SLIDERBARSYLE_INCLUDED
//
// sliderbar-style.hpp
//  A type that encapsulates the style of a SliderBar.
//
#include "sfml-util/brightness-enum.hpp"
#include "sfml-util/orientation-enum.hpp"


namespace sfml_util
{
namespace gui
{

    //encapsulates all the the style elements of a SliderBar
    struct SliderStyle
    {
        SliderStyle(const Orientation::Enum ORIENTATION       = Orientation::Vert,
                    const Brightness::Enum  BRIGHTNESS        = Brightness::Bright,
                    const bool              WILL_LABEL_ARROWS = true,
                    const bool              IS_LINE_LARGE     = true)
        :
            orientation    (ORIENTATION),
            brightness     (BRIGHTNESS),
            willLabelArrows(WILL_LABEL_ARROWS),
            isLineLarge    (IS_LINE_LARGE)
        {}

        Orientation::Enum orientation;
        Brightness::Enum  brightness;
        bool              willLabelArrows;
        bool              isLineLarge;
    };


    inline bool operator==(const SliderStyle & L, const SliderStyle & R)
    {
        return ((L.brightness == R.brightness) &&
                (L.isLineLarge == R.isLineLarge) &&
                (L.orientation == R.orientation) &&
                (L.willLabelArrows == R.willLabelArrows));
    }


    inline bool operator!=(const SliderStyle & L, const SliderStyle & R)
    {
        return ! (L == R);
    }

}
}

#endif //APPBASE_SFMLUTIL_SLIDERBARSYLE_INCLUDED
