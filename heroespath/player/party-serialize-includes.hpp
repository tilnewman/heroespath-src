#ifndef APPBASE_HEROESPATH_PLAYER_PARTYSERIALIZEINCLUDES_INCLUDED
#define APPBASE_HEROESPATH_PLAYER_PARTYSERIALIZEINCLUDES_INCLUDED
//
// party-serialize-includes.hpp
//  A collection of includes that allows boost serialization of Character and Party objects.
//  This must only be included once, in game-state-factory.cpp
//
#include <fstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "sfml-util/boost-serialize-includes.hpp"
#include <boost/serialization/export.hpp>

#include "heroespath/creature/condition.hpp"
BOOST_CLASS_EXPORT(heroespath::creature::Condition)

#include "heroespath/creature/conditions.hpp"
//Note:  Keep this list in sync with condition-enum.hpp and conditions.hpp
BOOST_CLASS_EXPORT(heroespath::creature::condition::Cond_Good)
BOOST_CLASS_EXPORT(heroespath::creature::condition::Cond_Tripped)
BOOST_CLASS_EXPORT(heroespath::creature::condition::Cond_Dazed)
BOOST_CLASS_EXPORT(heroespath::creature::condition::Cond_Unconscious)
BOOST_CLASS_EXPORT(heroespath::creature::condition::Cond_Stone)
BOOST_CLASS_EXPORT(heroespath::creature::condition::Cond_Dead)

#include "heroespath/item/enchantment.hpp"
BOOST_CLASS_EXPORT(heroespath::item::Enchantment)

#include "heroespath/item/item.hpp"
BOOST_CLASS_EXPORT(heroespath::item::Item)

#include "heroespath/creature/title.hpp"
BOOST_CLASS_EXPORT(heroespath::creature::Title)

#include "heroespath/creature/creature.hpp"
BOOST_CLASS_EXPORT(heroespath::creature::Creature)

#include "heroespath/player/character.hpp"
BOOST_CLASS_EXPORT(heroespath::player::Character)

#include "heroespath/player/party.hpp"
BOOST_CLASS_EXPORT(heroespath::player::Party)

#endif //APPBASE_HEROESPATH_PLAYER_PARTYSERIALIZEINCLUDES_INCLUDED
