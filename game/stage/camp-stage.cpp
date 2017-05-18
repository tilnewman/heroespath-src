//
// camp-stage.cpp
//
#include "camp-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "sfml-util/animation.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/popup-manager.hpp"

#include "game/game.hpp"
#include "game/game-data-file.hpp"
#include "game/state/game-state.hpp"
#include "game/state/game-state-factory.hpp"
#include "game/loop-manager.hpp"
#include "game/creature/algorithms.hpp"
#include "game/popup-info.hpp"
#include "game/creature/sex-enum.hpp"
#include "game/creature/body-type.hpp"
#include "game/player/character.hpp"
#include "game/player/party.hpp"

#include "utilz/vectors.hpp"
#include "utilz/real.hpp"


namespace game
{
namespace stage
{

    const std::string CampStage::NEWGAME_POPUP_NAME1_("NewGameIntroStoryPopup1");
    const std::string CampStage::NEWGAME_POPUP_NAME2_("NewGameIntroStoryPopup2");
    const std::string CampStage::NEWGAME_POPUP_NAME3_("NewGameIntroStoryPopup3");
    const std::string CampStage::NEWGAME_POPUP_NAME4_("NewGameIntroStoryPopup4");


    CampStage::CampStage()
    :
        Stage              ("Camp"),
        SCREEN_WIDTH_      (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_     (sfml_util::Display::Instance()->GetWinHeight()),
        mainMenuTitle_     (""),
        campfireTexture_   (),
        campfireSprite_    (),
        backgroundImage_   ("media-images-backgrounds-tile-darkknot"),
        fireAnim1SPtr_     (),
        showNewGamePopup1_ (false),
        showNewGamePopup2_ (false),
        showNewGamePopup3_ (false),
        showNewGamePopup4_ (false),
        ouroborosSPtr_     (),
        botSymbol_         ()
    {
        sfml_util::SoundManager::Instance()->MusicStart(sfml_util::music::Fire1);
    }


    CampStage::~CampStage()
    {
        sfml_util::SoundManager::Instance()->MusicStop(sfml_util::music::Fire1);

        //If the theme music volume was changed just because this was the
        //Camp Stage, then set it back again once leaving the Intro Stage.
        sfml_util::MusicOperatorSPtr_t musicOperatorSPtr(sfml_util::SoundManager::Instance()->MusicOperator(sfml_util::music::Theme));
        if (musicOperatorSPtr.get() != nullptr)
        {
            const float CURRENT_VOLUME(musicOperatorSPtr->Volume());
            const float INTENDED_VOLUME(sfml_util::SoundManager::Instance()->MusicVolume());
            if (utilz::IsRealClose(CURRENT_VOLUME, INTENDED_VOLUME) == false)
            {
                if (utilz::IsRealClose(INTENDED_VOLUME, 0.0f))
                    musicOperatorSPtr->VolumeFadeOut();
                else
                    musicOperatorSPtr->VolumeFadeTo(INTENDED_VOLUME, ((CURRENT_VOLUME < INTENDED_VOLUME) ? sfml_util::MusicOperator::FADE_MULT_DEFAULT_IN_ : sfml_util::MusicOperator::FADE_MULT_DEFAULT_OUT_));
            }
        }
    }


    bool CampStage::HandleCallback(const game::callback::PopupResponse & PACKAGE)
    {
        if (PACKAGE.Info().Name() == NEWGAME_POPUP_NAME1_)
            showNewGamePopup2_ = true;
        else if (PACKAGE.Info().Name() == NEWGAME_POPUP_NAME2_)
            showNewGamePopup3_ = true;
        else if (PACKAGE.Info().Name() == NEWGAME_POPUP_NAME3_)
            showNewGamePopup4_ = true;

        //if (PACKAGE.Info().Name() == NEWGAME_POPUP_NAME4_)
            //TODO goto fight encounter

        return true;
    }


    void CampStage::Setup()
    {
        //ouroboros
        ouroborosSPtr_.reset( new Ouroboros("CampStage's") );
        EntityAdd(ouroborosSPtr_);

        //campfire background image
        sfml_util::LoadImageOrTexture<sf::Texture>(campfireTexture_, GameDataFile::Instance()->GetMediaPath("media-images-campfire"));
        campfireSprite_.setTexture(campfireTexture_);
        campfireSprite_.setScale(0.8f, 0.8f);
        campfireSprite_.setPosition(SCREEN_WIDTH_ - campfireSprite_.getGlobalBounds().width - 75.0f, SCREEN_HEIGHT_ - campfireSprite_.getGlobalBounds().height - 60.0f);

        //campfire animation
        fireAnim1SPtr_.reset( new sfml_util::MultiTextureAnimation("Fire1",
                                                                   GameDataFile::Instance()->GetMediaPath("media-anim-images-dir-inferno"),
                                                                   (SCREEN_WIDTH_ - campfireSprite_.getGlobalBounds().width) - 85.0f,
                                                                   (SCREEN_HEIGHT_ - campfireSprite_.getGlobalBounds().height) - 175.0f,
                                                                   0.05f,
                                                                   1.20f,
                                                                   1.20f,
                                                                   sf::Color::White,
                                                                   sf::BlendAdd) );
        EntityAdd(fireAnim1SPtr_);

        //TEMP TODO REMOVE
        player::CharacterSPtr_t c1SPtr( new player::Character("TheWolfen1",    creature::sex::Male,   creature::BodyType::Make_Humanoid(), creature::race::Wolfen,  creature::role::Wolfen) );
        player::CharacterSPtr_t c2SPtr( new player::Character("TheWolfen2",    creature::sex::Male,   creature::BodyType::Make_Wolfen(),   creature::race::Wolfen,  creature::role::Wolfen) );
        player::CharacterSPtr_t c3SPtr( new player::Character("TheWolfen3",    creature::sex::Male,   creature::BodyType::Make_Wolfen(),   creature::race::Wolfen,  creature::role::Wolfen) );
        player::CharacterSPtr_t c4SPtr( new player::Character("TheThief",      creature::sex::Male,   creature::BodyType::Make_Humanoid(), creature::race::Gnome,   creature::role::Thief) );
        player::CharacterSPtr_t c5SPtr( new player::Character("TheCleric",     creature::sex::Female, creature::BodyType::Make_Humanoid(), creature::race::Human,   creature::role::Cleric) );
        player::CharacterSPtr_t c6SPtr( new player::Character("ThBeastmaster", creature::sex::Male,   creature::BodyType::Make_Humanoid(), creature::race::Human,   creature::role::Beastmaster) );
        player::PartySPtr_t partySPtr( new player::Party() );
        partySPtr->Add(c1SPtr);
        partySPtr->Add(c2SPtr);
        partySPtr->Add(c3SPtr);
        partySPtr->Add(c4SPtr);
        partySPtr->Add(c5SPtr);
        partySPtr->Add(c6SPtr);
        state::GameStateSPtr_t gameStateSPtr( new state::GameState(partySPtr) );
        gameStateSPtr->IsNewGameSet(true);
        Game::Instance()->StateSet(gameStateSPtr);

        showNewGamePopup1_ = game::Game::Instance()->State()->IsNewGame();
    }


    void CampStage::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        target.draw(backgroundImage_, states);
        target.draw(mainMenuTitle_, states);
        target.draw(campfireSprite_, states);
        botSymbol_.Draw(target, states);
        Stage::Draw(target, states);
    }


    void CampStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if (showNewGamePopup1_)
        {
            const game::PopupInfo POPUP_INFO( sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(NEWGAME_POPUP_NAME1_,
                                                                                                              std::string("The world of Etan suffers.\n\n").append(ComposeNewGamePopupText1()),
                                                                                                              sfml_util::PopupButtons::Continue,
                                                                                                              sfml_util::PopupImage::Large,
                                                                                                              sfml_util::Justified::Center,
                                                                                                              sfml_util::sound_effect::None) );

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup1_ = false;
        }
        else if (showNewGamePopup2_)
        {
            const game::PopupInfo POPUP_INFO( sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(NEWGAME_POPUP_NAME2_,
                                                                                                              ComposeNewGamePopupText2(),
                                                                                                              sfml_util::PopupButtons::Continue,
                                                                                                              sfml_util::PopupImage::Large,
                                                                                                              sfml_util::Justified::Center,
                                                                                                              sfml_util::sound_effect::None) );

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup2_ = false;
        }
        else if (showNewGamePopup3_)
        {
            const game::PopupInfo POPUP_INFO( sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(NEWGAME_POPUP_NAME3_,
                                                                                                              ComposeNewGamePopupText3(),
                                                                                                              sfml_util::PopupButtons::Continue,
                                                                                                              sfml_util::PopupImage::Large,
                                                                                                              sfml_util::Justified::Center,
                                                                                                              sfml_util::sound_effect::None) );

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup3_ = false;
        }
        else if (showNewGamePopup4_)
        {
            const game::PopupInfo POPUP_INFO( sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(NEWGAME_POPUP_NAME4_,
                                                                                                              ComposeNewGamePopupText4(),
                                                                                                              sfml_util::PopupButtons::Continue,
                                                                                                              sfml_util::PopupImage::Large,
                                                                                                              sfml_util::Justified::Center,
                                                                                                              sfml_util::sound_effect::None) );

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup4_ = false;
        }
    }


    const std::string CampStage::ComposeNewGamePopupText1()
    {
        std::ostringstream ss;
        ss << GameDataFile::Instance()->GetCopyStr("heroespath-intro-text1");
        return ss.str();
    }


    const std::string CampStage::ComposeNewGamePopupText2()
    {
        std::ostringstream ss;
        ss << game::GameDataFile::Instance()->GetCopyStr("heroespath-intro-text2");

        auto const PLAYERS_PVEC( creature::Algorithms::Players() );
        auto humansAndGnomesPVec( creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Human) );
        auto const GNOMES_PVEC( creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Gnome) );
        utilz::Vector::Append(GNOMES_PVEC, humansAndGnomesPVec);

        if (humansAndGnomesPVec.empty() == false)
            ss << " where " << creature::Algorithms::Names(humansAndGnomesPVec) << " call home";

        ss << ".\n\n" << GameDataFile::Instance()->GetCopyStr("heroespath-intro-text3");

        auto pixiesAndBeastmastersPVec( creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Pixie) );
        auto const BEASTMASTERS_PVEC( creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Beastmaster) );
        utilz::Vector::Append(BEASTMASTERS_PVEC, pixiesAndBeastmastersPVec, true);

        if (pixiesAndBeastmastersPVec.empty() == false)
            ss << " such as " << creature::Algorithms::Names(pixiesAndBeastmastersPVec) << ",";

        ss << " " << GameDataFile::Instance()->GetCopyStr("heroespath-intro-text4");
        return ss.str();
    }


    const std::string CampStage::ComposeNewGamePopupText3()
    {
        auto charToUsePtr = creature::CreaturePtr_t{nullptr};
        auto const PLAYERS_PVEC(creature::Algorithms::Players());

        auto const NOTBEASTS_PVEC(creature::Algorithms::FindByBeast(PLAYERS_PVEC, false));
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
        ss  << GameDataFile::Instance()->GetCopyStr("heroespath-intro-text5")
            << charToUsePtr->Name() << " "
            << GameDataFile::Instance()->GetCopyStr("heroespath-intro-text6") << "  "
            << creature::sex::HeSheIt(charToUsePtr->Sex(), false) << " "
            << GameDataFile::Instance()->GetCopyStr("heroespath-intro-text7") << " "
            << creature::sex::HimHerIt(charToUsePtr->Sex(), false) << ", but ";

        auto const BEAST_PVEC(creature::Algorithms::FindByBeast(PLAYERS_PVEC));
        auto const NONLOAD_NONBEAST_PVEC( utilz::Vector::Exclude(NOTBEASTS_PVEC, charToUsePtr) );

        if (NONLOAD_NONBEAST_PVEC.empty())
        {
            ss << "only the beasts ";

            std::size_t i(0);
            bool appendedFirstName(false);
            for (auto const & NEXT_BEAST_PTR: BEAST_PVEC)
            {
                ss  << ((appendedFirstName) ? ", " : " ")
                    << (((BEAST_PVEC.size() > 1) && ((BEAST_PVEC.size() - 1) == i++)) ? "and " : "")
                    << NEXT_BEAST_PTR->Name() << " ";

                if (NEXT_BEAST_PTR->Race().Which() == creature::race::Dragon)
                    ss << "the " << creature::role::ToString(NEXT_BEAST_PTR->Role().Which()) << " Dragon";
                else
                    ss << "the Wolfen";

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
                    << creature::race::ToString(NEXT_CHAR_PTR->Race().Which()) << " " << creature::role::ToString(NEXT_CHAR_PTR->Role().Which());

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

                    if (NEXT_BEAST_SPTR->Race().Which() == creature::race::Dragon)
                        ss << "the " << creature::role::ToString(NEXT_BEAST_SPTR->Role().Which()) << " Dragon";
                    else
                        ss << "the Wolfen";

                    appendedFirstName = true;
                }
            }
        }

        ss << " arrived.\n\n" << GameDataFile::Instance()->GetCopyStr("heroespath-intro-text8");
        return ss.str();
    }


    const std::string CampStage::ComposeNewGamePopupText4()
    {
        std::ostringstream ss;
        ss << GameDataFile::Instance()->GetCopyStr("heroespath-intro-text9");
        return ss.str();
    }

}
}
