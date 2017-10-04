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
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/list-box-item.hpp"

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


    DisplayMeasurements::DisplayMeasurements(const float COINS_IMAGE_BOTTOM)
    :
        screenWidth(sfml_util::Display::Instance()->GetWinWidth()),
        screenHeight(sfml_util::Display::Instance()->GetWinHeight()),
        innerPad(sfml_util::MapByRes(10.0f, 40.0f)),
        innerRect(
            innerPad,
            innerPad,
            screenWidth - (2.0f * innerPad),
            screenWidth - (2.0f * innerPad)),
        creatureImageLeft(innerRect.left + sfml_util::MapByRes(35.0f, 100.0f)),
        creatureImageScale(sfml_util::MapByRes(0.75f, 3.25f)),
        creatureImageHeight(
            sfml_util::gui::CreatureImageManager::DimmensionMax() * creatureImageScale),
        listboxHeightReduction(sfml_util::MapByRes(100.0f, 400.0f)),
        listboxScreenEdgeMargin(sfml_util::MapByRes(35.0f, 100.0f)),
        listboxBetweenSpacer(sfml_util::MapByRes(65.0f, 200.0f)),
        listboxWidth(
            ((innerRect.width - (2.0f * listboxScreenEdgeMargin)) - listboxBetweenSpacer) * 0.5f),
        treasureListboxLeft(innerRect.left + listboxScreenEdgeMargin),
        inventoryListboxLeft(treasureListboxLeft + listboxWidth + listboxBetweenSpacer),
        listboxTop(COINS_IMAGE_BOTTOM + (listboxHeightReduction * 0.25f)),
        listboxHeight(
            ((screenHeight - listboxTop) - listboxHeightReduction) - 20.0f),
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
        listboxItemSpacer(6.0f)
    {}

}//end of namespace treasure


    TreasureDisplayStage::TreasureDisplayStage(TreasureStage * treasureStagePtr)
    :
        Stage("TreasureDisplay", false),
        treasureStagePtr_(treasureStagePtr),
        titleImage_("treasure-button.png", true, 1.0f, 0.75f),
        bottomImage_(0.8f, true, sf::Color::White),
        ouroborosUPtr_(),
        listboxMoverUPtr_(),
        treasureListboxUPtr_(),
        inventoryListboxUPtr_(),
        backgroundTexture_(),
        backgroundSprite_(),
        corpseTexture_(),
        corpseSprite_(),
        treasureTexture_(),
        treasureSprite_(),
        coinsTexture_(),
        coinsSprite_(),
        treasureAvailable_(item::TreasureAvailable::NoTreasure)
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
        //target.draw(characterSprite_, STATES);

        if (item::TreasureAvailable::NoTreasure != treasureAvailable_)
        {
            target.draw(coinsSprite_, STATES);
        }

        Stage::Draw(target, STATES);
    }


    void TreasureDisplayStage::SetupInitial()
    {   
        SetupInitial_BackgroundImage();
        SetupInitial_Ouroboros();
    }


    void TreasureDisplayStage::SetupAfterPleaseWait(const item::TreasureImage::Enum WHICH_IMAGE)
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
        const item::ItemCache & HELD_CACHE,
        const item::ItemCache & LOCKBOX_CACHE)
    {
        treasureAvailable_ = TREASURE_AVAILABLE;
        
        SetupListbox(
            treasure::WhichListbox::Treasure,
            treasureListboxUPtr_,
            WhichTreasureItemsAreDisplayed(listboxMoverUPtr_, HELD_CACHE, LOCKBOX_CACHE));

        auto characterPtr{ WhichCharacterInventoryIsDisplayed(listboxMoverUPtr_) };

        SetupListbox(
            treasure::WhichListbox::Inventory,
            inventoryListboxUPtr_,
            characterPtr->Inventory().Items());

        //TODO
    }


    item::ItemPVec_t TreasureDisplayStage::WhichTreasureItemsAreDisplayed(
        const treasure::ListboxMoverUPtr_t & LISTBOX_MOVER_UPTR,
        const item::ItemCache & HELD_CACHE,
        const item::ItemCache & LOCKBOX_CACHE) const
    {
        if (LISTBOX_MOVER_UPTR.get() == nullptr)
        {
            if (LOCKBOX_CACHE.items_pvec.empty() == false)
            {
                return LOCKBOX_CACHE.items_pvec;
            }
            else
            {
                return HELD_CACHE.items_pvec;
            }
        }
        else
        {
            if (LISTBOX_MOVER_UPTR->Source() == game::stage::treasure::SourceType::Held)
            {
                return HELD_CACHE.items_pvec;
            }
            else
            {
                return LOCKBOX_CACHE.items_pvec;
            }
        }
    }


    creature::CreaturePtr_t TreasureDisplayStage::WhichCharacterInventoryIsDisplayed(
        const treasure::ListboxMoverUPtr_t & LISTBOX_MOVER_UPTR)
    {
        auto const CHARACTER_INDEX{ [&]() -> std::size_t
            {
                if (LISTBOX_MOVER_UPTR.get() == nullptr)
                {
                    return 0;
                }
                else
                {
                    return LISTBOX_MOVER_UPTR->TargetNumber();
                }
            }() };

        return Game::Instance()->State().Party().GetAtOrderPos(CHARACTER_INDEX);
    }


    bool TreasureDisplayStage::IsShowingHeldItems() const
    {
        if (nullptr == listboxMoverUPtr_.get())
        {
            return false;
        }
        else
        {
            return listboxMoverUPtr_->Source() == game::stage::treasure::SourceType::Held;
        }
    }


    std::size_t TreasureDisplayStage::CharacterIndexShowingInventory() const
    {
        if (nullptr == listboxMoverUPtr_.get())
        {
            return 0;
        }
        else
        {
            return listboxMoverUPtr_->TargetNumber();
        }
    }


    bool TreasureDisplayStage::IsAnythingAnimating() const
    {
        return ((nullptr != listboxMoverUPtr_.get()) && listboxMoverUPtr_->IsEitherMoving());
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
            (SCREEN_WIDTH * 0.5f * TREASURE_IMAGE_SCALE_ADJ) };
        
        auto const TREASURE_IMAGE_MAX_HEIGHT{
            (SCREEN_HEIGHT * 0.333f * TREASURE_IMAGE_SCALE_ADJ) };

        sfml_util::ScaleSpriteToFit(
            treasureSprite_,
            TREASURE_IMAGE_MAX_WIDTH,
            TREASURE_IMAGE_MAX_HEIGHT);

        treasureSprite_.setPosition(
            sfml_util::MapByRes(100.0f, 300.0f),
            titleImage_.Bottom() - sfml_util::MapByRes(100.0f, 325.0f));

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

            M_HP_LOG_DBG("\t********** " << ((WHICH_LISTBOX == treasure::WhichListbox::Inventory) ? "Inventory" : "treasure") << "  " << ITEM_PTR->Name());
        }
        
        const bool IS_ALREADY_INSTANTIATED(listboxUPtr.get() != nullptr);

        if (IS_ALREADY_INSTANTIATED)
        {
            EntityRemove(listboxUPtr.get());
        }

        treasure::ListboxColors listboxColors;

        treasure::DisplayMeasurements measurements(
            coinsSprite_.getPosition().y + coinsSprite_.getGlobalBounds().height);

        auto const LISTBOX_REGION{
            ((WHICH_LISTBOX == treasure::WhichListbox::Treasure) ?
                measurements.treasureListboxRegion :
                measurements.inventoryListboxRegion) };

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
            measurements.listboxMargin,
            measurements.listboxItemSpacer,
            LISTBOX_BOXINFO,
            listboxColors.line,
            sfml_util::gui::ListBox::NO_LIMIT_,
            this);

        if (IS_ALREADY_INSTANTIATED)
        {
            EntityAdd(listboxUPtr.get());
        }

        listboxUPtr->SetSelectedIndex(0);
        listboxUPtr->SetImageColor(listboxColors.image);
    }

}
}
