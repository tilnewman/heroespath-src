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
#ifndef GAME_CREATURE_CONDITIONS_INCLUDED
#define GAME_CREATURE_CONDITIONS_INCLUDED
//
// conditions.hpp
//  Instances and base classes of specific Conditions.
//
#include "game/creature/condition.hpp"
#include "game/creature/creature.hpp"
#include "game/stats/traits-set.hpp"

#include <string>
#include <utility>


namespace game
{
namespace creature
{
namespace condition
{

    class Good : public Condition
    {
    public:
        Good() : Condition() {}
        virtual ~Good() {}
    };


    class Bold : public Condition
    {
    public:
        Bold() : Condition(
            Conditions::Bold,
            false,
            stats::TraitSet( { 
                std::make_pair(stats::Traits::Strength, 25),
                std::make_pair(stats::Traits::Accuracy, 25),
                std::make_pair(stats::Traits::Charm, 25),
                std::make_pair(stats::Traits::Luck, 25),
                std::make_pair(stats::Traits::Speed, 25),
                std::make_pair(stats::Traits::Intelligence, 25) } ) ) {}

        virtual ~Bold() {}
    };


    class Heroic : public Condition
    {
    public:
        Heroic() : Condition(
            Conditions::Heroic,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, 50),
                std::make_pair(stats::Traits::Accuracy, 50),
                std::make_pair(stats::Traits::Charm, 50),
                std::make_pair(stats::Traits::Luck, 50),
                std::make_pair(stats::Traits::Speed, 50),
                std::make_pair(stats::Traits::Intelligence, 50) } ) ) {}

        virtual ~Heroic() {}
    };


    class Daunted : public Condition
    {
    public:
        Daunted() : Condition(
            Conditions::Daunted,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Accuracy, -25),
                std::make_pair(stats::Traits::Charm, -25),
                std::make_pair(stats::Traits::Speed, -16),
                std::make_pair(stats::Traits::Intelligence, -25) } ) ) {}

        virtual ~Daunted() {}
    };


    class Panic : public Condition
    {
    public:
        Panic() : Condition(
            Conditions::Panic,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Accuracy, -50),
                std::make_pair(stats::Traits::Charm, -50),
                std::make_pair(stats::Traits::Speed, -33),
                std::make_pair(stats::Traits::Intelligence, -50) } ) ) {}

        virtual ~Panic() {}
    };


    class Tripped : public Condition
    {
    public:
        Tripped() : Condition(
            Conditions::Tripped,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, -75),
                std::make_pair(stats::Traits::Accuracy, -75),
                std::make_pair(stats::Traits::Speed, -75) } ) ) {}

        virtual ~Tripped() {}

        virtual void PerTurnEffect(CreaturePtr_t          creaturePtr,
                                   combat::HitInfoVec_t & hitInfoVec,
                                   bool &                 hasTurnBeenConsumed) const;
    };


    class Pounced : public Condition
    {
    public:
        Pounced() : Condition(
            Conditions::Pounced,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, -75),
                std::make_pair(stats::Traits::Accuracy, -75),
                std::make_pair(stats::Traits::Speed, -75) } ) ) {}

        virtual ~Pounced() {}

        virtual void PerTurnEffect(CreaturePtr_t          creaturePtr,
                                   combat::HitInfoVec_t & hitInfoVec,
                                   bool &                 hasTurnBeenConsumed) const;
    };


    class Dazed : public Condition
    {
    public:
        Dazed() : Condition(
            Conditions::Dazed,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, -50),
                std::make_pair(stats::Traits::Accuracy, -66),
                std::make_pair(stats::Traits::Speed, -66),
                std::make_pair(stats::Traits::Intelligence, -50) } ) ) {}
        
        virtual ~Dazed() {}

        virtual void PerTurnEffect(CreaturePtr_t          creaturePtr,
                                   combat::HitInfoVec_t & hitInfoVec,
                                   bool &                 hasTurnBeenConsumed) const;
    };


    class Unconscious : public Condition
    {
    public:
        Unconscious() : Condition(
            Conditions::Unconscious,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, -9999),
                std::make_pair(stats::Traits::Accuracy, -9999),
                std::make_pair(stats::Traits::Charm, -9999),
                std::make_pair(stats::Traits::Speed, -9999),
                std::make_pair(stats::Traits::Intelligence, -9999), } ) ) {}

        virtual ~Unconscious() {}
    };


    class Stone : public Condition
    {
    public:
        Stone() : Condition(
            Conditions::Stone,
            true,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, -9999),
                std::make_pair(stats::Traits::Accuracy, -9999),
                std::make_pair(stats::Traits::Charm, -9999),
                std::make_pair(stats::Traits::Speed, -9999),
                std::make_pair(stats::Traits::Intelligence, -9999) } ) ) {}
        
        virtual ~Stone() {}
    };


    class Dead : public Condition
    {
    public:
        Dead() : Condition(
            Conditions::Dead,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, -9999),
                std::make_pair(stats::Traits::Accuracy, -9999),
                std::make_pair(stats::Traits::Charm, -9999),
                std::make_pair(stats::Traits::Luck, -9999),
                std::make_pair(stats::Traits::Speed, -9999),
                std::make_pair(stats::Traits::Intelligence, -9999) } ) ) {}
        
        virtual ~Dead() {}
    };


    class AsleepNatural : public Condition
    {
    public:
        AsleepNatural() : Condition(
            Conditions::AsleepNatural,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, -9999),
                std::make_pair(stats::Traits::Accuracy, -9999),
                std::make_pair(stats::Traits::Speed, -9999) } ) ) {}

        virtual ~AsleepNatural() {}

        virtual void PerTurnEffect(CreaturePtr_t          creaturePtr,
                                   combat::HitInfoVec_t & hitInfoVec,
                                   bool &                 hasTurnBeenConsumed) const;
    };


    class AsleepMagical : public Condition
    {
    public:
        AsleepMagical() : Condition(
            Conditions::AsleepMagical,
            true,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, -9999),
                std::make_pair(stats::Traits::Accuracy, -9999),
                std::make_pair(stats::Traits::Speed, -9999) } ) ) {}
        
        virtual ~AsleepMagical() {}

        virtual void PerTurnEffect(CreaturePtr_t          creaturePtr,
                                   combat::HitInfoVec_t & hitInfoVec,
                                   bool &                 hasTurnBeenConsumed) const;
    };


    class Poisoned : public Condition
    {
    public:
        Poisoned() : Condition(
            Conditions::Poisoned,
            false,
            stats::TraitSet( {
                std::make_pair(stats::Traits::Strength, -25),
                std::make_pair(stats::Traits::Charm, -50),
                std::make_pair(stats::Traits::Speed, -25) } ) ) {}
        
        virtual ~Poisoned() {}
        
        virtual void PerTurnEffect(CreaturePtr_t          creaturePtr,
                                   combat::HitInfoVec_t & hitInfoVec,
                                   bool &                 hasTurnBeenConsumed) const;
    };

}
}
}
#endif //GAME_CREATURE_CONDITIONS_INCLUDED
