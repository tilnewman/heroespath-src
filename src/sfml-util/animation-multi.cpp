// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// animation-multi.cpp
//
#include "animation-multi.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/texture-cache.hpp"

#include "game/game-data-file.hpp"

#include "misc/real.hpp"


namespace heroespath
{
namespace sfml_util
{

    AnimationMultiTexture::AnimationMultiTexture(
        const Animations::Enum ENUM,
        const sf::FloatRect &  REGION,
        const float            TIME_PER_FRAME_SEC,
        const sf::BlendMode &  BLEND_MODE,
        const sf::Color &      COLOR_FROM,
        const sf::Color &      COLOR_TO)
    :
        Animation(ENUM,
                  REGION,
                  TIME_PER_FRAME_SEC,
                  BLEND_MODE,
                  COLOR_FROM,
                  COLOR_TO),
        sprite_      (),
        origSizeV_   (0.0f, 0.0f),
        textureIdVec_(TextureCache::Instance()->AddAllInDirectoryByPath(
                         heroespath::game::GameDataFile::Instance()->GetMediaPath(
                            Animations::MediaPathKey(ENUM)), true))
    {
        M_ASSERT_OR_LOGANDTHROW_SS((textureIdVec_.empty() == false),
            "sfml_util::AnimationMultiTexture::Constructor() Failed to load any frame textures.");

        auto const FRAME_SIZE_INT_PAIR{ sfml_util::Animations::SizePair(ENUM) };
        origSizeV_.x = static_cast<float>(FRAME_SIZE_INT_PAIR.first);
        origSizeV_.y = static_cast<float>(FRAME_SIZE_INT_PAIR.second);

        entityRegion_.width = ((heroespath::misc::IsRealZero(REGION.width)) ? origSizeV_.x : REGION.width);
        entityRegion_.height = ((heroespath::misc::IsRealZero(REGION.height)) ? origSizeV_.y : REGION.height);

        sprite_.setTexture(TextureCache::Instance()->GetByIndex(textureIdVec_[0]));
        sprite_.setPosition(REGION.left, REGION.top);
        sprite_.setScale(entityRegion_.width / origSizeV_.x, entityRegion_.height / origSizeV_.y);
        sprite_.setColor(colorFrom_);

        //verify the sizes of all loaded textures match what is in animtion-enum.cpp
        auto const ORIG_SIZE_VU{ sf::Vector2u(static_cast<unsigned>(origSizeV_.x),
            static_cast<unsigned>(origSizeV_.y)) };
        //
        auto const NUM_TEXTURES{ textureIdVec_.size() };
        for (std::size_t i(0); i<NUM_TEXTURES; ++i)
        {
            auto const NEXT_TEXTURE_ID{ textureIdVec_[i] };

            auto const NEXT_SIZE_VU{ TextureCache::Instance()->
                GetByIndex(NEXT_TEXTURE_ID).getSize() };

            M_ASSERT_OR_LOGANDTHROW_SS(((NEXT_SIZE_VU.x == ORIG_SIZE_VU.x) &&
                (NEXT_SIZE_VU.y == ORIG_SIZE_VU.y)), "sfml_util::AnimationMultiTexture::"
                << "Constructor() found texture #" << i << " was not the correct size.  "
                << "(correct=" << ORIG_SIZE_VU.x << "x" << ORIG_SIZE_VU.y << " found="
                << NEXT_SIZE_VU.x << "x" << NEXT_SIZE_VU.y << ")");
        }
    }


    AnimationMultiTexture::~AnimationMultiTexture()
    {}


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
            if (++currentFrame_ >= (textureIdVec_.size() - 1))
            {
                currentFrame_ = 0;
                isFinished_ = true;
            }

            sprite_.setTexture(TextureCache::Instance()->GetByIndex(
                textureIdVec_[currentFrame_]));

            if (colorFrom_ != colorTo_)
            {
                auto const RATIO_COMPLETE{ static_cast<float>(currentFrame_) /
                    static_cast<float>(textureIdVec_.size() - 1) };

                sprite_.setColor(sfml_util::color::TransitionColor(colorFrom_,
                    colorTo_,
                    RATIO_COMPLETE));
            }

            sprite_.setScale(entityRegion_.width / origSizeV_.x,
                entityRegion_.height / origSizeV_.y);
        }

        return isFinished_;
    }


    void AnimationMultiTexture::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);
        sprite_.move(HORIZ, VERT);
    }

}
}
