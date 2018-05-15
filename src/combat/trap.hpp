// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_COMBAT_TRAP_HPP_INCLUDED
#define HEROESPATH_COMBAT_TRAP_HPP_INCLUDED
//
// trap.hpp
//
#include "misc/range.hpp"
#include "misc/types.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "stats/trait.hpp"

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

        sfml_util::sound_effect::Enum SoundEffect() const { return soundEffect_; }

        const std::string Description(const std::string & CONTAINER_NAME) const;

        Health_t RandomDamage() const;

        int Severity() const;

        std::size_t RandomEffectedPlayersCount() const;

        const std::string HitVerb() const { return hitVerb_; }

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
