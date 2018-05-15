// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_SLIDERBAREFFECTS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_SLIDERBAREFFECTS_HPP_INCLUDED
//
// sliderbar-effects.hpp
//  A SliderBar that controls the sound effects volume.
//
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/sliderbar-labeled.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Encapsulates a gui sliderbar with a label that controls the sound effects volume.
    class SliderBarLabeled_Effects : public gui::SliderBarLabeled
    {
    public:
        SliderBarLabeled_Effects(const SliderBarLabeled_Effects &) = delete;
        SliderBarLabeled_Effects(SliderBarLabeled_Effects &&) = delete;
        SliderBarLabeled_Effects & operator=(const SliderBarLabeled_Effects &) = delete;
        SliderBarLabeled_Effects & operator=(SliderBarLabeled_Effects &&) = delete;

        SliderBarLabeled_Effects(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const float LENGTH,
            const gui::SliderStyle & STYLE,
            const gui::MouseTextInfo & THREE_TEXT_INFOS_HOLDER,
            const float INITIAL_VALUE = 0.0f,
            const float RELATIVE_LABEL_POS_LEFT = 0.0f,
            const float RELATIVE_LABEL_POS_TOP = 0.0f); // must be [0.0f, 1.0f]

        virtual ~SliderBarLabeled_Effects();

        void OnChange(const float NEW_VALUE) override;
        void SetPadPosition() override;
        bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;
        void SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORD_V) override;
        bool UpdateTime(const float) override;

        static void SetPreventSoundEffect(const bool WILL_PREVENT)
        {
            willPreventPlayingSound_ = WILL_PREVENT;
        }

    private:
        static bool willPreventPlayingSound_;
        bool willPlaySound_;
        float timeSinceLastPlaySec_;
    };

    using SliderBarLabeled_EffectsUPtr_t = std::unique_ptr<SliderBarLabeled_Effects>;
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_SLIDERBAREFFECTS_HPP_INCLUDED
