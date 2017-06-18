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

#include "misc/vectors.hpp"

#include <tuple>
#include <algorithm>


namespace game
{
namespace combat
{

    TurnActionInfo::TurnActionInfo(const TurnAction::Enum        ACTION,
                                   const creature::CreaturePtr_t TARGET_PTR)
    :
        actionType_ (ACTION),
        targetsPVec_(1, TARGET_PTR),
        spellPtr_   (nullptr),
        songPtr_    (nullptr)
    {}


    TurnActionInfo::TurnActionInfo(const spell::SpellPtr_t          SPELL_PTR,
                                   const creature::CreaturePVec_t & TARGET_PVEC)
    :
        actionType_ (TurnAction::Cast),
        targetsPVec_(TARGET_PVEC),
        spellPtr_   (SPELL_PTR),
        songPtr_    (nullptr)
    {}


    TurnActionInfo::TurnActionInfo(const song::SongPtr_t            SONG_PTR,
                                   const creature::CreaturePVec_t & TARGET_PVEC)
    :
        actionType_ (TurnAction::PlaySong),
        targetsPVec_(TARGET_PVEC),
        spellPtr_   (nullptr),
        songPtr_    (SONG_PTR)
    {}


    TurnActionInfo::TurnActionInfo(const TurnActionInfo & TAI)
    :
        actionType_ (TAI.actionType_),

        //The lifetime of these objects is not managed by this class.
        //Usage is short-term observation only, so ptr copying is safe.
        targetsPVec_(TAI.targetsPVec_),
        spellPtr_   (TAI.spellPtr_),
        songPtr_    (TAI.songPtr_)
    {}


    TurnActionInfo & TurnActionInfo::operator=(const TurnActionInfo & TAI)
    {
        if ( & TAI != this)
        {
            actionType_  = TAI.actionType_;

            //See comment in the copy constructor regarding these pointers.
            targetsPVec_ = TAI.targetsPVec_;
            spellPtr_    = TAI.spellPtr_;
            songPtr_     = TAI.songPtr_;
        }

        return * this;
    }


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


    bool operator<(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        if (misc::Vector::OrderlessCompareLess(L.targetsPVec_, R.targetsPVec_) == true)
        {
            return true;
        }

        return std::tie(L.actionType_, L.spellPtr_, L.songPtr_)
               <
               std::tie(R.actionType_, R.spellPtr_, R.songPtr_);
    }


    bool operator==(const TurnActionInfo & L, const TurnActionInfo & R)
    {
        if (misc::Vector::OrderlessCompareEqual(L.targetsPVec_, R.targetsPVec_) == false)
        {
            return false;
        }

        return std::tie(L.actionType_, L.spellPtr_, L.songPtr_)
               ==
               std::tie(R.actionType_, R.spellPtr_, R.songPtr_);
    }

}
}
