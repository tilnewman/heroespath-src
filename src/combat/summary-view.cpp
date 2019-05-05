// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// summary-view.cpp
//
#include "summary-view.hpp"

#include "combat/combat-node.hpp"
#include "creature/creature.hpp"
#include "gui/font-manager.hpp"
#include "gui/image-loaders.hpp"
#include "gui/text-info.hpp"
#include "gui/text-region.hpp"
#include "item/item.hpp"
#include "misc/log-macros.hpp"
#include "misc/real.hpp"
#include "sfutil/display.hpp"
#include "sfutil/scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace combat
{

    const float ItemWithText::ITEM_IMAGE_SCALE_MIN_(0.25f);

    ItemWithText::ItemWithText(const item::ItemPtr_t ITEM_PTR)
        : item_ptr(ITEM_PTR)
        , cached_texture(gui::LoadAndCacheImage(ITEM_PTR))
        , sprite(cached_texture.Get())
        , name_text_region_uptr()
        , desc_text_region_uptr()
        , info_text_region_uptr()
    {
        const float ITEM_IMAGE_SCALE_DEFAULT(sfutil::MapByRes(ITEM_IMAGE_SCALE_MIN_, 1.0f));
        sprite.setScale(ITEM_IMAGE_SCALE_DEFAULT, ITEM_IMAGE_SCALE_DEFAULT);
        sprite.setColor(sf::Color(255, 255, 255, 64));
    }

    void ItemWithText::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        sf::RenderStates statesBlendMode(states);
        statesBlendMode.blendMode = sf::BlendAdd;
        target.draw(sprite, statesBlendMode);

        if (name_text_region_uptr)
        {
            name_text_region_uptr->draw(target, states);
        }

        if (desc_text_region_uptr)
        {
            desc_text_region_uptr->draw(target, states);
        }

        if (info_text_region_uptr)
        {
            info_text_region_uptr->draw(target, states);
        }
    }

    const float SummaryView::BACKGROUND_COLOR_ALPHA_(192.0f);
    const float SummaryView::SLIDER_SPEED_(4.0f);

    SummaryView::SummaryView()
        : BLOCK_POS_LEFT_(sfutil::MapByRes(5.0f, 40.0f))
        , BLOCK_POS_TOP_(sfutil::MapByRes(5.0f, 40.0f))
        , IMAGE_EDGE_PAD_(sfutil::MapByRes(5.0f, 20.0f))
        , IMAGE_BETWEEN_PAD_(sfutil::MapByRes(10.0f, 35.0f))
        , isTransToComplete_(false)
        , isTransBackComplete_(true)
        , movingDir_(gui::Moving::Still)
        , itemWithTextUVec_()
        , bgQuads_(sf::Quads, 4)
        , combatNodePtrOpt_()
        , nameTextRegionUPtr_()
        , rankTextRegionUPtr_()
        , healthTextRegionUPtr_()
        , descTextRegionUPtr_()
        , condTextRegionUPtr_()
        , armorTextRegionUPtr_()
        , geSlider_()
        , preventNextTrans_(false)
    {}

    void SummaryView::StartTransitionBack()
    {
        if (gui::Moving::Away == movingDir_)
        {
            return;
        }

        if ((gui::Moving::Still == movingDir_) && (false == isTransToComplete_))
        {
            return;
        }

        if (combatNodePtrOpt_)
        {
            const auto ENTITY_PTR_OPT { geSlider_.GetEntity() };
            const auto FROM { geSlider_.From() };
            const auto TO { geSlider_.To() };

            geSlider_ = gui::EntitySlider(
                ENTITY_PTR_OPT,
                TO,
                FROM,
                (SLIDER_SPEED_ * 2.0f),
                gui::WillOscillate::No,
                gui::WillAutoStart::Yes);

            movingDir_ = gui::Moving::Away;
            isTransToComplete_ = false;
            isTransBackComplete_ = false;
            combatNodePtrOpt_.value()->IsSummaryView(false);
            combatNodePtrOpt_.value()->IsMoving(true);
        }
    }

    void SummaryView::StartTransitionTo(
        const CombatNodePtr_t COMBAT_NODE_PTR,
        const sf::Vector2f & DEST_POS_V,
        const sf::FloatRect & ENEMYDISPLAY_RECT)
    {
        if (preventNextTrans_)
        {
            preventNextTrans_ = false;
            return;
        }

        combatNodePtrOpt_ = COMBAT_NODE_PTR;

        geSlider_ = gui::EntitySlider(
            gui::IEntityPtrOpt_t(COMBAT_NODE_PTR),
            COMBAT_NODE_PTR->GetEntityPos(),
            DEST_POS_V,
            SLIDER_SPEED_,
            gui::WillOscillate::No,
            gui::WillAutoStart::Yes);

        BackgroundColor(sf::Color::Transparent);
        BackgroundRegion(ENEMYDISPLAY_RECT);
        movingDir_ = gui::Moving::Toward;
        isTransToComplete_ = false;
        isTransBackComplete_ = false;
        COMBAT_NODE_PTR->IsMoving(true);
    }

    void SummaryView::BackgroundColor(const sf::Color & NEW_COLOR)
    {
        bgQuads_[0].color = NEW_COLOR;
        bgQuads_[1].color = NEW_COLOR;
        bgQuads_[2].color = NEW_COLOR;
        bgQuads_[3].color = NEW_COLOR;
    }

    const sf::Color SummaryView::BackgroundColor() const { return bgQuads_[0].color; }

    void SummaryView::BackgroundRegion(const sf::FloatRect & REGION)
    {
        bgQuads_[0].position = sf::Vector2f(REGION.left, REGION.top);
        bgQuads_[1].position = sf::Vector2f(REGION.left + REGION.width, REGION.top);
        bgQuads_[2].position = sf::Vector2f(REGION.left + REGION.width, REGION.top + REGION.height);
        bgQuads_[3].position = sf::Vector2f(REGION.left, REGION.top + REGION.height);
    }

    void SummaryView::SetTransitionToComplete()
    {
        isTransToComplete_ = true;
        isTransBackComplete_ = false;
        movingDir_ = gui::Moving::Still;

        if (combatNodePtrOpt_)
        {
            combatNodePtrOpt_.value()->IsSummaryView(true);
            combatNodePtrOpt_.value()->IsMoving(false);
        }
    }

    void SummaryView::SetTransitionBackComplete()
    {
        isTransToComplete_ = false;
        isTransBackComplete_ = true;
        movingDir_ = gui::Moving::Still;
        itemWithTextUVec_.clear();

        if (combatNodePtrOpt_)
        {
            combatNodePtrOpt_.value()->IsSummaryView(false);
            combatNodePtrOpt_.value()->IsMoving(false);
        }

        ReleaseCombatNodePointer();
    }

    void SummaryView::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        // always draw the background since it needs to be drawn during transitions
        // and is an inexpensive vertex array operation
        target.draw(bgQuads_, states);

        if (isTransToComplete_ && combatNodePtrOpt_)
        {
            nameTextRegionUPtr_->draw(target, states);
            rankTextRegionUPtr_->draw(target, states);
            healthTextRegionUPtr_->draw(target, states);
            condTextRegionUPtr_->draw(target, states);

            if (combatNodePtrOpt_.value()->Creature()->IsPlayerCharacter())
            {
                armorTextRegionUPtr_->draw(target, states);
            }
            else
            {
                descTextRegionUPtr_->draw(target, states);
            }

            for (const auto & ITEM_WITH_TEXT_UPTR : itemWithTextUVec_)
            {
                target.draw(*ITEM_WITH_TEXT_UPTR, states);
            }

            combatNodePtrOpt_.value()->draw(target, states);
        }
    }

    void SummaryView::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        const auto DID_SLIDER_STOP { geSlider_.UpdateAndReturnIsStopped(ELAPSED_TIME_SECONDS) };

        BackgroundColor(sf::Color(
            0, 0, 0, static_cast<sf::Uint8>(BACKGROUND_COLOR_ALPHA_ * geSlider_.PositionRatio())));

        if (DID_SLIDER_STOP)
        {
            if (gui::Moving::Toward == movingDir_)
            {
                SetTransitionToComplete();
            }
            else if (gui::Moving::Away == movingDir_)
            {
                SetTransitionBackComplete();
            }
        }
    }

    void SummaryView::SetupAndStartTransition(
        const CombatNodePtr_t combatNodePtr, const sf::FloatRect & COMBAT_REGION)
    {
        itemWithTextUVec_.clear();

        auto creaturePtr { combatNodePtr->Creature() };

        // name
        {
            std::string nameStr;
            if (creaturePtr->Name() != creaturePtr->RaceName())
            {
                nameStr += creaturePtr->Name() + "\n";
            }

            nameStr += creaturePtr->RaceName();

            if (creature::race::RaceRoleMatch(creaturePtr->Race(), creaturePtr->Role()) == false)
            {
                nameStr += ", " + creaturePtr->RoleName();
            }

            const gui::TextInfo CREATURE_NAME_TEXT_INFO(
                nameStr,
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::Light);

            nameTextRegionUPtr_
                = std::make_unique<gui::TextRegion>("SummaryView'sName", CREATURE_NAME_TEXT_INFO);
        }

        // rank
        {
            const std::string RANK_STR(
                "Rank " + creaturePtr->Rank().ToString() + " (" + creaturePtr->RankClassName()
                + ")");

            const gui::TextInfo CREATURE_RANK_TEXT_INFO(
                RANK_STR,
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::Light);

            rankTextRegionUPtr_
                = std::make_unique<gui::TextRegion>("SummaryView'sRank", CREATURE_RANK_TEXT_INFO);
        }

        // health
        {
            std::string healthStr("Health: ");
            if (creaturePtr->IsPlayerCharacter())
            {
                healthStr += creaturePtr->HealthCurrent().ToString() + "/"
                    + creaturePtr->HealthNormal().ToString();
            }
            else
            {
                healthStr += creaturePtr->HealthPercentStr();
            }

            const gui::TextInfo CREATURE_HEALTH_TEXT_INFO(
                healthStr,
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::Light);

            healthTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "SummaryView'sHealth", CREATURE_HEALTH_TEXT_INFO);
        }

        // armor
        {
            const std::string ARMOR_STR("Armor Rating: " + creaturePtr->ArmorRating().ToString());

            const gui::TextInfo CREATURE_ARMORRATING_TEXT_INFO(
                ARMOR_STR,
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::Light);

            armorTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "SummaryView'sArmorRating", CREATURE_ARMORRATING_TEXT_INFO);
        }

        // description
        {
            const gui::TextInfo CREATURE_DESC_TEXT_INFO(
                creaturePtr->Body().ToString(),
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::Light,
                gui::Justified::Left,
                sf::Text::Italic);

            descTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "SummaryView'sDesc", CREATURE_DESC_TEXT_INFO, sf::FloatRect());
        }

        // condition
        {
            const std::string COND_STR("Condition:  " + creaturePtr->ConditionNames(6));

            const gui::TextInfo CREATURE_CONDITIONS_TEXT_INFO(
                COND_STR,
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::Light);

            condTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "SummaryView'sCondition", CREATURE_CONDITIONS_TEXT_INFO);
        }

        const float IMAGE_POS_LEFT(COMBAT_REGION.left + BLOCK_POS_LEFT_ + IMAGE_EDGE_PAD_);
        const float IMAGE_POS_TOP(COMBAT_REGION.top + BLOCK_POS_TOP_ + IMAGE_EDGE_PAD_);
        const float IMAGE_WIDTH(combatNodePtr->GetEntityRegion().width);
        const float IMAGE_HEIGHT(
            combatNodePtr->GetEntityRegion().height + sfutil::MapByRes(20.0f, 60.0f));

        const float BETWEEN_TEXT_SPACER(0.0f);
        const float TOP_TEXT_SPACER(sfutil::MapByRes(10.0f, 25.0f));
        const float CREATURE_TEXT_POS_LEFT(IMAGE_POS_LEFT + IMAGE_WIDTH + IMAGE_EDGE_PAD_);

        if (creaturePtr->IsPlayerCharacter())
        {
            nameTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT, IMAGE_POS_TOP + TOP_TEXT_SPACER);

            rankTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT,
                nameTextRegionUPtr_->GetEntityRegion().top
                    + nameTextRegionUPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);

            healthTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT,
                rankTextRegionUPtr_->GetEntityRegion().top
                    + rankTextRegionUPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);

            armorTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT,
                healthTextRegionUPtr_->GetEntityRegion().top
                    + healthTextRegionUPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);

            condTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT,
                armorTextRegionUPtr_->GetEntityRegion().top
                    + armorTextRegionUPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);

            descTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT,
                condTextRegionUPtr_->GetEntityRegion().top
                    + condTextRegionUPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);
        }
        else
        {
            nameTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT, IMAGE_POS_TOP + TOP_TEXT_SPACER);

            rankTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT,
                nameTextRegionUPtr_->GetEntityRegion().top
                    + nameTextRegionUPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);

            healthTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT,
                rankTextRegionUPtr_->GetEntityRegion().top
                    + rankTextRegionUPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);

            condTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT,
                healthTextRegionUPtr_->GetEntityRegion().top
                    + healthTextRegionUPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);

            descTextRegionUPtr_->SetEntityPos(
                CREATURE_TEXT_POS_LEFT,
                condTextRegionUPtr_->GetEntityRegion().top
                    + condTextRegionUPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);
        }

        // Find the longest horizontal and vertical length so we know how big to make
        // the dark background, start by considering the enemy creature description.
        float enemyDetailsDisplayExtentHoriz(
            CREATURE_TEXT_POS_LEFT + descTextRegionUPtr_->GetEntityRegion().width);

        float enemyDetailsDisplayExtentVert(0.0f);

        // populate the enemy equipped item list with the following priority:
        // weapons first, then armor, then misc, then plain clothes
        const auto ITEMS_EQUIPPED_VEC { creaturePtr->Inventory().ItemsEquipped() };

        // first weapons
        item::ItemPVec_t weaponItemsToDisplay;
        weaponItemsToDisplay.reserve(ITEMS_EQUIPPED_VEC.size());

        item::ItemPVec_t weaponItemsToIgnore;
        weaponItemsToIgnore.reserve(ITEMS_EQUIPPED_VEC.size());

        for (const auto & NEXT_ITEM_PTR : ITEMS_EQUIPPED_VEC)
        {
            if ((NEXT_ITEM_PTR->IsWeapon()) && (NEXT_ITEM_PTR->IsBodypart() == false))
            {
                weaponItemsToDisplay.emplace_back(NEXT_ITEM_PTR);
            }
        }

        // only list bodypart weapons if there are not any others equipped
        if (weaponItemsToDisplay.empty())
        {
            for (const auto & NEXT_ITEM_PTR : ITEMS_EQUIPPED_VEC)
            {
                if ((NEXT_ITEM_PTR->IsWeapon()) && (NEXT_ITEM_PTR->IsBodypart()))
                {
                    weaponItemsToDisplay.emplace_back(NEXT_ITEM_PTR);
                }
            }
        }
        else
        {
            for (const auto & NEXT_ITEM_PTR : ITEMS_EQUIPPED_VEC)
            {
                if ((NEXT_ITEM_PTR->IsWeapon()) && (NEXT_ITEM_PTR->IsBodypart()))
                {
                    weaponItemsToIgnore.emplace_back(NEXT_ITEM_PTR);
                }
            }
        }

        auto makeAndppendItemWithText = [&](const item::ItemPtr_t & ITEM_PTR) {
            itemWithTextUVec_.emplace_back(std::make_unique<ItemWithText>(ITEM_PTR));
        };

        for (const auto & NEXT_ITEM_PTR : weaponItemsToDisplay)
        {
            makeAndppendItemWithText(NEXT_ITEM_PTR);
        }

        // then armor
        for (const auto & NEXT_ITEM_PTR : ITEMS_EQUIPPED_VEC)
        {
            if (NEXT_ITEM_PTR->IsArmor())
            {
                makeAndppendItemWithText(NEXT_ITEM_PTR);
            }
        }

        // then misc
        for (const auto & NEXT_ITEM_PTR : ITEMS_EQUIPPED_VEC)
        {
            if (NEXT_ITEM_PTR->MiscType() != item::misc_type::Not)
            {
                makeAndppendItemWithText(NEXT_ITEM_PTR);
            }
        }

        // then everything else (clothes, etc) if not already added or ignored
        for (const auto & NEXT_ITEM_PTR : ITEMS_EQUIPPED_VEC)
        {
            const auto ALREADY_ADDED { std::any_of(
                std::cbegin(itemWithTextUVec_),
                std::cend(itemWithTextUVec_),
                [NEXT_ITEM_PTR](const auto & UPTR) { return (UPTR->item_ptr == NEXT_ITEM_PTR); }) };

            if (ALREADY_ADDED == false)
            {
                const auto WILL_IGNORE { std::find(
                                             std::cbegin(weaponItemsToIgnore),
                                             std::cend(weaponItemsToIgnore),
                                             NEXT_ITEM_PTR)
                                         != std::cend(weaponItemsToIgnore) };

                if (WILL_IGNORE == false)
                {
                    makeAndppendItemWithText(NEXT_ITEM_PTR);
                }
            }
        }

        const float ITEM_IMAGE_HORIZ_MARGIN(sfutil::MapByRes(45.0f, 150.0f));
        const float ITEM_IMAGE_POS_LEFT(IMAGE_POS_LEFT + ITEM_IMAGE_HORIZ_MARGIN);
        const float ITEM_IMAGE_POS_TOP_START_MARGIN(sfutil::MapByRes(25.0f, 50.0f));

        const float ITEM_IMAGE_POS_TOP_START(
            IMAGE_POS_TOP + IMAGE_HEIGHT + ITEM_IMAGE_POS_TOP_START_MARGIN);

        const float VERT_SPACE_FOR_ITEMLIST(COMBAT_REGION.height - ITEM_IMAGE_POS_TOP_START);

        // find total item list height with default scale per item image
        float itemListHeightAtDefaultScale(0.0f);
        for (const auto & ITEM_WITH_TEXT_UPTR : itemWithTextUVec_)
        {
            itemListHeightAtDefaultScale
                += ITEM_WITH_TEXT_UPTR->sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_;
        }

        // if too tall to fit, then try using the minimum scale
        if (itemListHeightAtDefaultScale > VERT_SPACE_FOR_ITEMLIST)
        {
            float itemListHeightAtMinScale(0.0f);
            for (auto & nextItemTextUPtr : itemWithTextUVec_)
            {
                nextItemTextUPtr->sprite.setScale(
                    ItemWithText::ITEM_IMAGE_SCALE_MIN_, ItemWithText::ITEM_IMAGE_SCALE_MIN_);

                itemListHeightAtMinScale
                    += nextItemTextUPtr->sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_;
            }

            // if the item list is still too long/tall to fit, then only display a subset of
            // the items in the priority above
            if (itemListHeightAtMinScale > VERT_SPACE_FOR_ITEMLIST)
            {
                float height(0.0f);
                auto iter(std::cbegin(itemWithTextUVec_));
                for (; iter != std::cend(itemWithTextUVec_); ++iter)
                {
                    height += ((*iter)->sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_);

                    if (height > VERT_SPACE_FOR_ITEMLIST)
                    {
                        break;
                    }
                }

                itemWithTextUVec_.erase(iter, std::end(itemWithTextUVec_));
            }
        }

        // at this point, itemWithTextUVec_ contains a list of equipped item images that
        // will fit vertically in the combat window

        // set the item list image alpha values and positions, populate itemWithTextUVec_
        // with text, and find the total width and height of the list
        float longestItemHorizExtent(0.0f);
        float itemListHeight(0.0f);
        for (auto & nextItemTextUPtr : itemWithTextUVec_)
        {
            nextItemTextUPtr->sprite.setPosition(
                ITEM_IMAGE_POS_LEFT, ITEM_IMAGE_POS_TOP_START + itemListHeight);

            const auto NAME { nextItemTextUPtr->item_ptr->Name() };
            const gui::TextInfo ITEM_NAME_TEXT_INFO(
                NAME,
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::Light,
                gui::Justified::Left);

            const sf::FloatRect ITEM_NAME_RECT(
                ITEM_IMAGE_POS_LEFT + nextItemTextUPtr->sprite.getGlobalBounds().width
                    + IMAGE_EDGE_PAD_,
                ITEM_IMAGE_POS_TOP_START + itemListHeight,
                0.0f,
                0.0f);

            nextItemTextUPtr->name_text_region_uptr = std::make_unique<gui::TextRegion>(
                "CombatDisplay_EnemyDetails_ItemList_ItemName_" + NAME,
                ITEM_NAME_TEXT_INFO,
                ITEM_NAME_RECT);

            const gui::TextInfo ITEM_DESC_TEXT_INFO(
                nextItemTextUPtr->item_ptr->Desc(),
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::Light,
                gui::Justified::Left);

            const sf::FloatRect ITEM_DESC_RECT(
                ITEM_IMAGE_POS_LEFT + nextItemTextUPtr->sprite.getGlobalBounds().width
                    + IMAGE_EDGE_PAD_,
                ITEM_NAME_RECT.top
                    + nextItemTextUPtr->name_text_region_uptr->GetEntityRegion().height,
                0.0f,
                0.0f);

            nextItemTextUPtr->desc_text_region_uptr = std::make_unique<gui::TextRegion>(
                "CombatDisplay_EnemyDetails_ItemList_ItemDesc_" + NAME,
                ITEM_DESC_TEXT_INFO,
                ITEM_DESC_RECT);

            std::string infoStr;

            if (nextItemTextUPtr->item_ptr->IsQuestItem())
            {
                infoStr += "(Quest Item)";
            }

            if (nextItemTextUPtr->item_ptr->IsWeapon())
            {
                infoStr += std::string((infoStr.empty()) ? "" : ", ")
                    + "Damage: " + nextItemTextUPtr->item_ptr->DamageMin().ToString() + "-"
                    + nextItemTextUPtr->item_ptr->DamageMax().ToString();
            }
            else if (
                nextItemTextUPtr->item_ptr->IsArmor()
                || (nextItemTextUPtr->item_ptr->ArmorRating() > 0_armor))
            {
                infoStr += std::string((infoStr.empty()) ? "" : ", ")
                    + "Armor Rating: " + nextItemTextUPtr->item_ptr->ArmorRating().ToString();
            }

            if (infoStr.empty())
            {
                infoStr += " ";
            }

            const gui::TextInfo INFO_TEXT_INFO(
                infoStr,
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::Light,
                gui::Justified::Left);

            const sf::FloatRect INFO_RECT(
                ITEM_IMAGE_POS_LEFT + nextItemTextUPtr->sprite.getGlobalBounds().width
                    + IMAGE_EDGE_PAD_,
                ITEM_DESC_RECT.top
                    + nextItemTextUPtr->desc_text_region_uptr->GetEntityRegion().height,
                0.0f,
                0.0f);

            nextItemTextUPtr->info_text_region_uptr = std::make_unique<gui::TextRegion>(
                "CombatDisplay_EnemyDetails_ItemList_ItemInfo_" + NAME, INFO_TEXT_INFO, INFO_RECT);

            const float CURR_ITEM_HORIZ_EXTENT(
                ITEM_IMAGE_POS_LEFT + nextItemTextUPtr->sprite.getGlobalBounds().width
                + IMAGE_EDGE_PAD_
                + nextItemTextUPtr->desc_text_region_uptr->GetEntityRegion().width);

            if (longestItemHorizExtent < CURR_ITEM_HORIZ_EXTENT)
            {
                longestItemHorizExtent = CURR_ITEM_HORIZ_EXTENT;
            }

            const float CURR_ITEM_VERT_TEXT_EXTENT { (
                (infoStr == " ")
                    ? (ITEM_DESC_RECT.top
                       + nextItemTextUPtr->desc_text_region_uptr->GetEntityRegion().height)
                        - (ITEM_IMAGE_POS_TOP_START + itemListHeight)
                    : (INFO_RECT.top
                       + nextItemTextUPtr->info_text_region_uptr->GetEntityRegion().height)
                        - (ITEM_IMAGE_POS_TOP_START + itemListHeight)) };

            if (CURR_ITEM_VERT_TEXT_EXTENT < nextItemTextUPtr->sprite.getGlobalBounds().height)
            {
                itemListHeight
                    += nextItemTextUPtr->sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_;
            }
            else
            {
                itemListHeight += CURR_ITEM_VERT_TEXT_EXTENT + IMAGE_BETWEEN_PAD_;
                nextItemTextUPtr->sprite.move(
                    0.0f,
                    (CURR_ITEM_VERT_TEXT_EXTENT - nextItemTextUPtr->sprite.getGlobalBounds().height)
                        * 0.5f);
            }
        }

        // establish enemy details display extents
        if (enemyDetailsDisplayExtentHoriz < longestItemHorizExtent)
        {
            enemyDetailsDisplayExtentHoriz = longestItemHorizExtent;
        }

        enemyDetailsDisplayExtentVert = (ITEM_IMAGE_POS_TOP_START + itemListHeight);

        // final setup call
        const sf::Vector2f IMAGE_DEST_POS_V(IMAGE_POS_LEFT, IMAGE_POS_TOP);

        const sf::FloatRect SUMMARYVIEW_DISPLAY_REGION(
            COMBAT_REGION.left,
            COMBAT_REGION.top,
            (enemyDetailsDisplayExtentHoriz - COMBAT_REGION.left) + IMAGE_BETWEEN_PAD_,
            (enemyDetailsDisplayExtentVert - COMBAT_REGION.top) + IMAGE_BETWEEN_PAD_);

        StartTransitionTo(combatNodePtr, IMAGE_DEST_POS_V, SUMMARYVIEW_DISPLAY_REGION);
    }

} // namespace combat
} // namespace heroespath
