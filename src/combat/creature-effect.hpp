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
#ifndef GAME_COMBAT_CREATUREEFFEC_HPP_INCLUDED
#define GAME_COMBAT_CREATUREEFFEC_HPP_INCLUDED
//
// creature-effect.hpp
//
#include "hit-info.hpp"

#include "stats/trait.hpp"
#include "creature/condition-enum.hpp"

#include <vector>


namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
}
namespace song
{
    class Song;
    using SongPtr_t = Song *;
}
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
}

namespace combat
{

    //Everything required to describe all the effects a fight had on a creature.
    class CreatureEffect
    {
    public:
        explicit CreatureEffect(
            const creature::CreaturePtr_t CREATURE_PTR = nullptr,
            const HitInfoVec_t &          HIT_INFO_VEC = HitInfoVec_t());

        CreatureEffect(const CreatureEffect &);
        CreatureEffect & operator=(const CreatureEffect &);

        inline std::size_t             GetCount() const         { return hitInfoVec_.size(); }
        inline const HitInfoVec_t &    GetHitInfoVec() const    { return hitInfoVec_; }
        inline creature::CreaturePtr_t GetCreature() const      { return creaturePtr_; }

        bool GetWasHit() const;

        Health_t GetDamageTotal() const;

        const creature::CondEnumVec_t GetAllCondsAdded() const;
        const creature::CondEnumVec_t GetAllCondsRemoved() const;

        bool GetAllCondsAddedContains(const creature::Conditions::Enum) const;
        bool GetAllCondsRemovedContains(const creature::Conditions::Enum) const;

        bool AllCondsAddedRemove(const creature::Conditions::Enum);
        bool AllCondsRemovedRemove(const creature::Conditions::Enum);

        inline bool WasKill() const { return GetAllCondsAddedContains(creature::Conditions::Dead); }

        friend bool operator<(const CreatureEffect & L, const CreatureEffect & R);
        friend bool operator==(const CreatureEffect & L, const CreatureEffect & R);

    private:
        HitInfoVec_t hitInfoVec_;
        creature::CreaturePtr_t creaturePtr_;
    };

    using CreatureEffectVec_t = std::vector<CreatureEffect>;


    bool operator<(const CreatureEffect & L, const CreatureEffect & R);

    bool operator==(const CreatureEffect & L, const CreatureEffect & R);

    inline bool operator!=(const CreatureEffect & L, const CreatureEffect & R)
    {
        return ! (L == R);
    }

}
}

#endif //GAME_COMBAT_CREATUREEFFEC_HPP_INCLUDED
