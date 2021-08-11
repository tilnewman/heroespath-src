// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// turn-action-info.hpp
//
#include "turn-action-info.hpp"

#include "creature/creature.hpp"
#include "misc/vectors.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

#include <algorithm>
#include <tuple>

namespace heroespath
{
namespace combat
{

    TurnActionInfo::TurnActionInfo(
        const TurnAction::Enum ACTION, const creature::CreaturePVec_t & TARGETS_PVEC)
        : actionType_(ACTION)
        , targetsPVec_(TARGETS_PVEC)
        , spellPtrOpt_()
        , songPtrOpt_()
    {}

    TurnActionInfo::TurnActionInfo(
        const spell::SpellPtr_t SPELL_PTR, const creature::CreaturePVec_t & TARGET_PVEC)
        : actionType_(TurnAction::Cast)
        , targetsPVec_(TARGET_PVEC)
        , spellPtrOpt_(SPELL_PTR)
        , songPtrOpt_()
    {}

    TurnActionInfo::TurnActionInfo(
        const song::SongPtr_t SONG_PTR, const creature::CreaturePVec_t & TARGET_PVEC)
        : actionType_(TurnAction::PlaySong)
        , targetsPVec_(TARGET_PVEC)
        , spellPtrOpt_()
        , songPtrOpt_(SONG_PTR)
    {}

    const std::string TurnActionInfo::ToString() const
    {
        std::ostringstream ss;

        ss << "{type=" << TurnAction::ToString(actionType_);

        if (spellPtrOpt_)
        {
            ss << ", spell=" << spellPtrOpt_.value()->Name();
        }

        if (songPtrOpt_)
        {
            ss << ", song=" << songPtrOpt_.value()->Name();
        }

        ss << ", targets=";

        for (const auto & CREATURE_PTR : targetsPVec_)
        {
            ss << CREATURE_PTR->Name() << ", ";
        }

        ss << "}";

        return ss.str();
    }

    bool operator<(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        if (misc::Vector::OrderlessCompareLess(L.targetsPVec_, R.targetsPVec_) == true)
        {
            return true;
        }

        return std::tie(L.actionType_, L.spellPtrOpt_, L.songPtrOpt_)
            < std::tie(R.actionType_, R.spellPtrOpt_, R.songPtrOpt_);
    }

    bool operator==(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        if (std::tie(L.actionType_, L.spellPtrOpt_, L.songPtrOpt_)
            == std::tie(R.actionType_, R.spellPtrOpt_, R.songPtrOpt_))
        {
            return misc::Vector::OrderlessCompareEqual(L.targetsPVec_, R.targetsPVec_);
        }
        else
        {
            return false;
        }
    }

} // namespace combat
} // namespace heroespath
