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
#include "game/stats/stat-mult-set.hpp"

#include <string>


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
            stats::StatMultSet(1.33f, 1.33f, 1.33f, 1.33f, 1.33f, 1.33f)) {}

        virtual ~Bold() {}
    };


    class Heroic : public Condition
    {
    public:
        Heroic() : Condition(
            Conditions::Heroic,
            false,
            stats::StatMultSet(1.66f, 1.66f, 1.66f, 1.66f, 1.66f, 1.66f)) {}

        virtual ~Heroic() {}
    };


    class Daunted : public Condition
    {
    public:
        Daunted() : Condition(
            Conditions::Daunted,
            false,
            stats::StatMultSet(1.0f, 0.75f, 0.75f, 1.0f, 0.85f, 0.75f)) {}

        virtual ~Daunted() {}
    };


    class Panic : public Condition
    {
    public:
        Panic() : Condition(
            Conditions::Panic,
            false,
            stats::StatMultSet(1.0f, 0.5f, 0.5f, 1.0f, 0.75f, 0.5f)) {}

        virtual ~Panic() {}
    };


    class Tripped : public Condition
    {
    public:
        Tripped() : Condition(
            Conditions::Tripped,
            false,
            stats::StatMultSet(0.25f, 0.25f, 1.0f, 1.0f, 0.25f, 1.0f)) {}

        virtual ~Tripped() {}
    };


    class Dazed : public Condition
    {
    public:
        Dazed() : Condition(
            Conditions::Dazed,
            false,
            stats::StatMultSet(0.5f, (1.0f / 3.0f), 1.0f, 1.0f, (1.0f / 3.0f), 0.5f)) {}
        
        virtual ~Dazed() {}
    };


    class Unconscious : public Condition
    {
    public:
        Unconscious() : Condition(Conditions::Unconscious) {}
        virtual ~Unconscious() {}
    };


    class Stone : public Condition
    {
    public:
        Stone() : Condition(
            Conditions::Stone,
            true,
            stats::StatMultSet(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f)) {}
        
        virtual ~Stone() {}
    };


    class Dead : public Condition
    {
    public:
        Dead() : Condition(
            Conditions::Dead,
            false,
            stats::StatMultSet(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)) {}
        
        virtual ~Dead() {}
    };


    class AsleepNatural : public Condition
    {
    public:
        AsleepNatural() : Condition(
            Conditions::AsleepNatural,
            false,
            stats::StatMultSet(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f)) {}
        virtual ~AsleepNatural() {}
    };


    class AsleepMagical : public Condition
    {
    public:
        AsleepMagical() : Condition(
            Conditions::AsleepMagical,
            true,
            stats::StatMultSet(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f)) {}
        
        virtual ~AsleepMagical() {}
    };


    class Poisoned : public Condition
    {
    public:
        Poisoned() : Condition(
            Conditions::Poisoned,
            false,
            stats::StatMultSet(0.75f, 1.0f, 0.5f, 1.0f, 0.75f, 1.0f)) {}
        
        virtual ~Poisoned() {}
        virtual const CondEnumVec_t PerTurnChange(CreaturePtrC_t creaturePtrC);
    };

}
}
}
#endif //GAME_CREATURE_CONDITIONS_INCLUDED
