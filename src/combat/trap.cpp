// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// trap.cpp
//
#include "trap.hpp"

#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"

#include <cmath>

namespace heroespath
{
namespace combat
{

    Trap::Trap()
        : playerCountRange_(0, 0)
        , damageRange_(0.0, 0.0)
        , soundEffect_(gui::sound_effect::None)
        , descPrefix_("")
        , descPostfix_("")
        , willDescUseContainerName_(false)
        , hitVerb_("")
    {}

    Trap::Trap(
        const std::size_t PLAYER_COUNT_MIN,
        const std::size_t PLAYER_COUNT_MAX,
        const Health_t & DAMAGE_MIN,
        const Health_t & DAMAGE_MAX,
        const std::string & HIT_VERB,
        const gui::sound_effect::Enum SOUND_EFFECT,
        const std::string & DESCRIPTION)
        : playerCountRange_(PLAYER_COUNT_MIN, PLAYER_COUNT_MAX)
        , damageRange_(DAMAGE_MIN.GetAs<double>(), DAMAGE_MAX.GetAs<double>())
        , soundEffect_(SOUND_EFFECT)
        , descPrefix_(DESCRIPTION)
        , descPostfix_("")
        , willDescUseContainerName_(false)
        , hitVerb_(HIT_VERB)
    {}

    Trap::Trap(
        const std::size_t PLAYER_COUNT_MIN,
        const std::size_t PLAYER_COUNT_MAX,
        const Health_t & DAMAGE_MIN,
        const Health_t & DAMAGE_MAX,
        const std::string & HIT_VERB,
        const gui::sound_effect::Enum SOUND_EFFECT,
        const std::string & DESC_PREFIX,
        const std::string & DES_POSTFIX)
        : playerCountRange_(PLAYER_COUNT_MIN, PLAYER_COUNT_MAX)
        , damageRange_(DAMAGE_MIN.GetAs<double>(), DAMAGE_MAX.GetAs<double>())
        , soundEffect_(SOUND_EFFECT)
        , descPrefix_(DESC_PREFIX)
        , descPostfix_(DES_POSTFIX)
        , willDescUseContainerName_(true)
        , hitVerb_(HIT_VERB)
    {}

    const std::string Trap::Description(const std::string & CONTAINER_NAME) const
    {
        if (willDescUseContainerName_)
        {
            return descPrefix_ + CONTAINER_NAME + descPostfix_;
        }
        else
        {
            return descPrefix_ + descPostfix_;
        }
    }

    Health_t Trap::RandomDamage() const
    {
        const auto RANDOM_DAMAGE { Health_t::Make(
            misc::Random(damageRange_.Min(), damageRange_.Max())) };

        const auto SQRT_RANDOM_DAMAGE { Health_t::Make(std::sqrt(RANDOM_DAMAGE.GetAs<double>())) };

        const auto AVG_PLAYER_RANK_MINUS_ONE { [&]() {
            const auto AVG_PLAYER_RANK { FindAveragePlayerRank() };
            if (AVG_PLAYER_RANK < 2_rank)
            {
                return 1_rank;
            }
            else
            {
                return (AVG_PLAYER_RANK - 1_rank);
            }
        }() };

        const auto SQRT_AVG_PLAYER_RANK_MINUS_ONE { Rank_t::Make(
            std::sqrt(AVG_PLAYER_RANK_MINUS_ONE.GetAs<double>())) };

        const auto TOTAL_DAMAGE { RANDOM_DAMAGE
                                  + (SQRT_RANDOM_DAMAGE
                                     * Health_t::Make(SQRT_AVG_PLAYER_RANK_MINUS_ONE.Get())) };

        return TOTAL_DAMAGE;
    }

    int Trap::Severity() const
    {
        const auto SQRT_AVG_PLAYER_COUNT { std::sqrt(
            static_cast<double>(playerCountRange_.Mid()) * 10.0) };

        const auto SQRT_AVG_DAMAGE { std::sqrt(damageRange_.Mid() * 10.0) };

        return static_cast<int>(SQRT_AVG_PLAYER_COUNT * SQRT_AVG_DAMAGE);
    }

    std::size_t Trap::RandomEffectedPlayersCount() const
    {
        return misc::Random(playerCountRange_.Min(), playerCountRange_.Max());
    }

    Rank_t Trap::FindAveragePlayerRank() const
    {
        Rank_t rankSum { 0_rank };
        const auto CHARACTER_PTRS { game::Game::Instance()->State().Party().Characters() };
        for (const auto & CHARACTER_PTR : CHARACTER_PTRS)
        {
            rankSum += CHARACTER_PTR->Rank();
        }

        return rankSum / Rank_t::Make(CHARACTER_PTRS.size());
    }

} // namespace combat
} // namespace heroespath
