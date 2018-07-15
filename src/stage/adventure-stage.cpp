// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// adventure-stage.cpp
//
#include "adventure-stage.hpp"

#include "creature/creature.hpp"
#include "creature/player-party-factory.hpp"
#include "creature/player-party.hpp"
#include "game/game-state-factory.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-enum.hpp"
#include "stage/adventure-display-stage.hpp"

namespace heroespath
{
namespace stage
{

    AdventureStage::AdventureStage()
        : Stage(
              "Adventure",
              0.0f,
              0.0f,
              sfml_util::Display::Instance()->GetWinWidth(),
              sfml_util::Display::Instance()->GetWinHeight(),
              { sfml_util::Font::Default,
                sfml_util::Font::System,
                sfml_util::Font::Number,
                sfml_util::Font::SystemCondensed,
                sfml_util::Font::Number,
                sfml_util::Font::DialogMedieval,
                sfml_util::Font::DialogModern,
                sfml_util::Font::DefaultBoldFlavor,
                sfml_util::Font::Handwriting },
              true,
              { sfml_util::sound_effect::FootstepGrass,
                sfml_util::sound_effect::FootstepGravel,
                sfml_util::sound_effect::FootstepLeaves,
                sfml_util::sound_effect::FootstepSolid,
                sfml_util::sound_effect::FootstepWood })
        , interactionManager_()
        , adventureDisplayStagePtr_(new AdventureDisplayStage(interactionManager_))
    {}

    AdventureStage::~AdventureStage() = default;

    void AdventureStage::Setup()
    {
        adventureDisplayStagePtr_->Setup();

        // See AdventureDisplayStage::Setup() for where this is actually done.
        // The AdventureDisplayStage gives itself to the LoopManager so that it
        // can do so before adding its own substages, so that it will be drawn
        // before those substages.
        // game::LoopManager::Instance()->AddStage(adventureDisplayStagePtr_);
    }

    void AdventureStage::UpdateTime(const float) { interactionManager_.Update(); }

} // namespace stage
} // namespace heroespath
