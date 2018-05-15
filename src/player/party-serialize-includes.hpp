// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_PLAYER_PARTYSERIALIZEINCLUDES_HPP_INCLUDED
#define HEROESPATH_PLAYER_PARTYSERIALIZEINCLUDES_HPP_INCLUDED
//
// party-serialize-includes.hpp
//  A collection of includes that allows boost serialization of Character and Party objects.
//  This must only be included once, in game-state-factory.cpp
//
#include <fstream>

#include "misc/boost-serialize-includes.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>

#include "item/item.hpp"
BOOST_CLASS_EXPORT(heroespath::item::Item)

#include "creature/creature.hpp"
BOOST_CLASS_EXPORT(heroespath::creature::Creature)

#include "player/party.hpp"
BOOST_CLASS_EXPORT(heroespath::player::Party)

#endif // HEROESPATH_PLAYER_PARTYSERIALIZEINCLUDES_HPP_INCLUDED
