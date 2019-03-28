// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CHARACTERSTAGE_STAT_ANIM_HPP_INCLUDED
#define HEROESPATH_CHARACTERSTAGE_STAT_ANIM_HPP_INCLUDED
//
// character-stage-stat-anim.hpp
//
#include "creature/stat-set.hpp"
#include "creature/trait-enum.hpp"
#include "gui/sliders.hpp"

#include <SFML/Graphics/Drawable.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{

    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;

} // namespace gui
namespace stage
{

    class StatAnim : public sf::Drawable
    {
    public:
        StatAnim();
        virtual ~StatAnim();

        StatAnim(const StatAnim &) = delete;
        StatAnim(StatAnim &&) = delete;
        StatAnim & operator=(const StatAnim &) = delete;
        StatAnim & operator=(StatAnim &&) = delete;

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void Setup(const sf::FloatRect & REGION);

        void UpdateTime(const float ELAPSED_TIME_SEC);

        void Start() { isStarted_ = true; }

        void Stop() { isStarted_ = false; }

    private:
        void SetupAnimation();

    private:
        const float ANIM_SIZE_;
        const float ANIM_MOVE_SPEED_MIN_;
        const float ANIM_MOVE_SPEED_MAX_;
        // const float ANIM_TIME_PER_FRAME_MIN_;
        // const float ANIM_TIME_PER_FRAME_MAX_;
        // const float ANIM_SPIN_SPEED_;
        sf::FloatRect region_;
        gui::AnimationUPtr_t animUPtr_;
        gui::SliderDrift<float> animPosSliderHoriz_;
        gui::SliderDrift<float> animPosSliderVert_;
        bool isStarted_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CHARACTERSTAGE_STAT_ANIM_HPP_INCLUDED
