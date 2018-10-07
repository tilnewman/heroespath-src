// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// camp-stage.cpp
//
#include "camp-stage.hpp"

#include "creature/algorithms.hpp"
#include "creature/body-type.hpp"
#include "creature/creature.hpp"
#include "creature/player-party-factory.hpp"
#include "creature/player-party.hpp"
#include "creature/sex-enum.hpp"
#include "game/game-state-factory.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "misc/config-file.hpp"
#include "misc/real.hpp"
#include "misc/vectors.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-manager.hpp"
#include "sfml-util/animation-factory.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui-images.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

namespace heroespath
{
namespace stage
{

    const std::string CampStage::NEWHEROESPATH_POPUP_NAME1_ { "NewGameIntroStoryPopup1" };
    const std::string CampStage::NEWHEROESPATH_POPUP_NAME2_ { "NewGameIntroStoryPopup2" };
    const std::string CampStage::NEWHEROESPATH_POPUP_NAME3_ { "NewGameIntroStoryPopup3" };
    const std::string CampStage::NEWHEROESPATH_POPUP_NAME4_ { "NewGameIntroStoryPopup4" };

    CampStage::CampStage()
        : Stage(
              "Camp",
              { sfml_util::GuiFont::Default,
                sfml_util::GuiFont::System,
                sfml_util::GuiFont::SystemCondensed,
                sfml_util::GuiFont::Number,
                sfml_util::GuiFont::Handwriting },
              true)
        , stageTitle_("")
        , campfireCachedTexture_("media-images-campfire")
        , campfireSprite_(campfireCachedTexture_.Get())
        , backgroundBox_(
              "CampStage'sBackground",
              StageRegion(),
              sfml_util::BoxEntityInfo(sfml_util::CachedTexture(
                  "media-images-backgrounds-tile-darkknot",
                  sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated)))
        , fireAnimUPtr_()
        , showNewGamePopup1_(false)
        , showNewGamePopup2_(false)
        , showNewGamePopup3_(false)
        , showNewGamePopup4_(false)
        , ouroborosUPtr_()
        , botSymbol_()
    {
        sfml_util::SoundManager::Instance()->MusicStart(
            { sfml_util::music::FireOutdoor1, sfml_util::music::FireOutdoor2 });
    }

    CampStage::~CampStage()
    {
        sfml_util::SoundManager::Instance()->MusicStop(
            { sfml_util::music::FireOutdoor1, sfml_util::music::FireOutdoor2 });

        // If the theme music volume was changed just because this was the
        // Camp Stage, then set it back again once leaving the Intro Stage.
        sfml_util::SoundManager::Instance()->MusicVolumeFadeToCurrent(sfml_util::music::Theme);

        Stage::ClearAllEntities();
    }

    bool CampStage::HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t & PACKET_PTR)
    {
        auto const CALLBACK_NAME { PACKET_PTR->Name() };
        if (CALLBACK_NAME == NEWHEROESPATH_POPUP_NAME1_)
        {
            showNewGamePopup2_ = true;
        }
        else if (CALLBACK_NAME == NEWHEROESPATH_POPUP_NAME2_)
        {
            showNewGamePopup3_ = true;
        }
        else if (CALLBACK_NAME == NEWHEROESPATH_POPUP_NAME3_)
        {
            showNewGamePopup4_ = true;
        }

        // if (PACKAGE.Info().Name() == NEWHEROESPATH_POPUP_NAME4_)
        // TODO goto fight encounter

        return true;
    }

    void CampStage::Setup()
    {
        // ouroboros
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("CampStage's");
        EntityAdd(ouroborosUPtr_.get());

        // campfire image
        const auto CAMPFIRE_IMAGE_WIDTH { sfutil::ScreenRatioToPixelsHoriz(0.286f) };

        const auto CAMPFIRE_POS_LEFT { StageRegionWidth()
                                       - sfutil::ScreenRatioToPixelsHoriz(0.06f) };
        const auto CAMPFIRE_POS_TOP { StageRegionHeight()
                                      - sfutil::ScreenRatioToPixelsVert(0.0667f) };

        const sf::FloatRect CAMPFIRE_CONSTRAINING_REGION(
            CAMPFIRE_POS_LEFT, CAMPFIRE_POS_TOP, CAMPFIRE_IMAGE_WIDTH, 0.0f);

        sfutil::FitAndCenterTo(campfireSprite_, CAMPFIRE_CONSTRAINING_REGION);

        // campfire animation
        fireAnimUPtr_
            = sfml_util::AnimationFactory::Make(sfml_util::Animations::Inferno, 1.2f, 0.05f);

        const auto CAMPFIRE_ANIM_POS_V { sfutil::CenterToCopy(
            sfutil::Size(fireAnimUPtr_->GetEntityRegion()), CAMPFIRE_CONSTRAINING_REGION) };

        fireAnimUPtr_->SetEntityPos(CAMPFIRE_ANIM_POS_V);

        EntityAdd(fireAnimUPtr_.get());

        showNewGamePopup1_ = game::Game::Instance()->State().IsNewGame();
    }

    void CampStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundBox_, STATES);
        target.draw(stageTitle_, STATES);
        target.draw(campfireSprite_, STATES);
        target.draw(botSymbol_, STATES);
        Stage::Draw(target, STATES);
    }

    void CampStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        if (showNewGamePopup1_)
        {
            auto const POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME1_,
                std::string("The world of Etan suffers.\n\n").append(ComposeNewGamePopupText1()),
                popup::PopupButtons::Continue,
                popup::PopupImage::Large,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::None) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup1_ = false;
        }
        else if (showNewGamePopup2_)
        {
            auto const POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME2_,
                ComposeNewGamePopupText2(),
                popup::PopupButtons::Continue,
                popup::PopupImage::Large,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::None) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup2_ = false;
        }
        else if (showNewGamePopup3_)
        {
            auto const POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME3_,
                ComposeNewGamePopupText3(),
                popup::PopupButtons::Continue,
                popup::PopupImage::Large,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::None) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup3_ = false;
        }
        else if (showNewGamePopup4_)
        {
            auto const POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME4_,
                ComposeNewGamePopupText4(),
                popup::PopupButtons::Continue,
                popup::PopupImage::Large,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::None) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            showNewGamePopup4_ = false;
        }
    }

    const std::string CampStage::ComposeNewGamePopupText1()
    {
        std::ostringstream ss;
        ss << misc::ConfigFile::Instance()->Value("heroespath-intro-text1");
        return ss.str();
    }

    const std::string CampStage::ComposeNewGamePopupText2()
    {
        std::ostringstream ss;
        ss << misc::ConfigFile::Instance()->Value("heroespath-intro-text2");

        auto const PLAYERS_PVEC(creature::Algorithms::Players());

        auto humansAndGnomesPVec(
            creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Human));

        auto const GNOMES_PVEC(
            creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Gnome));

        misc::Vector::Append(GNOMES_PVEC, humansAndGnomesPVec);

        if (humansAndGnomesPVec.empty() == false)
        {
            ss << " where " << creature::Algorithms::Names(humansAndGnomesPVec) << " call home";
        }

        ss << ".\n\n" << misc::ConfigFile::Instance()->Value("heroespath-intro-text3");

        auto pixiesAndBeastmastersPVec(
            creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Pixie));

        auto const BEASTMASTERS_PVEC(
            creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Beastmaster));

        misc::Vector::Append(
            BEASTMASTERS_PVEC, pixiesAndBeastmastersPVec, misc::Vector::SortOpt::SortAndUnique);

        if (pixiesAndBeastmastersPVec.empty() == false)
        {
            ss << " such as " << creature::Algorithms::Names(pixiesAndBeastmastersPVec) << ",";
        }

        ss << " " << misc::ConfigFile::Instance()->Value("heroespath-intro-text4");
        return ss.str();
    }

    const std::string CampStage::ComposeNewGamePopupText3()
    {
        auto charToUsePtrOpt = creature::CreaturePtrOpt_t { boost::none };

        auto const PLAYERS_PVEC(creature::Algorithms::Players());

        auto const NOTBEASTS_PVEC(creature::Algorithms::FindByIsBeast(
            PLAYERS_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet));

        auto const KNIGHT_PVEC(
            creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Knight));

        if (KNIGHT_PVEC.empty() == false)
        {
            charToUsePtrOpt = KNIGHT_PVEC[0];
        }
        else
        {
            auto const CLERIC_PVEC(
                creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Cleric));

            if (CLERIC_PVEC.empty() == false)
            {
                charToUsePtrOpt = CLERIC_PVEC[0];
            }
            else
            {
                auto const BARD_PVEC(
                    creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Bard));

                if (BARD_PVEC.empty() == false)
                {
                    charToUsePtrOpt = BARD_PVEC[0];
                }
                else
                {
                    auto const SORCERER_PVEC(
                        creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Sorcerer));

                    if (SORCERER_PVEC.empty() == false)
                    {
                        charToUsePtrOpt = SORCERER_PVEC[0];
                    }
                    else
                    {
                        auto const BEASTMASTER_PVEC(creature::Algorithms::FindByRole(
                            PLAYERS_PVEC, creature::role::Beastmaster));

                        if (BEASTMASTER_PVEC.empty() == false)
                        {
                            charToUsePtrOpt = BEASTMASTER_PVEC[0];
                        }
                        else
                        {
                            auto const ARCHER_PVEC(creature::Algorithms::FindByRole(
                                PLAYERS_PVEC, creature::role::Archer));

                            if (ARCHER_PVEC.empty() == false)
                            {
                                charToUsePtrOpt = ARCHER_PVEC[0];
                            }
                            else
                            {
                                auto const HUMAN_PVEC(creature::Algorithms::FindByRace(
                                    PLAYERS_PVEC, creature::race::Human));

                                if (HUMAN_PVEC.empty() == false)
                                {
                                    charToUsePtrOpt = HUMAN_PVEC[0];
                                }
                                else
                                {
                                    auto const GNOME_PVEC(creature::Algorithms::FindByRace(
                                        PLAYERS_PVEC, creature::race::Gnome));

                                    if (GNOME_PVEC.empty() == false)
                                    {
                                        charToUsePtrOpt = GNOME_PVEC[0];
                                    }
                                    else
                                    {
                                        // give up and use the first non beast character
                                        charToUsePtrOpt = NOTBEASTS_PVEC.at(0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        std::ostringstream ss;
        ss << misc::ConfigFile::Instance()->Value("heroespath-intro-text5")
           << charToUsePtrOpt.value()->Name() << " "
           << misc::ConfigFile::Instance()->Value("heroespath-intro-text6") << "  "
           << creature::sex::HeSheIt(charToUsePtrOpt.value()->Sex(), false) << " "
           << misc::ConfigFile::Instance()->Value("heroespath-intro-text7") << " "
           << creature::sex::HimHerIt(charToUsePtrOpt.value()->Sex(), false) << ", but ";

        auto const BEAST_PVEC { creature::Algorithms::FindByIsBeast(PLAYERS_PVEC) };
        auto const NONLOAD_NONBEAST_PVEC { misc::Vector::Exclude(
            NOTBEASTS_PVEC, charToUsePtrOpt.value()) };

        if (NONLOAD_NONBEAST_PVEC.empty())
        {
            ss << "only the beasts ";

            std::size_t i(0);
            bool appendedFirstName(false);
            for (auto const & NEXT_BEAST_PTR : BEAST_PVEC)
            {
                ss << ((appendedFirstName) ? ", " : " ")
                   << (((BEAST_PVEC.size() > 1) && ((BEAST_PVEC.size() - 1) == i++)) ? "and " : "")
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
                ss << ((appendedFirstName) ? ", " : "")
                   << (((NONLOAD_NONBEAST_PVEC.size() > 1)
                        && ((NONLOAD_NONBEAST_PVEC.size() - 1) == i++))
                           ? "and "
                           : "")
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
                for (auto const & NEXT_BEAST_PTR : BEAST_PVEC)
                {
                    ss << ((appendedFirstName) ? ", " : "")
                       << (((BEAST_PVEC.size() > 1) && ((BEAST_PVEC.size() - 1) == i++)) ? "and "
                                                                                         : "")
                       << NEXT_BEAST_PTR->Name() << " ";

                    if (NEXT_BEAST_PTR->Race() == creature::race::Dragon)
                    {
                        ss << "the " << creature::role::ToString(NEXT_BEAST_PTR->Role())
                           << " Dragon";
                    }
                    else
                    {
                        ss << "the Wolfen";
                    }

                    appendedFirstName = true;
                }
            }
        }

        ss << " arrived.\n\n" << misc::ConfigFile::Instance()->Value("heroespath-intro-text8");

        return ss.str();
    }

    const std::string CampStage::ComposeNewGamePopupText4()
    {
        std::ostringstream ss;
        ss << misc::ConfigFile::Instance()->Value("heroespath-intro-text9");
        return ss.str();
    }

} // namespace stage
} // namespace heroespath
