// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-detail-viewer.hpp
//
#include "item-detail-viewer.hpp"

#include "item/item.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/image-loaders.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfutil/display.hpp"

#include <sstream>

namespace heroespath
{
namespace stage
{

    ItemDetailViewer::ItemDetailViewer()
        : SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , HALF_SCREEN_WIDTH_(SCREEN_WIDTH_ * 0.5f)
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , IMAGE_TOP_SPACER_(sfutil::MapByRes(35.0f, 100.0f))
        , INNER_SPACER_(20.0f)
        , DOUBLE_INNER_SPACER_(INNER_SPACER_ * 2.0f)
        , TARGET_WIDTH_(SCREEN_WIDTH_ * 0.75f)
        , TARGET_RECT_(
              HALF_SCREEN_WIDTH_ - (TARGET_WIDTH_ * 0.5f),
              sfutil::MapByRes(35.0f, 100.0f),
              TARGET_WIDTH_,
              SCREEN_HEIGHT_ * 0.85f)
        , SLIDER_SPEED_(6.0f)
        , backgroundQuads_(sf::Quads, 4)
        , cachedTextureOpt_()
        , sprite_()
        , slider_(SLIDER_SPEED_, sfml_util::WillOscillate::No, sfml_util::WillAutoStart::No)
        , textRegionUPtr_()
        , sourceRect()
        , itemPtrOpt_()
        , willShowImage_(false)
        , isShowing_(false)
    {
        slider_.SetTo();
    }

    void ItemDetailViewer::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        // Always draw because it is a fast operation and will
        // be fully transparent when should not be drawn.
        target.draw(backgroundQuads_, states);

        if (willShowImage_)
        {
            target.draw(sprite_, states);
        }

        if (textRegionUPtr_)
        {
            target.draw(*textRegionUPtr_, states);
        }
    }

    void ItemDetailViewer::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        const auto SLIDER_POSITION_RATIO { slider_.UpdateAndReturnPositionRatio(
            ELAPSED_TIME_SECONDS) };

        if (SLIDER_POSITION_RATIO > 0.1f)
        {
            SetupBackgroundQuadColors(0.0f);
        }
        else
        {
            SetupBackgroundQuadColors(SLIDER_POSITION_RATIO);
        }

        SetupBackgroundQuadPositions(SLIDER_POSITION_RATIO);

        if (slider_.IsAtTo() && slider_.IsStopped())
        {
            SetupImage(itemPtrOpt_);
            SetupText(itemPtrOpt_);
            isShowing_ = true;
        }
    }

    void ItemDetailViewer::FadeIn(const item::ItemPtr_t ITEM_PTR, const sf::FloatRect & RECT)
    {
        if (slider_.IsMovingToward() == false)
        {
            sourceRect = RECT;
            itemPtrOpt_ = ITEM_PTR;

            slider_.ReverseDirection();
            slider_.Start();

            sfml_util::SoundManager::Instance()->PlaySfx_TickOn();
        }
    }

    void ItemDetailViewer::FadeOut()
    {
        if (slider_.IsMovingToward())
        {
            SetupImage(boost::none);
            SetupText(boost::none);

            isShowing_ = false;

            slider_.ReverseDirection();
            slider_.Start();

            sfml_util::SoundManager::Instance()->PlaySfx_TickOff();
        }
    }

    void ItemDetailViewer::SetupBackgroundQuadColors(const float PROGRESS_RATIO)
    {
        const auto BACKGROUND_COLOR { sf::Color(
            0, 0, 0, static_cast<sf::Uint8>(PROGRESS_RATIO * 255.0f)) };

        backgroundQuads_[0].color = BACKGROUND_COLOR;
        backgroundQuads_[1].color = BACKGROUND_COLOR;
        backgroundQuads_[2].color = BACKGROUND_COLOR;
        backgroundQuads_[3].color = BACKGROUND_COLOR;
    }

    void ItemDetailViewer::SetupBackgroundQuadPositions(const float RATIO)
    {
        const auto SOURCE_LEFT { sourceRect.left };
        const auto SOURCE_RIGHT { sourceRect.left + sourceRect.width };
        const auto SOURCE_TOP { sourceRect.top };
        const auto SOURCE_BOT { sourceRect.top + sourceRect.height };

        const auto TARGET_LEFT { TARGET_RECT_.left };
        const auto TARGET_RIGHT { TARGET_RECT_.left + TARGET_RECT_.width };
        const auto TARGET_TOP { TARGET_RECT_.top };
        const auto TARGET_BOT { TARGET_RECT_.top + TARGET_RECT_.height };

        const auto LEFT_MORE { SOURCE_LEFT - ((SOURCE_LEFT - TARGET_LEFT) * RATIO) };
        const auto LEFT_LESS { SOURCE_LEFT + ((TARGET_LEFT - SOURCE_LEFT) * RATIO) };
        const auto LEFT { ((SOURCE_LEFT > TARGET_LEFT) ? LEFT_MORE : LEFT_LESS) };

        const auto RIGHT_MORE { SOURCE_RIGHT - ((SOURCE_RIGHT - TARGET_RIGHT) * RATIO) };
        const auto RIGHT_LESS { SOURCE_RIGHT + ((TARGET_RIGHT - SOURCE_RIGHT) * RATIO) };
        const auto RIGHT { (SOURCE_RIGHT > TARGET_RIGHT) ? RIGHT_MORE : RIGHT_LESS };

        const auto TOP { SOURCE_TOP - ((SOURCE_TOP - TARGET_TOP) * RATIO) };

        const auto BOT_MORE { SOURCE_BOT - ((SOURCE_BOT - TARGET_BOT) * RATIO) };
        const auto BOT_LESS { SOURCE_BOT + ((TARGET_BOT - SOURCE_BOT) * RATIO) };
        const auto BOT { (SOURCE_BOT > TARGET_BOT) ? BOT_MORE : BOT_LESS };

        backgroundQuads_[0].position = sf::Vector2f(LEFT, TOP);
        backgroundQuads_[1].position = sf::Vector2f(RIGHT, TOP);
        backgroundQuads_[2].position = sf::Vector2f(RIGHT, BOT);
        backgroundQuads_[3].position = sf::Vector2f(LEFT, BOT);
    }

    void ItemDetailViewer::SetupImage(const item::ItemPtrOpt_t ITEM_PTR_OPT)
    {
        if (!ITEM_PTR_OPT)
        {
            willShowImage_ = false;
            return;
        }

        willShowImage_ = true;

        cachedTextureOpt_ = sfml_util::LoadAndCacheImage(ITEM_PTR_OPT.value());

        sprite_.setTexture(cachedTextureOpt_->Get(), true);

        // found by experiment to look good at various resolutions
        const float SCALE(sfutil::MapByRes(0.75f, 1.25f));
        sprite_.setScale(SCALE, SCALE);

        sprite_.setPosition(
            HALF_SCREEN_WIDTH_ - (sprite_.getGlobalBounds().width * 0.5f),
            TARGET_RECT_.left + INNER_SPACER_);
    }

    void ItemDetailViewer::SetupText(const item::ItemPtrOpt_t ITEM_PTR_OPT)
    {
        if (!ITEM_PTR_OPT)
        {
            textRegionUPtr_.reset();
            return;
        }

        const auto ITEM_PTR { ITEM_PTR_OPT.value() };

        std::ostringstream ss;
        ss << ITEM_PTR->Name() << "\n" << ITEM_PTR->Desc() << "\n\n";

        if (ITEM_PTR->Category() != item::category::None)
        {
            ss << item::category::ToString(ITEM_PTR->Category(), misc::Wrap::Yes) << "\n";
        }

        if (ITEM_PTR->ExclusiveRole() != creature::role::Count)
        {
            ss << "(can only be used by " << creature::role::ToString(ITEM_PTR->ExclusiveRole())
               << "s)\n";
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

        const sfml_util::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::GuiFont::Default,
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color::White,
            sfml_util::Justified::Center);

        const auto TEXT_LEFT { TARGET_RECT_.left + INNER_SPACER_ };

        const auto TEXT_TOP { sprite_.getGlobalBounds().top + sprite_.getGlobalBounds().height
                              + INNER_SPACER_ };

        const auto TEXT_WIDTH { TARGET_RECT_.width - DOUBLE_INNER_SPACER_ };

        const auto SPRITE_BOTTOM { sprite_.getPosition().y + sprite_.getGlobalBounds().height };

        const auto TEXT_HEIGHT { (TARGET_RECT_.height - SPRITE_BOTTOM) - DOUBLE_INNER_SPACER_ };

        const sf::FloatRect TEXT_RECT { TEXT_LEFT, TEXT_TOP, TEXT_WIDTH, TEXT_HEIGHT };

        textRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
            "ItemDetailViewer'sTextRegion", TEXT_INFO, TEXT_RECT);
    }

} // namespace stage
} // namespace heroespath
