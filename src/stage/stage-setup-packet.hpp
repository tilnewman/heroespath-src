// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_STAGE_SETUP_PACKET_HPP_INCLUDED
#define HEROESPATH_STAGE_STAGE_SETUP_PACKET_HPP_INCLUDED
//
// stage-setup-packet.hpp
//
#include "game/phase-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "stage/i-stage.hpp"
#include "stage/stage-enum.hpp"

#include <sstream>
#include <string>
#include <tuple>

namespace heroespath
{

namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
} // namespace creature

namespace popup
{
    class PopupInfo;
}

namespace stage
{

    // wraps all the information needed to setup the Inventory Stage
    struct InventorySetupPacket
    {
        InventorySetupPacket(
            const game::Phase::Enum PREV_CALLING_PHASE,
            const creature::CreaturePtr_t TURN_CREATURE_PTR,
            const creature::CreaturePtr_t INVENTORY_CREATURE_PTR)
            : prev_calling_phase(PREV_CALLING_PHASE)
            , turn_creature_ptr(TURN_CREATURE_PTR)
            , inventory_creature_ptr(INVENTORY_CREATURE_PTR)
        {}

        InventorySetupPacket(const InventorySetupPacket &) = default;
        InventorySetupPacket(InventorySetupPacket &&) = default;
        InventorySetupPacket & operator=(const InventorySetupPacket &) = default;
        InventorySetupPacket & operator=(InventorySetupPacket &&) = default;

        const std::string ToString() const;

        game::Phase::Enum prev_calling_phase;
        creature::CreaturePtr_t turn_creature_ptr;
        creature::CreaturePtr_t inventory_creature_ptr;
    };

    using InventorySetupPacketOpt_t = boost::optional<InventorySetupPacket>;

    inline bool operator==(const InventorySetupPacket & L, const InventorySetupPacket & R)
    {
        return (
            std::tie(L.prev_calling_phase, L.turn_creature_ptr, L.inventory_creature_ptr)
            == std::tie(R.prev_calling_phase, R.turn_creature_ptr, R.inventory_creature_ptr));
    }

    inline bool operator!=(const InventorySetupPacket & L, const InventorySetupPacket & R)
    {
        return !(L == R);
    }

    // wraps all the information needed to setup ANY Stage
    struct SetupPacket
    {
        explicit SetupPacket(
            const stage::Stage::Enum STAGE,
            const bool WILL_ADVANCE_TURN = false,
            const InventorySetupPacketOpt_t INVENTORY_PACKET_OPT = boost::none)
            : stage(STAGE)
            , will_advance_turn(WILL_ADVANCE_TURN)
            , inventory_packet_opt(INVENTORY_PACKET_OPT)
        {}

        SetupPacket(const SetupPacket &) = default;
        SetupPacket(SetupPacket &&) = default;
        SetupPacket & operator=(const SetupPacket &) = default;
        SetupPacket & operator=(SetupPacket &&) = default;

        const std::string ToString() const
        {
            std::ostringstream ss;

            ss << "cmd=stage_setup=" << stage::Stage::ToStringNoThrow(stage);

            if (will_advance_turn)
            {
                ss << ", stage_setup_will_advance_turn";
            }

            if (inventory_packet_opt)
            {
                ss << ", " << inventory_packet_opt->ToString();
            }

            return ss.str();
        }

        stage::Stage::Enum stage;
        bool will_advance_turn;
        InventorySetupPacketOpt_t inventory_packet_opt;
    };

    using SetupPacketOpt_t = boost::optional<SetupPacket>;

    inline bool operator==(const SetupPacket & L, const SetupPacket & R)
    {
        return (
            std::tie(L.stage, L.will_advance_turn, L.inventory_packet_opt)
            == std::tie(R.stage, R.will_advance_turn, R.inventory_packet_opt));
    }

    inline bool operator!=(const SetupPacket & L, const SetupPacket & R) { return !(L == R); }

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_STAGE_SETUP_PACKET_HPP_INCLUDED
