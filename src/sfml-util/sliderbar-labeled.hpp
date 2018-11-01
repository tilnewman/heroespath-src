// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SLIDERBARLABELED_HPP_INCLUDED
#define HEROESPATH_GUI_SLIDERBARLABELED_HPP_INCLUDED
//
// sliderbar-labeled.hpp
//  A SliderBar that also displays the current value.
//
#include "sfml-util/mouse-text-info.hpp"
#include "sfml-util/sliderbar.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

    // Encapsulates a gui sliderbar with a label
    class SliderBarLabeled : public SliderBar
    {
    public:
        SliderBarLabeled(const SliderBarLabeled &) = delete;
        SliderBarLabeled(SliderBarLabeled &&) = delete;
        SliderBarLabeled & operator=(const SliderBarLabeled &) = delete;
        SliderBarLabeled & operator=(SliderBarLabeled &&) = delete;

        SliderBarLabeled(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const float LENGTH,
            const SliderStyle & STYLE,
            const MouseTextInfo & THREE_TEXT_INFOS,
            const float INITIAL_VALUE = 0.0f,
            const float LABEL_POS_OFFSET_LEFT = 0.0f,
            const float LABEL_POS_OFFSET_TOP = 0.0f); // must be [0.0f, 1.0f]

        virtual ~SliderBarLabeled();

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void MoveEntityPos(const float HORIZ, const float VERT) override;

    protected:
        void OnChange(const float) override;

        const TextInfo TextInfoFromCurrentPositionPercent(const int PERCENT) const;

        virtual const TextInfo CreateTextToDisplay(const float CURRENT_POS_RATIO);

    protected:
        // This var is not really about mouse text info but about the three TextInfo
        // objects required to display the current slider number.  A MouseTextInfo object
        // is just being used to hold three TextInfo objects.
        // Down==when zero, Over==when 100, and Up==when all other values.
        MouseTextInfo threeTextInfosHolder_;

        TextRegionUPtr_t textRegionUPtr_;
        float labelOffsetX_;
        float labelOffsetY_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SLIDERBARLABELED_HPP_INCLUDED
