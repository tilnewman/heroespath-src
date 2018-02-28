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
// adventure-stage.cpp
//
#include "adventure-stage.hpp"

#include "game/loop-manager.hpp"
#include "game/game.hpp"
#include "state/game-state.hpp"
#include "state/game-state-factory.hpp"
#include "player/party.hpp"
#include "player/party-factory.hpp"
#include "player/character.hpp"
#include "stage/adventure-display-stage.hpp"

#include "sfml-util/display.hpp"


namespace heroespath
{
namespace stage
{

    AdventureStage::AdventureStage()
    :
        Stage(
            "Adventure",
            0.0f,
            0.0f,
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight()),
        adventureDisplayStagePtr_(nullptr)
    {}


    AdventureStage::~AdventureStage()
    {}


    void AdventureStage::Setup()
    {
        //TEMP TODO REMOVE -once done testing
        //create a party of characters to work with during testing
        state::GameStateFactory::Instance()->NewGame(player::PartyFactory::MakeFakeForTesting());

        adventureDisplayStagePtr_ = new AdventureDisplayStage(this);
        adventureDisplayStagePtr_->Setup();

        //See AdventureDisplayStage::Setup() for where this is actually done.
        //The AdventureDisplayStage gives itself to the LoopManager so that it
        //can do so before adding it's own substages, so that it will be drawn
        //before those substages.
        //game::LoopManager::Instance()->AddStage(adventureDisplayStagePtr_);
    }

}
}
