// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_ENCHANTMENT_WAREHOUSE_HPP_INCLUDED
#define HEROESPATH_CREATURE_ENCHANTMENT_WAREHOUSE_HPP_INCLUDED
//
// enchantment-warehouse.hpp
//  Responsible for managing the lifetime of all enchantment objects.
//
#include "misc/not-null-warehouse.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace creature
{

    class Enchantment;
    using EnchantmentPtr_t = misc::NotNull<Enchantment *>;
    using EnchantmentPVec_t = std::vector<EnchantmentPtr_t>;

    // Subsystem responsible for the lifetimes of Enchantment objects.
    // This class does not new the objects, but it does delete them.
    class EnchantmentWarehouse
    {
    public:
        EnchantmentWarehouse(const EnchantmentWarehouse &) = delete;
        EnchantmentWarehouse(EnchantmentWarehouse &&) = delete;
        EnchantmentWarehouse & operator=(const EnchantmentWarehouse &) = delete;
        EnchantmentWarehouse & operator=(EnchantmentWarehouse &&) = delete;

        EnchantmentWarehouse();
        ~EnchantmentWarehouse();

        static misc::NotNull<EnchantmentWarehouse *> Instance();
        static void Acquire();
        static void Release();

        static misc::NotNullWarehouse<Enchantment> & Access() { return Instance()->Warehouse(); }
        misc::NotNullWarehouse<Enchantment> & Warehouse() { return warehouse_; }

    private:
        static std::unique_ptr<EnchantmentWarehouse> instanceUPtr_;
        misc::NotNullWarehouse<Enchantment> warehouse_;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_ENCHANTMENT_WAREHOUSE_HPP_INCLUDED
