// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// party.hpp (player)
//
#include "party.hpp"

#include "creature/creature-warehouse.hpp"
#include "creature/creature.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/serialize-helpers.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace player
{

    const std::size_t Party::MAX_CHARACTER_COUNT_{ 6 };

    Party::Party(
        const avatar::Avatar::Enum PARTY_AVATAR, const creature::CreaturePVec_t & CHARACTERS_PVEC)
        : avatar_(PARTY_AVATAR)
        , charactersPVec_(CHARACTERS_PVEC)
    {}

    Party::~Party() { creature::CreatureWarehouse::Access().Free(charactersPVec_); }

    const creature::CreaturePtr_t Party::GetNextInOrder(
        const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_CHOOSE_AFTER)
    {
        for (auto iter(charactersPVec_.begin()); iter != charactersPVec_.end(); ++iter)
        {
            if (CREATURE_PTR == *iter)
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
                        return *(iter - 1);
                    }
                }
            }
        }

        std::ostringstream ss;
        ss << "player::party::GetNextInOrder(creature={" << CREATURE_PTR->ToString()
           << "}, WILL_CHOOSE_AFTER=" << std::boolalpha << WILL_CHOOSE_AFTER
           << ") but that CREATURE_PTR was not found in the party list.";

        throw std::runtime_error(ss.str());
    }

    const creature::CreaturePtr_t Party::GetAtOrderPos(const std::size_t INDEX_NUM)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (INDEX_NUM < charactersPVec_.size()),
            "player::party::GetAtOrderPos(INDEX_NUM="
                << INDEX_NUM << ") was given an out of range INDEX_NUM.  (it was too big, max is "
                << charactersPVec_.size() - 1 << ")");

        std::size_t indexCounter(0);
        for (auto iter(charactersPVec_.begin()); iter != charactersPVec_.end(); ++iter)
        {
            if (indexCounter++ == INDEX_NUM)
            {
                return *iter;
            }
        }

        std::ostringstream ss;
        ss << "player::party::GetAtOrderPos(INDEX_NUM=" << INDEX_NUM
           << ") never found an index matching what was given.";

        throw std::runtime_error(ss.str());
    }

    std::size_t Party::GetOrderNum(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        const std::size_t NUM_CHARACTERS(charactersPVec_.size());
        for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
        {
            if (charactersPVec_[i] == CREATURE_PTR)
            {
                return i;
            }
        }

        std::ostringstream ss;
        ss << "player::party::GetOrderNum(creature={" << CREATURE_PTR->ToString()
           << "}) never found that creature in the party vec.  Put another way, the function"
           << "was given a creature that was not in the character vec.";

        throw std::runtime_error(ss.str());
    }

    std::size_t Party::GetNumHumanoid() const
    {
        std::size_t count(0);
        for (auto const & NEXT_CHARACTER_PTR : charactersPVec_)
        {
            if (NEXT_CHARACTER_PTR->Body().IsHumanoid())
            {
                ++count;
            }
        }

        return count;
    }

    void Party::BeforeSerialize()
    {
        misc::SerializeHelp::BeforeSerialize(charactersPVec_, charactersToSerializePVec_);
    }

    void Party::AfterSerialize()
    {
        misc::SerializeHelp::AfterSerialize(
            charactersToSerializePVec_, creature::CreatureWarehouse::Access());
    }

    void Party::AfterDeserialize()
    {
        misc::SerializeHelp::AfterDeserialize(
            charactersPVec_, charactersToSerializePVec_, creature::CreatureWarehouse::Access());
    }

} // namespace player
} // namespace heroespath
