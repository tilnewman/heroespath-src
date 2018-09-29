// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// animation-single.cpp
//
#include "animation-single.hpp"

#include "misc/real.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace sfml_util
{

    AnimationSingleTexture::AnimationSingleTexture(
        const Animations::Enum ENUM,
        const sf::FloatRect & REGION,
        const float TIME_PER_FRAME_SEC,
        const sf::BlendMode & BLEND_MODE,
        const sf::Color & COLOR_FROM,
        const sf::Color & COLOR_TO)
        : Animation(ENUM, REGION, TIME_PER_FRAME_SEC, BLEND_MODE, COLOR_FROM, COLOR_TO)
        , cachedTexture_(sfml_util::Animations::MediaPathKey(ENUM))
        , sprite_(cachedTexture_.Get())
        , origSizeV_(sfml_util::Animations::SizePair(ENUM))
        , rects_()
    {
        entityRegion_.width = ((misc::IsRealZero(REGION.width)) ? origSizeV_.x : REGION.width);
        entityRegion_.height = ((misc::IsRealZero(REGION.height)) ? origSizeV_.y : REGION.height);

        sprite_.setPosition(entityRegion_.left, entityRegion_.top);
        sprite_.setScale(entityRegion_.width / origSizeV_.x, entityRegion_.height / origSizeV_.y);
        sprite_.setColor(colorFrom_);

        const sf::Vector2i TEXTURE_SIZE_V(cachedTexture_.Get().getSize());
        const sf::Vector2i FRAME_SIZE_V(origSizeV_);

        M_ASSERT_OR_LOGANDTHROW_SS(
            ((TEXTURE_SIZE_V.x >= FRAME_SIZE_V.x) && (TEXTURE_SIZE_V.y >= FRAME_SIZE_V.y)),
            "sfml_util::AnimationSingleTexture::Constructor(enum="
                << Animations::ToString(ENUM) << ", region=" << REGION << ", time_per_frames_sec="
                << TIME_PER_FRAME_SEC << ", color_from=" << COLOR_FROM << ", color_to=" << COLOR_TO
                << ") failed because the frame size was bigger than the image size. (image_size="
                << TEXTURE_SIZE_V << ", frame_size=" << FRAME_SIZE_V << ")");

        sf::Vector2i posV { 0, 0 };
        while (posV.y < TEXTURE_SIZE_V.y)
        {
            while (posV.x < TEXTURE_SIZE_V.x)
            {
                rects_.emplace_back(sf::IntRect(posV, FRAME_SIZE_V));
                posV.x += FRAME_SIZE_V.x;
            };

            posV.x = 0;
            posV.y += FRAME_SIZE_V.y;
        };

        auto const CALCULATED_FRAME_COUNT_EXPECTED { static_cast<std::size_t>(
            (TEXTURE_SIZE_V.x / FRAME_SIZE_V.x) * (TEXTURE_SIZE_V.y / FRAME_SIZE_V.y)) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (CALCULATED_FRAME_COUNT_EXPECTED == rects_.size()),
            "sfml_util::AnimationSingleTexture::Constructor(enum="
                << Animations::ToString(ENUM) << ", region=" << REGION << ", time_per_frames_sec="
                << TIME_PER_FRAME_SEC << ", color_from=" << COLOR_FROM << ", color_to=" << COLOR_TO
                << ") Failed to create the expected number "
                   "of frames. (image_size="
                << TEXTURE_SIZE_V << ", frame_size=" << FRAME_SIZE_V << ")  (actual="
                << rects_.size() << ", expected=" << CALCULATED_FRAME_COUNT_EXPECTED << ")");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (rects_.empty() == false),
            "sfml_util::AnimationSingleTexture::Constructor(enum="
                << Animations::ToString(ENUM) << ", region=" << REGION << ", time_per_frames_sec="
                << TIME_PER_FRAME_SEC << ", color_from=" << COLOR_FROM << ", color_to=" << COLOR_TO
                << ") Failed to create any frames. (image_size=" << TEXTURE_SIZE_V
                << ", frame_size=" << FRAME_SIZE_V
                << ")  (expected=" << CALCULATED_FRAME_COUNT_EXPECTED << ")");

        sprite_.setTextureRect(rects_[0]);
    }

    AnimationSingleTexture::~AnimationSingleTexture() = default;

    void AnimationSingleTexture::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = blendMode_;
        target.draw(sprite_, states);
    }

    void AnimationSingleTexture::SetEntityPos(const float LEFT, const float TOP)
    {
        MoveEntityPos(LEFT - entityRegion_.left, TOP - entityRegion_.top);
    }

    void AnimationSingleTexture::SetEntityRegion(const sf::FloatRect & R)
    {
        Entity::SetEntityRegion(R);
        sfutil::SetSizeAndPos(sprite_, R);
    }

    void AnimationSingleTexture::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);
        sprite_.move(HORIZ, VERT);
    }

    bool AnimationSingleTexture::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        frameTimerSec_ += ELAPSED_TIME_SEC;
        while (frameTimerSec_ > timePerFrameSec_)
        {
            frameTimerSec_ -= timePerFrameSec_;
            if (++currentFrame_ >= (rects_.size() - 1))
            {
                currentFrame_ = 0;
                isFinished_ = true;
            }

            sprite_.setTextureRect(rects_[currentFrame_]);

            if (colorFrom_ != colorTo_)
            {
                auto const RATIO_COMPLETE { static_cast<float>(currentFrame_)
                                            / static_cast<float>(rects_.size() - 1) };

                sprite_.setColor(sfutil::Transition(colorFrom_, colorTo_, RATIO_COMPLETE));
            }

            sprite_.setScale(
                entityRegion_.width / origSizeV_.x, entityRegion_.height / origSizeV_.y);
        }

        return isFinished_;
    }

} // namespace sfml_util
} // namespace heroespath
