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
// summary-view.cpp
//
#include "summary-view.hpp"

#include "sfml-util/sfml-util.hpp"
#include "misc/real.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "game/log-macros.hpp"
#include "game/creature/creature.hpp"
#include "game/combat/combat-node.hpp"


namespace game
{
namespace combat
{

    ItemWithText::ItemWithText(const item::ItemSPtr_t & ITEM_SPTR)
    :
        sprite               (),
        texture              (),
        name_text_region_sptr(),
        desc_text_region_sptr(),
        info_text_region_sptr(),
        item_sptr            (ITEM_SPTR)
    {}


    const float SummaryView::BACKGROUND_COLOR_ALPHA_(192.0f);
    const float SummaryView::SLIDER_SPEED_(4.0f);


    SummaryView::SummaryView()
    :
        BLOCK_POS_LEFT_      (sfml_util::MapByRes(5.0f, 40.0f)),
        BLOCK_POS_TOP_       (sfml_util::MapByRes(5.0f, 40.0f)),
        IMAGE_EDGE_PAD_      (sfml_util::MapByRes(5.0f, 20.0f)),
        IMAGE_BETWEEN_PAD_   (sfml_util::MapByRes(10.0f, 35.0f)),
        IMAGE_COLOR_ALPHA_   (64),
        isTransToComplete_   (false),
        isTransBackComplete_ (true),
        movingDir_           (sfml_util::Moving::Still),
        itemWithTextVec_     (),
        bgQuads_             (sf::Quads, 4),
        combatNodePtr_       (),
        nameTextRegionSPtr_  (),
        healthTextRegionSPtr_(),
        descTextRegionSPtr_  (),
        condTextRegionSPtr_  (),
        armorTextRegionSPtr_ (),
        slider_              (),
        preventNextTrans_    (false)
    {}


    void SummaryView::StartTransitionBack()
    {
        if (sfml_util::Moving::Away == movingDir_)
            return;

        if ((sfml_util::Moving::Still == movingDir_) && (false == isTransToComplete_))
            return;

        if (combatNodePtr_ != nullptr)
        {
            slider_.Speed(SLIDER_SPEED_ * 2.0f);
            slider_.StartMovingAway();
            movingDir_ = sfml_util::Moving::Away;
            isTransToComplete_ = false;
            isTransBackComplete_ = false;
            combatNodePtr_->IsSummaryView(false);
            combatNodePtr_->IsMoving(true);
        }
    }


    void SummaryView::StartTransitionTo(CombatNodePtr_t       combatNodePtr,
                                        const sf::Vector2f &  DEST_POS_V,
                                        const sf::FloatRect & ENEMYDISPLAY_RECT)
    {
        if (preventNextTrans_)
        {
            preventNextTrans_ = false;
            return;
        }

        combatNodePtr_ = combatNodePtr;
        slider_.Setup(combatNodePtr, combatNodePtr->GetEntityPos(), DEST_POS_V, SLIDER_SPEED_);
        slider_.StartMovingToward();
        BackgroundColor(sf::Color::Transparent);
        BackgroundRegion(ENEMYDISPLAY_RECT);
        movingDir_ = sfml_util::Moving::Toward;
        isTransToComplete_ = false;
        isTransBackComplete_ = false;
        combatNodePtr->IsMoving(true);
    }


    void SummaryView::BackgroundColor(const sf::Color & NEW_COLOR)
    {
        bgQuads_[0].color = NEW_COLOR;
        bgQuads_[1].color = NEW_COLOR;
        bgQuads_[2].color = NEW_COLOR;
        bgQuads_[3].color = NEW_COLOR;
    }


    const sf::Color SummaryView::BackgroundColor() const
    {
        return bgQuads_[0].color;
    }


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
        movingDir_ = sfml_util::Moving::Still;
        if (combatNodePtr_ != nullptr)
        {
            combatNodePtr_->IsSummaryView(true);
            combatNodePtr_->IsMoving(false);
        }
    }


    void SummaryView::SetTransitionBackComplete()
    {
        isTransToComplete_ = false;
        isTransBackComplete_ = true;
        movingDir_ = sfml_util::Moving::Still;
        itemWithTextVec_.clear();
        if (combatNodePtr_ != nullptr)
        {
            combatNodePtr_->IsSummaryView(false);
            combatNodePtr_->IsMoving(false);
        }
        ReleaseCombatNodePointer();
    }


    void SummaryView::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        //always draw the background since it needs to be drawn during transitions and is an inexpensive vertex array operation
        target.draw(bgQuads_, states);

        if (isTransToComplete_ && (combatNodePtr_ != nullptr))
        {
            nameTextRegionSPtr_->draw(target, states);
            healthTextRegionSPtr_->draw(target, states);
            condTextRegionSPtr_->draw(target, states);

            if (combatNodePtr_->Creature()->IsPlayerCharacter())
            {
                armorTextRegionSPtr_->draw(target, states);
            }
            else
            {
                descTextRegionSPtr_->draw(target, states);
            }

            sf::RenderStates statesBlendMode(states);
            statesBlendMode.blendMode = sf::BlendAdd;

            for (auto const & NEXT_ITEM_WITH_TEXT : itemWithTextVec_)
            {
                target.draw(NEXT_ITEM_WITH_TEXT.sprite, statesBlendMode);
                NEXT_ITEM_WITH_TEXT.name_text_region_sptr->draw(target, states);
                NEXT_ITEM_WITH_TEXT.desc_text_region_sptr->draw(target, states);
                NEXT_ITEM_WITH_TEXT.info_text_region_sptr->draw(target, states);
            }

            combatNodePtr_->draw(target, states);
        }
    }


    void SummaryView::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (sfml_util::Moving::Away == movingDir_)
        {
            if (slider_.UpdateTime(ELAPSED_TIME_SECONDS))
                BackgroundColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(BACKGROUND_COLOR_ALPHA_ * (1.0f - slider_.CurrentAverage()))));
            else
                SetTransitionBackComplete();
        }
        else if (sfml_util::Moving::Toward ==  movingDir_)
        {
            if (slider_.UpdateTime(ELAPSED_TIME_SECONDS))
                BackgroundColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(BACKGROUND_COLOR_ALPHA_ * slider_.CurrentAverage())));
            else
                SetTransitionToComplete();
        }
    }


    void SummaryView::SetupAndStartTransition(CombatNodePtr_t combatNodePtr, const sf::FloatRect & COMBAT_REGION)
    {
        itemWithTextVec_.clear();

        std::ostringstream ss;
        if (combatNodePtr->Creature()->Name() != combatNodePtr->Creature()->Race().Name())
            ss << combatNodePtr->Creature()->Name() << "\n";

        ss << combatNodePtr->Creature()->Race().Name();

        if (combatNodePtr->Creature()->Role().Which() != creature::role::Wolfen)
            ss << ", " + combatNodePtr->Creature()->Role().Name();

        const sfml_util::gui::TextInfo CREATURE_NAME_TEXT_INFO(ss.str(),
                                                               sfml_util::FontManager::Instance()->Font_Default1(),
                                                               sfml_util::FontManager::Instance()->Size_Small(),
                                                               sfml_util::FontManager::Color_Light());

        nameTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sName", CREATURE_NAME_TEXT_INFO, sf::FloatRect()) );

        std::ostringstream healthSS;
        healthSS << "Health: ";
        if (combatNodePtr->Creature()->IsPlayerCharacter())
        {
            healthSS << combatNodePtr->Creature()->HealthCurrent() << "/" << combatNodePtr->Creature()->HealthNormal();
        }
        else
        {
            healthSS << combatNodePtr->Creature()->HealthPercentStr();
        }
        const sfml_util::gui::TextInfo CREATURE_HEALTH_TEXT_INFO(healthSS.str(),
                                                                 sfml_util::FontManager::Instance()->Font_Default1(),
                                                                 sfml_util::FontManager::Instance()->Size_Small(),
                                                                 sfml_util::FontManager::Color_Light());
        healthTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sHealth", CREATURE_HEALTH_TEXT_INFO, sf::FloatRect()) );

        std::ostringstream armorRatingSS;
        armorRatingSS << "Armor Rating: " << combatNodePtr->Creature()->ArmorRating();
        const sfml_util::gui::TextInfo CREATURE_ARMORRATING_TEXT_INFO(armorRatingSS.str(),
                                                                      sfml_util::FontManager::Instance()->Font_Default1(),
                                                                      sfml_util::FontManager::Instance()->Size_Small(),
                                                                      sfml_util::FontManager::Color_Light());
        armorTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sArmorRating", CREATURE_ARMORRATING_TEXT_INFO, sf::FloatRect()) );

        const sfml_util::gui::TextInfo CREATURE_DESC_TEXT_INFO(combatNodePtr->Creature()->Body().ToString(),
                                                               sfml_util::FontManager::Instance()->Font_Default1(),
                                                               sfml_util::FontManager::Instance()->Size_Small(),
                                                               sfml_util::FontManager::Color_Light(),
                                                               sf::BlendAlpha,
                                                               sf::Text::Italic);
        descTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sDesc", CREATURE_DESC_TEXT_INFO, sf::FloatRect()) );

        std::ostringstream condSS;
        condSS << "Condition:  " << combatNodePtr->Creature()->ConditionNames(6);
        const sfml_util::gui::TextInfo CREATURE_CONDITIONS_TEXT_INFO(condSS.str(),
                                                                     sfml_util::FontManager::Instance()->Font_Default1(),
                                                                     sfml_util::FontManager::Instance()->Size_Small(),
                                                                     sfml_util::FontManager::Color_Light());
        condTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sCondition", CREATURE_CONDITIONS_TEXT_INFO, sf::FloatRect()) );

        const float IMAGE_POS_LEFT(COMBAT_REGION.left + BLOCK_POS_LEFT_ + IMAGE_EDGE_PAD_);
        const float IMAGE_POS_TOP(COMBAT_REGION.top + BLOCK_POS_TOP_ + IMAGE_EDGE_PAD_);
        const float IMAGE_WIDTH(combatNodePtr->GetEntityRegion().width);
        const float IMAGE_HEIGHT(combatNodePtr->GetEntityRegion().height + sfml_util::MapByRes(20.0f, 60.0f));

        const float BETWEEN_TEXT_SPACER(0.0f);
        const float TOP_TEXT_SPACER(sfml_util::MapByRes(10.0f, 25.0f));
        const float CREATURE_TEXT_POS_LEFT(IMAGE_POS_LEFT + IMAGE_WIDTH + IMAGE_EDGE_PAD_);

        if (combatNodePtr->Creature()->IsPlayerCharacter())
        {
            nameTextRegionSPtr_->SetEntityPos(CREATURE_TEXT_POS_LEFT, IMAGE_POS_TOP + TOP_TEXT_SPACER);
            healthTextRegionSPtr_->SetEntityPos(CREATURE_TEXT_POS_LEFT, nameTextRegionSPtr_->GetEntityRegion().top + nameTextRegionSPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);
            armorTextRegionSPtr_->SetEntityPos(CREATURE_TEXT_POS_LEFT, healthTextRegionSPtr_->GetEntityRegion().top + healthTextRegionSPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);
            condTextRegionSPtr_->SetEntityPos(CREATURE_TEXT_POS_LEFT, armorTextRegionSPtr_->GetEntityRegion().top + armorTextRegionSPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);
            descTextRegionSPtr_->SetEntityPos(CREATURE_TEXT_POS_LEFT, condTextRegionSPtr_->GetEntityRegion().top + condTextRegionSPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);
        }
        else
        {
            nameTextRegionSPtr_->SetEntityPos(CREATURE_TEXT_POS_LEFT, IMAGE_POS_TOP + TOP_TEXT_SPACER);
            healthTextRegionSPtr_->SetEntityPos(CREATURE_TEXT_POS_LEFT, nameTextRegionSPtr_->GetEntityRegion().top + nameTextRegionSPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);
            condTextRegionSPtr_->SetEntityPos(CREATURE_TEXT_POS_LEFT, healthTextRegionSPtr_->GetEntityRegion().top + healthTextRegionSPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);
            descTextRegionSPtr_->SetEntityPos(CREATURE_TEXT_POS_LEFT, condTextRegionSPtr_->GetEntityRegion().top + condTextRegionSPtr_->GetEntityRegion().height + BETWEEN_TEXT_SPACER);
        }

        //find the longest horizontal and vertical length so we know how big to make the dark background, start by considering the enemy creature description
        float enemyDetailsDisplayExtentHoriz(CREATURE_TEXT_POS_LEFT + descTextRegionSPtr_->GetEntityRegion().width);
        float enemyDetailsDisplayExtentVert(0.0f);

        //populate the enemy equipped item list with the following priority: weapons first, then armor, then misc, then plain clothes

        //first weapons
        item::ItemSVec_t weaponItemsToDisplay;
        item::ItemSVec_t weaponItemsToIgnore;
        for (auto const & NEXT_ITEM_SPTR : combatNodePtr->Creature()->Inventory().ItemsEquipped())
            if ((NEXT_ITEM_SPTR->IsWeapon()) && (NEXT_ITEM_SPTR->IsBodypart() == false))
                weaponItemsToDisplay.push_back(NEXT_ITEM_SPTR);

        //only list bodypart weapons if there are not any others equipped
        if (weaponItemsToDisplay.empty())
        {
            for (auto const & NEXT_ITEM_SPTR : combatNodePtr->Creature()->Inventory().ItemsEquipped())
                if ((NEXT_ITEM_SPTR->IsWeapon()) && (NEXT_ITEM_SPTR->IsBodypart()))
                    weaponItemsToDisplay.push_back(NEXT_ITEM_SPTR);
        }
        else
        {
            for (auto const & NEXT_ITEM_SPTR : combatNodePtr->Creature()->Inventory().ItemsEquipped())
                if ((NEXT_ITEM_SPTR->IsWeapon()) && (NEXT_ITEM_SPTR->IsBodypart()))
                    weaponItemsToIgnore.push_back(NEXT_ITEM_SPTR);
        }

        for(auto const & NEXT_ITEM_SPTR : weaponItemsToDisplay)
            itemWithTextVec_.push_back(ItemWithText(NEXT_ITEM_SPTR));

        //then armor
        for (auto const & NEXT_ITEM_SPTR : combatNodePtr->Creature()->Inventory().ItemsEquipped())
            if (NEXT_ITEM_SPTR->IsArmor())
                itemWithTextVec_.push_back(ItemWithText(NEXT_ITEM_SPTR));

        //then misc
        for (auto const & NEXT_ITEM_SPTR : combatNodePtr->Creature()->Inventory().ItemsEquipped())
            if (NEXT_ITEM_SPTR->MiscType() != item::misc_type::NotMisc)
                itemWithTextVec_.push_back(ItemWithText(NEXT_ITEM_SPTR));

        //then everything else (clothes, etc)
        for (auto const & NEXT_ITEM_SPTR : combatNodePtr->Creature()->Inventory().ItemsEquipped())
        {
            const ItemWithTextVecCIter_t IWTV_CITER(std::find_if(itemWithTextVec_.begin(),
                                                                 itemWithTextVec_.end(),
                                                                 [&NEXT_ITEM_SPTR] (const ItemWithText & IWT) { return IWT.item_sptr == NEXT_ITEM_SPTR; }));

            const item::ItemSVecCIter_t IV_CITER(std::find_if(weaponItemsToIgnore.begin(),
                                                              weaponItemsToIgnore.end(),
                                                              [&NEXT_ITEM_SPTR] (const item::ItemSPtr_t & ITEM_SPTR_FROM_TOIGNORE) { return ITEM_SPTR_FROM_TOIGNORE == NEXT_ITEM_SPTR; }));

            if ((IV_CITER == weaponItemsToIgnore.end()) && (IWTV_CITER == itemWithTextVec_.end()))
                itemWithTextVec_.push_back(ItemWithText(NEXT_ITEM_SPTR));
        }

        const float ITEM_IMAGE_HORIZ_MARGIN(sfml_util::MapByRes(45.0f, 150.0f));
        const float ITEM_IMAGE_POS_LEFT(IMAGE_POS_LEFT + ITEM_IMAGE_HORIZ_MARGIN);
        const float ITEM_IMAGE_POS_TOP_START_MARGIN(sfml_util::MapByRes(25.0f, 50.0f));
        const float ITEM_IMAGE_POS_TOP_START(IMAGE_POS_TOP + IMAGE_HEIGHT + ITEM_IMAGE_POS_TOP_START_MARGIN);
        const float VERT_SPACE_FOR_ITEMLIST(COMBAT_REGION.height - ITEM_IMAGE_POS_TOP_START);

        const float ITEM_IMAGE_SCALE_MIN(0.25f);
        const float ITEM_IMAGE_SCALE_DEFAULT(sfml_util::MapByRes(ITEM_IMAGE_SCALE_MIN, 1.0f));

        //find total item list height with default scale per item image
        float itemListHeightAtDefaultScale(0.0f);
        for (auto & nextItemText : itemWithTextVec_)
        {
            sfml_util::gui::ItemImageManager::Instance()->Load(nextItemText.texture, nextItemText.item_sptr);
            nextItemText.sprite = sf::Sprite(nextItemText.texture);
            nextItemText.sprite.setScale(ITEM_IMAGE_SCALE_DEFAULT, ITEM_IMAGE_SCALE_DEFAULT);
            itemListHeightAtDefaultScale += nextItemText.sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_;
        }

        //if too tall to fit, then try using the minimum scale
        if (itemListHeightAtDefaultScale > VERT_SPACE_FOR_ITEMLIST)
        {
            float itemListHeightAtMinScale(0.0f);
            for (auto & nextItemText : itemWithTextVec_)
            {
                nextItemText.sprite.setScale(ITEM_IMAGE_SCALE_MIN, ITEM_IMAGE_SCALE_MIN);
                itemListHeightAtMinScale += nextItemText.sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_;
            }

            //if the item list is still too long/tall to fit, then only display a subset of the items in the priority above
            if (itemListHeightAtMinScale > VERT_SPACE_FOR_ITEMLIST)
            {
                ItemWithTextVec_t itemWithTextVec_Copy(itemWithTextVec_);
                itemWithTextVec_.clear();
                float height(0.0f);
                for (auto const & NEXT_ITEM_TEXT : itemWithTextVec_Copy)
                {
                    if ((height + NEXT_ITEM_TEXT.sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_) < VERT_SPACE_FOR_ITEMLIST)
                    {
                        height += (NEXT_ITEM_TEXT.sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_);
                        itemWithTextVec_.push_back(NEXT_ITEM_TEXT);
                    }
                    else
                        break;
                }
            }
        }

        //at this point, itemWithTextVec_ contains a list of equipped item images that will fit vertically in the combat window

        //set the item list image alpha values and positions, populate itemWithTextVec_ with text, and find the total width and height of the list
        float longestItemHorizExtent(0.0f);
        float itemListHeight(0.0f);
        for (auto & nextItemText : itemWithTextVec_)
        {
            nextItemText.sprite.setColor(sf::Color(255, 255, 255, IMAGE_COLOR_ALPHA_));

            nextItemText.sprite.setPosition(ITEM_IMAGE_POS_LEFT, ITEM_IMAGE_POS_TOP_START + itemListHeight);

            const sfml_util::gui::TextInfo ITEM_NAME_TEXT_INFO(nextItemText.item_sptr->Name(),
                                                               sfml_util::FontManager::Instance()->Font_Default1(),
                                                               sfml_util::FontManager::Instance()->Size_Small(),
                                                               sfml_util::FontManager::Color_Light(),
                                                               sfml_util::Justified::Left);

            const sf::FloatRect ITEM_NAME_RECT(ITEM_IMAGE_POS_LEFT + nextItemText.sprite.getGlobalBounds().width + IMAGE_EDGE_PAD_,
                                               ITEM_IMAGE_POS_TOP_START + itemListHeight,
                                               0.0f,
                                               0.0f);

            nextItemText.name_text_region_sptr.reset(new sfml_util::gui::TextRegion("CombatDisplay_EnemyDetails_ItemList_ItemName_" + nextItemText.item_sptr->Name(), ITEM_NAME_TEXT_INFO, ITEM_NAME_RECT));

            const sfml_util::gui::TextInfo ITEM_DESC_TEXT_INFO(nextItemText.item_sptr->Desc(),
                                                               sfml_util::FontManager::Instance()->Font_Default1(),
                                                               sfml_util::FontManager::Instance()->Size_Small(),
                                                               sfml_util::FontManager::Color_Light(),
                                                               sfml_util::Justified::Left);

            const sf::FloatRect ITEM_DESC_RECT(ITEM_IMAGE_POS_LEFT + nextItemText.sprite.getGlobalBounds().width + IMAGE_EDGE_PAD_,
                                               ITEM_NAME_RECT.top + nextItemText.name_text_region_sptr->GetEntityRegion().height,
                                               0.0f,
                                               0.0f);

            nextItemText.desc_text_region_sptr.reset(new sfml_util::gui::TextRegion("CombatDisplay_EnemyDetails_ItemList_ItemDesc_" + nextItemText.item_sptr->Name(), ITEM_DESC_TEXT_INFO, ITEM_DESC_RECT));

            std::ostringstream infoSS;
            if (nextItemText.item_sptr->IsMagical())
            {
                infoSS << "(Magical)";
            }
            if (nextItemText.item_sptr->Category() & item::category::QuestItem)
            {
                infoSS << ((infoSS.str().empty()) ? "" : ", ") << "(Quest Item)";
            }
            if (nextItemText.item_sptr->IsWeapon())
            {
                infoSS << ((infoSS.str().empty()) ? "" : ", ") << "Damage: " << nextItemText.item_sptr->DamageMin() << "-" << nextItemText.item_sptr->DamageMax();
            }
            else if (nextItemText.item_sptr->IsArmor() || (nextItemText.item_sptr->ArmorRating() > 0))
            {
                infoSS << ((infoSS.str().empty()) ? "" : ", ") << "Armor Rating: " << nextItemText.item_sptr->ArmorRating();
            }
            if (infoSS.str().empty())
            {
                infoSS << " ";
            }
            const sfml_util::gui::TextInfo INFO_TEXT_INFO(infoSS.str(),
                                                          sfml_util::FontManager::Instance()->Font_Default1(),
                                                          sfml_util::FontManager::Instance()->Size_Small(),
                                                          sfml_util::FontManager::Color_Light(),
                                                          sfml_util::Justified::Left);

            const sf::FloatRect INFO_RECT(ITEM_IMAGE_POS_LEFT + nextItemText.sprite.getGlobalBounds().width + IMAGE_EDGE_PAD_,
                                          ITEM_DESC_RECT.top + nextItemText.desc_text_region_sptr->GetEntityRegion().height,
                                          0.0f,
                                          0.0f);

            nextItemText.info_text_region_sptr.reset( new sfml_util::gui::TextRegion("CombatDisplay_EnemyDetails_ItemList_ItemInfo_" + nextItemText.item_sptr->Name(), INFO_TEXT_INFO, INFO_RECT) );

            const float CURR_ITEM_HORIZ_EXTENT(ITEM_IMAGE_POS_LEFT + nextItemText.sprite.getGlobalBounds().width + IMAGE_EDGE_PAD_ + nextItemText.desc_text_region_sptr->GetEntityRegion().width);
            if (longestItemHorizExtent < CURR_ITEM_HORIZ_EXTENT)
                longestItemHorizExtent = CURR_ITEM_HORIZ_EXTENT;

            const float CURR_ITEM_VERT_TEXT_EXTENT{ ((infoSS.str() == " ") ? (ITEM_DESC_RECT.top + nextItemText.desc_text_region_sptr->GetEntityRegion().height) - (ITEM_IMAGE_POS_TOP_START + itemListHeight) : (INFO_RECT.top + nextItemText.info_text_region_sptr->GetEntityRegion().height) - (ITEM_IMAGE_POS_TOP_START + itemListHeight)) };

            if (CURR_ITEM_VERT_TEXT_EXTENT < nextItemText.sprite.getGlobalBounds().height)
            {
                itemListHeight += nextItemText.sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_;
            }
            else
            {
                itemListHeight += CURR_ITEM_VERT_TEXT_EXTENT + IMAGE_BETWEEN_PAD_;
                nextItemText.sprite.move(0.0f, (CURR_ITEM_VERT_TEXT_EXTENT - nextItemText.sprite.getGlobalBounds().height) * 0.5f);
            }
        }

        //establish enemy details display extents
        if (enemyDetailsDisplayExtentHoriz < longestItemHorizExtent)
            enemyDetailsDisplayExtentHoriz = longestItemHorizExtent;

        enemyDetailsDisplayExtentVert = (ITEM_IMAGE_POS_TOP_START + itemListHeight);

        //final setup call
        const sf::Vector2f IMAGE_DEST_POS_V(IMAGE_POS_LEFT, IMAGE_POS_TOP);
        const sf::FloatRect SUMMARYVIEW_DISPLAY_REGION(COMBAT_REGION.left, COMBAT_REGION.top, (enemyDetailsDisplayExtentHoriz - COMBAT_REGION.left) + IMAGE_BETWEEN_PAD_, (enemyDetailsDisplayExtentVert - COMBAT_REGION.top) + IMAGE_BETWEEN_PAD_);
        StartTransitionTo(combatNodePtr, IMAGE_DEST_POS_V, SUMMARYVIEW_DISPLAY_REGION);
    }

}
}
