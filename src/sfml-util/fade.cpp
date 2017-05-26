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
// fade.cpp
//
#include "fade.hpp"


namespace sfml_util
{

    Fade::Fade()
    :
        targetColorRed_   (0.0f),
        targetColorGreen_ (0.0f),
        targetColorBlue_  (0.0f),
        targetColorAlpha_ (0.0f),
        currentColorRed_  (0.0f),
        currentColorGreen_(0.0f),
        currentColorBlue_ (0.0f),
        currentColorAlpha_(0.0f),
        speedMultiplier_  (1.0f),
        quad_             (sf::Quads, 4),
        isRedIncr_        (false),
        isBlueIncr_       (false),
        isGreenIncr_      (false),
        isAlphaIncr_      (false)
    {}


    Fade::Fade( const float REGION_LEFT,
                const float REGION_TOP,
                const float REGION_WIDTH,
                const float REGION_HEIGHT )
    :
        targetColorRed_   (0.0f),
        targetColorGreen_ (0.0f),
        targetColorBlue_  (0.0f),
        targetColorAlpha_ (0.0f),
        currentColorRed_  (0.0f),
        currentColorGreen_(0.0f),
        currentColorBlue_ (0.0f),
        currentColorAlpha_(0.0f),
        speedMultiplier_  (1.0f),
        quad_             (sf::Quads, 4),
        isRedIncr_        (false),
        isBlueIncr_       (false),
        isGreenIncr_      (false),
        isAlphaIncr_      (false)
    {
        Setup(REGION_LEFT, REGION_TOP, REGION_WIDTH, REGION_HEIGHT);
    }


    Fade::Fade( const sf::FloatRect & SCREEN_REGION)
    :
        targetColorRed_   (0.0f),
        targetColorGreen_ (0.0f),
        targetColorBlue_  (0.0f),
        targetColorAlpha_ (0.0f),
        currentColorRed_  (0.0f),
        currentColorGreen_(0.0f),
        currentColorBlue_ (0.0f),
        currentColorAlpha_(0.0f),
        speedMultiplier_  (1.0f),
        quad_             (sf::Quads, 4),
        isRedIncr_        (false),
        isBlueIncr_       (false),
        isGreenIncr_      (false),
        isAlphaIncr_      (false)
    {
        Setup(SCREEN_REGION);
    }


    Fade::~Fade()
    {}


    void Fade::UpdateRegion(const float WIDTH, const float HEIGHT)
    {
        quad_[0].position = sf::Vector2f(0.0f, 0.0f);
        quad_[1].position = sf::Vector2f(WIDTH, 0.0f);
        quad_[2].position = sf::Vector2f(WIDTH, HEIGHT);
        quad_[3].position = sf::Vector2f(0.0f, HEIGHT);
    }


    void Fade::Setup(const sf::FloatRect & SCREEN_REGION)
    {
        Setup(SCREEN_REGION.left, SCREEN_REGION.top, SCREEN_REGION.width, SCREEN_REGION.height);
    }


    void Fade::Setup(const float REGION_LEFT,
                     const float REGION_TOP,
                     const float REGION_WIDTH,
                     const float REGION_HEIGHT)
    {
        quad_[0].position = sf::Vector2f(REGION_LEFT, REGION_TOP);
        quad_[1].position = sf::Vector2f(REGION_WIDTH, REGION_TOP);
        quad_[2].position = sf::Vector2f(REGION_WIDTH, REGION_HEIGHT);
        quad_[3].position = sf::Vector2f(REGION_LEFT, REGION_HEIGHT);
    }


    void Fade::FadeTo(const sf::Color & NEW_COLOR, const float SPEED_MULT)
    {
        targetColorRed_   = static_cast<float>(NEW_COLOR.r);
        targetColorGreen_ = static_cast<float>(NEW_COLOR.g);
        targetColorBlue_  = static_cast<float>(NEW_COLOR.b);
        targetColorAlpha_ = static_cast<float>(NEW_COLOR.a);
        speedMultiplier_  = SPEED_MULT;

        isRedIncr_   = (targetColorRed_   > currentColorRed_);
        isGreenIncr_ = (targetColorGreen_ > currentColorGreen_);
        isBlueIncr_  = (targetColorBlue_  > currentColorBlue_);
        isAlphaIncr_ = (targetColorAlpha_ > currentColorAlpha_);
    }


    void Fade::Reset(const sf::Color & NEW_COLOR)
    {
        currentColorRed_   = static_cast<float>(NEW_COLOR.r);
        currentColorGreen_ = static_cast<float>(NEW_COLOR.g);
        currentColorBlue_  = static_cast<float>(NEW_COLOR.b);
        currentColorAlpha_ = static_cast<float>(NEW_COLOR.a);

        targetColorRed_   = static_cast<float>(NEW_COLOR.r);
        targetColorGreen_ = static_cast<float>(NEW_COLOR.g);
        targetColorBlue_  = static_cast<float>(NEW_COLOR.b);
        targetColorAlpha_ = static_cast<float>(NEW_COLOR.a);

        isRedIncr_   = false;
        isGreenIncr_ = false;
        isBlueIncr_  = false;
        isAlphaIncr_ = false;
    }


    const sf::Color Fade::GetCurrentColor() const
    {
        return sf::Color(static_cast<sf::Uint8>(currentColorRed_),
                         static_cast<sf::Uint8>(currentColorGreen_),
                         static_cast<sf::Uint8>(currentColorBlue_),
                         static_cast<sf::Uint8>(currentColorAlpha_));
    }


    const sf::Color Fade::GetTargetColor() const
    {
        return sf::Color(static_cast<sf::Uint8>(targetColorRed_),
                         static_cast<sf::Uint8>(targetColorGreen_),
                         static_cast<sf::Uint8>(targetColorBlue_),
                         static_cast<sf::Uint8>(targetColorAlpha_));
    }


    bool Fade::Update(const float SECONDS)
    {
        const float ADJUSTMENT(SECONDS * speedMultiplier_);

        //adjust color values for the time elapsed
        if ((isRedIncr_) && (currentColorRed_ < targetColorRed_))
            currentColorRed_ += ADJUSTMENT;
        else
            if ((false == isRedIncr_) && (currentColorRed_ > targetColorRed_))
                currentColorRed_ -= ADJUSTMENT;

        if ((isGreenIncr_) && (currentColorGreen_ < targetColorGreen_))
            currentColorGreen_ += ADJUSTMENT;
        else
            if ((false == isGreenIncr_) && (currentColorGreen_ > targetColorGreen_))
                currentColorGreen_ -= ADJUSTMENT;

        if ((isBlueIncr_) && (currentColorBlue_ < targetColorBlue_))
            currentColorBlue_ += ADJUSTMENT;
        else
            if ((false == isBlueIncr_) && (currentColorBlue_ > targetColorBlue_))
                currentColorBlue_ -= ADJUSTMENT;

        if ((isAlphaIncr_) && (currentColorAlpha_ < targetColorAlpha_))
            currentColorAlpha_ += ADJUSTMENT;
        else
            if ((false == isAlphaIncr_) && (currentColorAlpha_ > targetColorAlpha_))
                currentColorAlpha_ -= ADJUSTMENT;

        //clamp to sane values
        if (currentColorRed_ < 0.0f)
            currentColorRed_ = 0.0f;
        else
            if (currentColorRed_ > 255.0f)
                currentColorRed_ = 255.0f;

        if (currentColorGreen_ < 0.0f)
            currentColorGreen_ = 0.0f;
        else
            if (currentColorGreen_ > 255.0f)
                currentColorGreen_ = 255.0f;

        if (currentColorBlue_ < 0.0f)
            currentColorBlue_ = 0.0f;
        else
            if (currentColorBlue_ > 255.0f)
                currentColorBlue_ = 255.0f;

        if (currentColorAlpha_ < 0.0f)
            currentColorAlpha_ = 0.0f;
        else
            if (currentColorAlpha_ > 255.0f)
                currentColorAlpha_ = 255.0f;

        auto & quadZeroColor{ quad_[0].color };
        quadZeroColor.r = static_cast<sf::Uint8>(currentColorRed_);
        quadZeroColor.g = static_cast<sf::Uint8>(currentColorGreen_);
        quadZeroColor.b = static_cast<sf::Uint8>(currentColorBlue_);
        quadZeroColor.a = static_cast<sf::Uint8>(currentColorAlpha_);

        //set all quad corners to be the same color for a full solid color square that covers the whole region
        quad_[1].color = quad_[0].color;
        quad_[2].color = quad_[0].color;
        quad_[3].color = quad_[0].color;

        //return true if the target color has been reached, or passed
        bool hasReachedTargetColor((GetCurrentColor() == GetTargetColor()));
        if (hasReachedTargetColor)
        {
            return true;
        }
        else
        {
            if ((isRedIncr_) && (currentColorRed_ < targetColorRed_)) return false;
            if ((false == isRedIncr_) && (currentColorRed_ > targetColorRed_)) return false;

            if ((isGreenIncr_) && (currentColorGreen_ < targetColorGreen_)) return false;
            if ((false == isGreenIncr_) && (currentColorGreen_ > targetColorGreen_)) return false;

            if ((isBlueIncr_) && (currentColorBlue_ < targetColorBlue_)) return false;
            if ((false == isBlueIncr_) && (currentColorBlue_ > targetColorBlue_)) return false;

            if ((isAlphaIncr_) && (currentColorAlpha_ < targetColorAlpha_)) return false;
            if ((false == isAlphaIncr_) && (currentColorAlpha_ > targetColorAlpha_)) return false;

            return true;
        }
    }


    void Fade::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(quad_, states);
    }

}
