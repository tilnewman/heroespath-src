// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_CREATURE_WAREHOUSE_HPP_INCLUDED
#define HEROESPATH_CREATURE_CREATURE_WAREHOUSE_HPP_INCLUDED
//
// creature-warehouse.hpp
//
#include "misc/not-null-warehouse.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
    using CreatureUPtr_t = std::unique_ptr<Creature>;
    using CreatureUVec_t = std::vector<CreatureUPtr_t>;

    // Subsystem responsible for the lifetimes of player::Character objects.
    // This class does not new the objects, but it is the one and only place where they are deleted.
    class CreatureWarehouse
    {
    public:
        CreatureWarehouse(const CreatureWarehouse &) = delete;
        CreatureWarehouse(CreatureWarehouse &&) = delete;
        CreatureWarehouse & operator=(const CreatureWarehouse &) = delete;
        CreatureWarehouse & operator=(CreatureWarehouse &&) = delete;

    public:
        CreatureWarehouse();
        ~CreatureWarehouse();

        static misc::NotNull<CreatureWarehouse *> Instance();
        static void Acquire();
        static void Release();

        static misc::NotNullWarehouse<creature::Creature> & Access()
        {
            return Instance()->Warehouse();
        }

        misc::NotNullWarehouse<creature::Creature> & Warehouse() { return warehouse_; }

    private:
        static std::unique_ptr<CreatureWarehouse> instanceUPtr_;
        misc::NotNullWarehouse<creature::Creature> warehouse_;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_CREATURE_WAREHOUSE_HPP_INCLUDED
