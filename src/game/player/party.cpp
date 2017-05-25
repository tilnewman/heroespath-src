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
// party.hpp (player)
//
#include "party.hpp"

#include "game/player/character.hpp"

#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace player
{

    const std::size_t Party::MAX_CHARACTER_COUNT_(6);


    Party::Party(const CharacterSVec_t & CHARACTERS_SVEC)
    :
        charactersSVec_(CHARACTERS_SVEC)
    {}


    Party::~Party()
    {}


    bool Party::Add(const CharacterSPtr_t & CHARACTER_SPTR, std::string & error_msg)
    {
        if (IsAddAllowed(CHARACTER_SPTR, error_msg))
        {
            charactersSVec_.push_back(CHARACTER_SPTR);
            return true;
        }
        else
        {
            return false;
        }
    }


    bool Party::IsAddAllowed(const CharacterSPtr_t &, std::string & error_msg)
    {
        //TODO more checking here

        if (charactersSVec_.size() < MAX_CHARACTER_COUNT_)
        {
            return true;
        }
        else
        {
            error_msg = "too many characters in the party";
            return false;
        }
    }


    bool Party::Remove(const CharacterSPtr_t & CHARACTER_SPTR)
    {
        const CharacterSVec_t::iterator FOUND_ITER( std::find(charactersSVec_.begin(), charactersSVec_.end(), CHARACTER_SPTR) );

        if (FOUND_ITER == charactersSVec_.end())
            return false;
        else
        {
            charactersSVec_.erase(FOUND_ITER);
            return true;
        }
    }


    creature::CreatureSPtr_t Party::GetNextInOrder(const creature::CreatureSPtr_t & CREATURE_SPTR, const bool WILL_CHOOSE_AFTER)
    {
        return GetNextInOrder(CREATURE_SPTR.get(), WILL_CHOOSE_AFTER);
    }


    creature::CreatureSPtr_t Party::GetNextInOrder(creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_CHOOSE_AFTER)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr), "game::player::party::GetNextInOrder(WILL_CHOOSE_AFTER=" << std::boolalpha << WILL_CHOOSE_AFTER << ") was given a null CREATURE_CPTRC.");

        const creature::UniqueTraits_t TRAITS(CREATURE_CPTRC->UniqueTraits());

        for (CharacterSVec_t::iterator iter(charactersSVec_.begin()); iter != charactersSVec_.end(); ++iter)
        {
            if (TRAITS == (*iter)->UniqueTraits())
            {
                if (WILL_CHOOSE_AFTER)
                {
                    if ((iter + 1) == charactersSVec_.end())
                        return charactersSVec_[0];
                    else
                        return *(iter + 1);
                }
                else
                {
                    if (iter == charactersSVec_.begin())
                        return charactersSVec_[charactersSVec_.size() - 1];
                    else
                        return * (iter - 1);
                }
            }
        }

        std::ostringstream ss;
        ss << "game::player::party::GetNextInOrder(creature_name=" << CREATURE_CPTRC->Name() << ", WILL_CHOOSE_AFTER=" << std::boolalpha << WILL_CHOOSE_AFTER << ") but that CREATURE_CPTRC was not found in the party list.";
        throw std::runtime_error(ss.str());
    }


    creature::CreatureSPtr_t Party::GetAtOrderPos(const std::size_t INDEX_NUM)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((INDEX_NUM < charactersSVec_.size()), "game::player::party::GetAtOrderPos(INDEX_NUM=" << INDEX_NUM << ") was given an out of range INDEX_NUM.  (it was too big, max is " << charactersSVec_.size() - 1 << ")");

        std::size_t indexCounter(0);
        for (CharacterSVec_t::iterator iter(charactersSVec_.begin()); iter != charactersSVec_.end(); ++iter)
            if (indexCounter++ == INDEX_NUM)
                return * iter;

        std::ostringstream ss;
        ss << "game::player::party::GetAtOrderPos(INDEX_NUM=" << INDEX_NUM << ") never found an index matching what was given.";
        throw std::runtime_error(ss.str());
    }


    std::size_t Party::GetOrderNum(const creature::CreatureSPtr_t & CREATURE_SPTR) const
    {
        return GetOrderNum(CREATURE_SPTR.get());
    }


    std::size_t Party::GetOrderNum(creature::CreatureCPtrC_t CREATURE_CPTRC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr), "game::player::party::GetOrderNum() was given a null CREATURE_CPTRC.");

        const std::size_t NUM_CHARACTERS(charactersSVec_.size());
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            if (charactersSVec_[i].get() == CREATURE_CPTRC)
                return i;

        std::ostringstream ss;
        ss << "game::player::party::GetOrderNum(creature_name=" << CREATURE_CPTRC->Name() << ") never found that creature in the party vec.  Put another way, the function was given a creature that was not in the character vec.";
        throw std::runtime_error(ss.str());
    }


    std::size_t Party::GetNumHumanoid() const
    {
        std::size_t count(0);
        for (auto const & NEXT_CHARACTER_SPTR : charactersSVec_)
            if (NEXT_CHARACTER_SPTR->Body().IsHumanoid())
                ++count;

        return count;
    }

}
}
