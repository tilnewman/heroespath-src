// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-animation.hpp
//
#include "text-animation.hpp"

#include "gui/font-manager.hpp"
#include "gui/text-region.hpp"
#include "misc/random.hpp"
#include "sfutil/font.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <algorithm>
#include <string>

namespace heroespath
{
namespace gui
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
            const auto START_POS_LEFT { REGION.left + (REGION.width * 0.5f) };
            const auto START_POS_TOP { REGION.top + (REGION.height * 0.85f) };
            startPosV_ = sf::Vector2f(START_POS_LEFT, START_POS_TOP);

            const auto END_POS_LEFT { START_POS_LEFT };
            const auto END_POS_TOP { REGION.top };
            endPosV_ = sf::Vector2f(END_POS_LEFT, END_POS_TOP);

            const TextInfo TEXT_INFO(
                TEXT,
                gui::GuiFont::Default,
                START_FONT_SIZE_,
                START_COLOR_,
                gui::Justified::Center);

            sf::FloatRect r(0.0f, 0.0f, 0.0f, 0.0f);

            textRegionUPtr_ = std::make_unique<TextRegion>("TextAnimation's", TEXT_INFO, r);

            const auto TEXT_POS_LEFT { START_POS_LEFT
                                       - (textRegionUPtr_->GetEntityRegion().width * 0.5f) };

            const auto TEXT_POS_TOP { START_POS_TOP };

            textRegionUPtr_->SetEntityPos(TEXT_POS_LEFT, TEXT_POS_TOP);
            textRegionUPtr_->SetEntityColorFgBoth(START_COLOR_);
        }

        void TextAnimation::Update(const float ELAPSED_TIME_SEC)
        {
            if (isFinished_)
            {
                return;
            }

            const auto SLIDER_POS { slider_.Update(ELAPSED_TIME_SEC) };

            const auto FONT_SIZE { START_FONT_SIZE_
                                   + static_cast<unsigned int>(
                                       static_cast<float>(END_FONT_SIZE_ - START_FONT_SIZE_)
                                       * SLIDER_POS) };

            const TextInfo TEXT_INFO(
                TEXT_,
                gui::GuiFont::Default,
                FONT_SIZE,
                sfutil::Transition(START_COLOR_, END_COLOR_, SLIDER_POS),
                gui::Justified::Center,
                sf::Text::Bold);

            sf::FloatRect r(0.0f, 0.0f, 0.0f, 0.0f);

            textRegionUPtr_->Setup(TEXT_INFO, r);

            const auto POS_LEFT { (startPosV_.x + ((endPosV_.x - startPosV_.x) * SLIDER_POS))
                                  - (textRegionUPtr_->GetEntityRegion().width * 0.5f) };

            const auto POS_TOP { startPosV_.y + ((endPosV_.y - startPosV_.y) * 2.0f * SLIDER_POS) };

            textRegionUPtr_->SetEntityPos(POS_LEFT, POS_TOP);

            isFinished_ = slider_.IsStopped();
        }

        void TextAnimation::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            states.blendMode = sf::BlendAdd;
            target.draw(*textRegionUPtr_, states);
        }

    } // namespace animation
} // namespace gui
} // namespace heroespath
