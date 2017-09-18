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
// treasure-stage.cpp
//
#include "treasure-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/text-region.hpp"

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "game/combat/encounter.hpp"
#include "game/non-player/party.hpp"
#include "game/non-player/character.hpp"
#include "game/creature/algorithms.hpp"
#include "game/item/item-profile-warehouse.hpp"

//TODO TEMP REMOVE -once done testing
#include "game/player/party.hpp"
#include "game/player/fake-party.hpp"
#include "game/state/game-state.hpp"
#include "game/state/game-state-factory.hpp"

#include "misc/real.hpp"
#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

#include <string>
#include <sstream>
#include <exception>
#include <algorithm>


namespace game
{
namespace stage
{

    //all of these names must be unique
    const std::string TreasureStage::POPUP_NAME_ITEMPROFILE_PLEASEWAIT_{
        "PopupName_ItemProfilePleaseWait" };

    const std::string TreasureStage::POPUP_NAME_ALL_ENEMIES_RAN_{
        "PopupName_AllEnemiesRan" };

    const std::string TreasureStage::POPUP_NAME_WORN_ONLY_{
        "PopupName_WornOnly" };

    const std::string TreasureStage::POPUP_NAME_LOCKBOX_ONLY_{
        "PopupName_LockboxOnly" };

    const std::string TreasureStage::POPUP_NAME_LOCKBOX_AND_WORD_{
        "PopupName_LockboxAndWorn" };


    TreasureStage::TreasureStage()
    :
        Stage               ("Treasure"),
        setupCountdown_     (0),
        phase_              (Phase::PreSetupDelay),
        bgTexture_          (),
        bgSprite_           (),
        corpseTexture_      (),
        corpseSprite_       (),
        treasureTexture_    (),
        treasureSprite_     (),
        coinsTexture_       (),
        coinsSprite_        (),
        treasureImageType_  (item::TreasureImage::Count),
        blurbTextRegionUPtr_(),
        itemCacheHeld_      (),
        itemCacheLockbox_   (),
        treasureAvailable_  (item::TreasureAvailable::Count)
    {}


    TreasureStage::~TreasureStage()
    {
        combat::Encounter::Instance()->EndTreasureStageTasks(
            itemCacheHeld_, itemCacheLockbox_);

        ClearAllEntities();
    }


    bool TreasureStage::HandleCallback(const game::callback::PopupResponse & POPUP_RESPONSE)
    {
        if (POPUP_RESPONSE.Info().Name() == POPUP_NAME_ITEMPROFILE_PLEASEWAIT_)
        {
            SetupAfterDelay();
            return false;
        }

        return true;
    }


    void TreasureStage::Setup()
    {
        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };
        auto const SCREEN_HEIGHT{ sfml_util::Display::Instance()->GetWinHeight() };

        //background image
        {
            sfml_util::LoadTexture(bgTexture_,
                GameDataFile::Instance()->GetMediaPath("media-images-backgrounds-paper-2b"));

            bgSprite_.setTexture(bgTexture_);
            bgSprite_.setPosition(0.0f, 0.0f);

            auto const SCALE_HORIZ{ SCREEN_WIDTH / bgSprite_.getLocalBounds().width };
            auto const SCALE_VERT{ SCREEN_HEIGHT / bgSprite_.getLocalBounds().height };
            bgSprite_.setScale(SCALE_HORIZ, SCALE_VERT);
        }

        if (game::item::ItemProfileWarehouse::Instance()->Count() == 0)
        {
            //This 20 was found by experiment to be a good number of draw frames to allow the
            //background image to fade in a little bit before displaying the 'Please Wait' popup.
            setupCountdown_ = 40;
        }
        else
        {
            setupCountdown_ = 0;
            SetupAfterDelay();
        }
    }


    void TreasureStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        //If the ItemProfileWarehouse needs to be setup, then wait for the background
        //to fade in a little before displaying the 'Please Wait' popup.
        if (setupCountdown_ > 0)
        {
            if (0 == --setupCountdown_)
            {
                LoopManager::Instance()->PopupWaitBegin(this,
                    sfml_util::gui::PopupManager::Instance()->
                    CreateItemProfilePleaseWaitPopupInfo(POPUP_NAME_ITEMPROFILE_PLEASEWAIT_));
            }
        }

        target.draw(bgSprite_, STATES);
        target.draw(corpseSprite_, STATES);
        target.draw(treasureSprite_, STATES);
        target.draw(coinsSprite_, STATES);
        Stage::Draw(target, STATES);
    }


    void TreasureStage::SetupTreasureImage(const item::TreasureImage::Enum E)
    {
        //treasure image
        sfml_util::LoadTexture(treasureTexture_,
            GameDataFile::Instance()->GetMediaPath(item::TreasureImage::ToKey(E)));

        treasureSprite_.setTexture(treasureTexture_);

        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };
        auto const SCREEN_HEIGHT{ sfml_util::Display::Instance()->GetWinHeight() };

        auto const TREASURE_IMAGE_MAX_WIDTH{ (SCREEN_WIDTH * 0.5f) };
        auto const TREASURE_IMAGE_MAX_HEIGHT{ (SCREEN_HEIGHT * 0.333f) };

        auto const SCALE_HORIZ{ TREASURE_IMAGE_MAX_WIDTH /
            treasureSprite_.getLocalBounds().width };

        treasureSprite_.setScale(SCALE_HORIZ, SCALE_HORIZ);

        if (treasureSprite_.getGlobalBounds().height > TREASURE_IMAGE_MAX_HEIGHT)
        {
            auto const SCALE_VERT{ TREASURE_IMAGE_MAX_HEIGHT /
                treasureSprite_.getLocalBounds().height };

            treasureSprite_.setScale(SCALE_VERT, SCALE_VERT);
        }

        auto const TREASURE_IMAGE_LEFT{ sfml_util::MapByRes(100.0f, 300.0f) };

        auto const TREASURE_IMAGE_TOP{
            (sfml_util::MapByRes(50.0f, 150.0f) + (SCREEN_HEIGHT * 0.166f)) -
            (treasureSprite_.getGlobalBounds().height * 0.5f) };

        treasureSprite_.setPosition(TREASURE_IMAGE_LEFT, TREASURE_IMAGE_TOP);
        treasureSprite_.setColor(sf::Color(255, 255, 255, 192));

        //coins image
        if ((E == item::TreasureImage::ChestOpen) ||
            (E == item::TreasureImage::LockboxOpen))
        {
            auto const COINS_LEFT{ TREASURE_IMAGE_LEFT +
                (treasureSprite_.getGlobalBounds().width * 0.80f) };

            auto const COINS_TOP{ TREASURE_IMAGE_TOP +
                (treasureSprite_.getGlobalBounds().height * 0.75f) };

            coinsSprite_.setPosition(COINS_LEFT, COINS_TOP);
        }
        else
        {
            coinsSprite_.setPosition(SCREEN_WIDTH + 1.0f, SCREEN_HEIGHT + 1.0f);
        }
    }


    const std::string TreasureStage::GetCorpseImageKeyFromEnemyParty() const
    {
        auto const DEAD_ENEMY_CHARACTERS_PVEC{
            combat::Encounter::Instance()->DeadNonPlayerParty().Characters() };

        misc::StrVec_t corpseKeyStrVec;
        for (auto const NEXT_ENEMY_CHARACTER_PTR : DEAD_ENEMY_CHARACTERS_PVEC)
        {
            auto const CORPSE_KEY_STR_VEC{ GetCorpseImageKeyFromRace(
                NEXT_ENEMY_CHARACTER_PTR->Race()) };

            std::copy(CORPSE_KEY_STR_VEC.begin(),
                      CORPSE_KEY_STR_VEC.end(),
                      std::back_inserter(corpseKeyStrVec));
        }

        if (DEAD_ENEMY_CHARACTERS_PVEC.empty() || corpseKeyStrVec.empty())
        {
            M_HP_LOG_ERR("game::stage::TreasureStage::GetCorpseImageKeyFromEnemyParty() "
                << "was unable to gather any key strings.  Using default image.  "
                << "DEAD_ENEMY_CHARACTERS_PVEC.size()=" << DEAD_ENEMY_CHARACTERS_PVEC.size()
                << ", corpseKeyStrVec.size()=" << corpseKeyStrVec.size());

            auto const DEFAULT_CORPSE_KEY_STR_VEC{
                GetCorpseImageKeyFromRace(creature::race::Human) };

            return DEFAULT_CORPSE_KEY_STR_VEC.at(static_cast<std::size_t>(
                misc::random::Int(static_cast<int>(DEFAULT_CORPSE_KEY_STR_VEC.size()) - 1)));
        }
        else
        {
            //Allow duplicates in corpseKeyStrVec so that the more a race was faced
            //during combat means the more likely that corpse image is shown.
            return corpseKeyStrVec.at(static_cast<std::size_t>(
                misc::random::Int(static_cast<int>(corpseKeyStrVec.size()) - 1)));
        }
    }


    const misc::StrVec_t TreasureStage::GetCorpseImageKeyFromRace(const creature::race::Enum E) const
    {
        switch (E)
        {
            case creature::race::Shade:
            case creature::race::Ghoul:
            case creature::race::Human:
            case creature::race::Gnome:
            case creature::race::Pixie:
            case creature::race::Witch:
            case creature::race::Halfling:
            case creature::race::Skeleton:
            {
                return { "media-images-bones-skull-humaniod",
                         "media-images-bones-skull-humaniod-pile-1",
                         "media-images-bones-skull-humaniod-pile-2",
                         "media-images-bones-skull-humaniod-pile-3" };
            }

            case creature::race::Wyvern:
            case creature::race::Dragon:
            case creature::race::Hydra:
            {
                return { "media-images-bones-skull-dragon-1",
                         "media-images-bones-skull-dragon-2",
                         "media-images-bones-skull-dragon-3",
                         "media-images-bones-skull-dragon-4" };
            }

            case creature::race::Orc:
            case creature::race::Goblin:
            {
                return { "media-images-bones-skull-goblin" };
            }

            case creature::race::Pug:
            case creature::race::Newt:
            {
                return { "media-images-bones-skull-animal-2" };
            }

            case creature::race::Naga:
            case creature::race::LizardWalker:
            {
                //the orc skull looked better as a naga/lizard skull
                return { "media-images-bones-skull-orc" };
            }

            case creature::race::Bog:
            {
                return { "media-images-bones-skull-bog" };
            }

            case creature::race::Spider:
            case creature::race::CaveCrawler:
            {
                return { "media-images-bones-cave-crawler" };
            }

            case creature::race::Minotaur:
            {
                return { "media-images-bones-skull-minotaur" };
            }

            case creature::race::Plant:
            {
                return { "media-images-bones-skull-animal-1" };
            }

            case creature::race::Beetle:
            {
                return { "media-images-bones-beetle" };
            }

            case creature::race::Demon:
            {
                return { "media-images-bones-skull-demon" };
            }

            case creature::race::Griffin:
            {
                return { "media-images-bones-griffin" };
            }

            case creature::race::Boar:
            case creature::race::LionBoar:
            case creature::race::Ramonaut:
            case creature::race::Wereboar:
            {
                return { "media-images-bones-skull-animal-3" };
            }

            case creature::race::Wolfen:
            case creature::race::Lion:
            case creature::race::Werebear:
            case creature::race::Werewolf:
            {
                return { "media-images-bones-wolfen" };
            }

            case creature::race::Serpent:
            case creature::race::Cobra:
            {
                return { "media-images-bones-skull-snake" };
            }

            case creature::race::Werecat:
            {
                return { "media-images-bones-cat" };
            }

            case creature::race::Ogre:
            case creature::race::Golem:
            case creature::race::Troll:
            case creature::race::Giant:
            {
                return { "media-images-bones-skull-giant" };
            }

            case creature::race::Bat:
            case creature::race::Werebat:
            {
                return { "media-images-bones-bat" };
            }

            case creature::race::Harpy:
            {
                return { "media-images-bones-harpy" };
            }

            case creature::race::ThreeHeadedHound:
            {
                return { "media-images-bones-three-headed-hound" };
            }

            case creature::race::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::stage::TreasureStage::GetImageKeyFromRace(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    void TreasureStage::SetupAfterDelay()
    {
        phase_ = Phase::InitialSetup;

        //TEMP TODO REMOVE -once done testing
        //create a party of characters to work with during testing
        state::GameStateFactory::Instance()->NewGame(game::player::FakeParty::Make());

        //TODO TEMP REMOVE -once finished testing
        //create a fake collection of dead creatures, using the predetermined initial encounter
        combat::Encounter::Instance()->BeginCombatTasks();
        //
        auto const NONPLAYER_CREATURE_PVEC{ creature::Algorithms::NonPlayers() };
        for (auto const NEXT_CREATURE_PTR : NONPLAYER_CREATURE_PVEC)
        {
            combat::Encounter::Instance()->HandleKilledCreature(NEXT_CREATURE_PTR);
        }
        //
        combat::Encounter::Instance()->EndCombatTasks();

        treasureImageType_ = combat::Encounter::Instance()->BeginTreasureStageTasks();
        itemCacheHeld_ = combat::Encounter::Instance()->TakeDeadNonPlayerItemsHeldCache();
        itemCacheLockbox_ = combat::Encounter::Instance()->TakeDeadNonPlayerItemsLockboxCache();

        SetupCorpseImage();
        SetupTreasureImage(treasureImageType_);
        treasureAvailable_ = DetermineTreasureAvailableState();
        PromptUserBasedonTreasureAvailability(treasureAvailable_, treasureImageType_);
    }


    void TreasureStage::SetupCoinsImage()
    {
        sfml_util::LoadTexture(coinsTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-coins"));

        coinsSprite_.setTexture(coinsTexture_);

        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };
        
        auto const COINS_IMAGE_WIDTH{ (SCREEN_WIDTH * 0.25f) };

        auto const COINS_SCALE{ (COINS_IMAGE_WIDTH / coinsSprite_.getLocalBounds().width) /
            2.0f };

        coinsSprite_.setScale(COINS_SCALE, COINS_SCALE);

        coinsSprite_.setColor(sf::Color(255, 255, 255, 192));
    }


    item::TreasureAvailable::Enum TreasureStage::DetermineTreasureAvailableState()
    {
        if ((itemCacheHeld_.Empty() == false) && (itemCacheLockbox_.Empty() == false))
        {
            return item::TreasureAvailable::NoTreasure;
        }
        else if ((itemCacheHeld_.Empty() == false) && itemCacheLockbox_.Empty())
        {
            return item::TreasureAvailable::WornOnly;
        }
        else if (itemCacheHeld_.Empty() && (itemCacheLockbox_.Empty() == false))
        {
            return item::TreasureAvailable::LockboxOnly;
        }
        else
        {
            return item::TreasureAvailable::WornAndLockbox;
        }
    }


    void TreasureStage::SetupCorpseImage()
    {
        sfml_util::LoadTexture(corpseTexture_,
            GameDataFile::Instance()->GetMediaPath(GetCorpseImageKeyFromEnemyParty()));

        corpseSprite_.setTexture(corpseTexture_);

        auto const SCREEN_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() };
        auto const SCREEN_HEIGHT{ sfml_util::Display::Instance()->GetWinHeight() };

        auto const CORPSE_IMAGE_MAX_WIDTH{ (SCREEN_WIDTH * 0.75f) };
        auto const CORPSE_IMAGE_MAX_HEIGHT{ (SCREEN_HEIGHT * 0.5f) };

        auto const SCALE_HORIZ{ CORPSE_IMAGE_MAX_WIDTH /
            corpseSprite_.getLocalBounds().width };

        corpseSprite_.setScale(SCALE_HORIZ, SCALE_HORIZ);

        if (corpseSprite_.getGlobalBounds().height > CORPSE_IMAGE_MAX_HEIGHT)
        {
            auto const SCALE_VERT{ CORPSE_IMAGE_MAX_HEIGHT /
                corpseSprite_.getLocalBounds().height };

            corpseSprite_.setScale(SCALE_VERT, SCALE_VERT);
        }

        auto const CORPSE_IMAGE_LEFT{ (SCREEN_WIDTH * 0.5f) -
            (corpseSprite_.getGlobalBounds().width * 0.5f) };

        auto const CORPSE_IMAGE_TOP{ SCREEN_HEIGHT - (sfml_util::MapByRes(50.0f, 150.0f) +
            corpseSprite_.getGlobalBounds().height) };

        corpseSprite_.setPosition(CORPSE_IMAGE_LEFT, CORPSE_IMAGE_TOP);
        corpseSprite_.setColor(sf::Color(255, 255, 255, 50));
    }


    void TreasureStage::PromptUserBasedonTreasureAvailability(
        const item::TreasureAvailable::Enum TREASURE_AVAILABLE,
        const item::TreasureImage::Enum TREASURE_IMAGE)
    {
        switch (TREASURE_AVAILABLE)
        {
            case item::TreasureAvailable::NoTreasure:
            {
                std::ostringstream ss;
                ss << "Your enemies have no possessions worn or carried in a lockbox.  "
                    << "Click Continue to return to the Adventure screen.";

                auto const POPUP_INFO{ sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_ALL_ENEMIES_RAN_,
                    ss.str(),
                    sfml_util::PopupButtons::Continue,
                    sfml_util::PopupImage::Regular) };

                game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::WornOnly:
            {
                std::ostringstream ss;
                ss << "Your enemies have possessions worn but they carried no lockbox.  "
                    << "Click Continue to pick through what they left behind.";

                auto const POPUP_INFO{ sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_WORN_ONLY_,
                    ss.str(),
                    sfml_util::PopupButtons::Continue,
                    sfml_util::PopupImage::Regular) };

                game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::LockboxOnly:
            {
                std::ostringstream ss;
                ss << "Your enemies have no possessions on them, but they carried a "
                    << ((TREASURE_IMAGE == item::TreasureImage::ChestClosed) ? "chest" : "lockbox")
                    << ".  Attempt to pick the lock?";

                auto const POPUP_INFO{ sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_LOCKBOX_ONLY_,
                    ss.str(),
                    sfml_util::PopupButtons::YesNo,
                    sfml_util::PopupImage::Regular) };

                game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::WornAndLockbox:
            {
                std::ostringstream ss;
                ss << "Your enemies have possessions on them and they also carried a "
                    << ((TREASURE_IMAGE == item::TreasureImage::ChestClosed) ? "chest" : "lockbox")
                    << ".  Attempt to pick the lock?";

                auto const POPUP_INFO{ sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_LOCKBOX_AND_WORD_,
                    ss.str(),
                    sfml_util::PopupButtons::YesNo,
                    sfml_util::PopupImage::Regular) };

                game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
                break;
            }
            case item::TreasureAvailable::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::stage::TreasureStage::PromptUserBasedonTreasureAvailability("
                    << treasureAvailable_ << ")_InvalidValueError.";

                throw std::range_error(ss.str());
            }
        }
    }

}
}
