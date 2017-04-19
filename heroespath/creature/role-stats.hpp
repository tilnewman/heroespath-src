#ifndef APPBASE_HEROESPATH_CREATURE_ROLESTATS_INCLUDED
#define APPBASE_HEROESPATH_CREATURE_ROLESTATS_INCLUDED
//
// role-stats.hpp
//  Specific creature role stat modifiers.
//
#include "heroespath/creature/role-enum.hpp"
#include "heroespath/stats/stat-set.hpp"


namespace heroespath
{
namespace creature
{

    const stats::StatSet StatModifierByRole(const role::Enum);

}
}
#endif //APPBASE_HEROESPATH_CREATURE_ROLESTATS_INCLUDED
