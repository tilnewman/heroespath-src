// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
        , spellPtrOpt_(boost::none)
        , songPtr_(nullptr)
    {}

    TurnActionInfo::TurnActionInfo(
        const spell::SpellPtr_t SPELL_PTR, const creature::CreaturePVec_t & TARGET_PVEC)
        : actionType_(TurnAction::Cast)
        , targetsPVec_(TARGET_PVEC)
        , spellPtrOpt_(SPELL_PTR)
        , songPtr_(nullptr)
    {}

    TurnActionInfo::TurnActionInfo(
        const song::SongPtr_t SONG_PTR, const creature::CreaturePVec_t & TARGET_PVEC)
        : actionType_(TurnAction::PlaySong)
        , targetsPVec_(TARGET_PVEC)
        , spellPtrOpt_(boost::none)
        , songPtr_(SONG_PTR)
    {}

    creature::CreaturePtr_t TurnActionInfo::Target() const
    {
        if (targetsPVec_.empty())
        {
            return nullptr;
        }
        else
        {
            return targetsPVec_[0];
        }
    }

    const std::string TurnActionInfo::ToString() const
    {
        std::ostringstream ss;

        ss << "{type=" << TurnAction::ToString(actionType_);

        if (spellPtrOpt_)
        {
            ss << ", spell=" << spellPtrOpt_->Obj().Name();
        }

        if (nullptr != songPtr_)
        {
            ss << ", song=" << songPtr_->Name();
        }

        ss << ", targets=";

        for (auto const CREATURE_PTR : targetsPVec_)
        {
            if (CREATURE_PTR == nullptr)
            {
                ss << "null, ";
            }
            else
            {
                ss << CREATURE_PTR->Name() << ", ";
            }
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

        return std::tie(L.actionType_, L.spellPtrOpt_, L.songPtr_)
            < std::tie(R.actionType_, R.spellPtrOpt_, R.songPtr_);
    }

    bool operator==(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        if (std::tie(L.actionType_, L.spellPtrOpt_, L.songPtr_)
            == std::tie(R.actionType_, R.spellPtrOpt_, R.songPtr_))
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
