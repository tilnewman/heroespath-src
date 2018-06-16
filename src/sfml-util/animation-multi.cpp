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

#include "game/game-data-file.hpp"
#include "misc/real.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/texture-cache.hpp"

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
        , origSizeV_(sfml_util::ConvertVector<unsigned, float>(cachedTextures_.Front().getSize()))
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

    void AnimationMultiTexture::SetEntityPos(const sf::Vector2f & V)
    {
        entityRegion_ = sf::FloatRect(V.x, V.y, entityRegion_.width, entityRegion_.height);
        sprite_.setPosition(V);
    }

    void AnimationMultiTexture::SetEntityPos(const float LEFT, const float TOP)
    {
        SetEntityPos(sf::Vector2f(LEFT, TOP));
    }

    void AnimationMultiTexture::SetEntityRegion(const sf::FloatRect & R)
    {
        entityRegion_ = R;
        sprite_.setPosition(R.left, R.top);
        sprite_.setScale(entityRegion_.width / origSizeV_.x, entityRegion_.height / origSizeV_.y);
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
                auto const RATIO_COMPLETE{ static_cast<float>(currentFrame_)
                                           / static_cast<float>(cachedTextures_.Size() - 1) };

                sprite_.setColor(
                    sfml_util::color::TransitionColor(colorFrom_, colorTo_, RATIO_COMPLETE));
            }

            sprite_.setScale(
                entityRegion_.width / origSizeV_.x, entityRegion_.height / origSizeV_.y);
        }

        return isFinished_;
    }

    void AnimationMultiTexture::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);
        sprite_.move(HORIZ, VERT);
    }

} // namespace sfml_util
} // namespace heroespath
