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
// treasure-display-stage.hpp
//
#include "treasure-display-stage.hpp"

#include "game/game.hpp"
#include "game/state/game-state.hpp"
#include "game/game-data-file.hpp"
#include "game/combat/encounter.hpp"
#include "game/stage/treasure-stage.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/item/item.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"

#include "misc/vectors.hpp"


namespace game
{
namespace stage
{
namespace treasure
{

    ListboxColors::ListboxColors()
    :
        image(sf::Color(255, 255, 255, 190)),
        line(sfml_util::FontManager::Color_GrayDark()),
        foreground(line),
        background(sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 220)),
        title(sfml_util::FontManager::Color_Orange() - sf::Color(130, 130, 130, 0)),
        colorSet(foreground, background)
    {}


    DisplayMeasurements::DisplayMeasurements(
        const float COINS_IMAGE_BOTTOM,
        const float BOTTOM_SYMBOL_HEIGHT)
    :
        screenWidth(sfml_util::Display::Instance()->GetWinWidth()),
        screenHeight(sfml_util::Display::Instance()->GetWinHeight()),
        innerPad(sfml_util::MapByRes(10.0f, 40.0f)),
        innerRect(
            innerPad,
            innerPad,
            screenWidth - (2.0f * innerPad),
            screenWidth - (2.0f * innerPad)),
        listboxScreenEdgeMargin(sfml_util::MapByRes(35.0f, 100.0f)),
        listboxBetweenSpacer(sfml_util::MapByRes(65.0f, 200.0f)),
        listboxWidth(
            ((innerRect.width - (2.0f * listboxScreenEdgeMargin)) - listboxBetweenSpacer) * 0.5f),
        treasureListboxLeft(innerRect.left + listboxScreenEdgeMargin),
        inventoryListboxLeft(treasureListboxLeft + listboxWidth + listboxBetweenSpacer),
        listboxTop(COINS_IMAGE_BOTTOM + sfml_util::MapByRes(30.0f, 70.0f)),
        listboxHeight((screenHeight - listboxTop) - BOTTOM_SYMBOL_HEIGHT),
        treasureListboxRegion(
            treasureListboxLeft,
            listboxTop,
            listboxWidth,
            listboxHeight),
        inventoryListboxRegion(
            inventoryListboxLeft,
            listboxTop,
            listboxWidth,
            listboxHeight),
        listboxMargin(10.0f),
        listboxItemSpacer(6.0f),
        characterImageLeft(inventoryListboxRegion.left),
        characterImageScale(sfml_util::MapByRes(0.75f, 3.25f))
    {}

}//end of namespace treasure


    TreasureDisplayStage::TreasureDisplayStage(TreasureStage * treasureStagePtr)
    :
        Stage("TreasureDisplay", false),
        treasureStagePtr_(treasureStagePtr),
        titleImage_("treasure-button.png", true, 1.0f, 0.75f),
        bottomImage_(0.8f, true, sf::Color::White, 0.4f),
        ouroborosUPtr_(),
        stageMoverUPtr_(),
        treasureListboxUPtr_(),
        inventoryListboxUPtr_(),
        treasureLabelUPtr_(),
        inventoryLabelUPtr_(),
        weightLabelUPtr_(),
        backgroundTexture_(),
        backgroundSprite_(),
        corpseTexture_(),
        corpseSprite_(),
        treasureTexture_(),
        treasureSprite_(),
        coinsTexture_(),
        coinsSprite_(),
        characterTexture_(),
        characterImageUPtr_(),
        treasureAvailable_(item::TreasureAvailable::NoTreasure),
        treasureImage_(item::TreasureImage::Count)
    {}


    bool TreasureDisplayStage::HandleCallback(
        const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        return treasureStagePtr_->HandleListboxCallback(
            treasureListboxUPtr_.get(),
            inventoryListboxUPtr_.get(),
            PACKAGE);
    }


    void TreasureDisplayStage::Setup()
    {
        Stage::StageRegionSet(sf::FloatRect(
            0.0f,
            0.0f,
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight()));

        SetupInitial();
    }


    void TreasureDisplayStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundSprite_, STATES);
        target.draw(titleImage_, STATES);
        target.draw(bottomImage_, STATES);
        target.draw(corpseSprite_, STATES);
        target.draw(treasureSprite_, STATES);
        
        if (characterImageUPtr_.get() != nullptr)
        {
            target.draw( * characterImageUPtr_, STATES);
        }

        if (item::TreasureAvailable::NoTreasure != treasureAvailable_)
        {
            target.draw(coinsSprite_, STATES);
        }

        Stage::Draw(target, STATES);
    }


    void TreasureDisplayStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (stageMoverUPtr_.get() != nullptr)
        {
            if (stageMoverUPtr_->UpdateTimeTreasure(ELAPSED_TIME_SECONDS))
            {
                UpdateTreasureVisuals();
            }

            if (stageMoverUPtr_->UpdateTimeInventory(ELAPSED_TIME_SECONDS))
            {
                UpdateInventoryVisuals();
            }
        }
    }


    void TreasureDisplayStage::SetupInitial()
    {   
        SetupInitial_BackgroundImage();
        SetupInitial_Ouroboros();
    }


    void TreasureDisplayStage::SetupAfterPleaseWait(
        const item::TreasureImage::Enum WHICH_IMAGE)
    {
        SetupAfterPleaseWait_CorpseImage();
        SetupAfterPleaseWait_TreasureImage(WHICH_IMAGE);
        SetupAfterPleaseWait_CoinsImage();
    }


    void TreasureDisplayStage::UpdateTreasureImage(
        const item::TreasureImage::Enum WHICH_IMAGE)
    {
        if (item::TreasureImage::ChestOpen == WHICH_IMAGE)
        {
            sfml_util::LoadTexture(
                treasureTexture_,
                GameDataFile::Instance()->GetMediaPath("media-images-chest-open"));
        }
        else if (item::TreasureImage::LockboxOpen == WHICH_IMAGE)
        {
            sfml_util::LoadTexture(
                treasureTexture_,
                GameDataFile::Instance()->GetMediaPath("media-images-lockbox-open"));
        }
    }


    void TreasureDisplayStage::SetupForCollection(
        const item::TreasureAvailable::Enum TREASURE_AVAILABLE,
        const item::TreasureImage::Enum TREASURE_IMAGE)
    {
        treasureAvailable_ = TREASURE_AVAILABLE;
        treasureImage_ = TREASURE_IMAGE;
        
        auto const TREASURE_SOURCE{ TreasureSource() };
        
        stageMoverUPtr_ = std::make_unique<stage::treasure::StageMover>(TREASURE_SOURCE);

        SetupForCollection_TreasureListbox(TREASURE_SOURCE);
        SetupForCollection_InventoryListbox();
        SetupForCollection_TreasureListboxLabel();
        SetupForCollection_InventoryListboxLabel();
        SetupForCollection_CharacterImage();
        SetupForCollection_InventoryWeightText();

        stageMoverUPtr_->StartAll();
    }

    
    bool TreasureDisplayStage::IsShowingHeldItems() const
    {
        if (nullptr == stageMoverUPtr_.get())
        {
            return false;
        }
        else
        {
            return stageMoverUPtr_->Source() == game::stage::treasure::Type::Held;
        }
    }


    std::size_t TreasureDisplayStage::CharacterIndexShowingInventory() const
    {
        if (nullptr == stageMoverUPtr_.get())
        {
            return 0;
        }
        else
        {
            return stageMoverUPtr_->InventoryCharacterIndex();
        }
    }


    bool TreasureDisplayStage::IsAnythingAnimating() const
    {
        return ((nullptr != stageMoverUPtr_.get()) && stageMoverUPtr_->IsEitherMoving());
    }


    bool TreasureDisplayStage::CanTreasureChange() const
    {
        return (item::TreasureAvailable::HeldAndLockbox == treasureAvailable_);
    }


    void TreasureDisplayStage::TreasureChange() const
    {
        if (stageMoverUPtr_.get() != nullptr)
        {
            stageMoverUPtr_->TreasureSwitch();
        }
    }


    void TreasureDisplayStage::UpdateItemCaches(
        const item::ItemCache & HELD_CACHE,
        const item::ItemCache & LOCKBOX_CACHE)
    {
        heldCache_ = HELD_CACHE;
        lockboxCache_ = LOCKBOX_CACHE;
    }


    std::size_t TreasureDisplayStage::CharacterIndex() const
    {
        if (stageMoverUPtr_.get() == nullptr)
        {
            return 0;
        }
        else
        {
            return stageMoverUPtr_->InventoryCharacterIndex();
        }
    }


    std::size_t TreasureDisplayStage::CharacterIndexMax() const
    {
        return Game::Instance()->State().Party().Characters().size() - 1;
    }


    void TreasureDisplayStage::InventoryChange(const std::size_t CHARACTER_INDEX)
    {
        if (stageMoverUPtr_.get() != nullptr)
        {
            UpdateInventoryVisuals();
            stageMoverUPtr_->InventoryIndexSet(CHARACTER_INDEX);
        }
    }


    treasure::DisplayMeasurements TreasureDisplayStage::CreateDisplayMeasurements() const
    {
        return treasure::DisplayMeasurements(
            coinsSprite_.getPosition().y + coinsSprite_.getGlobalBounds().height,
            bottomImage_.Height());
    }


    void TreasureDisplayStage::SetupInitial_BackgroundImage()
    {
        sfml_util::LoadTexture(backgroundTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-paper-2b"));

        backgroundSprite_.setTexture(backgroundTexture_);
        backgroundSprite_.setPosition(0.0f, 0.0f);

        backgroundSprite_.setScale(
            sfml_util::Display::Instance()->GetWinWidth() / backgroundSprite_.getLocalBounds().width,
            sfml_util::Display::Instance()->GetWinHeight() / backgroundSprite_.getLocalBounds().height);
    }


    void TreasureDisplayStage::SetupInitial_Ouroboros()
    {
        ouroborosUPtr_ = std::make_unique<Ouroboros>("TreasureDisplayStage's", true);
        EntityAdd(ouroborosUPtr_.get());
    }


    void TreasureDisplayStage::SetupAfterPleaseWait_CorpseImage()
    {
        sfml_util::LoadTexture(corpseTexture_,
            GameDataFile::Instance()->GetMediaPath(CorpseImageKeyFromEnemyParty()));

        corpseSprite_.setTexture(corpseTexture_);

        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };
        auto const SCREEN_HEIGHT{ sfml_util::Display::Instance()->GetWinHeight() };

        sfml_util::ScaleSpriteToFit(
            corpseSprite_,
            (SCREEN_WIDTH * 0.75f),
            (SCREEN_HEIGHT * 0.5f));

        auto const CORPSE_IMAGE_LEFT{
            (SCREEN_WIDTH * 0.5f) - (corpseSprite_.getGlobalBounds().width * 0.5f) };

        auto const CORPSE_IMAGE_TOP{
            bottomImage_.Top() - corpseSprite_.getGlobalBounds().height };

        corpseSprite_.setPosition(CORPSE_IMAGE_LEFT, CORPSE_IMAGE_TOP);
        corpseSprite_.setColor(sf::Color(255, 255, 255, 50));
    }


    //The scales/colors/positions found by experiment to look good at various resolutions.
    void TreasureDisplayStage::SetupAfterPleaseWait_TreasureImage(
        const item::TreasureImage::Enum WHICH_IMAGE)
    {
        auto const TREASURE_IMAGE_KEY{ item::TreasureImage::ToImageKey(WHICH_IMAGE) };

        auto const TREASURE_IMAGE_SCALE_NEED_REDUCTION{
            (TREASURE_IMAGE_KEY == "media-images-bones-bone-pile-1") ||
            (TREASURE_IMAGE_KEY == "media-images-bones-bone-pile-2") };

        sfml_util::LoadTexture(treasureTexture_,
            GameDataFile::Instance()->GetMediaPath(TREASURE_IMAGE_KEY));

        treasureSprite_.setTexture(treasureTexture_);

        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };
        auto const SCREEN_HEIGHT{ sfml_util::Display::Instance()->GetWinHeight() };

        auto const TREASURE_IMAGE_SCALE_ADJ{
            ((TREASURE_IMAGE_SCALE_NEED_REDUCTION) ? 0.75f : 1.0f) };

        auto const TREASURE_IMAGE_MAX_WIDTH{
            (SCREEN_WIDTH * 0.36f * TREASURE_IMAGE_SCALE_ADJ) };
        
        auto const TREASURE_IMAGE_MAX_HEIGHT{
            (SCREEN_HEIGHT * 0.275f * TREASURE_IMAGE_SCALE_ADJ) };

        sfml_util::ScaleSpriteToFit(
            treasureSprite_,
            TREASURE_IMAGE_MAX_WIDTH,
            TREASURE_IMAGE_MAX_HEIGHT);

        treasureSprite_.setPosition(
            sfml_util::MapByRes(50.0f, 350.0f),
            titleImage_.Bottom() - sfml_util::MapByRes(50.0f, 275.0f));

        treasureSprite_.setColor(sf::Color(255, 255, 255, 192));
    }


    //The scales/colors/positions found by experiment to look good at various resolutions.
    void TreasureDisplayStage::SetupAfterPleaseWait_CoinsImage()
    {
        sfml_util::LoadTexture(coinsTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-coins"));

        coinsSprite_.setTexture(coinsTexture_);

        auto const COINS_IMAGE_WIDTH{ (sfml_util::Display::Instance()->GetWinWidth() * 0.125f) };
        auto const COINS_SCALE{ COINS_IMAGE_WIDTH / coinsSprite_.getLocalBounds().width };
        coinsSprite_.setScale(COINS_SCALE, COINS_SCALE);

        coinsSprite_.setColor(sf::Color(255, 255, 255, 192));

        auto const COINS_LEFT{ treasureSprite_.getPosition().x +
            (treasureSprite_.getGlobalBounds().width * 0.80f) };

        auto const COINS_TOP{ treasureSprite_.getPosition().y +
            (treasureSprite_.getGlobalBounds().height * 0.75f) };

        coinsSprite_.setPosition(COINS_LEFT, COINS_TOP);
    }


    const std::string TreasureDisplayStage::CorpseImageKeyFromEnemyParty() const
    {
        auto const DEAD_ENEMY_CHARACTERS_PVEC{
            combat::Encounter::Instance()->DeadNonPlayerParty().Characters() };

        misc::StrVec_t corpseKeyStrVec;
        for (auto const NEXT_ENEMY_CHARACTER_PTR : DEAD_ENEMY_CHARACTERS_PVEC)
        {
            auto const CORPSE_KEY_STR_VEC{ creature::race::CorpseImageKeys(
                NEXT_ENEMY_CHARACTER_PTR->Race()) };

            std::copy(CORPSE_KEY_STR_VEC.begin(),
                CORPSE_KEY_STR_VEC.end(),
                std::back_inserter(corpseKeyStrVec));
        }

        if (DEAD_ENEMY_CHARACTERS_PVEC.empty() || corpseKeyStrVec.empty())
        {
            M_HP_LOG_ERR("stage::TreasureDisplayStage::GetCorpseImageKeyFromEnemyParty() "
                << "was unable to gather any key strings.  Using default image.  "
                << "DEAD_ENEMY_CHARACTERS_PVEC.size()=" << DEAD_ENEMY_CHARACTERS_PVEC.size()
                << ", corpseKeyStrVec.size()=" << corpseKeyStrVec.size());

            auto const DEFAULT_CORPSE_KEY_STR_VEC{
                creature::race::CorpseImageKeys(creature::race::Human) };

            return misc::Vector::SelectRandom(DEFAULT_CORPSE_KEY_STR_VEC);
        }
        else
        {
            //Allow duplicates in corpseKeyStrVec so that the more a race was faced
            //during combat means the more likely that corpse image is shown.
            return misc::Vector::SelectRandom(corpseKeyStrVec);
        }
    }


    creature::CreaturePtr_t TreasureDisplayStage::WhichCharacterInventoryIsDisplayed()
    {
        auto const CHARACTER_INDEX{ [&]() -> std::size_t
            {
                if (stageMoverUPtr_.get() == nullptr)
                {
                    return 0;
                }
                else
                {
                    return stageMoverUPtr_->InventoryCharacterIndex();
                }
            }() };

        return Game::Instance()->State().Party().GetAtOrderPos(CHARACTER_INDEX);
    }


    void TreasureDisplayStage::SetupForCollection_TreasureListbox(
        const stage::treasure::Type TREASURE_SOURCE)
    {
        SetupListbox(
            treasure::WhichListbox::Treasure,
            treasureListboxUPtr_,
            ((TREASURE_SOURCE == stage::treasure::Type::Container) ?
                lockboxCache_.items_pvec :
                heldCache_.items_pvec));
        
        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const TREASURE_ONSCREEN_POS_V{
            sf::Vector2f(
                MEASUREMENTS.treasureListboxRegion.left,
                MEASUREMENTS.treasureListboxRegion.top) };

        auto const TREASURE_OFFSCREEN_POS_V{
            sf::Vector2f(
                (MEASUREMENTS.treasureListboxRegion.width * -1.0f) - 10.0f,
                MEASUREMENTS.treasureListboxRegion.top) };

        stageMoverUPtr_->AddTreasureObject(
            treasureListboxUPtr_.get(),
            TREASURE_ONSCREEN_POS_V,
            TREASURE_OFFSCREEN_POS_V);
    }


    void TreasureDisplayStage::SetupForCollection_InventoryListbox()
    {
        SetupListbox(
            treasure::WhichListbox::Inventory,
            inventoryListboxUPtr_,
            WhichCharacterInventoryIsDisplayed()->Inventory().Items());
     
        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const INVENTORY_ONSCREEN_POS_V{
            sf::Vector2f(
                MEASUREMENTS.inventoryListboxRegion.left,
                MEASUREMENTS.inventoryListboxRegion.top) };

        auto const INVENTORY_OFFSCREEN_POS_V{
            sf::Vector2f(
                (MEASUREMENTS.screenWidth + 10.0f),
                MEASUREMENTS.inventoryListboxRegion.top) };
        
        stageMoverUPtr_->AddInventoryObject(
            inventoryListboxUPtr_.get(),
            INVENTORY_ONSCREEN_POS_V,
            INVENTORY_OFFSCREEN_POS_V);
    }


    void TreasureDisplayStage::SetupForCollection_TreasureListboxLabel()
    {
        SetupTreasureListboxLabel();

        auto const TREASURE_LABEL_ONSCREEN_POS{ treasureLabelUPtr_->GetEntityPos() };

        auto const TREASURE_LABEL_OFFSCREEN_POS{ sf::Vector2f(
            -750.0f, //this value is big enough for both possible labels
            TREASURE_LABEL_ONSCREEN_POS.y) };

        stageMoverUPtr_->AddTreasureObject(
            treasureLabelUPtr_.get(),
            TREASURE_LABEL_ONSCREEN_POS,
            TREASURE_LABEL_OFFSCREEN_POS);
    }


    void TreasureDisplayStage::SetupForCollection_InventoryListboxLabel()
    {
        SetupInventoryListboxLabel();

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };
        auto const OFFSCREEN_POS_RIGHT{ MEASUREMENTS.screenWidth + 1.0f };
        auto const INVENTORY_LABEL_ONSCREEN_POS{ inventoryLabelUPtr_->GetEntityPos() };

        auto const INVENTORY_LABEL_OFFSCREEN_POS{ sf::Vector2f(
            OFFSCREEN_POS_RIGHT,
            INVENTORY_LABEL_ONSCREEN_POS.y) };

        stageMoverUPtr_->AddInventoryObject(
            inventoryLabelUPtr_.get(),
            INVENTORY_LABEL_ONSCREEN_POS,
            INVENTORY_LABEL_OFFSCREEN_POS);
    }


    void TreasureDisplayStage::SetupForCollection_CharacterImage()
    {
        SetupCharacterImage();

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };
        auto const OFFSCREEN_POS_RIGHT{ MEASUREMENTS.screenWidth + 1.0f };
        auto const CHARACTER_IMAGE_ONSCREEN_POS{ characterImageUPtr_->GetEntityPos() };

        auto const CHARACTER_IMAGE_OFFSCREEN_POS{ sf::Vector2f(
            OFFSCREEN_POS_RIGHT,
            CHARACTER_IMAGE_ONSCREEN_POS.y) };

        stageMoverUPtr_->AddInventoryObject(
            characterImageUPtr_.get(),
            CHARACTER_IMAGE_ONSCREEN_POS,
            CHARACTER_IMAGE_OFFSCREEN_POS);
    }


    void TreasureDisplayStage::SetupForCollection_InventoryWeightText()
    {
        SetupInventoryWeightText();

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };
        auto const OFFSCREEN_POS_RIGHT{ MEASUREMENTS.screenWidth + 1.0f };
        auto const INVENTORY_WEIGHT_ONSCREEN_POS{ weightLabelUPtr_->GetEntityPos() };

        auto const INVENTORY_WEIGHT_OFFSCREEN_POS{ sf::Vector2f(
            OFFSCREEN_POS_RIGHT,
            INVENTORY_WEIGHT_ONSCREEN_POS.y) };

        stageMoverUPtr_->AddInventoryObject(
            weightLabelUPtr_.get(),
            INVENTORY_WEIGHT_ONSCREEN_POS,
            INVENTORY_WEIGHT_OFFSCREEN_POS);
    }


    void TreasureDisplayStage::SetupListbox(
        const treasure::WhichListbox WHICH_LISTBOX,
        sfml_util::gui::ListBoxUPtr_t & listboxUPtr,
        const item::ItemPVec_t & ITEMS_PVEC)
    {
        sfml_util::gui::TextInfo listboxTextInfo(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        sfml_util::gui::ListBoxItemSLst_t listboxItemsSList;

        for (auto const ITEM_PTR : ITEMS_PVEC)
        {
            listboxTextInfo.text = ITEM_PTR->Name();

            listboxItemsSList.push_back( std::make_shared<sfml_util::gui::ListBoxItem>(
                "TreasureDisplayStage_CharacterInventoryListboxItem_" + ITEM_PTR->Name(),
                listboxTextInfo,
                ITEM_PTR) );
        }
        
        auto const PREV_ENTITY_PTR{ listboxUPtr.get() };

        if (PREV_ENTITY_PTR != nullptr)
        {
            EntityRemove(PREV_ENTITY_PTR);
        }

        treasure::ListboxColors listboxColors;

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const LISTBOX_REGION{
            ((WHICH_LISTBOX == treasure::WhichListbox::Treasure) ?
                MEASUREMENTS.treasureListboxRegion :
                MEASUREMENTS.inventoryListboxRegion) };

        const sfml_util::gui::box::Info LISTBOX_BOXINFO(
            1,
            true,
            LISTBOX_REGION,
            listboxColors.colorSet,
            sfml_util::gui::BackgroundInfo(listboxColors.background));

        listboxUPtr = std::make_unique<sfml_util::gui::ListBox>(
            "TreasureDisplayStage's_CharacterInventoryListBox",
            LISTBOX_REGION,
            listboxItemsSList,
            this,
            MEASUREMENTS.listboxMargin,
            MEASUREMENTS.listboxItemSpacer,
            LISTBOX_BOXINFO,
            listboxColors.line,
            sfml_util::gui::ListBox::NO_LIMIT_,
            this);

        EntityAdd(listboxUPtr.get());
        
        listboxUPtr->SetSelectedIndex(0);
        listboxUPtr->SetImageColor(listboxColors.image);

        if ((stageMoverUPtr_.get() != nullptr) &&
            (PREV_ENTITY_PTR != nullptr))
        {
            stageMoverUPtr_->ReplaceEntity(PREV_ENTITY_PTR, listboxUPtr.get());
        }
    }


    void TreasureDisplayStage::SetupTreasureListboxLabel()
    {
        auto const LABEL_TEXT{ [&]() -> std::string
            {
                if (TreasureSource() == stage::treasure::Type::Held)
                {
                    return "Items Worn by Enemies";
                }
                else
                {
                    return "Items in the " +
                        item::TreasureImage::ToContainerName(treasureImage_, true);
                }
            }() };

        const sfml_util::gui::TextInfo TEXT_INFO(
            LABEL_TEXT,
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Large(),
            sf::Color::Black,
            sfml_util::Justified::Left);

        auto const PREV_ENTITY_PTR{ treasureLabelUPtr_.get() };
        
        if (PREV_ENTITY_PTR != nullptr)
        {
            EntityRemove(PREV_ENTITY_PTR);
        }
        
        //initial position doesn't matter since the position must be set after rendering
        auto const EMPTY_RECT{ sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f) };

        treasureLabelUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TreasureStage'sTreasureListboxLabel", TEXT_INFO, EMPTY_RECT);
        
        auto const MEASUREMENTS{ CreateDisplayMeasurements() };
        
        auto const LEFT{ MEASUREMENTS.treasureListboxRegion.left };
        
        auto const TOP{ (MEASUREMENTS.treasureListboxRegion.top -
                treasureLabelUPtr_->GetEntityRegion().height) + 10.0f };
        
        treasureLabelUPtr_->SetEntityPos(LEFT, TOP);
        
        EntityAdd(treasureLabelUPtr_.get());

        if ((stageMoverUPtr_.get() != nullptr) &&
            (PREV_ENTITY_PTR != nullptr))
        {
            stageMoverUPtr_->ReplaceEntity(PREV_ENTITY_PTR, treasureLabelUPtr_.get());
        }
    }


    void TreasureDisplayStage::SetupInventoryListboxLabel()
    {
        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };

        auto const IS_BEAST{ CREATURE_PTR->IsBeast() };

        std::ostringstream ss;
        ss << CREATURE_PTR->Name()
            << ((IS_BEAST) ? " cannot carry items" : "'s Inventory");

        const sfml_util::gui::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Large(),
            ((IS_BEAST) ? sf::Color(100, 0, 0) : sf::Color::Black),
            sfml_util::Justified::Left);

        auto const PREV_ENTITY_PTR{ inventoryLabelUPtr_.get() };

        if (PREV_ENTITY_PTR != nullptr)
        {
            EntityRemove(PREV_ENTITY_PTR);
        }

        //initial position doesn't matter since the position must be set after rendering
        auto const EMPTY_RECT{ sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f) };

        inventoryLabelUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TreasureStage'sInventoryListboxLabel", TEXT_INFO, EMPTY_RECT);

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const LEFT{ MEASUREMENTS.inventoryListboxRegion.left };

        auto const TOP{ (MEASUREMENTS.inventoryListboxRegion.top -
            inventoryLabelUPtr_->GetEntityRegion().height) + 10.0f };

        inventoryLabelUPtr_->SetEntityPos(LEFT, TOP);

        EntityAdd(inventoryLabelUPtr_.get());

        if ((stageMoverUPtr_.get() != nullptr) &&
            (PREV_ENTITY_PTR != nullptr))
        {
            stageMoverUPtr_->ReplaceEntity(PREV_ENTITY_PTR, inventoryLabelUPtr_.get());
        }
    }


    void TreasureDisplayStage::SetupCharacterImage()
    {
        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };
        
        sfml_util::gui::CreatureImageManager::Instance()->GetImage(
            characterTexture_,
            CREATURE_PTR->ImageFilename(),
            true);
        
        sfml_util::Invert(characterTexture_);
        sfml_util::Mask(characterTexture_, sf::Color::White);

        sf::Sprite sprite;
        sprite.setTexture(characterTexture_, true);

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };
        
        sprite.setScale(
            MEASUREMENTS.characterImageScale, MEASUREMENTS.characterImageScale);

        sprite.setPosition(
            MEASUREMENTS.characterImageLeft,
            (inventoryLabelUPtr_->GetEntityPos().y - sprite.getGlobalBounds().height) -
                sfml_util::MapByRes(30.0f, 60.0f));

        sprite.setColor(sf::Color(255, 255, 255, 127));

        auto const PREV_ENTITY_PTR{ characterImageUPtr_.get() };

        if (PREV_ENTITY_PTR != nullptr)
        {
            EntityRemove(PREV_ENTITY_PTR);
        }

        characterImageUPtr_ = std::make_unique<sfml_util::gui::GuiImage>(
            "TreasureDisplayStage's_CharacterImage",
            sprite.getGlobalBounds(),
            sprite);

        EntityAdd(characterImageUPtr_.get());

        if ((stageMoverUPtr_.get() != nullptr) &&
            (PREV_ENTITY_PTR != nullptr))
        {
            stageMoverUPtr_->ReplaceEntity(PREV_ENTITY_PTR, characterImageUPtr_.get());
        }
    }


    void TreasureDisplayStage::SetupInventoryWeightText()
    {
        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };

        std::ostringstream ss;

        if (CREATURE_PTR->IsBeast())
        {
            ss << " ";
        }
        else
        {
            ss << "Weight: " << CREATURE_PTR->Inventory().Weight() << "/"
                << CREATURE_PTR->WeightCanCarry();
        }

        const sfml_util::gui::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sf::Color::Black,
            sfml_util::Justified::Left);

        auto const PREV_ENTITY_PTR{ weightLabelUPtr_.get() };

        if (PREV_ENTITY_PTR != nullptr)
        {
            EntityRemove(weightLabelUPtr_.get());
        }

        //initial position doesn't matter since the position must be set after rendering
        auto const EMPTY_RECT{ sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f) };

        weightLabelUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TreasureStage'sInventoryweightLabel", TEXT_INFO, EMPTY_RECT);

        auto const MEASUREMENTS{ CreateDisplayMeasurements() };

        auto const LEFT{
           (MEASUREMENTS.inventoryListboxRegion.left + MEASUREMENTS.inventoryListboxRegion.width) -
                weightLabelUPtr_->GetEntityRegion().width};

        auto const TOP{ (MEASUREMENTS.inventoryListboxRegion.top -
            weightLabelUPtr_->GetEntityRegion().height) + 10.0f };

        weightLabelUPtr_->SetEntityPos(LEFT, TOP);

        EntityAdd(weightLabelUPtr_.get());

        if ((stageMoverUPtr_.get() != nullptr) &&
            (PREV_ENTITY_PTR != nullptr))
        {
            stageMoverUPtr_->ReplaceEntity(PREV_ENTITY_PTR, weightLabelUPtr_.get());
        }
    }


    stage::treasure::Type TreasureDisplayStage::TreasureSource() const
    {
        if (stageMoverUPtr_.get() == nullptr)
        {
            return (
                ((treasureAvailable_ == item::TreasureAvailable::HeldAndLockbox) ||
                 (treasureAvailable_ == item::TreasureAvailable::LockboxOnly)) ?
                    stage::treasure::Type::Container :
                    stage::treasure::Type::Held);
        }
        else
        {
            return stageMoverUPtr_->Source();
        }
    }


    void TreasureDisplayStage::UpdateTreasureVisuals()
    {
        SetupListbox(
            treasure::WhichListbox::Treasure,
            treasureListboxUPtr_,
            ((TreasureSource() == stage::treasure::Type::Container) ?
                lockboxCache_.items_pvec :
                heldCache_.items_pvec));

        SetupTreasureListboxLabel();
    }


    void TreasureDisplayStage::UpdateInventoryVisuals()
    {
        SetupListbox(
            treasure::WhichListbox::Inventory,
            inventoryListboxUPtr_,
            WhichCharacterInventoryIsDisplayed()->Inventory().Items());

        SetupInventoryListboxLabel();
        SetupCharacterImage();
        SetupInventoryWeightText();
    }

}
}
