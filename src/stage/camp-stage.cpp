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
#include "gui/animation-factory.hpp"
#include "gui/font-manager.hpp"
#include "gui/gui-images.hpp"
#include "gui/ouroboros.hpp"
#include "gui/sound-effects-enum.hpp"
#include "gui/sound-manager.hpp"
#include "misc/config-file.hpp"
#include "misc/real.hpp"
#include "misc/vectors.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-response.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    const std::string CampStage::NEWHEROESPATH_POPUP_NAME1_ { "NewGameIntroStoryPopup1" };
    const std::string CampStage::NEWHEROESPATH_POPUP_NAME2_ { "NewGameIntroStoryPopup2" };
    const std::string CampStage::NEWHEROESPATH_POPUP_NAME3_ { "NewGameIntroStoryPopup3" };
    const std::string CampStage::NEWHEROESPATH_POPUP_NAME4_ { "NewGameIntroStoryPopup4" };

    CampStage::CampStage()
        : StageBase(
              "Camp",
              { gui::GuiFont::Default,
                gui::GuiFont::System,
                gui::GuiFont::SystemCondensed,
                gui::GuiFont::Number,
                gui::GuiFont::Handwriting })
        , stageTitle_(gui::MenuImage::Count)
        , campfireCachedTexture_("media-image-campfire")
        , campfireSprite_(campfireCachedTexture_.Get())
        , backgroundBox_(
              "CampStage'sBackground",
              StageRegion(),
              gui::BoxEntityInfo(gui::CachedTexture(
                  "media-image-background-tile-darkknot",
                  gui::ImageOpt::Default | gui::ImageOpt::Repeated)))
        , fireAnimUPtr_()
        , showNewGamePopup1_(false)
        , showNewGamePopup2_(false)
        , showNewGamePopup3_(false)
        , showNewGamePopup4_(false)
        , ouroborosUPtr_()
        , bottomSymbol_()
    {
        gui::SoundManager::Instance()->MusicStart(
            { gui::music::FireOutdoor1, gui::music::FireOutdoor2 });
    }

    CampStage::~CampStage()
    {
        gui::SoundManager::Instance()->MusicStop(
            { gui::music::FireOutdoor1, gui::music::FireOutdoor2 });

        // If the theme music volume was changed just because this was the
        // Camp Stage, then set it back again once leaving the Intro Stage.
        gui::SoundManager::Instance()->MusicVolumeFadeToCurrent(gui::music::Theme);

        StageBase::ClearAllEntities();
    }

    const std::string CampStage::HandleCallback(
        const misc::PopupCallback_t::Packet_t & PACKET, const std::string & PACKET_DESCRIPTION)
    {
        const auto POPUP_NAME { PACKET.curently_open_popup_name };

        if (POPUP_NAME == NEWHEROESPATH_POPUP_NAME1_)
        {
            showNewGamePopup2_ = true;

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "player closed new game popup #1 and spawned #2");
        }
        else if (POPUP_NAME == NEWHEROESPATH_POPUP_NAME2_)
        {
            showNewGamePopup3_ = true;

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "player closed new game popup #2 and spawned #3");
        }
        else if (POPUP_NAME == NEWHEROESPATH_POPUP_NAME3_)
        {
            showNewGamePopup4_ = true;

            return MakeCallbackHandlerMessage(
                PACKET_DESCRIPTION, "player closed new game popup #3 and spawned #4");
        }

        // if (PACKAGE.Info().Name() == NEWHEROESPATH_POPUP_NAME4_)
        // TODO goto fight encounter

        return MakeCallbackHandlerMessage(
            PACKET_DESCRIPTION, "popup callback NOT HANDLED TODO START GAME");
    }

    void CampStage::Setup()
    {
        // ouroboros
        ouroborosUPtr_ = std::make_unique<gui::Ouroboros>("CampStage's");
        EntityAdd(ouroborosUPtr_);

        // campfire image
        const auto CAMPFIRE_IMAGE_WIDTH { sfutil::ScreenRatioToPixelsHoriz(0.286f) };

        const auto CAMPFIRE_POS_LEFT { StageRegion().width
                                       - sfutil::ScreenRatioToPixelsHoriz(0.06f) };
        const auto CAMPFIRE_POS_TOP { StageRegion().height
                                      - sfutil::ScreenRatioToPixelsVert(0.0667f) };

        const sf::FloatRect CAMPFIRE_CONSTRAINING_REGION(
            CAMPFIRE_POS_LEFT, CAMPFIRE_POS_TOP, CAMPFIRE_IMAGE_WIDTH, 0.0f);

        sfutil::FitAndCenterTo(campfireSprite_, CAMPFIRE_CONSTRAINING_REGION);

        // campfire animation
        fireAnimUPtr_ = gui::AnimationFactory::Make(gui::Animations::Inferno, 1.2f);

        const auto CAMPFIRE_ANIM_POS_V { sfutil::CenterToCopy(
            sfutil::Size(fireAnimUPtr_->GetEntityRegion()), CAMPFIRE_CONSTRAINING_REGION) };

        fireAnimUPtr_->SetEntityPos(CAMPFIRE_ANIM_POS_V);

        EntityAdd(fireAnimUPtr_);

        showNewGamePopup1_ = game::Game::Instance()->State().IsNewGame();
    }

    void CampStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(backgroundBox_, states);
        target.draw(stageTitle_, states);
        target.draw(campfireSprite_, states);
        target.draw(bottomSymbol_, states);
        StageBase::draw(target, states);
    }

    void CampStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);

        if (showNewGamePopup1_)
        {
            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME1_,
                std::string("The world of Etan suffers.\n\n").append(ComposeNewGamePopupText1()),
                popup::PopupButtons::Continue,
                popup::PopupImage::Large,
                gui::Justified::Center,
                gui::sound_effect::None) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
            showNewGamePopup1_ = false;
        }
        else if (showNewGamePopup2_)
        {
            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME2_,
                ComposeNewGamePopupText2(),
                popup::PopupButtons::Continue,
                popup::PopupImage::Large,
                gui::Justified::Center,
                gui::sound_effect::None) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
            showNewGamePopup2_ = false;
        }
        else if (showNewGamePopup3_)
        {
            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME3_,
                ComposeNewGamePopupText3(),
                popup::PopupButtons::Continue,
                popup::PopupImage::Large,
                gui::Justified::Center,
                gui::sound_effect::None) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
            showNewGamePopup3_ = false;
        }
        else if (showNewGamePopup4_)
        {
            const auto POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
                NEWHEROESPATH_POPUP_NAME4_,
                ComposeNewGamePopupText4(),
                popup::PopupButtons::Continue,
                popup::PopupImage::Large,
                gui::Justified::Center,
                gui::sound_effect::None) };

            SpawnPopup(misc::MakeNotNull(this), POPUP_INFO);
            showNewGamePopup4_ = false;
        }
    }

    const std::string CampStage::ComposeNewGamePopupText1()
    {
        return misc::ConfigFile::Instance()->Value("intro-text1");
    }

    const std::string CampStage::ComposeNewGamePopupText2()
    {
        std::string result(misc::ConfigFile::Instance()->Value("intro-text2"));

        const auto PLAYERS_PVEC(creature::Algorithms::Players());

        auto humansAndGnomesPVec(
            creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Human));

        const auto GNOMES_PVEC(
            creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Gnome));

        misc::Vector::Append(GNOMES_PVEC, humansAndGnomesPVec);

        if (humansAndGnomesPVec.empty() == false)
        {
            result += " where " + creature::Algorithms::Names(humansAndGnomesPVec) + " call home";
        }

        result += ".\n\n" + misc::ConfigFile::Instance()->Value("intro-text3");

        auto pixiesAndBeastmastersPVec(
            creature::Algorithms::FindByRace(PLAYERS_PVEC, creature::race::Pixie));

        const auto BEASTMASTERS_PVEC(
            creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Beastmaster));

        misc::Vector::Append(
            BEASTMASTERS_PVEC, pixiesAndBeastmastersPVec, misc::Vector::SortOpt::SortAndUnique);

        if (pixiesAndBeastmastersPVec.empty() == false)
        {
            result += " such as " + creature::Algorithms::Names(pixiesAndBeastmastersPVec) + ",";
        }

        result += " " + misc::ConfigFile::Instance()->Value("intro-text4");
        return result;
    }

    const std::string CampStage::ComposeNewGamePopupText3()
    {
        auto charToUsePtrOpt = creature::CreaturePtrOpt_t { boost::none };

        const auto PLAYERS_PVEC(creature::Algorithms::Players());

        const auto NOTBEASTS_PVEC(creature::Algorithms::FindByIsBeast(
            PLAYERS_PVEC, creature::Algorithms::CriteriaOpt::DoesNotMeet));

        const auto KNIGHT_PVEC(
            creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Knight));

        if (KNIGHT_PVEC.empty() == false)
        {
            charToUsePtrOpt = KNIGHT_PVEC[0];
        }
        else
        {
            const auto CLERIC_PVEC(
                creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Cleric));

            if (CLERIC_PVEC.empty() == false)
            {
                charToUsePtrOpt = CLERIC_PVEC[0];
            }
            else
            {
                const auto BARD_PVEC(
                    creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Bard));

                if (BARD_PVEC.empty() == false)
                {
                    charToUsePtrOpt = BARD_PVEC[0];
                }
                else
                {
                    const auto SORCERER_PVEC(
                        creature::Algorithms::FindByRole(PLAYERS_PVEC, creature::role::Sorcerer));

                    if (SORCERER_PVEC.empty() == false)
                    {
                        charToUsePtrOpt = SORCERER_PVEC[0];
                    }
                    else
                    {
                        const auto BEASTMASTER_PVEC(creature::Algorithms::FindByRole(
                            PLAYERS_PVEC, creature::role::Beastmaster));

                        if (BEASTMASTER_PVEC.empty() == false)
                        {
                            charToUsePtrOpt = BEASTMASTER_PVEC[0];
                        }
                        else
                        {
                            const auto ARCHER_PVEC(creature::Algorithms::FindByRole(
                                PLAYERS_PVEC, creature::role::Archer));

                            if (ARCHER_PVEC.empty() == false)
                            {
                                charToUsePtrOpt = ARCHER_PVEC[0];
                            }
                            else
                            {
                                const auto HUMAN_PVEC(creature::Algorithms::FindByRace(
                                    PLAYERS_PVEC, creature::race::Human));

                                if (HUMAN_PVEC.empty() == false)
                                {
                                    charToUsePtrOpt = HUMAN_PVEC[0];
                                }
                                else
                                {
                                    const auto GNOME_PVEC(creature::Algorithms::FindByRace(
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

        auto & charToUseCreaturePtr { charToUsePtrOpt.value() };

        std::ostringstream ss;
        ss << misc::ConfigFile::Instance()->Value("intro-text5") << charToUseCreaturePtr->Name()
           << " " << misc::ConfigFile::Instance()->Value("intro-text6") << "  "
           << creature::sex::HeSheIt(charToUseCreaturePtr->Sex(), false) << " "
           << misc::ConfigFile::Instance()->Value("intro-text7") << " "
           << creature::sex::HimHerIt(charToUseCreaturePtr->Sex(), false) << ", but ";

        const auto BEAST_PVEC { creature::Algorithms::FindByIsBeast(PLAYERS_PVEC) };
        const auto NONLOAD_NONBEAST_PVEC { misc::Vector::Exclude(
            NOTBEASTS_PVEC, charToUseCreaturePtr) };

        if (NONLOAD_NONBEAST_PVEC.empty())
        {
            ss << "only the beasts ";

            std::size_t i(0);
            bool appendedFirstName(false);
            for (const auto & NEXT_BEAST_PTR : BEAST_PVEC)
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
            for (const auto & NEXT_CHAR_PTR : NONLOAD_NONBEAST_PVEC)
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
                for (const auto & NEXT_BEAST_PTR : BEAST_PVEC)
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

        ss << " arrived.\n\n" << misc::ConfigFile::Instance()->Value("intro-text8");

        return ss.str();
    }

    const std::string CampStage::ComposeNewGamePopupText4()
    {
        return misc::ConfigFile::Instance()->Value("intro-text9");
    }

} // namespace stage
} // namespace heroespath
