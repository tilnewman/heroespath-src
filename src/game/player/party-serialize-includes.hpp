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
#ifndef GAME_PLAYER_PARTYSERIALIZEINCLUDES_INCLUDED
#define GAME_PLAYER_PARTYSERIALIZEINCLUDES_INCLUDED
//
// party-serialize-includes.hpp
//  A collection of includes that allows boost serialization of Character and Party objects.
//  This must only be included once, in game-state-factory.cpp
//
#include <fstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "misc/boost-serialize-includes.hpp"
#include <boost/serialization/export.hpp>

#include "game/item/item.hpp"
BOOST_CLASS_EXPORT(game::item::Item)

#include "game/creature/creature.hpp"
BOOST_CLASS_EXPORT(game::creature::Creature)

#include "game/player/character.hpp"
BOOST_CLASS_EXPORT(game::player::Character)

#include "game/player/party.hpp"
BOOST_CLASS_EXPORT(game::player::Party)

#endif //GAME_PLAYER_PARTYSERIALIZEINCLUDES_INCLUDED
