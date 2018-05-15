// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
    using SongPtr_t = misc::NotNull<Song *>;
    using SongPtrOpt_t = boost::optional<SongPtr_t>;
} // namespace song
namespace creature
{
    // forward declarations
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
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
        const spell::SpellPtrOpt_t Spell() const { return spellPtrOpt_; }
        const song::SongPtrOpt_t Song() const { return songPtrOpt_; }
        const creature::CreaturePVec_t & Targets() const { return targetsPVec_; }
        const std::string ToString() const;

        friend bool operator<(const TurnActionInfo & A, const TurnActionInfo & B);
        friend bool operator==(const TurnActionInfo & A, const TurnActionInfo & B);

    private:
        TurnAction::Enum actionType_;
        creature::CreaturePVec_t targetsPVec_;
        spell::SpellPtrOpt_t spellPtrOpt_;
        song::SongPtrOpt_t songPtrOpt_;
    };

    bool operator<(const TurnActionInfo & L, const TurnActionInfo & R);

    bool operator==(const TurnActionInfo & L, const TurnActionInfo & R);

    inline bool operator!=(const TurnActionInfo & L, const TurnActionInfo & R) { return !(L == R); }

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TURNACTIONINFO_HPP_INCLUDED
