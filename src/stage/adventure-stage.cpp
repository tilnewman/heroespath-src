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
#include "sfml-util/display.hpp"
#include "sfml-util/font-enum.hpp"
#include "stage/adventure-display-stage.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace stage
{

    AdventureStage::AdventureStage()
        : StageBase(
            "Adventure",
            { sfml_util::GuiFont::Default,
              sfml_util::GuiFont::System,
              sfml_util::GuiFont::Number,
              sfml_util::GuiFont::SystemCondensed,
              sfml_util::GuiFont::Number,
              sfml_util::GuiFont::DialogMedieval,
              sfml_util::GuiFont::DialogModern,
              sfml_util::GuiFont::DefaultBoldFlavor,
              sfml_util::GuiFont::Handwriting },
            true,
            { sfml_util::sound_effect::FootstepGrass,
              sfml_util::sound_effect::FootstepGravel,
              sfml_util::sound_effect::FootstepLeaves,
              sfml_util::sound_effect::FootstepSolid,
              sfml_util::sound_effect::FootstepWood })
    {}

    AdventureStage::~AdventureStage() = default;

} // namespace stage
} // namespace heroespath
