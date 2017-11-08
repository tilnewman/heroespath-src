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
// animation-single.cpp
//
#include "animation-single.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/texture-cache.hpp"

#include "game/game-data-file.hpp"

#include "misc/real.hpp"


namespace sfml_util
{

    AnimationSingleTexture::AnimationSingleTexture(
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
        sprite_(),
        origSizeV_(0.0f, 0.0f),
        rects_()
    {
        auto const FRAME_SIZE_INT_PAIR{ sfml_util::Animations::SizePair(ENUM) };
        origSizeV_.x = static_cast<float>(FRAME_SIZE_INT_PAIR.first);
        origSizeV_.y = static_cast<float>(FRAME_SIZE_INT_PAIR.second);

        entityRegion_.width = ((heroespath::misc::IsRealZero(REGION.width)) ? origSizeV_.x : REGION.width);
        entityRegion_.height = ((heroespath::misc::IsRealZero(REGION.height)) ? origSizeV_.y : REGION.height);

        auto const TEXTURE_ID_NUM{ TextureCache::Instance()->AddByPath(
            heroespath::game::GameDataFile::Instance()->GetMediaPath(
                sfml_util::Animations::MediaPathKey(ENUM)), true) };

        sprite_.setTexture(TextureCache::Instance()->GetByIndex(TEXTURE_ID_NUM));
        sprite_.setPosition(GetEntityPos());
        sprite_.setScale(entityRegion_.width / origSizeV_.x, entityRegion_.height / origSizeV_.y);
        sprite_.setColor(colorFrom_);

        auto const TEXTURE_WIDTH { static_cast<int>(sprite_.getLocalBounds().width) };
        auto const TEXTURE_HEIGHT{ static_cast<int>(sprite_.getLocalBounds().height) };

        int posX(0);
        int posY(0);

        while(posY < (TEXTURE_HEIGHT - FRAME_SIZE_INT_PAIR.first))
        {
            if (rects_.empty() == false)
            {
                posY += FRAME_SIZE_INT_PAIR.second;
            }

            while (posX < (TEXTURE_WIDTH - FRAME_SIZE_INT_PAIR.first))
            {
                if (rects_.empty() == false)
                {
                    posX += FRAME_SIZE_INT_PAIR.first;
                }

                rects_.push_back( sf::IntRect(posX,
                                              posY,
                                              FRAME_SIZE_INT_PAIR.first,
                                              FRAME_SIZE_INT_PAIR.second) );
            };

            posX = 0;
        };

        M_ASSERT_OR_LOGANDTHROW_SS((rects_.empty() == false),
            "sfml_util::AnimationSingleTexture::Constructor() Failed to find any frame rects.");

        sprite_.setTextureRect(rects_[0]);
    }


    AnimationSingleTexture::~AnimationSingleTexture()
    {}


    void AnimationSingleTexture::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.blendMode = blendMode_;
        target.draw(sprite_, states);
    }


    void AnimationSingleTexture::SetEntityPos(const sf::Vector2f & V)
    {
        entityRegion_ = sf::FloatRect(V.x, V.y, entityRegion_.width, entityRegion_.height);
        sprite_.setPosition(V);
    }


    void AnimationSingleTexture::SetEntityPos(const float LEFT, const float TOP)
    {
        SetEntityPos(sf::Vector2f(LEFT, TOP));
    }


    void AnimationSingleTexture::SetEntityRegion(const sf::FloatRect & R)
    {
        entityRegion_ = R;
        sprite_.setPosition(R.left, R.top);
        sprite_.setScale(entityRegion_.width / origSizeV_.x, entityRegion_.height / origSizeV_.y);
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
                auto const RATIO_COMPLETE{ static_cast<float>(currentFrame_) /
                    static_cast<float>(rects_.size() - 1) };

                sprite_.setColor(sfml_util::color::TransitionColor(colorFrom_,
                    colorTo_, RATIO_COMPLETE));
            }

            sprite_.setScale(entityRegion_.width / origSizeV_.x,
                entityRegion_.height / origSizeV_.y);
        }

        return isFinished_;
    }


    void AnimationSingleTexture::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);
        sprite_.move(HORIZ, VERT);
    }

}
