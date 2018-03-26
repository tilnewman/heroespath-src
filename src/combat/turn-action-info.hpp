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
#ifndef HEROESPATH_COMBAT_TURNACTIONINFO_HPP_INCLUDED
#define HEROESPATH_COMBAT_TURNACTIONINFO_HPP_INCLUDED
//
// turn-action-info.hpp
//
#include "combat/strategy-enums.hpp"
#include "combat/turn-action-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
    using SpellPtrOpt_t = boost::optional<SpellPtr_t>;
} // namespace spell
namespace song
{
    class Song;
    using SongPtr_t = Song *;
} // namespace song
namespace creature
{
    // forward declarations
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace combat
{

    // Everything required to describe what an enemy did on its turn
    class TurnActionInfo
    {
    public:
        explicit TurnActionInfo(
            const TurnAction::Enum ACTION = TurnAction::Nothing,
            const creature::CreaturePVec_t & TARGETS_PVEC = creature::CreaturePVec_t());

        TurnActionInfo(
            const spell::SpellPtr_t SPELL_PTR, const creature::CreaturePVec_t & TARGET_PVEC);

        TurnActionInfo(
            const song::SongPtr_t SONG_PTR, const creature::CreaturePVec_t & TARGET_PVEC);

        TurnAction::Enum Action() const { return actionType_; }
        spell::SpellPtrOpt_t Spell() const { return spellPtrOpt_; }
        song::SongPtr_t Song() const { return songPtr_; }
        const creature::CreaturePVec_t & Targets() const { return targetsPVec_; }
        creature::CreaturePtr_t Target() const;
        const std::string ToString() const;

        friend bool operator<(const TurnActionInfo & A, const TurnActionInfo & B);
        friend bool operator==(const TurnActionInfo & A, const TurnActionInfo & B);

    private:
        TurnAction::Enum actionType_;
        creature::CreaturePVec_t targetsPVec_;
        spell::SpellPtrOpt_t spellPtrOpt_;
        song::SongPtr_t songPtr_;
    };

    bool operator<(const TurnActionInfo & L, const TurnActionInfo & R);

    bool operator==(const TurnActionInfo & L, const TurnActionInfo & R);

    inline bool operator!=(const TurnActionInfo & L, const TurnActionInfo & R) { return !(L == R); }

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TURNACTIONINFO_HPP_INCLUDED
