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
// creature-wrapper.cpp
//
#include "creature-wrapper.hpp"

#include "game/creature/creature.hpp"
#include "game/player/character.hpp"
#include "game/non-player/character.hpp"
#include "utilz/assertlogandthrow.hpp"


namespace game
{

    CreatureWrapper::CreatureWrapper(const creature::CreatureSPtr_t & CREATURE_SPTR)
    :
        creatureSPtr_(CREATURE_SPTR)
    {
        Verify();
    }


    CreatureWrapper::CreatureWrapper(const player::CharacterSPtr_t & PLAYER_CHAR_SPTR)
    :
        creatureSPtr_(PLAYER_CHAR_SPTR)
    {
        Verify();
    }


    CreatureWrapper::CreatureWrapper(const non_player::CharacterSPtr_t & NON_PLAYER_CHAR_SPTR)
    :
        creatureSPtr_(NON_PLAYER_CHAR_SPTR)
    {
        Verify();
    }


    bool CreatureWrapper::IsPlayerCharacter() const
    {
        return creatureSPtr_->IsPlayerCharacter();
    }


    void CreatureWrapper::Verify() const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((creatureSPtr_.get() != nullptr), "game::CreatureWrapper::Verify()  A CreatureWrapper object was constructed with a shared_ptr that was null.");
    }

}
