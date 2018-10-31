// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// stage-setup-packet.cpp
//
#include "stage-setup-packet.hpp"

#include "creature/creature.hpp"
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"

namespace heroespath
{
namespace stage
{

    const std::string InventorySetupPacket::ToString() const
    {
        return "cmd=inventory_stage_setup, inventory_stage_prev_calling_phase="
            + game::Phase::ToStringNoThrow(prev_calling_phase) + ", inventory_stage_turn_creature="
            + turn_creature_ptr->NameAndRaceAndRole() + ", inventory_stage_inventory_creature="
            + inventory_creature_ptr->NameAndRaceAndRole();
    }

} // namespace stage
} // namespace heroespath
