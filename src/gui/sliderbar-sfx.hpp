// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SLIDERBAR_EFFECTS_HPP_INCLUDED
#define HEROESPATH_GUI_SLIDERBAR_EFFECTS_HPP_INCLUDED
//
// sliderbar-sfx.hpp
//
#include "gui/mouse-text-info.hpp"
#include "gui/sliderbar-labeled.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Encapsulates a gui sliderbar with a label that controls the sound effects volume.
    class SliderBarSfx : public SliderBarLabeled
    {
    public:
        SliderBarSfx(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const float LENGTH,
            const SliderStyle & STYLE,
            const MouseTextInfo & THREE_TEXT_INFOS,
            const float INITIAL_VALUE = 0.0f,
            const float RELATIVE_LABEL_POS_LEFT = 0.0f,
            const float RELATIVE_LABEL_POS_TOP = 0.0f); // must be [0.0f, 1.0f]

        virtual ~SliderBarSfx();

        void OnChange(const float NEW_VALUE) override;
        bool UpdateTime(const float) override;

    protected:
        const TextInfo CreateTextToDisplay(const float CURRENT_POS_RATIO) override;

    private:
        static bool isConstructionAndInitFinished_;
        float timeSinceLastPlaySec_;
    };

    using SliderBarSfxUPtr_t = std::unique_ptr<SliderBarSfx>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SLIDERBAR_EFFECTS_HPP_INCLUDED
