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
#include "gui/display.hpp"
#include "gui/font-enum.hpp"
#include "stage/adventure-display-stage.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    AdventureStage::AdventureStage()
        : StageBase(
            "Adventure",
            { gui::GuiFont::Default,
              gui::GuiFont::System,
              gui::GuiFont::Number,
              gui::GuiFont::SystemCondensed,
              gui::GuiFont::Number,
              gui::GuiFont::DialogMedieval,
              gui::GuiFont::DialogModern,
              gui::GuiFont::DefaultBoldFlavor,
              gui::GuiFont::Handwriting },
            { gui::sound_effect::FootstepGrass,
              gui::sound_effect::FootstepGravel,
              gui::sound_effect::FootstepLeaves,
              gui::sound_effect::FootstepSolid,
              gui::sound_effect::FootstepWood })
    {}

    AdventureStage::~AdventureStage() = default;

} // namespace stage
} // namespace heroespath
