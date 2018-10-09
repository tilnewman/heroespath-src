// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// animation-multi.cpp
//
#include "animation-multi.hpp"

#include "misc/config-file.hpp"
#include "misc/real.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace sfml_util
{

    AnimationMultiTexture::AnimationMultiTexture(
        const Animations::Enum ENUM,
        const sf::FloatRect & REGION,
        const float TIME_PER_FRAME_SEC,
        const sf::BlendMode & BLEND_MODE,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO)
        : Animation(ENUM, REGION, TIME_PER_FRAME_SEC, BLEND_MODE, COLOR_FROM, COLOR_TO)
        , cachedTextures_(Animations::MediaPathKey(ENUM))
        , sprite_(cachedTextures_.Front())
        , origSizeV_(sf::Vector2f(cachedTextures_.Front().getSize()))
    {
        entityRegion_.width = ((misc::IsRealZero(REGION.width)) ? origSizeV_.x : REGION.width);
        entityRegion_.height = ((misc::IsRealZero(REGION.height)) ? origSizeV_.y : REGION.height);

        sprite_.setPosition(REGION.left, REGION.top);
        sprite_.setScale(entityRegion_.width / origSizeV_.x, entityRegion_.height / origSizeV_.y);
        sprite_.setColor(colorFrom_);
    }

    AnimationMultiTexture::~AnimationMultiTexture() = default;

    void AnimationMultiTexture::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = blendMode_;
        target.draw(sprite_, states);
    }

    void AnimationMultiTexture::SetEntityPos(const float LEFT, const float TOP)
    {
        MoveEntityPos(LEFT - entityRegion_.left, TOP - entityRegion_.top);
    }

    void AnimationMultiTexture::SetEntityRegion(const sf::FloatRect & R)
    {
        Entity::SetEntityRegion(R);
        sfutil::SetSizeAndPos(sprite_, R);
    }

    void AnimationMultiTexture::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);
        sprite_.move(HORIZ, VERT);
    }

    bool AnimationMultiTexture::UpdateTime(const float SECONDS)
    {
        frameTimerSec_ += SECONDS;
        while (frameTimerSec_ > timePerFrameSec_)
        {
            frameTimerSec_ -= timePerFrameSec_;
            if (++currentFrame_ >= (cachedTextures_.Size() - 1))
            {
                currentFrame_ = 0;
                isFinished_ = true;
            }

            sprite_.setTexture(cachedTextures_[currentFrame_]);

            if (colorFrom_ != colorTo_)
            {
                const auto RATIO_COMPLETE { static_cast<float>(currentFrame_)
                                            / static_cast<float>(cachedTextures_.Size() - 1) };

                sprite_.setColor(sfutil::Transition(colorFrom_, colorTo_, RATIO_COMPLETE));
            }

            sprite_.setScale(
                entityRegion_.width / origSizeV_.x, entityRegion_.height / origSizeV_.y);
        }

        return isFinished_;
    }

} // namespace sfml_util
} // namespace heroespath
