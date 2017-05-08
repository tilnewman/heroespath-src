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

#include "utilz/boost-serialize-includes.hpp"
#include <boost/serialization/export.hpp>

#include "game/item/item.hpp"
BOOST_CLASS_EXPORT(game::item::Item)

#include "game/creature/title.hpp"
BOOST_CLASS_EXPORT(game::creature::Title)

#include "game/creature/creature.hpp"
BOOST_CLASS_EXPORT(game::creature::Creature)

#include "game/player/character.hpp"
BOOST_CLASS_EXPORT(game::player::Character)

#include "game/player/party.hpp"
BOOST_CLASS_EXPORT(game::player::Party)

#endif //GAME_PLAYER_PARTYSERIALIZEINCLUDES_INCLUDED
