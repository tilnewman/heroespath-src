// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// item-warehouse.cpp
//
#include "item-warehouse.hpp"

#include "item/item.hpp"
#include "log/log-macros.hpp"

namespace heroespath
{
namespace item
{

    std::unique_ptr<ItemWarehouse> ItemWarehouse::instanceUPtr_;

    ItemWarehouse::ItemWarehouse()
        : warehouse_()
    {
        M_HP_LOG_DBG("Subsystem Construction: ItemWarehouse");
    }

    ItemWarehouse::~ItemWarehouse() { M_HP_LOG_DBG("Subsystem Destruction: ItemWarehouse"); }

    misc::NotNull<ItemWarehouse *> ItemWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() before Acquire(): ItemWarehouse");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void ItemWarehouse::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<ItemWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: ItemWarehouse");
        }
    }

    void ItemWarehouse::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "item::ItemWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

} // namespace item
} // namespace heroespath
