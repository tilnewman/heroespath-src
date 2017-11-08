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
// item-detail-viewer.hpp
//
#include "item-detail-viewer.hpp"

#include "item/item.hpp"

#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/sound-manager.hpp"

#include <sstream>


namespace heroespath
{
namespace stage
{

    ItemDetailViewer::ItemDetailViewer()
    :
        SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth()),
        HALF_SCREEN_WIDTH_(SCREEN_WIDTH_ * 0.5f),
        SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight()),
        IMAGE_TOP_SPACER_(sfml_util::MapByRes(35.0f, 100.0f)),
        INNER_SPACER_( 20.0f ),
        DOUBLE_INNER_SPACER_(INNER_SPACER_ * 2.0f),
        TARGET_WIDTH_(SCREEN_WIDTH_ * 0.75f),
        TARGET_RECT_(
            HALF_SCREEN_WIDTH_ - (TARGET_WIDTH_ * 0.5f),
            sfml_util::MapByRes(35.0f, 100.0f),
            TARGET_WIDTH_,
            SCREEN_HEIGHT_ * 0.85f),
        SLIDER_SPEED_(6.0f),
        backgroundQuads_(sf::Quads, 4),
        texture_(),
        sprite_(),
        slider_(),
        textRegionUPtr_(),
        sourceRect(),
        itemPtr_(nullptr),
        willShowImage_(false),
        isBeforeAnyChange_(true),
        isShowing_(false)
    {
        slider_.Setup(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(1.0f, 1.0f), SLIDER_SPEED_);
        slider_.ChangeDirection();
    }


    void ItemDetailViewer::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        //Always draw because it is a fast operation and will
        //be fully transparent when should not be drawn.
        target.draw(backgroundQuads_, states);

        if (willShowImage_)
        {
            target.draw(sprite_, states);
        }

        if (textRegionUPtr_.get() != nullptr)
        {
            target.draw( * textRegionUPtr_, states);
        }
    }


    void ItemDetailViewer::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        auto const PRE_IS_MOVING_TOWARD{ slider_.Direction() == sfml_util::Moving::Toward };
        auto const IS_FINISHED_MOVING{ slider_.UpdateTime(ELAPSED_TIME_SECONDS) };
        auto const IS_MOVING_TOWARD{ slider_.Direction() == sfml_util::Moving::Toward };

        auto const PROGRESS_RATIO{ ((IS_MOVING_TOWARD) ?
            slider_.ProgressRatio() : (1.0f - slider_.ProgressRatio())) };

        if (slider_.ProgressRatio() > 0.1f)
        {
            isBeforeAnyChange_ = false;
        }

        if (isBeforeAnyChange_)
        {
            SetupBackgroundQuadColors(0.0f);
        }
        else
        {
            SetupBackgroundQuadColors(PROGRESS_RATIO);
        }

        SetupBackgroundQuadPositions(PROGRESS_RATIO);

        if (IS_FINISHED_MOVING &&
            PRE_IS_MOVING_TOWARD &&
            (slider_.IsMoving() == false))
        {
            SetupImage(itemPtr_);
            SetupText(itemPtr_);
            isShowing_ = true;
        }
    }


    void ItemDetailViewer::FadeIn(const item::ItemPtr_t ITEM_PTR, const sf::FloatRect & RECT)
    {
        if (slider_.Direction() == sfml_util::Moving::Away)
        {
            sourceRect = RECT;
            itemPtr_ = ITEM_PTR;

            slider_.ChangeDirection();
            slider_.Start();

            sfml_util::SoundManager::Instance()->PlaySfx_TickOn();
        }
    }


    void ItemDetailViewer::FadeOut()
    {
        if (slider_.Direction() == sfml_util::Moving::Toward)
        {
            SetupImage(nullptr);
            SetupText(nullptr);

            isShowing_ = false;

            slider_.ChangeDirection();
            slider_.Start();

            sfml_util::SoundManager::Instance()->PlaySfx_TickOff();
        }
    }


    void ItemDetailViewer::SetupBackgroundQuadColors(const float PROGRESS_RATIO)
    {
        auto const BACKGROUND_COLOR{ sf::Color(
            0,
            0,
            0,
            static_cast<sf::Uint8>(PROGRESS_RATIO * 255.0f)) };

        backgroundQuads_[0].color = BACKGROUND_COLOR;
        backgroundQuads_[1].color = BACKGROUND_COLOR;
        backgroundQuads_[2].color = BACKGROUND_COLOR;
        backgroundQuads_[3].color = BACKGROUND_COLOR;
    }


    void ItemDetailViewer::SetupBackgroundQuadPositions(const float RATIO)
    {
        auto const SOURCE_LEFT{ sourceRect.left };
        auto const SOURCE_RIGHT{ sourceRect.left + sourceRect.width };
        auto const SOURCE_TOP{ sourceRect.top };
        auto const SOURCE_BOT{ sourceRect.top + sourceRect.height };

        auto const TARGET_LEFT{ TARGET_RECT_.left };
        auto const TARGET_RIGHT{ TARGET_RECT_.left + TARGET_RECT_.width };
        auto const TARGET_TOP{ TARGET_RECT_.top };
        auto const TARGET_BOT{ TARGET_RECT_.top + TARGET_RECT_.height };

        auto const LEFT_MORE{ SOURCE_LEFT - ((SOURCE_LEFT - TARGET_LEFT) * RATIO) };
        auto const LEFT_LESS{ SOURCE_LEFT + ((TARGET_LEFT - SOURCE_LEFT) * RATIO) };
        auto const LEFT{ ((SOURCE_LEFT > TARGET_LEFT) ? LEFT_MORE : LEFT_LESS) };

        auto const RIGHT_MORE{ SOURCE_RIGHT - ((SOURCE_RIGHT - TARGET_RIGHT) * RATIO) };
        auto const RIGHT_LESS{ SOURCE_RIGHT + ((TARGET_RIGHT - SOURCE_RIGHT) * RATIO) };
        auto const RIGHT{ (SOURCE_RIGHT > TARGET_RIGHT) ? RIGHT_MORE : RIGHT_LESS };

        auto const TOP{ SOURCE_TOP - ((SOURCE_TOP - TARGET_TOP) * RATIO) };

        auto const BOT_MORE{ SOURCE_BOT - ((SOURCE_BOT - TARGET_BOT) * RATIO) };
        auto const BOT_LESS{ SOURCE_BOT + ((TARGET_BOT - SOURCE_BOT) * RATIO) };
        auto const BOT{ (SOURCE_BOT > TARGET_BOT) ? BOT_MORE : BOT_LESS };

        backgroundQuads_[0].position = sf::Vector2f(LEFT, TOP);
        backgroundQuads_[1].position = sf::Vector2f(RIGHT, TOP);
        backgroundQuads_[2].position = sf::Vector2f(RIGHT, BOT);
        backgroundQuads_[3].position = sf::Vector2f(LEFT, BOT);
    }


    void ItemDetailViewer::SetupImage(const item::ItemPtr_t ITEM_PTR)
    {
        if (ITEM_PTR == nullptr)
        {
            willShowImage_ = false;
            return;
        }

        willShowImage_ = true;

        sfml_util::gui::ItemImageManager::Instance()->Load(texture_, ITEM_PTR);

        sprite_.setTexture(texture_, true);

        //found by experiment to look good at various resolutions
        const float SCALE(sfml_util::MapByRes(0.75f, 1.25f));
        sprite_.setScale(SCALE, SCALE);

        sprite_.setPosition(
            HALF_SCREEN_WIDTH_ - (sprite_.getGlobalBounds().width * 0.5f),
            TARGET_RECT_.left + INNER_SPACER_);
    }


    void ItemDetailViewer::SetupText(const item::ItemPtr_t ITEM_PTR)
    {
        if (ITEM_PTR == nullptr)
        {
            textRegionUPtr_.reset();
            return;
        }

        std::ostringstream ss;
        ss << ITEM_PTR->Name() << "\n"
            << ITEM_PTR->Desc() << "\n\n"
            << item::category::ToString(ITEM_PTR->Category(), true) << "\n";

        if (ITEM_PTR->ExclusiveRole() != creature::role::Count)
        {
            ss << "(can only be used by "
                << creature::role::ToString(ITEM_PTR->ExclusiveRole()) << "s)\n";
        }

        ss << "\n";

        ss << "weighs " << ITEM_PTR->Weight() << "\n"
            << "worth about " << ITEM_PTR->Price() << " coins\n";

        if (ITEM_PTR->IsWeapon())
        {
            ss << "Damage:  " << ITEM_PTR->DamageMin() << "-" << ITEM_PTR->DamageMax();
        }
        else if (ITEM_PTR->IsArmor())
        {
            ss << "Armor Bonus:  " << ITEM_PTR->ArmorRating();
        }

        ss << "\n\n";

        const sfml_util::gui::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color::White,
            sfml_util::Justified::Center);

        auto const TEXT_LEFT{ TARGET_RECT_.left + INNER_SPACER_ };

        auto const TEXT_TOP{
            sprite_.getGlobalBounds().top + sprite_.getGlobalBounds().height + INNER_SPACER_ };

        auto const TEXT_WIDTH{ TARGET_RECT_.width - DOUBLE_INNER_SPACER_ };

        auto const SPRITE_BOTTOM{ sprite_.getPosition().y + sprite_.getGlobalBounds().height };

        auto const TEXT_HEIGHT{ (TARGET_RECT_.height - SPRITE_BOTTOM) - DOUBLE_INNER_SPACER_ };

        const sf::FloatRect TEXT_RECT{ TEXT_LEFT, TEXT_TOP, TEXT_WIDTH, TEXT_HEIGHT };

        textRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "ItemDetailViewer'sTextRegion", TEXT_INFO, TEXT_RECT);
    }

}
}
