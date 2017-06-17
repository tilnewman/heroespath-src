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
// song-base.cpp
//
#include "song.hpp"

#include "game/creature/creature.hpp"

#include "misc/strings.hpp"
#include "misc/random.hpp"

#include <sstream>
#include <algorithm>


namespace game
{
namespace song
{

    const std::string Song::EFFECT_STR_SUCCESS_         { "" };
    const std::string Song::EFFECT_STR_NOTHING_TO_DO_   { "Nothing to do." };
    const std::string Song::EFFECT_STR_IS_ALREADY_      { " is already " };
    const std::string Song::EFFECT_STR_RESISTED_        { " resisted the song's effects." };
    const std::string Song::EFFECT_STR_NOT_EFFECTED_    { " is not effected." };
    const int         Song::EFFECTS_ALL_CREATURES_COUNT_{ -1 };


    Song::Song(const Songs::Enum      WHICH,
               const SongType::Enum   TYPE,
               const EffectType::Enum EFFECT_TYPE,
               const Phase::Enum      VALID_PHASES,
               const stats::Mana_t    MANA_COST,
               const stats::Rank_t    RANK,
               const TargetType::Enum TARGET_TYPE)
    :
        which_      (WHICH),
        type_       (TYPE),
        effectType_ (EFFECT_TYPE),
        rank_       (RANK),
        validPhases_(VALID_PHASES),
        manaCost_   (MANA_COST),
        targetType_ (TARGET_TYPE)
    {}


    Song::~Song()
    {}


    const std::string Song::ToString() const
    {
        std::ostringstream ss;
        ss  << Name() << "  -" << DescDetails();
        return ss.str();
    }


    const std::string Song::DescDetails() const
    {
        std::ostringstream ss;
        ss << "A " << misc::String::DecorateNumber(rank_) << " rank"
           << EffectType::Name(effectType_) << " magical ";

        if (SongType::Guitar == type_)
        {
            ss << "guitar tune";
        }
        else
        {
            ss << "drum beat";
        }

        ss << " that can be played during " << Phase::ToString(validPhases_, false)
           << ", targeting " << TargetType::Name(targetType_)
           << ", and costing " << manaCost_ << " mana.";

        return ss.str();
    }


    const std::string Song::DescComplete() const
    {
        std::ostringstream ss;
        ss << Desc() << "  " << DescExtra() << " " << DescDetails();
        return ss.str();
    }


    const ContentAndNamePos Song::ActionPhraseBeginning(
        creature::CreaturePtr_t) const
    {
        std::ostringstream ss;

        ss << "'s ";

        if (SongType::Guitar == type_)
        {
            ss << "fingers ";

            switch(misc::random::Int(3))
            {
                case 1:     { ss << "tickle";  break; }
                case 2:     { ss << "pluck";  break; }
                case 3:     { ss << "strum";  break; }
                default:    { ss << "dance along";  break; }
            }

            ss << " the strings";
        }
        else
        {
            ss << "hands ";

            switch(misc::random::Int(2))
            {
                case 1:     { ss << "tap";  break; }
                case 2:     { ss << "rap";  break; }
                default:    { ss << "pound";  break; }
            }

            ss << " the back of the lute like a drum";
        }

        return ContentAndNamePos(ss.str(), NamePosition::SourceBefore);
    }

}
}
