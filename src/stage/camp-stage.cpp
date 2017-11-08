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
// camp-stage.cpp
//
#include "camp-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/gui/gui-elements.hpp"

#include "popup/popup-info.hpp"
#include "popup/popup-manager.hpp"

#include "game/game.hpp"
#include "game/game-data-file.hpp"
#include "state/game-state.hpp"
#include "state/game-state-factory.hpp"
#include "game/loop-manager.hpp"
#include "creature/algorithms.hpp"
#include "creature/sex-enum.hpp"
#include "creature/body-type.hpp"
#include "player/character.hpp"
#include "player/party.hpp"
#include "sfml-util/ouroboros.hpp"

#include "misc/vectors.hpp"
#include "misc/real.hpp"


namespace heroespath
{
namespace stage
{

    const std::string CampStage::NEWHEROESPATH_POPUP_NAME1_{ "NewGameIntroStoryPopup1" };
    const std::string CampStage::NEWHEROESPATH_POPUP_NAME2_{ "NewGameIntroStoryPopup2" };
    const std::string CampStage::NEWHEROESPATH_POPUP_NAME3_{ "NewGameIntroStoryPopup3" };
    const std::string CampStage::NEWHEROESPATH_POPUP_NAME4_{ "NewGameIntroStoryPopup4" };


    CampStage::CampStage()
    :
        Stage              ("Camp"),
        SCREEN_WIDTH_      (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_     (sfml_util::Display::Instance()->GetWinHeight()),
        mainMenuTitle_     (""),
        campfireTexture_   (),
        campfireSprite_    (),
        backgroundImage_   ("media-images-backgrounds-tile-darkknot"),
        fireAnimUPtr_      (),
        showNewGamePopup1_ (false),
        showNewGamePopup2_ (false),
        showNewGamePopup3_ (false),
        showNewGamePopup4_ (false),
        ouroborosUPtr_     (),
        botSymbol_         ()
    {
        sfml_util::SoundManager::Instance()->MusicStart(sfml_util::music::Fire1);
    }


    CampStage::~CampStage()
    {
        sfml_util::SoundManager::Instance()->MusicStop(sfml_util::music::Fire1);

        //If the theme music volume was changed just because this was the
        //Camp Stage, then set it back again once leaving the Intro Stage.
        sfml_util::SoundManager::Instance()->MusicVolumeFadeToCurrent(sfml_util::music::Theme);

        ClearAllEntities();
    }


    bool CampStage::HandleCallback(const heroespath::popup::PopupResponse & PACKAGE)
    {
        if (PACKAGE.Info().Name() == NEWHEROESPATH_POPUP_NAME1_)
        {
            showNewGamePopup2_ = true;
        }
        else if (PACKAGE.Info().Name() == NEWHEROESPATH_POPUP_NAME2_)
        {
            showNewGamePopup3_ = true;
        }
        else if (PACKAGE.Info().Name() == NEWHEROESPATH_POPUP_NAME3_)
        {
            showNewGamePopup4_ = true;
        }

        //if (PACKAGE.Info().Name() == NEWHEROESPATH_POPUP_NAME4_)
            //TODO goto fight encounter

        return true;
    }


    void CampStage::Setup()
    {
        //ouroboros
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("CampStage's");
        EntityAdd(ouroborosUPtr_.get());

        //campfire background image
        sfml_util::LoadTexture(
            campfireTexture_,
            game::GameDataFile::Instance()->GetMediaPath("media-images-campfire"));

        campfireSprite_.setTexture(campfireTexture_);
        campfireSprite_.setScale(0.8f, 0.8f);

        auto const CAMPFIRE_BOUNDS_BEFORE{ campfireSprite_.getGlobalBounds() };

        campfireSprite_.setPosition(
            SCREEN_WIDTH_ - CAMPFIRE_BOUNDS_BEFORE.width - 75.0f,
            SCREEN_HEIGHT_ - CAMPFIRE_BOUNDS_BEFORE.height - 60.0f);

        //campfire animation
        fireAnimUPtr_ = sfml_util::AnimationFactory::Make(
            sfml_util::Animations::Inferno,
            1.2f,
            0.05f);

        fireAnimUPtr_->SetEntityPos(
            (SCREEN_WIDTH_ - campfireSprite_.getGlobalBounds().width) - 85.0f,
            (SCREEN_HEIGHT_ - campfireSprite_.getGlobalBounds().height) - 175.0f);

        EntityAdd(fireAnimUPtr_.get());

        //TEMP TODO REMOVE -make a party to test this stage with
        auto c1Ptr{ new player::Character("TheWolfen1",    creature::sex::Male,   creature::BodyType::Make_Humanoid(), creature::race::Wolfen,  creature::role::Wolfen) };
        auto c2Ptr{ new player::Character("TheWolfen2",    creature::sex::Male,   creature::BodyType::Make_Wolfen(),   creature::race::Wolfen,  creature::role::Wolfen) };
        auto c3Ptr{ new player::Character("TheWolfen3",    creature::sex::Male,   creature::BodyType::Make_Wolfen(),   creature::race::Wolfen,  creature::role::Wolfen) };
        auto c4Ptr{ new player::Character("TheThief",      creature::sex::Male,   creature::BodyType::Make_Humanoid(), creature::race::Gnome,   creature::role::Thief) };
        auto c5Ptr{ new player::Character("TheCleric",     creature::sex::Female, creature::BodyType::Make_Humanoid(), creature::race::Human,   creature::role::Cleric) };
        auto c6Ptr{ new player::Character("ThBeastmaster", creature::sex::Male,   creature::BodyType::Make_Humanoid(), creature::race::Human,   creature::role::Beastmaster) };
        player::PartyPtr_t partyPtr{ new player::Party() };
        std::string errMsgIgnored{ "" };
        partyPtr->Add(c1Ptr, errMsgIgnored);
        partyPtr->Add(c2Ptr, errMsgIgnored);
        partyPtr->Add(c3Ptr, errMsgIgnored);
        partyPtr->Add(c4Ptr, errMsgIgnored);
        partyPtr->Add(c5Ptr, errMsgIgnored);
        partyPtr->Add(c6Ptr, errMsgIgnored);
        auto gameStatePtr = new state::GameState(partyPtr);
        gameStatePtr->IsNewGameSet(true);
        game::Game::Instance()->StateStore(gameStatePtr);

        showNewGamePopup1_ = game::Game::Instance()->State().IsNewGame();
    }


    void CampStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(mainMenuTitle_, STATES);
        target.draw(campfireSprite_, STATES);
        target.draw(botSymbol_, STATES);
        Stage::Draw(target, STATES);
    }


    void CampStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if (showNewGamePopup1_)
        {
            auto const POPUP_INFO{ heroespath::popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME1_,
                std::string("The world of Etan suffers.\n\n").append(ComposeNewGamePopupText1()),
                heroespath::popup::PopupButtons::Continue,
                heroespath::popup::PopupImage::Large,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::None) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup1_ = false;
        }
        else if (showNewGamePopup2_)
        {
            auto const POPUP_INFO{ heroespath::popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME2_,
                ComposeNewGamePopupText2(),
                heroespath::popup::PopupButtons::Continue,
                heroespath::popup::PopupImage::Large,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::None) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup2_ = false;
        }
        else if (showNewGamePopup3_)
        {
            auto const POPUP_INFO{ heroespath::popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME3_,
                ComposeNewGamePopupText3(),
                heroespath::popup::PopupButtons::Continue,
                heroespath::popup::PopupImage::Large,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::None) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup3_ = false;
        }
        else if (showNewGamePopup4_)
        {
            auto const POPUP_INFO{ heroespath::popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME4_,
                ComposeNewGamePopupText4(),
                heroespath::popup::PopupButtons::Continue,
                heroespath::popup::PopupImage::Large,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::None) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup4_ = false;
        }
    }


    const std::string CampStage::ComposeNewGamePopupText1()
    {
        std::ostringstream ss;
        ss << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text1");
        return ss.str();
    }


    const std::string CampStage::ComposeNewGamePopupText2()
    {
        std::ostringstream ss;
        ss << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text2");

        auto const PLAYERS_PVEC( creature::Algorithms::Players() );

        auto humansAndGnomesPVec( creature::Algorithms::FindByRace(
            PLAYERS_PVEC, creature::race::Human) );

        auto const GNOMES_PVEC( creature::Algorithms::FindByRace(
            PLAYERS_PVEC, creature::race::Gnome) );

        misc::Vector::Append(GNOMES_PVEC, humansAndGnomesPVec);

        if (humansAndGnomesPVec.empty() == false)
        {
            ss << " where " << creature::Algorithms::Names(humansAndGnomesPVec) << " call home";
        }

        ss << ".\n\n" << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text3");

        auto pixiesAndBeastmastersPVec( creature::Algorithms::FindByRace(
            PLAYERS_PVEC, creature::race::Pixie) );

        auto const BEASTMASTERS_PVEC( creature::Algorithms::FindByRole(
            PLAYERS_PVEC, creature::role::Beastmaster) );

        misc::Vector::Append(
            BEASTMASTERS_PVEC,
            pixiesAndBeastmastersPVec,
            misc::Vector::SortOpt::SortAndUnique);

        if (pixiesAndBeastmastersPVec.empty() == false)
        {
            ss << " such as " << creature::Algorithms::Names(pixiesAndBeastmastersPVec) << ",";
        }

        ss << " " << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text4");
        return ss.str();
    }


    const std::string CampStage::ComposeNewGamePopupText3()
    {
        auto charToUsePtr = creature::CreaturePtr_t{nullptr};
        auto const PLAYERS_PVEC(creature::Algorithms::Players());

        auto const NOTBEASTS_PVEC(creature::Algorithms::FindByIsBeast(
            PLAYERS_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet));

        auto const KNIGHT_PVEC(creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Knight));

        if (KNIGHT_PVEC.empty() == false)
        {
            charToUsePtr = KNIGHT_PVEC[0];
        }
        else
        {
            auto const CLERIC_PVEC(creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Cleric));
            if (CLERIC_PVEC.empty() == false)
            {
                charToUsePtr = CLERIC_PVEC[0];
            }
            else
            {
                auto const BARD_PVEC(creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Bard));
                if (BARD_PVEC.empty() == false)
                {
                    charToUsePtr = BARD_PVEC[0];
                }
                else
                {
                    auto const SORCERER_PVEC(creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Sorcerer));
                    if (SORCERER_PVEC.empty() == false)
                    {
                        charToUsePtr = SORCERER_PVEC[0];
                    }
                    else
                    {
                        auto const BEASTMASTER_PVEC(creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Beastmaster));
                        if (BEASTMASTER_PVEC.empty() == false)
                        {
                            charToUsePtr = BEASTMASTER_PVEC[0];
                        }
                        else
                        {
                            auto const ARCHER_PVEC(creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Archer));
                            if (ARCHER_PVEC.empty() == false)
                            {
                                charToUsePtr = ARCHER_PVEC[0];
                            }
                            else
                            {
                                auto const HUMAN_PVEC(creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Human));
                                if (HUMAN_PVEC.empty() == false)
                                {
                                    charToUsePtr = HUMAN_PVEC[0];
                                }
                                else
                                {
                                    auto const GNOME_PVEC(creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Gnome));
                                    if (GNOME_PVEC.empty() == false)
                                    {
                                        charToUsePtr = GNOME_PVEC[0];
                                    }
                                    else
                                    {
                                        //give up and use the first non beast character
                                        charToUsePtr = NOTBEASTS_PVEC.at(0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        std::ostringstream ss;
        ss  << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text5")
            << charToUsePtr->Name() << " "
            << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text6") << "  "
            << creature::sex::HeSheIt(charToUsePtr->Sex(), false) << " "
            << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text7") << " "
            << creature::sex::HimHerIt(charToUsePtr->Sex(), false) << ", but ";

        auto const BEAST_PVEC{ creature::Algorithms::FindByIsBeast(PLAYERS_PVEC) };
        auto const NONLOAD_NONBEAST_PVEC{ misc::Vector::Exclude(NOTBEASTS_PVEC, charToUsePtr) };

        if (NONLOAD_NONBEAST_PVEC.empty())
        {
            ss << "only the beasts ";

            std::size_t i(0);
            bool appendedFirstName(false);
            for (auto const & NEXT_BEAST_PTR: BEAST_PVEC)
            {
                ss  << ((appendedFirstName) ? ", " : " ")
                    << (((BEAST_PVEC.size() > 1) && ((BEAST_PVEC.size() - 1) == i++)) ? "and " :"")
                    << NEXT_BEAST_PTR->Name() << " ";

                if (NEXT_BEAST_PTR->Race() == creature::race::Dragon)
                {
                    ss << "the " << creature::role::ToString(NEXT_BEAST_PTR->Role()) << " Dragon";
                }
                else
                {
                    ss << "the Wolfen";
                }

                appendedFirstName = true;
            }
        }
        else
        {
            ss << " only ";

            std::size_t i(0);
            bool appendedFirstName(false);
            for (auto const & NEXT_CHAR_PTR : NONLOAD_NONBEAST_PVEC)
            {
                ss  << ((appendedFirstName) ? ", " : "")
                    << (((NONLOAD_NONBEAST_PVEC.size() > 1) && ((NONLOAD_NONBEAST_PVEC.size() - 1) == i++)) ? "and " : "")
                    << NEXT_CHAR_PTR->Name() << " the "
                    << creature::race::ToString(NEXT_CHAR_PTR->Race()) << " "
                    << creature::role::ToString(NEXT_CHAR_PTR->Role());

                appendedFirstName = true;
            }

            if (BEAST_PVEC.empty() == false)
            {
                ss << ", along with the beast" << ((BEAST_PVEC.size() > 1) ? "s" : "");

                i = 0;
                appendedFirstName = false;
                for (auto const & NEXT_BEAST_SPTR : BEAST_PVEC)
                {
                    ss << ((appendedFirstName) ? ", " : "")
                        << (((BEAST_PVEC.size() > 1) && ((BEAST_PVEC.size() - 1) == i++)) ? "and " : "")
                        << NEXT_BEAST_SPTR->Name() << " ";

                    if (NEXT_BEAST_SPTR->Race() == creature::race::Dragon)
                    {
                        ss << "the " << creature::role::ToString(NEXT_BEAST_SPTR->Role()) << " Dragon";
                    }
                    else
                    {
                        ss << "the Wolfen";
                    }

                    appendedFirstName = true;
                }
            }
        }

        ss << " arrived.\n\n" << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text8");
        return ss.str();
    }


    const std::string CampStage::ComposeNewGamePopupText4()
    {
        std::ostringstream ss;
        ss << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text9");
        return ss.str();
    }

}
}
