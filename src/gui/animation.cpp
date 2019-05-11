// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// animation.cpp
//
#include "animation.hpp"

#include "gui/cached-texture.hpp"
#include "misc/random.hpp"
#include "misc/strings.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/scale.hpp"
#include "sfutil/vector-and-rect.hpp"

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
        : Entity(NAMEOF_ENUM_STR(ENUM) + "_Animation", REGION)
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
        , cachedTextures_(PathWrapper(Animations::MediaPath(ENUM)))
        , frameIndexRects_()
    {
        entityRegion_.width = ((misc::IsRealZero(REGION.width)) ? origSizeV_.x : REGION.width);
        entityRegion_.height = ((misc::IsRealZero(REGION.height)) ? origSizeV_.y : REGION.height);

        frameIndexRects_ = MakeFrameIndexRects(
            cachedTextures_, sf::Vector2i { origSizeV_ }, gui::Animations::TextureRectTrim(ENUM));

        if (frameIndexRects_.empty())
        {
            M_HP_LOG_ERR(
                "Failed to load/find any animation frames." << NAMEOF_ENUM(ENUM)
                                                            << M_HP_VAR_STR(cachedTextures_.Path())
                                                            << M_HP_VAR_STR(origSizeV_));
        }

        SetupSprite();
    }

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

    void Animation::ColorTransition(const sf::Color & FROM, const sf::Color & TO)
    {
        colorFrom_ = FROM;
        colorTo_ = TO;
    }

    void Animation::RandomVaryTimePerFrame()
    {
        const auto VARY_SEC_MAX { 0.02f };
        timePerFrameSec_ -= VARY_SEC_MAX * 0.5f;
        timePerFrameSec_ += misc::Random(VARY_SEC_MAX);
    }

    const Animation::IndexRectVec_t Animation::MakeFrameIndexRects(
        const gui::CachedTextures & CACHED_TEXTURES,
        const sf::Vector2i & FRAME_SIZE_V,
        const sf::IntRect & TRIM) const
    {
        IndexRectVec_t indexRects;

        const auto TEXTURE_COUNT { CACHED_TEXTURES.Size() };
        for (std::size_t textureIndex(0); textureIndex < TEXTURE_COUNT; ++textureIndex)
        {
            const auto & CACHED_TEXTURE { CACHED_TEXTURES.At(textureIndex) };
            const sf::Vector2i TEXTURE_SIZE_V { CACHED_TEXTURE.getSize() };
            const auto FRAME_RECTS { FrameRectsInTextureOfSize(TEXTURE_SIZE_V, FRAME_SIZE_V) };

            if (FRAME_RECTS.empty())
            {
                M_HP_LOG_ERR(
                    "Failed to calculate frame rects for texture."
                    + M_HP_VAR_STR(CACHED_TEXTURES.Path()) + M_HP_VAR_STR(textureIndex)
                    + M_HP_VAR_STR(TEXTURE_SIZE_V) + M_HP_VAR_STR(FRAME_SIZE_V));
            }

            for (const auto & FRAME_RECT : FRAME_RECTS)
            {
                auto frameRectFinal { FRAME_RECT };

                frameRectFinal.left += TRIM.left;
                frameRectFinal.top += TRIM.top;
                frameRectFinal.width -= TRIM.width;
                frameRectFinal.height -= TRIM.height;

                indexRects.emplace_back(std::make_pair(textureIndex, frameRectFinal));
            }
        }

        return indexRects;
    }

    const Animation::RectsVec_t Animation::FrameRectsInTextureOfSize(
        const sf::Vector2i & TEXTURE_SIZE_V, const sf::Vector2i & FRAME_SIZE_V) const
    {
        if (FRAME_SIZE_V == TEXTURE_SIZE_V)
        {
            return { sf::IntRect(sf::Vector2i(), FRAME_SIZE_V) };
        }

        RectsVec_t frameRects;
        for (int vert(0); vert < TEXTURE_SIZE_V.y; vert += FRAME_SIZE_V.y)
        {
            for (int horiz(0); horiz < TEXTURE_SIZE_V.x; horiz += FRAME_SIZE_V.x)
            {
                frameRects.emplace_back(sf::IntRect(sf::Vector2i(horiz, vert), FRAME_SIZE_V));
            }
        }

        return frameRects;
    }

    void Animation::SetupSprite()
    {
        if (frameIndex_ >= frameIndexRects_.size())
        {
            return;
        }

        const auto & INDEX_RECT_PAIR { frameIndexRects_.at(frameIndex_) };

        sprite_.setTexture(cachedTextures_.At(INDEX_RECT_PAIR.first));
        sprite_.setTextureRect(INDEX_RECT_PAIR.second);
        sfutil::FitAndCenterTo(sprite_, GetEntityRegion());

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
