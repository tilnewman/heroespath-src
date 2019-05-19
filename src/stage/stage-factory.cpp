// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// stage-factory.cpp
//
#include "stage-factory.hpp"

#include "combat/combat-anim.hpp"
#include "combat/combat-display.hpp"
#include "gui/cloud-animation.hpp"
#include "gui/song-animation.hpp"
#include "gui/sparkle-animation.hpp"
#include "gui/sparks-animation.hpp"
#include "gui/text-animation.hpp"
#include "popup/popup-stage-char-select.hpp"
#include "popup/popup-stage-combat-over.hpp"
#include "popup/popup-stage-generic.hpp"
#include "popup/popup-stage-image-fade.hpp"
#include "popup/popup-stage-image-select.hpp"
#include "popup/popup-stage-inventory-prompt.hpp"
#include "popup/popup-stage-musicsheet.hpp"
#include "popup/popup-stage-num-select.hpp"
#include "popup/popup-stage-res-change.hpp"
#include "popup/popup-stage-spellbook.hpp"
#include "popup/popup-stage-system-error.hpp"
#include "popup/popup-stage-treasure-trap.hpp"
#include "stage/adventure-display-stage.hpp"
#include "stage/adventure-stage-interact-stage.hpp"
#include "stage/adventure-stage.hpp"
#include "stage/camp-stage.hpp"
#include "stage/character-stage.hpp"
#include "stage/combat-stage.hpp"
#include "stage/credits-stage.hpp"
#include "stage/inn-stage.hpp"
#include "stage/intro-stage.hpp"
#include "stage/inventory-stage.hpp"
#include "stage/load-game-menu-stage.hpp"
#include "stage/main-menu-stage.hpp"
#include "stage/party-stage.hpp"
#include "stage/settings-stage.hpp"
#include "stage/treasure-display-stage.hpp"
#include "stage/treasure-stage.hpp"

#include <sstream>
#include <stdexcept>

namespace heroespath
{
namespace stage
{

    IStageUVec_t StageFactory::Make(const SetupPacket & SETUP_PACKET)
    {
        IStageUVec_t stageVec;

        try
        {
            stageVec = CreateAll(SETUP_PACKET);
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_ERR(
                "Exception=\"" << EXCEPTION.what() << "\" thrown during "
                               << NAMEOF_ENUM(SETUP_PACKET.stage)
                               << " stage creation, which is fatal.  Re-Throwing "
                                  "to kill the game.");

            throw;
        }

        SetupAll(stageVec);

        return stageVec;
    }

    IStageUVec_t StageFactory::CreateAll(const SetupPacket & SETUP_PACKET)
    {
        IStageUVec_t stageVec;

        switch (SETUP_PACKET.stage)
        {
            case stage::Stage::Settings:
            {
                stageVec.emplace_back(std::make_unique<stage::SettingsStage>());
                break;
            }
            case stage::Stage::Menu:
            {
                stageVec.emplace_back(std::make_unique<stage::MainMenuStage>());
                break;
            }
            case stage::Stage::Intro:
            {
                stageVec.emplace_back(std::make_unique<stage::IntroStage>());
                break;
            }
            case stage::Stage::Credits:
            {
                stageVec.emplace_back(std::make_unique<stage::CreditsStage>());
                break;
            }
            case stage::Stage::Party:
            {
                stageVec.emplace_back(std::make_unique<stage::PartyStage>());
                break;
            }
            case stage::Stage::Camp:
            {
                stageVec.emplace_back(std::make_unique<stage::CampStage>());
                break;
            }
            case stage::Stage::Load:
            {
                stageVec.emplace_back(std::make_unique<stage::LoadGameStage>());
                break;
            }
            case stage::Stage::Character:
            {
                stageVec.emplace_back(std::make_unique<stage::CharacterStage>());
                break;
            }
            case stage::Stage::Inn:
            {
                stageVec.emplace_back(std::make_unique<stage::InnStage>());
                break;
            }
            case stage::Stage::Combat:
            {
                combat::CombatAnimationUPtr_t combatAnimUPtr {
                    std::make_unique<combat::CombatAnimation>()
                };

                auto viewStageUPtr { std::make_unique<combat::CombatDisplay>(
                    combat::CombatAnimationPtr_t(combatAnimUPtr.get())) };

                auto modelStageUPtr { std::make_unique<stage::CombatStage>(
                    std::move(combatAnimUPtr),
                    combat::CombatDisplayPtr_t(viewStageUPtr.get()),
                    SETUP_PACKET.will_advance_turn) };

                modelStageUPtr->PreSetup();
                viewStageUPtr->StageRegion(modelStageUPtr->CombatRegion());

                // view must be first to be Setup() and processes by Loop::Execute()
                stageVec.emplace_back(std::move(viewStageUPtr));
                stageVec.emplace_back(std::move(modelStageUPtr));

                break;
            }
            case stage::Stage::Treasure:
            {
                auto viewStageUPtr { std::make_unique<stage::TreasureDisplayStage>() };
                auto modelStageUPtr { std::make_unique<stage::TreasureStage>() };

                viewStageUPtr->SetModelStage(TreasureStagePtr_t(modelStageUPtr.get()));
                modelStageUPtr->SetViewStage(TreasureDisplayStagePtr_t(viewStageUPtr.get()));

                // view must be first to be Setup() and processes by Loop::Execute()
                stageVec.emplace_back(std::move(viewStageUPtr));
                stageVec.emplace_back(std::move(modelStageUPtr));

                break;
            }
            case stage::Stage::Inventory:
            {
                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (SETUP_PACKET.inventory_packet_opt),
                    "stage::StageFactory::Make(setup_packet="
                        << SETUP_PACKET.ToString() << ") has no inventory setup information.");

                stageVec.emplace_back(std::make_unique<stage::InventoryStage>(
                    SETUP_PACKET.inventory_packet_opt.value().turn_creature_ptr,
                    SETUP_PACKET.inventory_packet_opt.value().inventory_creature_ptr,
                    SETUP_PACKET.inventory_packet_opt.value().prev_calling_phase));

                break;
            }
            case stage::Stage::Adventure:
            {
                auto modelStageUPtr { std::make_unique<stage::AdventureStage>() };

                auto viewStageUPtr { std::make_unique<stage::AdventureDisplayStage>() };

                auto interactStageUPtr { std::make_unique<stage::InteractStage>(
                    viewStageUPtr->GetInteractionManagerForStageFactory()) };

                interactStageUPtr->PreSetup(
                    viewStageUPtr->CalcInteractRegionForStageFactory(),
                    viewStageUPtr->GetMapForStageFactory());

                // view must be first to be Setup() and processes by Loop::Execute()
                stageVec.emplace_back(std::move(viewStageUPtr));
                stageVec.emplace_back(std::move(interactStageUPtr));
                stageVec.emplace_back(std::move(modelStageUPtr));

                break;
            }
            case stage::Stage::None:
            case stage::Stage::Previous:
            case stage::Stage::Exit:
            case stage::Stage::Popup:
            case stage::Stage::Next:
            case stage::Stage::Save:
            case stage::Stage::Help:
            case stage::Stage::Count:
            default:
            {
                std::ostringstream ss;

                ss << "stage::StageFactory::Make(setup_packet=" << SETUP_PACKET.ToString()
                   << ") but that stage is invalid.";

                throw std::runtime_error(ss.str());
            }
        }

        return stageVec;
    }

    void StageFactory::SetupAll(IStageUVec_t & stageVec)
    {
        for (auto & iStageUPtr : stageVec)
        {
            try
            {
                iStageUPtr->Setup();
            }
            catch (const std::exception & EXCEPTION)
            {
                M_HP_LOG_ERR(
                    "Exception=\"" << EXCEPTION.what() << "\" thrown during "
                                   << iStageUPtr->GetStageName()
                                   << " stage setup, which is fatal.  Re-Throwing "
                                      "to kill the game.");

                throw;
            }
        }
    }

    IStageUPtr_t StageFactory::MakePopup(const popup::PopupInfo & POPUP_INFO)
    {
        switch (POPUP_INFO.Stage())
        {
            case popup::PopupStage::Generic:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageGeneric>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::CharacterSelect:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageCharacterSelect>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::CombatOver:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageCombatOver>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::ImageFade:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageImageFade>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::ImageSelect:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageImageSelect>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::InventoryPrompt:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageInventoryPrompt>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::Musicsheet:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageMusicSheet>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::NumberSelect:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageNumberSelect>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::ResolutionChange:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageResChange>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::Spellbook:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageSpellbook>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::SystemError:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageSystemError>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::TreasureTrap:
            {
                IStageUPtr_t popupStageUPtr { std::make_unique<popup::PopupStageTreasureTrap>(
                    POPUP_INFO) };

                popupStageUPtr->Setup();
                return popupStageUPtr;
            }
            case popup::PopupStage::Count:
            default:
            {
                std::ostringstream ss;
                ss << "stage::StageFactory::MakePopup(popup_info={" << POPUP_INFO.ToStringShort()
                   << "}) but the stage=" << NAMEOF_ENUM(POPUP_INFO.Stage())
                   << ") is invalid (out of bounds).";

                throw std::range_error(ss.str());
            }
        }
    }

} // namespace stage
} // namespace heroespath
