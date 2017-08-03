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

//TODO TEMP REMOVE -once done testing
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/creature/name-info.hpp"
#include "game/player/initial.hpp"
#include "game/state/game-state.hpp"
#include "game/state/game-state-factory.hpp"

#include "misc/real.hpp"
#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <sstream>


namespace game
{
namespace stage
{

    TreasureStage::TreasureStage()
    :
        Stage               ("Treasure"),
        //phase_              (PhaseType::InitialSetup),
        bgTexture_          (),
        bgSprite_           (),
        corpseTexture_      (),
        corpseSprite_       (),
        treasureTexture_    (),
        treasureSprite_     (),
        coinsTexture_       (),
        coinsSprite_        (),
        blurbTextRegionUPtr_()
    {}


    TreasureStage::~TreasureStage()
    {
        combat::Encounter::Instance()->EndTreasureStageTasks();
        ClearAllEntities();
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

        //TEMP TODO REMOVE -once done testing
        //create a party of characters to work with during testing
        std::string errMsgIgnored{ "" };
        player::PartyPtr_t partyPtr{ new player::Party() };
        const int STAT_BASE_HIGH{18};
        const int STAT_BASE_MED{9};
        const int STAT_BASE_LOW{5};
        const int STAT_RAND{6};
        {
            const stats::StatSet FIREBRAND_STATS(STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                                                 STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                                                 STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
                                                 STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
                                                 STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                                                 STAT_BASE_MED  + misc::random::Int(STAT_RAND));

            const std::string FIREBRAND_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "F"));

            auto firebrandPtr{  new player::Character(FIREBRAND_NAME,
                                                      creature::sex::Male,
                                                      creature::BodyType::Make_Dragon(),
                                                      creature::race::Dragon,
                                                      creature::role::Firebrand,
                                                      FIREBRAND_STATS) };

            player::Initial::Setup(firebrandPtr);
            partyPtr->Add(firebrandPtr, errMsgIgnored);
        }

        {
            const stats::StatSet BARD_STATS(STAT_BASE_MED + misc::random::Int(STAT_RAND),
                                            STAT_BASE_MED + misc::random::Int(STAT_RAND),
                                            STAT_BASE_MED + misc::random::Int(STAT_RAND),
                                            STAT_BASE_LOW + misc::random::Int(STAT_RAND),
                                            STAT_BASE_MED + misc::random::Int(STAT_RAND),
                                            STAT_BASE_MED + misc::random::Int(STAT_RAND));

            const std::string BARD_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "B"));

            auto bardPtr{ new player::Character(BARD_NAME,
                                                creature::sex::Male,
                                                creature::BodyType::Make_Humanoid(),
                                                creature::race::Human,
                                                creature::role::Bard,
                                                BARD_STATS) };

            player::Initial::Setup(bardPtr);
            partyPtr->Add(bardPtr, errMsgIgnored);
        }
       
        {
            const stats::StatSet BEASTMASTER_STATS(STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                                                   STAT_BASE_MED  + misc::random::Int(STAT_RAND),
                                                   STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
                                                   STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
                                                   STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                                                   STAT_BASE_MED  + misc::random::Int(STAT_RAND));

            const std::string BEASTMASTER_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "G"));

            auto bmPtr{ new player::Character(BEASTMASTER_NAME,
                                              creature::sex::Male,
                                              creature::BodyType::Make_Humanoid(),
                                              creature::race::Human,
                                              creature::role::Beastmaster,
                                              BEASTMASTER_STATS) };

            player::Initial::Setup(bmPtr);
            partyPtr->Add(bmPtr, errMsgIgnored);
        }
       
        {
            const stats::StatSet THEIF_STATS(STAT_BASE_LOW  +     misc::random::Int(STAT_RAND),
                                             STAT_BASE_LOW  +     misc::random::Int(STAT_RAND),
                                             STAT_BASE_LOW  +     misc::random::Int(STAT_RAND),
                                             STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND),
                                             STAT_BASE_HIGH + 7 + misc::random::Int(STAT_RAND),
                                             STAT_BASE_LOW  +     misc::random::Int(STAT_RAND));

            const std::string THEIF_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "T"));

            auto thiefPtr{ new player::Character(THEIF_NAME,
                                                 creature::sex::Male,
                                                 creature::BodyType::Make_Humanoid(),
                                                 creature::race::Gnome,
                                                 creature::role::Thief,
                                                 THEIF_STATS) };

            player::Initial::Setup(thiefPtr);
            partyPtr->Add(thiefPtr, errMsgIgnored);
        }
       
        {
            const stats::StatSet CLERIC_STATS(1             +       misc::random::Int(STAT_RAND),
                                              STAT_BASE_LOW +       misc::random::Int(STAT_RAND),
                                              STAT_BASE_HIGH +      misc::random::Int(STAT_RAND),
                                              STAT_BASE_MED +       misc::random::Int(STAT_RAND),
                                              STAT_BASE_HIGH + 20 + misc::random::Int(STAT_RAND),
                                              STAT_BASE_HIGH +      misc::random::Int(STAT_RAND));

            const std::string CLERIC_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "C"));

            auto clericPtr{ new player::Character(CLERIC_NAME,
                                                  creature::sex::Female,
                                                  creature::BodyType::Make_Pixie(),
                                                  creature::race::Pixie,
                                                  creature::role::Cleric,
                                                  CLERIC_STATS) };

            player::Initial::Setup(clericPtr);
            partyPtr->Add(clericPtr, errMsgIgnored);
        }
       
        {
            const stats::StatSet SYLAVIN_STATS(STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                                               STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                                               STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
                                               STAT_BASE_LOW  + misc::random::Int(STAT_RAND),
                                               STAT_BASE_HIGH + misc::random::Int(STAT_RAND),
                                               STAT_BASE_MED  + misc::random::Int(STAT_RAND));

            const std::string SYLAVIN_NAME(boost::algorithm::replace_last_copy(
                creature::NameInfo::Instance()->LargestName(),
                creature::NameInfo::Instance()->LargestLetterString(),
                "S"));

            auto sylavinPtr{ new player::Character(SYLAVIN_NAME,
                                                   creature::sex::Male,
                                                   creature::BodyType::Make_Dragon(),
                                                   creature::race::Dragon,
                                                   creature::role::Sylavin,
                                                   SYLAVIN_STATS) };

            player::Initial::Setup(sylavinPtr);
            partyPtr->Add(sylavinPtr, errMsgIgnored);
        }
        state::GameStateFactory::Instance()->NewGame(partyPtr);

        //TODO TEMP REMOVE -once finished testing
        //create a fake collection of dead creatures, using the predetermined initial encounter
        combat::Encounter::Instance()->BeginCombatTasks();

        auto const NONPLAYER_CREATURE_PVEC{ creature::Algorithms::NonPlayers() };
        for (auto const NEXT_CREATURE_PTR : NONPLAYER_CREATURE_PVEC)
        {
            combat::Encounter::Instance()->HandleKilledCreature(NEXT_CREATURE_PTR);
        }

        combat::Encounter::Instance()->EndCombatTasks();
        combat::Encounter::Instance()->BeginTreasureStageTasks();

        //corpse image
        {
            sfml_util::LoadTexture(corpseTexture_,
                GameDataFile::Instance()->GetMediaPath(GetCorpseImageKeyFromEnemyParty()));

            corpseSprite_.setTexture(corpseTexture_);

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

        //coins image
        sfml_util::LoadTexture(coinsTexture_,
            GameDataFile::Instance()->GetMediaPath("media-images-coins"));

        coinsSprite_.setTexture(coinsTexture_);

        auto const COINS_IMAGE_WIDTH{ (SCREEN_WIDTH * 0.25f) };

        auto const COINS_SCALE{ (COINS_IMAGE_WIDTH / coinsSprite_.getLocalBounds().width) /
            2.0f };

        coinsSprite_.setScale(COINS_SCALE, COINS_SCALE);

        coinsSprite_.setColor(sf::Color(255, 255, 255, 192));

        //set initial treasure image
        SetupTreasureImage(GetInitialTreasureImageFromEnemyParty());

        //TODO setup initial popup text, either
        // - all enemies ran away so there is no looting or treasure
        // - x bodies lay dead before you on the battlefield, loot their bodies for equipment?
        // - x bodies lay dead before you on the battlefield, but they were animals so they
        //     have no equipment or treasure to loot.  Press a key to continue adventuring.
    }


    void TreasureStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(bgSprite_, STATES);
        target.draw(corpseSprite_, STATES);
        target.draw(treasureSprite_, STATES);
        target.draw(coinsSprite_, STATES);
        Stage::Draw(target, STATES);
    }


    void TreasureStage::SetupTreasureImage(const combat::TreasureImage::Enum E)
    {
        //treasure image
        sfml_util::LoadTexture(treasureTexture_,
            GameDataFile::Instance()->GetMediaPath(combat::TreasureImage::ToKey(E)));

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
        if ((E == combat::TreasureImage::ChestOpen) ||
            (E == combat::TreasureImage::LockboxOpen))
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


    combat::TreasureImage::Enum TreasureStage::GetInitialTreasureImageFromEnemyParty() const
    {
        //TODO
        /*auto const DEAD_ENEMY_CHARACTERS_PVEC{
            combat::Encounter::Instance()->DeadNonPlayerParty().Characters() };

        for (auto const NEXT_ENEMY_CHARACTER_PTR : DEAD_ENEMY_CHARACTERS_PVEC)
        {

        }*/

        return combat::TreasureImage::LockboxOpen;
    }

}
}
