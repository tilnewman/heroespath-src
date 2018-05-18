// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_ROLESTATS_HPP_INCLUDED
#define HEROESPATH_CREATURE_ROLESTATS_HPP_INCLUDED
//
// role-stats.hpp
//  Specific creature role stat modifiers.
//
#include "creature/role-enum.hpp"
#include "creature/stat-set.hpp"
#include "misc/vector-map.hpp"
#include <string>

namespace heroespath
{
namespace creature
{

    using RoleStatSetMap_t = misc::VectorMap<role::Enum, StatSet>;

    // Responsible for role related stat modifiers, for loading them from the
    // game data file and for providing access to them.
    struct RoleStatModifier
    {
        static const StatSet Get(const role::Enum);

    private:
        static RoleStatSetMap_t roleStatSetMap_;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ROLESTATS_HPP_INCLUDED
