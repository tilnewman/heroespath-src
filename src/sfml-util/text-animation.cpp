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
// text-animation.hpp
//
#include "text-animation.hpp"

#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/sfml-util.hpp"

#include "misc/random.hpp"

#include <algorithm>
#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace animation
    {

        TextAnimation::TextAnimation(
            const std::string & TEXT,
            const sf::FloatRect & REGION,
            const float SPEED,
            const unsigned int START_FONT_SIZE,
            const unsigned int END_FONT_SIZE,
            const sf::Color & START_COLOR,
            const sf::Color & END_COLOR)
            : TEXT_(TEXT)
            , START_FONT_SIZE_(START_FONT_SIZE)
            , END_FONT_SIZE_(END_FONT_SIZE)
            , START_COLOR_(START_COLOR)
            , END_COLOR_(END_COLOR)
            , startPosV_(0.0f, 0.0f)
            , endPosV_(0.0f, 0.0f)
            , isFinished_(false)
            , textRegionUPtr_()
            , slider_(std::max(1.0f, SPEED), 0.5f)
        {
            auto const START_POS_LEFT{ REGION.left + (REGION.width * 0.5f) };
            auto const START_POS_TOP{ REGION.top + (REGION.height * 0.85f) };
            startPosV_ = sf::Vector2f(START_POS_LEFT, START_POS_TOP);

            auto const END_POS_LEFT{ START_POS_LEFT };
            auto const END_POS_TOP{ REGION.top };
            endPosV_ = sf::Vector2f(END_POS_LEFT, END_POS_TOP);

            const gui::TextInfo TEXT_INFO(
                TEXT,
                sfml_util::FontManager::Instance()->Font_Default1(),
                START_FONT_SIZE_,
                START_COLOR_,
                sfml_util::Justified::Center);

            sf::FloatRect r(0.0f, 0.0f, 0.0f, 0.0f);

            textRegionUPtr_ = std::make_unique<gui::TextRegion>("TextAnimation's", TEXT_INFO, r);

            auto const TEXT_POS_LEFT{ START_POS_LEFT
                                      - (textRegionUPtr_->GetEntityRegion().width * 0.5f) };

            auto const TEXT_POS_TOP{ START_POS_TOP };

            textRegionUPtr_->SetEntityPos(TEXT_POS_LEFT, TEXT_POS_TOP);

            textRegionUPtr_->SetEntityColorFgBoth(START_COLOR_);
        }

        void TextAnimation::Update(const float ELAPSED_TIME_SEC)
        {
            if (isFinished_)
            {
                return;
            }

            auto const SLIDER_POS{ slider_.Update(ELAPSED_TIME_SEC) };

            auto const FONT_SIZE{ START_FONT_SIZE_
                                  + static_cast<unsigned int>(
                                        static_cast<float>(END_FONT_SIZE_ - START_FONT_SIZE_)
                                        * SLIDER_POS) };

            const gui::TextInfo TEXT_INFO(
                TEXT_,
                sfml_util::FontManager::Instance()->Font_Default1(),
                FONT_SIZE,
                sfml_util::color::TransitionColor(START_COLOR_, END_COLOR_, SLIDER_POS),
                sf::BlendAlpha,
                sf::Text::Bold,
                sfml_util::Justified::Center);

            sf::FloatRect r(0.0f, 0.0f, 0.0f, 0.0f);

            textRegionUPtr_->Setup(TEXT_INFO, r);

            auto const POS_LEFT{ (startPosV_.x + ((endPosV_.x - startPosV_.x) * SLIDER_POS))
                                 - (textRegionUPtr_->GetEntityRegion().width * 0.5f) };

            auto const POS_TOP{ startPosV_.y + ((endPosV_.y - startPosV_.y) * 2.0f * SLIDER_POS) };

            textRegionUPtr_->SetEntityPos(POS_LEFT, POS_TOP);

            isFinished_ = slider_.IsDone();
        }

        void TextAnimation::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            states.blendMode = sf::BlendAdd;
            target.draw(*textRegionUPtr_, states);
        }
    } // namespace animation
} // namespace sfml_util
} // namespace heroespath
