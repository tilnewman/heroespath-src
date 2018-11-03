// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GAME_NPC_FACTORY_HPP_INCLUDED
#define HEROESPATH_GAME_NPC_FACTORY_HPP_INCLUDED
//
// npc-factory.hpp
//
#include "game/npc-placeholder.hpp"

#include <vector>

namespace heroespath
{
namespace game
{

    class Npc;
    using NpcPtr_t = misc::NotNull<Npc *>;
    using NpcPVec_t = std::vector<NpcPtr_t>;

    // Responsible for making random NPCs.
    class NpcFactory
    {
    public:
        NpcFactory(const NpcFactory &) = delete;
        NpcFactory(NpcFactory &&) = delete;
        NpcFactory & operator=(const NpcFactory &) = delete;
        NpcFactory & operator=(NpcFactory &&) = delete;

        NpcFactory() = default;

        const NpcPVec_t Make(const NpcPlaceholder &) const;
    };

} // namespace game
} // namespace heroespath

#endif // HEROESPATH_GAME_NPC_FACTORY_HPP_INCLUDED
