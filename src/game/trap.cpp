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
// trap.cpp
//
#include "trap.hpp"

#include "game/game.hpp"
#include "game/state/game-state.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"

#include "misc/real.hpp"
#include "misc/random.hpp"

#include <cmath>


namespace game
{

    Trap::Trap(
        const std::size_t PLAYER_COUNT_MIN,
        const std::size_t PLAYER_COUNT_MAX,
        const stats::Trait_t DAMAGE_MIN,
        const stats::Trait_t DAMAGE_MAX,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const std::string & DESCRIPTION)
    :
        playerCountRange_(PLAYER_COUNT_MIN, PLAYER_COUNT_MAX),
        damageRange_(DAMAGE_MIN, DAMAGE_MAX),
        soundEffect_(SOUND_EFFECT),
        descPrefix_(DESCRIPTION),
        descPostfix_(""),
        willDescUseContainerName_(false)
    {}


    Trap::Trap(
        const std::size_t PLAYER_COUNT_MIN,
        const std::size_t PLAYER_COUNT_MAX,
        const stats::Trait_t DAMAGE_MIN,
        const stats::Trait_t DAMAGE_MAX,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const std::string & DESC_PREFIX,
        const std::string & DES_POSTFIX)
        :
        playerCountRange_(PLAYER_COUNT_MIN, PLAYER_COUNT_MAX),
        damageRange_(DAMAGE_MIN, DAMAGE_MAX),
        soundEffect_(SOUND_EFFECT),
        descPrefix_(DESC_PREFIX),
        descPostfix_(DES_POSTFIX),
        willDescUseContainerName_(true)
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


    stats::Trait_t Trap::RandomDamage() const
    {
        auto const RANDOM_DAMAGE{
            static_cast<double>(misc::random::Int(damageRange_.Min(), damageRange_.Max())) };

        auto const SQRT_RANDOM_DAMAGE{ std::sqrt(static_cast<double>(RANDOM_DAMAGE)) };

        auto const AVG_PLAYER_RANK_MINUS_ONE{ [&]()
            {
                auto const AVG_PLAYER_RANK{ static_cast<double>(FindAveragePlayerRank()) };
                if (AVG_PLAYER_RANK < 2.0)
                {
                    return 1.0;
                }
                else
                {
                    return AVG_PLAYER_RANK - 1.0;
                }
            }() };

        auto const SQRT_AVG_PLAYER_RANK_MINUS_ONE{ std::sqrt(AVG_PLAYER_RANK_MINUS_ONE) };

        auto const TOTAL_DAMAGE{
            RANDOM_DAMAGE + (SQRT_RANDOM_DAMAGE * SQRT_AVG_PLAYER_RANK_MINUS_ONE) };

        return static_cast<stats::Trait_t>(TOTAL_DAMAGE);
    }


    int Trap::Severity() const
    {
        auto const SQRT_AVG_PLAYER_COUNT{
            std::sqrt(static_cast<double>(playerCountRange_.Mid())) };

        auto const AVG_DAMAGE{ static_cast<double>(damageRange_.Mid()) };

        return static_cast<int>(SQRT_AVG_PLAYER_COUNT * AVG_DAMAGE);
    }


    std::size_t Trap::RandomEffectedPlayersCount() const
    {
        return static_cast<std::size_t>(
            misc::random::Int(damageRange_.Min(), damageRange_.Max()));
    }


    stats::Trait_t Trap::FindAveragePlayerRank() const
    {
        stats::Trait_t rankSum{ 0 };
        auto const CHARACTER_PTRS{ Game::Instance()->State().Party().Characters() };
        for (auto const CHARACTER_PTR : CHARACTER_PTRS)
        {
            rankSum += CHARACTER_PTR->Rank();
        }

        return rankSum / static_cast<int>(CHARACTER_PTRS.size());
    }

}