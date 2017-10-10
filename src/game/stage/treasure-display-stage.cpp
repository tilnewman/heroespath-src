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



    const float TreasureDisplayStage::ITEM_DETAIL_TIMEOUT_SEC_{ 3.0f };


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
        coinsTextUPtr_(),
        gemsTextUPtr_(),
        weightTextUPtr_(),
        instrTextUPtr_(),
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
        treasureImage_(item::TreasureImage::Count),
        redXTexture_(),
        redXImageUPtr_(),
        itemDetailTimer_(0.0f),
        itemDetailViewer_(),
        mousePos_(0.0f, 0.0f),
        heldCache_(),
        lockboxCache_()
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
        
        if (instrTextUPtr_.get() != nullptr)
        {
            target.draw( * instrTextUPtr_, STATES);
        }

        if (characterImageUPtr_.get() != nullptr)
        {
            target.draw( * characterImageUPtr_, STATES);
        }

        if (item::TreasureAvailable::NoTreasure != treasureAvailable_)
        {
            target.draw(coinsSprite_, STATES);
        }

        Stage::Draw(target, STATES);

        if (redXImageUPtr_.get() != nullptr)
        {
            target.draw( * redXImageUPtr_, STATES);
        }

        target.draw(itemDetailViewer_, STATES);
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

        itemDetailViewer_.UpdateTime(ELAPSED_TIME_SECONDS);

        itemDetailTimer_ += ELAPSED_TIME_SECONDS;
        if (itemDetailTimer_ > ITEM_DETAIL_TIMEOUT_SEC_)
        {
            itemDetailTimer_ = 0;

            auto const ITEM_DETAILS{ MouseOverListboxItemDetails(mousePos_) };

            if (ITEM_DETAILS.IsValid())
            {
                itemDetailViewer_.FadeIn(ITEM_DETAILS.item_ptr, ITEM_DETAILS.rect);
            }
        }
    }


    bool TreasureDisplayStage::KeyRelease(const sf::Event::KeyEvent &)
    {
        ItemViewerInterruption();
        return false;
    }


    void TreasureDisplayStage::UpdateMousePos(const sf::Vector2i & MOUSE_POS_V)
    {
        auto const NEW_MOUSE_POS{ sfml_util::ConvertVector<int, float>(MOUSE_POS_V) };

        auto const MAX_DIFF{ 3.0f };
        auto const DIFF_VECTOR{ mousePos_ - NEW_MOUSE_POS };
        if ((std::abs(DIFF_VECTOR.x) > MAX_DIFF) ||
            (std::abs(DIFF_VECTOR.y) > MAX_DIFF))
        {
            ItemViewerInterruption();
        }

        mousePos_ = NEW_MOUSE_POS;
    }


    void TreasureDisplayStage::UpdateMouseDown(const sf::Vector2f &)
    {
        ItemViewerInterruption();
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
        
        stageMoverUPtr_ = std::make_unique<stage::treasure::StageMover>(
            TREASURE_SOURCE,
            WhichCharacterInventoryIsDisplayedIndex());
        
        SetupForCollection_TreasureListbox(TREASURE_SOURCE);
        SetupForCollection_InventoryListbox();
        
        SetupForCollection_TreasureListboxLabel();
        
        SetupForCollection_InventoryCharacterImage();
        SetupForCollection_InventoryListboxLabel();
        SetupForCollection_InventoryCoinsText();
        SetupForCollection_InventoryGemsText();
        SetupForCollection_InventoryWeightText();
        SetupForCollection_InventoryRedXImage();

        SetupForCollection_InstructionsText();
        
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


    std::size_t TreasureDisplayStage::WhichCharacterInventoryIsDisplayedIndex()
    {
        if (stageMoverUPtr_.get() == nullptr)
        {
            auto const NUM_CHARACTERS{ Game::Instance()->State().Party().Characters().size() };

            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                if (Game::Instance()->State().Party().GetAtOrderPos(i)->IsBeast() == false)
                {
                    return i;
                }
            }

            //should never reach this code since one party member must be non-beast
            return 0;
        }
        else
        {
            return stageMoverUPtr_->InventoryCharacterIndex();
        }
    }


    creature::CreaturePtr_t TreasureDisplayStage::WhichCharacterInventoryIsDisplayed()
    {
        return Game::Instance()->
            State().Party().GetAtOrderPos(WhichCharacterInventoryIsDisplayedIndex());
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
                CalculateHorizOffscreenPos(),
                MEASUREMENTS.inventoryListboxRegion.top) };
        
        stageMoverUPtr_->AddInventoryObject(
            inventoryListboxUPtr_.get(),
            INVENTORY_ONSCREEN_POS_V,
            INVENTORY_OFFSCREEN_POS_V);
    }


    void TreasureDisplayStage::SetupForCollection_TreasureListboxLabel()
    {
        SetupTreasure_ListboxLabel();

        stageMoverUPtr_->AddTreasureObject(
            treasureLabelUPtr_.get(),
            treasureLabelUPtr_->GetEntityPos(),
            sf::Vector2f(-750.0f, treasureLabelUPtr_->GetEntityPos().y) );
    }


    void TreasureDisplayStage::SetupForCollection_InventoryListboxLabel()
    {
        SetupInventory_ListboxLabel();

        stageMoverUPtr_->AddInventoryObject(
            inventoryLabelUPtr_.get(),
            inventoryLabelUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), inventoryLabelUPtr_->GetEntityPos().y) );
    }


    void TreasureDisplayStage::SetupForCollection_InventoryCoinsText()
    {
        SetupInventory_CoinsText();

        stageMoverUPtr_->AddInventoryObject(
            coinsTextUPtr_.get(),
            coinsTextUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), coinsTextUPtr_->GetEntityPos().y) );
    }


    void TreasureDisplayStage::SetupForCollection_InventoryGemsText()
    {
        SetupInventory_GemsText();

        stageMoverUPtr_->AddInventoryObject(
            gemsTextUPtr_.get(),
            gemsTextUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), gemsTextUPtr_->GetEntityPos().y));
    }


    void TreasureDisplayStage::SetupForCollection_InventoryCharacterImage()
    {
        SetupInventory_CharacterImage();

        stageMoverUPtr_->AddInventoryObject(
            characterImageUPtr_.get(),
            characterImageUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), characterImageUPtr_->GetEntityPos().y) );
    }


    void TreasureDisplayStage::SetupForCollection_InventoryWeightText()
    {
        SetupInventory_WeightText();

        stageMoverUPtr_->AddInventoryObject(
            weightTextUPtr_.get(),
            weightTextUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), weightTextUPtr_->GetEntityPos().y));
    }


    void TreasureDisplayStage::SetupForCollection_InventoryRedXImage()
    {
        SetupInventory_RedXImage();

        stageMoverUPtr_->AddInventoryObject(
            redXImageUPtr_.get(),
            inventoryListboxUPtr_->GetEntityPos(),
            sf::Vector2f(CalculateHorizOffscreenPos(), inventoryListboxUPtr_->GetEntityPos().y) );
    }


    void TreasureDisplayStage::SetupForCollection_InstructionsText()
    {
        const sfml_util::gui::TextInfo TEXT_INFO(
            "(press spacebar to change treasure, use arrows or numbers to change characters)",
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            sfml_util::FontManager::Color_GrayDark(),
            sf::BlendAlpha,
            sf::Text::Italic,
            sfml_util::Justified::Left);

        //initial position doesn't matter since the position must be set after rendering
        auto const EMPTY_RECT{ sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f) };

        instrTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "TreasureStage'sInstructionText", TEXT_INFO, EMPTY_RECT);

        //the +93 and -10 were found by experiment to look better
        instrTextUPtr_->SetEntityPos(
            (CreateDisplayMeasurements().screenWidth * 0.5f) -
            (instrTextUPtr_->GetEntityRegion().width * 0.5f) + 93.0f,
            titleImage_.Bottom(false) - 10.0f);

        EntityAdd(instrTextUPtr_.get());
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


    void TreasureDisplayStage::SetupTreasure_ListboxLabel()
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
            sfml_util::FontManager::Color_GrayDarker(),
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


    void TreasureDisplayStage::SetupInventory_CharacterImage()
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
            inventoryListboxUPtr_->GetEntityPos().y -
                (sfml_util::gui::CreatureImageManager::Instance()->DimmensionMax() *
                    MEASUREMENTS.characterImageScale));

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


    void TreasureDisplayStage::SetupInventory_ListboxLabel()
    {
        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };
        auto const IS_BEAST{ CREATURE_PTR->IsBeast() };

        SetupInventoryText(
            inventoryLabelUPtr_,
            "ListboxLabel",
            ((IS_BEAST) ? "(beasts cannot carry items)" : CREATURE_PTR->Name()),
            CalculateInventoryTextPosLeft(),
            characterImageUPtr_->GetEntityPos().y + sfml_util::MapByRes(25.0f, 50.0f),
            sfml_util::FontManager::Instance()->Size_Large());
    }


    void TreasureDisplayStage::SetupInventory_CoinsText()
    {
        std::ostringstream ss;
        ss << "Coins: ";

        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };
        if (CREATURE_PTR->IsBeast())
        {
            ss << "NA";
        }
        else
        {
            ss << CREATURE_PTR->Inventory().Coins() << "/";

            stats::Trait_t coinSum{ 0 };
            for (auto const NEXT_CREATURE_PTR : Game::Instance()->State().Party().Characters())
            {
                coinSum += NEXT_CREATURE_PTR->Inventory().Coins();
            }

            ss << coinSum;
        }

        SetupInventoryText(
            coinsTextUPtr_,
            "CoinsText",
            ss.str(),
            CalculateInventoryTextPosLeft(),
            (inventoryLabelUPtr_->GetEntityPos().y + inventoryLabelUPtr_->GetEntityRegion().height) -
                CalculateInventoryTextVertShift(),
            sfml_util::FontManager::Instance()->Size_Normal());
    }


    void TreasureDisplayStage::SetupInventory_GemsText()
    {
        std::ostringstream ss;
        ss << "Gems: ";

        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };
        if (CREATURE_PTR->IsBeast())
        {
            ss << "NA";
        }
        else
        {
            ss << CREATURE_PTR->Inventory().Gems() << "/";

            stats::Trait_t gemSum{ 0 };
            for (auto const NEXT_CREATURE_PTR : Game::Instance()->State().Party().Characters())
            {
                gemSum += NEXT_CREATURE_PTR->Inventory().Gems();
            }

            ss << gemSum;
        }

        SetupInventoryText(
            gemsTextUPtr_,
            "GemsText",
            ss.str(),
            CalculateInventoryTextPosLeft(),
            (coinsTextUPtr_->GetEntityPos().y + coinsTextUPtr_->GetEntityRegion().height) -
                CalculateInventoryTextVertShift(),
            sfml_util::FontManager::Instance()->Size_Normal());
    }


    void TreasureDisplayStage::SetupInventory_WeightText()
    {
        std::ostringstream ss;
        ss << "Weight: ";

        auto const CREATURE_PTR{ WhichCharacterInventoryIsDisplayed() };
        if (CREATURE_PTR->IsBeast())
        {
            ss << "NA";
        }
        else
        {
            ss << CREATURE_PTR->Inventory().Weight() << "/" << CREATURE_PTR->WeightCanCarry()
                << "  ("
                << static_cast<int>(static_cast<float>(CREATURE_PTR->Inventory().Weight()) /
                        static_cast<float>(CREATURE_PTR->WeightCanCarry()) * 100.0f)
                << "%)";
        }

        SetupInventoryText(
            weightTextUPtr_,
            "WeightText",
            ss.str(),
            CalculateInventoryTextPosLeft(),
            (gemsTextUPtr_->GetEntityPos().y + gemsTextUPtr_->GetEntityRegion().height) -
                CalculateInventoryTextVertShift(),
            sfml_util::FontManager::Instance()->Size_Normal());
    }


    void TreasureDisplayStage::SetupInventory_RedXImage()
    {
        if (WhichCharacterInventoryIsDisplayed()->IsBeast())
        {
            sfml_util::LoadTexture(
                redXTexture_,
                GameDataFile::Instance()->GetMediaPath("media-images-misc-x"));
        }
        else
        {
            redXTexture_ = sf::Texture();
        }

        sf::Sprite sprite(redXTexture_);
        sprite.setColor(sf::Color(192, 0, 0, 100));

        auto const INVENTORY_RECT{ inventoryListboxUPtr_->GetEntityRegion() };
        sfml_util::CenterAndScaleSpriteToFit(sprite, INVENTORY_RECT);

        auto const PREV_ENTITY_PTR{ redXImageUPtr_.get() };

        if (PREV_ENTITY_PTR != nullptr)
        {
            EntityRemove(PREV_ENTITY_PTR);
        }

        redXImageUPtr_ = std::make_unique<sfml_util::gui::GuiImage>(
            "TreasureDisplayStage's_RedXImage",
            INVENTORY_RECT,
            sprite);

        if ((stageMoverUPtr_.get() != nullptr) &&
            (PREV_ENTITY_PTR != nullptr))
        {
            stageMoverUPtr_->ReplaceEntity(PREV_ENTITY_PTR, redXImageUPtr_.get());
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

        SetupTreasure_ListboxLabel();
    }


    void TreasureDisplayStage::UpdateInventoryVisuals()
    {
        SetupListbox(
            treasure::WhichListbox::Inventory,
            inventoryListboxUPtr_,
            WhichCharacterInventoryIsDisplayed()->Inventory().Items());

        SetupInventory_ListboxLabel();
        SetupInventory_CharacterImage();
        SetupInventory_CoinsText();
        SetupInventory_GemsText();
        SetupInventory_WeightText();
        SetupInventory_RedXImage();
    }


    void TreasureDisplayStage::SetupInventoryText(
        sfml_util::gui::TextRegionUPtr_t & textRegionUPtr,
        const std::string & NAME,
        const std::string & TEXT,
        const float HORIZ_POS,
        const float VERT_POS,
        const unsigned int FONT_SIZE)
    {
        const sfml_util::gui::TextInfo TEXT_INFO(
            TEXT,
            sfml_util::FontManager::Instance()->Font_Default2(),
            FONT_SIZE,
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        auto const PREV_ENTITY_PTR{ textRegionUPtr.get() };

        if (PREV_ENTITY_PTR != nullptr)
        {
            EntityRemove(PREV_ENTITY_PTR);
        }

        //initial position doesn't matter since the position must be set after rendering
        auto const EMPTY_RECT{ sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f) };

        textRegionUPtr = std::make_unique<sfml_util::gui::TextRegion>(
            "TreasureStage'sInventory" + NAME, TEXT_INFO, EMPTY_RECT);

        textRegionUPtr->SetEntityPos(HORIZ_POS, VERT_POS);

        EntityAdd(textRegionUPtr.get());

        if ((stageMoverUPtr_.get() != nullptr) &&
            (PREV_ENTITY_PTR != nullptr))
        {
            stageMoverUPtr_->ReplaceEntity(PREV_ENTITY_PTR, textRegionUPtr.get());
        }
    }


    float TreasureDisplayStage::CalculateInventoryTextPosLeft() const
    {
        return characterImageUPtr_->GetEntityPos().x +
            (sfml_util::gui::CreatureImageManager::Instance()->DimmensionMax() *
                CreateDisplayMeasurements().characterImageScale);
    }


    void TreasureDisplayStage::ItemViewerInterruption()
    {
        itemDetailTimer_ = 0.0f;
        itemDetailViewer_.FadeOut();
    }


    const treasure::ItemDetails TreasureDisplayStage::MouseOverListboxItemDetails(
        const sf::Vector2f & MOUSE_POS) const
    {
        if ((treasureListboxUPtr_.get() != nullptr) &&
            (inventoryListboxUPtr_.get() != nullptr))
        {
            if (treasureListboxUPtr_->GetEntityRegion().contains(MOUSE_POS))
            {
                auto const LISTBOX_ITEM_SPTR{ treasureListboxUPtr_->GetAtPosition(MOUSE_POS) };
                if ((LISTBOX_ITEM_SPTR.get() != nullptr) &&
                    (LISTBOX_ITEM_SPTR->ITEM_CPTR != nullptr))
                {
                    return treasure::ItemDetails(
                        treasureListboxUPtr_->GetRectAtLocation(MOUSE_POS),
                        LISTBOX_ITEM_SPTR->ITEM_CPTR);
                }
            }
            else if (inventoryListboxUPtr_->GetEntityRegion().contains(MOUSE_POS))
            {
                auto const LISTBOX_ITEM_SPTR{ inventoryListboxUPtr_->GetAtPosition(MOUSE_POS) };
                if ((LISTBOX_ITEM_SPTR.get() != nullptr) &&
                    (LISTBOX_ITEM_SPTR->ITEM_CPTR != nullptr))
                {
                    return treasure::ItemDetails(
                        inventoryListboxUPtr_->GetRectAtLocation(MOUSE_POS),
                        LISTBOX_ITEM_SPTR->ITEM_CPTR);
                }
            }
        }

        return treasure::ItemDetails();
    }

}
}
