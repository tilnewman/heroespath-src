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
#ifndef HEROESPATH_STATE_NPC_FACTORY_HPP_INCLUDED
#define HEROESPATH_STATE_NPC_FACTORY_HPP_INCLUDED
//
// npc-factory.hpp
//
#include "misc/not-null-warehouse.hpp"
#include "state/npc-placeholder.hpp"

namespace heroespath
{
namespace state
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

} // namespace state
} // namespace heroespath

#endif // HEROESPATH_STATE_NPC_FACTORY_HPP_INCLUDED
