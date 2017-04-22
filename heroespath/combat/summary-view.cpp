//
// summary-view.cpp
//
#include "summary-view.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/real-utils.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "heroespath/log-macros.hpp"
#include "heroespath/creature/creature.hpp"
#include "heroespath/combat/combat-node.hpp"


namespace heroespath
{
namespace combat
{

    ItemWithText::ItemWithText(const item::ItemSPtr_t & ITEM_SPTR)
    :
        sprite               (),
        texture_sptr         (),
        name_text_region_sptr(),
        desc_text_region_sptr(),
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
        combatNodeSPtr_      (),
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

        if (combatNodeSPtr_.get() != nullptr)
        {
            slider_.Speed(SLIDER_SPEED_ * 2.0f);
            slider_.StartMovingAway();
            movingDir_ = sfml_util::Moving::Away;
            isTransToComplete_ = false;
            isTransBackComplete_ = false;
            combatNodeSPtr_->IsSummaryView(false);
            combatNodeSPtr_->IsMoving(true);
        }
    }


    void SummaryView::StartTransitionTo(CombatNodeSPtr_t &   combatNodeSPtr,
                                        const sf::Vector2f &  DEST_POS_V,
                                        const sf::FloatRect & ENEMYDISPLAY_RECT)
    {
        if (preventNextTrans_)
        {
            preventNextTrans_ = false;
            return;
        }

        combatNodeSPtr_ = combatNodeSPtr;
        slider_.Setup(combatNodeSPtr, combatNodeSPtr->GetEntityPos(), DEST_POS_V, SLIDER_SPEED_);
        slider_.StartMovingToward();
        BackgroundColor(sf::Color::Transparent);
        BackgroundRegion(ENEMYDISPLAY_RECT);
        movingDir_ = sfml_util::Moving::Toward;
        isTransToComplete_ = false;
        isTransBackComplete_ = false;
        combatNodeSPtr->IsMoving(true);
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
        if (combatNodeSPtr_.get() != nullptr)
        {
            combatNodeSPtr_->IsSummaryView(true);
            combatNodeSPtr_->IsMoving(false);
        }
    }


    void SummaryView::SetTransitionBackComplete()
    {
        isTransToComplete_ = false;
        isTransBackComplete_ = true;
        movingDir_ = sfml_util::Moving::Still;
        itemWithTextVec_.clear();
        if (combatNodeSPtr_.get() != nullptr)
        {
            combatNodeSPtr_->IsSummaryView(false);
            combatNodeSPtr_->IsMoving(false);
        }
    }


    void SummaryView::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        //always draw the background since it needs to be drawn during transitions and is an inexpensive vertex array operation
        target.draw(bgQuads_, states);

        if (isTransToComplete_)
        {
            nameTextRegionSPtr_->draw(target, states);
            healthTextRegionSPtr_->draw(target, states);
            condTextRegionSPtr_->draw(target, states);
            
            if (combatNodeSPtr_->Creature()->IsPlayerCharacter())
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
            }

            combatNodeSPtr_->draw(target, states);
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


    void SummaryView::SetupAndStartTransition(CombatNodeSPtr_t & combatNodeSPtr, const sf::FloatRect & COMBAT_REGION)
    {
        itemWithTextVec_.clear();

        std::ostringstream ss;
        if (combatNodeSPtr->Creature()->Name() != combatNodeSPtr->Creature()->Race().Name())
            ss << combatNodeSPtr->Creature()->Name() << "\n";

        ss << combatNodeSPtr->Creature()->Race().Name();

        if (combatNodeSPtr->Creature()->Role().Which() != creature::role::Wolfen)
            ss << ", " + combatNodeSPtr->Creature()->Role().Name();

        const sfml_util::gui::TextInfo CREATURE_NAME_TEXT_INFO(ss.str(),
                                                               sfml_util::FontManager::Instance()->Font_Default1(),
                                                               sfml_util::FontManager::Instance()->Size_Small(),
                                                               sfml_util::FontManager::Color_Light());

        nameTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sName", CREATURE_NAME_TEXT_INFO, sf::FloatRect()) );

        std::ostringstream healthSS;
        healthSS << "Health: ";
        if (combatNodeSPtr->Creature()->IsPlayerCharacter())
        {
            healthSS << combatNodeSPtr->Creature()->HealthCurrent() << "/" << combatNodeSPtr->Creature()->HealthNormal();
        }
        else
        {
            healthSS << combatNodeSPtr->Creature()->HealthPercentStr();
        }
        const sfml_util::gui::TextInfo CREATURE_HEALTH_TEXT_INFO(healthSS.str(),
                                                                 sfml_util::FontManager::Instance()->Font_Default1(),
                                                                 sfml_util::FontManager::Instance()->Size_Small(),
                                                                 sfml_util::FontManager::Color_Light());
        healthTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sHealth", CREATURE_HEALTH_TEXT_INFO, sf::FloatRect()) );

        std::ostringstream armorRatingSS;
        armorRatingSS << "Armor Rating: " << combatNodeSPtr->Creature()->ArmorRating();
        const sfml_util::gui::TextInfo CREATURE_ARMORRATING_TEXT_INFO(armorRatingSS.str(),
                                                                      sfml_util::FontManager::Instance()->Font_Default1(),
                                                                      sfml_util::FontManager::Instance()->Size_Small(),
                                                                      sfml_util::FontManager::Color_Light());
        armorTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sArmorRating", CREATURE_ARMORRATING_TEXT_INFO, sf::FloatRect()) );

        const sfml_util::gui::TextInfo CREATURE_DESC_TEXT_INFO(combatNodeSPtr->Creature()->Body().ToString(),
                                                               sfml_util::FontManager::Instance()->Font_Default1(),
                                                               sfml_util::FontManager::Instance()->Size_Small(),
                                                               sfml_util::FontManager::Color_Light(),
                                                               sf::BlendAlpha,
                                                               sf::Text::Italic);
        descTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sDesc", CREATURE_DESC_TEXT_INFO, sf::FloatRect()) );

        std::ostringstream condSS;
        condSS << "Condition:  " << combatNodeSPtr->Creature()->ConditionList();
        const sfml_util::gui::TextInfo CREATURE_CONDITIONS_TEXT_INFO(condSS.str(),
                                                                     sfml_util::FontManager::Instance()->Font_Default1(),
                                                                     sfml_util::FontManager::Instance()->Size_Small(),
                                                                     sfml_util::FontManager::Color_Light());
        condTextRegionSPtr_.reset( new sfml_util::gui::TextRegion("SummaryView'sCondition", CREATURE_CONDITIONS_TEXT_INFO, sf::FloatRect()) );

        const float IMAGE_POS_LEFT(COMBAT_REGION.left + BLOCK_POS_LEFT_ + IMAGE_EDGE_PAD_);
        const float IMAGE_POS_TOP(COMBAT_REGION.top + BLOCK_POS_TOP_ + IMAGE_EDGE_PAD_);
        const float IMAGE_WIDTH(combatNodeSPtr->GetEntityRegion().width);
        const float IMAGE_HEIGHT(combatNodeSPtr->GetEntityRegion().height + sfml_util::MapByRes(20.0f, 60.0f));

        const float BETWEEN_TEXT_SPACER(0.0f);
        const float TOP_TEXT_SPACER(sfml_util::MapByRes(10.0f, 25.0f));
        const float CREATURE_TEXT_POS_LEFT(IMAGE_POS_LEFT + IMAGE_WIDTH + IMAGE_EDGE_PAD_);

        if (combatNodeSPtr->Creature()->IsPlayerCharacter())
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
        for (auto const & NEXT_ITEM_SPTR : combatNodeSPtr->Creature()->Inventory().ItemsEquipped())
            if ((NEXT_ITEM_SPTR->IsWeapon()) && (NEXT_ITEM_SPTR->IsBodypart() == false))
                weaponItemsToDisplay.push_back(NEXT_ITEM_SPTR);

        //only list bodypart weapons if there are not any others equipped
        if (weaponItemsToDisplay.empty())
        {
            for (auto const & NEXT_ITEM_SPTR : combatNodeSPtr->Creature()->Inventory().ItemsEquipped())
                if ((NEXT_ITEM_SPTR->IsWeapon()) && (NEXT_ITEM_SPTR->IsBodypart()))
                    weaponItemsToDisplay.push_back(NEXT_ITEM_SPTR);
        }
        else
        {
            for (auto const & NEXT_ITEM_SPTR : combatNodeSPtr->Creature()->Inventory().ItemsEquipped())
                if ((NEXT_ITEM_SPTR->IsWeapon()) && (NEXT_ITEM_SPTR->IsBodypart()))
                    weaponItemsToIgnore.push_back(NEXT_ITEM_SPTR);
        }

        for(auto const & NEXT_ITEM_SPTR : weaponItemsToDisplay)
            itemWithTextVec_.push_back(ItemWithText(NEXT_ITEM_SPTR));

        //then armor
        for (auto const & NEXT_ITEM_SPTR : combatNodeSPtr->Creature()->Inventory().ItemsEquipped())
            if (NEXT_ITEM_SPTR->IsArmor())
                itemWithTextVec_.push_back(ItemWithText(NEXT_ITEM_SPTR));

        //then misc
        for (auto const & NEXT_ITEM_SPTR : combatNodeSPtr->Creature()->Inventory().ItemsEquipped())
            if (NEXT_ITEM_SPTR->MiscType() != item::misc_type::NotMisc)
                itemWithTextVec_.push_back(ItemWithText(NEXT_ITEM_SPTR));

        //then everything else (clothes, etc)
        for (auto const & NEXT_ITEM_SPTR : combatNodeSPtr->Creature()->Inventory().ItemsEquipped())
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
            nextItemText.texture_sptr = sfml_util::gui::ItemImageManager::Instance()->Load(nextItemText.item_sptr);
            nextItemText.sprite = sf::Sprite(*nextItemText.texture_sptr);
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
        const float ITEM_TEXT_VERT_POS_MULT( sfml_util::MapByRes(2.5f, 4.0f) );
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
            nextItemText.name_text_region_sptr->MoveEntityPos(0.0f, (nextItemText.sprite.getGlobalBounds().height - (nextItemText.name_text_region_sptr->GetEntityRegion().height * ITEM_TEXT_VERT_POS_MULT)));

            const sfml_util::gui::TextInfo ITEM_DESC_TEXT_INFO(nextItemText.item_sptr->Desc(),
                                                               sfml_util::FontManager::Instance()->Font_Default1(),
                                                               sfml_util::FontManager::Instance()->Size_Small(),
                                                               sfml_util::FontManager::Color_Light(),
                                                               sfml_util::Justified::Left);

            const sf::FloatRect ITEM_DESC_RECT(ITEM_IMAGE_POS_LEFT + nextItemText.sprite.getGlobalBounds().width + IMAGE_EDGE_PAD_,
                                               ITEM_IMAGE_POS_TOP_START + itemListHeight + nextItemText.name_text_region_sptr->GetEntityRegion().height,
                                               0.0f,
                                               0.0f);

            nextItemText.desc_text_region_sptr.reset(new sfml_util::gui::TextRegion("CombatDisplay_EnemyDetails_ItemList_ItemDesc_" + nextItemText.item_sptr->Name(), ITEM_DESC_TEXT_INFO, ITEM_DESC_RECT));
            nextItemText.desc_text_region_sptr->MoveEntityPos(0.0f, (nextItemText.sprite.getGlobalBounds().height - (nextItemText.name_text_region_sptr->GetEntityRegion().height * ITEM_TEXT_VERT_POS_MULT)));

            const float CURR_ITEM_HORIZ_EXTENT(ITEM_IMAGE_POS_LEFT + nextItemText.sprite.getGlobalBounds().width + IMAGE_EDGE_PAD_ + nextItemText.desc_text_region_sptr->GetEntityRegion().width);
            if (longestItemHorizExtent < CURR_ITEM_HORIZ_EXTENT)
                longestItemHorizExtent = CURR_ITEM_HORIZ_EXTENT;

            itemListHeight += nextItemText.sprite.getGlobalBounds().height + IMAGE_BETWEEN_PAD_;
        }

        //establish enemy details display extents
        if (enemyDetailsDisplayExtentHoriz < longestItemHorizExtent)
            enemyDetailsDisplayExtentHoriz = longestItemHorizExtent;

        enemyDetailsDisplayExtentVert = (ITEM_IMAGE_POS_TOP_START + itemListHeight);

        //final setup call
        const sf::Vector2f IMAGE_DEST_POS_V(IMAGE_POS_LEFT, IMAGE_POS_TOP);
        const sf::FloatRect SUMMARYVIEW_DISPLAY_REGION(COMBAT_REGION.left, COMBAT_REGION.top, (enemyDetailsDisplayExtentHoriz - COMBAT_REGION.left) + IMAGE_BETWEEN_PAD_, (enemyDetailsDisplayExtentVert - COMBAT_REGION.top) + IMAGE_BETWEEN_PAD_);
        StartTransitionTo(combatNodeSPtr, IMAGE_DEST_POS_V, SUMMARYVIEW_DISPLAY_REGION);
    }

}
}
