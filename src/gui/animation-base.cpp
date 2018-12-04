// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// animation-base.cpp
//
#include "animation-base.hpp"
#include "gui/cached-texture.hpp"
#include "misc/random.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace gui
{

    Animation::Animation(
        const Animations::Enum ENUM,
        const sf::FloatRect & REGION,
        const float TIME_PER_FRAME_SEC,
        const sf::BlendMode & BLEND_MODE,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO)
        : Entity(Animations::ToString(ENUM) + "_Animation", REGION)
        , which_(ENUM)
        , blendMode_(BLEND_MODE)
        , timePerFrameSec_(TIME_PER_FRAME_SEC)
        , frameIndex_(0)
        , frameTimerSec_(0.0f)
        , colorFrom_(COLOR_FROM)
        , colorTo_(COLOR_TO)
        , sprite_()
        , origSizeV_(gui::Animations::ImageSize(ENUM))
        , isFinished_(false)
        , hasAlreadyUpdated_(false)
    {
        entityRegion_.width = ((misc::IsRealZero(REGION.width)) ? origSizeV_.x : REGION.width);
        entityRegion_.height = ((misc::IsRealZero(REGION.height)) ? origSizeV_.y : REGION.height);
    }

    Animation::~Animation() = default;

    void Animation::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = blendMode_;
        target.draw(sprite_, states);
    }

    void Animation::SetEntityPos(const float LEFT, const float TOP)
    {
        MoveEntityPos(LEFT - entityRegion_.left, TOP - entityRegion_.top);
    }

    void Animation::SetEntityRegion(const sf::FloatRect & R)
    {
        Entity::SetEntityRegion(R);
        SetupSprite();
    }

    void Animation::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);
        SetupSprite();
    }

    bool Animation::UpdateTime(const float SECONDS)
    {
        const auto FRAME_COUNT { FrameCount() };
        if (FRAME_COUNT == 0)
        {
            return false;
        }

        if (false == hasAlreadyUpdated_)
        {
            hasAlreadyUpdated_ = true;
            SetupSprite();
        }

        frameTimerSec_ += SECONDS;
        while (frameTimerSec_ > timePerFrameSec_)
        {
            frameTimerSec_ -= timePerFrameSec_;

            if (++frameIndex_ >= (FRAME_COUNT - 1))
            {
                frameIndex_ = 0;
                isFinished_ = true;
            }

            SetupSprite();
        }

        return isFinished_;
    }

    void Animation::RandomVaryTimePerFrame()
    {
        const auto VARY_SEC_MAX { 0.02f };
        timePerFrameSec_ -= VARY_SEC_MAX * 0.5f;
        timePerFrameSec_ += misc::random::Float(VARY_SEC_MAX);
    }

    void Animation::SetupSprite()
    {
        sprite_.setTexture(GetTexture(frameIndex_));
        sprite_.setTextureRect(GetTextureRect(frameIndex_));
        sfutil::SetSizeAndPos(sprite_, GetEntityRegion());

        if (colorFrom_ == colorTo_)
        {
            sprite_.setColor(colorFrom_);
        }
        else
        {
            const auto RATIO_COMPLETE { static_cast<float>(frameIndex_)
                                        / static_cast<float>(FrameCount() - 1) };

            sprite_.setColor(sfutil::Transition(colorFrom_, colorTo_, RATIO_COMPLETE));
        }
    }

} // namespace gui
} // namespace heroespath
