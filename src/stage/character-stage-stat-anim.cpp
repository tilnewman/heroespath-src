// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// character-stage-stat-anim.cpp
//
#include "character-stage-stat-anim.hpp"

#include "gui/animation-factory.hpp"
#include "gui/sound-manager.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/primitives.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    StatAnim::StatAnim()
        : ANIM_SIZE_(sfutil::ScreenRatioToPixelsHoriz(0.15f))
        , ANIM_MOVE_SPEED_MIN_(0.05f)
        , ANIM_MOVE_SPEED_MAX_(0.5f)
        //, ANIM_TIME_PER_FRAME_MIN_(0.01f)
        //, ANIM_TIME_PER_FRAME_MAX_(0.05f)
        //, ANIM_SPIN_SPEED_(0.01f)
        , region_()
        , animUPtr_(gui::AnimationFactory::Make(
              gui::Animations::SmokeSwirl, sf::FloatRect(0.0f, 0.0f, ANIM_SIZE_, ANIM_SIZE_)))
        , animPosSliderHoriz_()
        , animPosSliderVert_()
    {
        SetupAnimation();
    }

    StatAnim::~StatAnim() = default;

    void StatAnim::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(*animUPtr_, states);
    }

    void StatAnim::Setup(const sf::FloatRect & REGION)
    {
        region_ = REGION;
        SetupAnimation();
    }

    void StatAnim::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        animUPtr_->UpdateTime(ELAPSED_TIME_SEC);

        animUPtr_->SetEntityPos(
            animPosSliderHoriz_.Update(ELAPSED_TIME_SEC),
            animPosSliderVert_.Update(ELAPSED_TIME_SEC));

        // if (isSpinSpeedIncreasing_ && (animUPtr_->TimePerFrame() > ANIM_TIME_PER_FRAME_MIN_))
        //{
        //    animUPtr_->TimePerFrameAdj(-1.0f * (ELAPSED_TIME_SEC * (ANIM_SPIN_SPEED_ * 2.0f)));
        //}
        // else if (!isSpinSpeedIncreasing_ && (animUPtr_->TimePerFrame() <
        // ANIM_TIME_PER_FRAME_MAX_))
        //{
        //    animUPtr_->TimePerFrameAdj(ELAPSED_TIME_SEC * ANIM_SPIN_SPEED_);
        //}
    }

    // void StatAnim::WillSpinFaster(const bool WILL_SPIN_FASTER)
    //{
    //    if (!isSpinSpeedIncreasing_ && WILL_SPIN_FASTER)
    //    {
    //        gui::SoundManager::Instance()
    //            ->GetSoundEffectSet(gui::sound_effect_set::Wind)
    //            .PlayRandom();
    //    }
    //
    //    isSpinSpeedIncreasing_ = WILL_SPIN_FASTER;
    //}

    void StatAnim::SetupAnimation()
    {
        const auto REGION_LEFT { sfutil::Left(region_) };

        const auto REGION_RIGHT { (sfutil::Right(region_) - animUPtr_->GetEntityRegion().width) };

        const auto REGION_TOP { sfutil::Top(region_) };

        const auto REGION_BOTTOM { (
            sfutil::Bottom(region_) - animUPtr_->GetEntityRegion().height) };

        animPosSliderHoriz_ = gui::SliderDrift<float>(
            REGION_LEFT,
            REGION_RIGHT,
            ANIM_MOVE_SPEED_MIN_,
            ANIM_MOVE_SPEED_MAX_,
            misc::Random(REGION_LEFT, REGION_RIGHT),
            misc::Random(REGION_LEFT, REGION_RIGHT));

        animPosSliderVert_ = gui::SliderDrift<float>(
            REGION_TOP,
            REGION_BOTTOM,
            ANIM_MOVE_SPEED_MIN_,
            ANIM_MOVE_SPEED_MAX_,
            misc::Random(REGION_TOP, REGION_BOTTOM),
            misc::Random(REGION_TOP, REGION_BOTTOM));
    }

} // namespace stage
} // namespace heroespath
