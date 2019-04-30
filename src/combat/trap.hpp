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
#include "creature/trait.hpp"
#include "game/strong-types.hpp"
#include "gui/sound-effects-enum.hpp"
#include "misc/range.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace combat
{

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
            const gui::sound_effect::Enum SOUND_EFFECT,
            const std::string & DESCRIPTION);

        Trap(
            const std::size_t PLAYER_COUNT_MIN,
            const std::size_t PLAYER_COUNT_MAX,
            const Health_t & DAMAGE_MIN,
            const Health_t & DAMAGE_MAX,
            const std::string & HIT_VERB,
            const gui::sound_effect::Enum SOUND_EFFECT,
            const std::string & DESC_PREFIX,
            const std::string & DES_POSTFIX);

        gui::sound_effect::Enum SoundEffect() const { return soundEffect_; }

        const std::string Description(const std::string & CONTAINER_NAME) const;

        Health_t RandomDamage() const;

        int Severity() const;

        std::size_t RandomEffectedPlayersCount() const;

        const std::string HitVerb() const { return hitVerb_; }

    private:
        Rank_t FindAveragePlayerRank() const;

    private:
        misc::Range<std::size_t> playerCountRange_;
        misc::Range<double> damageRange_;
        gui::sound_effect::Enum soundEffect_;
        std::string descPrefix_;
        std::string descPostfix_;
        bool willDescUseContainerName_;
        std::string hitVerb_;
    };

    using TrapVec_t = std::vector<Trap>;

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_COMBAT_TRAP_HPP_INCLUDED
