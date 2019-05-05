// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// player-party.hpp
//
#include "player-party.hpp"

#include "creature/creature-warehouse.hpp"
#include "creature/creature.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/serialize-helpers.hpp"
#include "misc/strings.hpp"

#include <sstream>
#include <stdexcept>

namespace heroespath
{
namespace creature
{

    const std::size_t PlayerParty::MAX_CHARACTER_COUNT_ { 6 };

    PlayerParty::PlayerParty(
        const avatar::Avatar::Enum PARTY_AVATAR, const creature::CreaturePVec_t & CHARACTERS_PVEC)
        : avatar_(PARTY_AVATAR)
        , charactersPVec_(CHARACTERS_PVEC)
    {}

    PlayerParty::~PlayerParty() { creature::CreatureWarehouse::Access().Free(charactersPVec_); }

    const creature::CreaturePtr_t PlayerParty::GetNextInOrder(
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
                        return charactersPVec_.front();
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
                        return charactersPVec_.back();
                    }
                    else
                    {
                        return *(iter - 1);
                    }
                }
            }
        }

        M_HP_LOG_ERR(
            "CREATURE_PTR not in charactersPVec_.  Returning the first character instead."
            + M_HP_VAR_STR(CREATURE_PTR->ToString()) + M_HP_VAR_STR(charactersPVec_.size())
            + M_HP_VAR_STR(WILL_CHOOSE_AFTER));

        return charactersPVec_.front();
    }

    const creature::CreaturePtr_t PlayerParty::GetAtOrderPos(const std::size_t INDEX)
    {
        if (INDEX >= charactersPVec_.size())
        {
            M_HP_LOG_ERR(
                "INDEX out of bounds.  Returning the last character instead." + M_HP_VAR_STR(INDEX)
                + M_HP_VAR_STR(charactersPVec_.size()));

            return charactersPVec_.back();
        }
        else
        {
            return charactersPVec_.at(INDEX);
        }
    }

    std::size_t PlayerParty::GetOrderNum(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        const auto CHARACTER_COUNT { charactersPVec_.size() };
        for (std::size_t index(0); index < CHARACTER_COUNT; ++index)
        {
            if (charactersPVec_.at(index) == CREATURE_PTR)
            {
                return index;
            }
        }

        M_HP_LOG_ERR(
            "CREATURE_PTR not in charactersPVec_.  Returning zero (the first character) instead."
            + M_HP_VAR_STR(CREATURE_PTR->ToString()) + M_HP_VAR_STR(charactersPVec_.size()));

        return 0;
    }

    std::size_t PlayerParty::GetNumHumanoid() const
    {
        std::size_t count(0);
        for (const auto & NEXT_CHARACTER_PTR : charactersPVec_)
        {
            if (NEXT_CHARACTER_PTR->Body().IsHumanoid())
            {
                ++count;
            }
        }

        return count;
    }

    void PlayerParty::BeforeSerialize()
    {
        misc::SerializeHelp::BeforeSerialize(charactersPVec_, charactersToSerializePVec_);
    }

    void PlayerParty::AfterSerialize()
    {
        misc::SerializeHelp::AfterSerialize(
            charactersToSerializePVec_, creature::CreatureWarehouse::Access());
    }

    void PlayerParty::AfterDeserialize()
    {
        misc::SerializeHelp::AfterDeserialize(
            charactersPVec_, charactersToSerializePVec_, creature::CreatureWarehouse::Access());
    }

} // namespace creature
} // namespace heroespath
