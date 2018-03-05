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
#ifndef HEROESPATH_COMBAT_TRAP_HPP_INCLUDED
#define HEROESPATH_COMBAT_TRAP_HPP_INCLUDED
//
// trap.hpp
//
#include "misc/types.hpp"
#include "stats/trait.hpp"

#include "sfml-util/sound-effects-enum.hpp"

#include "misc/range.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace combat
{

    using SizeRange_t = misc::Range<std::size_t>;
    using HealthRange_t = misc::Range<Health_t>;

    // Responsible for wrapping common state and operations of traps.
    class Trap
    {
    public:
        Trap();

        Trap(
            const std::size_t PLAYER_COUNT_MIN,
            const std::size_t PLAYER_COUNT_MAX,
            const Health_t & DAMAGE_MIN,
            const Health_t & DAMAGE_MAX,
            const std::string & HIT_VERB,
            const sfml_util::sound_effect::Enum SOUND_EFFECT,
            const std::string & DESCRIPTION);

        Trap(
            const std::size_t PLAYER_COUNT_MIN,
            const std::size_t PLAYER_COUNT_MAX,
            const Health_t & DAMAGE_MIN,
            const Health_t & DAMAGE_MAX,
            const std::string & HIT_VERB,
            const sfml_util::sound_effect::Enum SOUND_EFFECT,
            const std::string & DESC_PREFIX,
            const std::string & DES_POSTFIX);

        inline sfml_util::sound_effect::Enum SoundEffect() const { return soundEffect_; }

        const std::string Description(const std::string & CONTAINER_NAME) const;

        Health_t RandomDamage() const;

        int Severity() const;

        std::size_t RandomEffectedPlayersCount() const;

        inline const std::string HitVerb() const { return hitVerb_; }

    private:
        Rank_t FindAveragePlayerRank() const;

    private:
        SizeRange_t playerCountRange_;
        HealthRange_t damageRange_;
        sfml_util::sound_effect::Enum soundEffect_;
        std::string descPrefix_;
        std::string descPostfix_;
        bool willDescUseContainerName_;
        std::string hitVerb_;
    };

    using TrapVec_t = std::vector<Trap>;
} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TRAP_HPP_INCLUDED
