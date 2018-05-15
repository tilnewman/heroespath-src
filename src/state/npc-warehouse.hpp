// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STATE_NPC_WAREHOUSE_HPP_INCLUDED
#define HEROESPATH_STATE_NPC_WAREHOUSE_HPP_INCLUDED
//
// npc-warehouse.hpp
//  Responsible for managing the lifetime of all Npc objects.
//
#include "misc/not-null-warehouse.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace state
{

    class Npc;
    using NpcPtr_t = misc::NotNull<Npc *>;
    using NpcPVec_t = std::vector<NpcPtr_t>;

    // Subsystem responsible for the lifetimes of Npc objects.
    // This class does not new the objects, but it does delete them.
    class NpcWarehouse
    {
    public:
        NpcWarehouse(const NpcWarehouse &) = delete;
        NpcWarehouse(NpcWarehouse &&) = delete;
        NpcWarehouse & operator=(const NpcWarehouse &) = delete;
        NpcWarehouse & operator=(NpcWarehouse &&) = delete;

    public:
        NpcWarehouse();
        ~NpcWarehouse();

        static misc::NotNull<NpcWarehouse *> Instance();
        static void Acquire();
        static void Release();

        static misc::NotNullWarehouse<Npc> & Access() { return Instance()->Warehouse(); }
        misc::NotNullWarehouse<Npc> & Warehouse() { return warehouse_; }

    private:
        static std::unique_ptr<NpcWarehouse> instanceUPtr_;
        misc::NotNullWarehouse<Npc> warehouse_;
    };

} // namespace state
} // namespace heroespath

#endif // HEROESPATH_STATE_NPC_WAREHOUSE_HPP_INCLUDED
