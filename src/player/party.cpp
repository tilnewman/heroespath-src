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

#include "log/log-macros.hpp"
#include "player/character.hpp"
#include "player/character-warehouse.hpp"

#include "misc/assertlogandthrow.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace player
{

    const std::size_t Party::MAX_CHARACTER_COUNT_(6);


    Party::Party(const CharacterPVec_t & CHARACTERS_PVEC)
    :
        charactersPVec_()
    {
        for (auto const NEXT_CHARACTER_PTR : CHARACTERS_PVEC)
        {
            charactersPVec_.push_back( CharacterWarehouse::Instance()->Store(NEXT_CHARACTER_PTR) );
        }
    }


    Party::~Party()
    {
        for (auto & nextCharacterPtr : charactersPVec_)
        {
            CharacterWarehouse::Instance()->Free(nextCharacterPtr);
        }

        charactersPVec_.clear();
    }


    bool Party::Add(const CharacterPtr_t CHARACTER_PTR, std::string & error_msg)
    {
        if (IsAddAllowed(CHARACTER_PTR, error_msg))
        {
            charactersPVec_.push_back( CharacterWarehouse::Instance()->Store(CHARACTER_PTR) );
            return true;
        }
        else
        {
            return false;
        }
    }


    bool Party::IsAddAllowed(const CharacterPtr_t, std::string & error_msg)
    {
        if (charactersPVec_.size() < MAX_CHARACTER_COUNT_)
        {
            return true;
        }
        else
        {
            error_msg = "too many characters in the party";
            return false;
        }
    }


    bool Party::Remove(CharacterPtr_t characterPtr)
    {
        auto const FOUND_ITER{ std::find(charactersPVec_.begin(),
                                         charactersPVec_.end(),
                                         characterPtr) };

        if (FOUND_ITER == charactersPVec_.end())
        {
            M_HP_LOG_ERR("player::Party::Remove((" << characterPtr << ") name=\""
                << characterPtr->Name() << "\") was not found in the party.");

            return false;
        }
        else
        {
            CharacterWarehouse::Instance()->Free(characterPtr);
            charactersPVec_.erase(FOUND_ITER);
            return true;
        }
    }


    creature::CreaturePtr_t Party::GetNextInOrder(
        creature::CreatureCPtrC_t CREATURE_CPTRC,
        const bool                WILL_CHOOSE_AFTER)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "player::party::GetNextInOrder(WILL_CHOOSE_AFTER=" << std::boolalpha
            << WILL_CHOOSE_AFTER << ") was given a null CREATURE_CPTRC.");

        const creature::UniqueTraits_t TRAITS(CREATURE_CPTRC->UniqueTraits());

        for (CharacterPVec_t::iterator iter(charactersPVec_.begin());
            iter != charactersPVec_.end(); ++iter)
        {
            if (TRAITS == (*iter)->UniqueTraits())
            {
                if (WILL_CHOOSE_AFTER)
                {
                    if ((iter + 1) == charactersPVec_.end())
                    {
                        return charactersPVec_[0];
                    }
                    else
                    {
                        return *(iter + 1);
                    }
                }
                else
                {
                    if (iter == charactersPVec_.begin())
                    {
                        return charactersPVec_[charactersPVec_.size() - 1];
                    }
                    else
                    {
                        return * (iter - 1);
                    }
                }
            }
        }

        std::ostringstream ss;
        ss << "player::party::GetNextInOrder(creature_name=" << CREATURE_CPTRC->Name()
            << ", WILL_CHOOSE_AFTER=" << std::boolalpha << WILL_CHOOSE_AFTER
            << ") but that CREATURE_CPTRC was not found in the party list.";

        throw std::runtime_error(ss.str());
    }


    creature::CreaturePtr_t Party::GetAtOrderPos(const std::size_t INDEX_NUM)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((INDEX_NUM < charactersPVec_.size()),
            "player::party::GetAtOrderPos(INDEX_NUM=" << INDEX_NUM
            << ") was given an out of range INDEX_NUM.  (it was too big, max is "
            << charactersPVec_.size() - 1 << ")");

        std::size_t indexCounter(0);
        for (CharacterPVec_t::iterator iter(charactersPVec_.begin());
            iter != charactersPVec_.end(); ++iter)
        {
            if (indexCounter++ == INDEX_NUM)
            {
                return * iter;
            }
        }

        std::ostringstream ss;
        ss << "player::party::GetAtOrderPos(INDEX_NUM=" << INDEX_NUM
            << ") never found an index matching what was given.";

        throw std::runtime_error(ss.str());
    }


    std::size_t Party::GetOrderNum(creature::CreatureCPtrC_t CREATURE_CPTRC) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((CREATURE_CPTRC != nullptr),
            "player::party::GetOrderNum() was given a null CREATURE_CPTRC.");

        const std::size_t NUM_CHARACTERS(charactersPVec_.size());
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            if (charactersPVec_[i] == CREATURE_CPTRC)
            {
                return i;
            }
        }

        std::ostringstream ss;
        ss << "player::party::GetOrderNum(creature_name=" << CREATURE_CPTRC->Name()
            << ") never found that creature in the party vec.  Put another way, the function"
            << "was given a creature that was not in the character vec.";

        throw std::runtime_error(ss.str());
    }


    std::size_t Party::GetNumHumanoid() const
    {
        std::size_t count(0);
        for (auto const NEXT_CHARACTER_PTR : charactersPVec_)
        {
            if (NEXT_CHARACTER_PTR->Body().IsHumanoid())
            {
                ++count;
            }
        }

        return count;
    }

}
}
