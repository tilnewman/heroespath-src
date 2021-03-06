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
#include "misc/log-macros.hpp"

namespace heroespath
{
namespace item
{

    std::unique_ptr<ItemWarehouse> ItemWarehouse::instanceUPtr_;

    ItemWarehouse::ItemWarehouse()
        : warehouse_(1000) // found by experiment to be a good upper bound for the game
    {
        M_HP_LOG_DBG("Subsystem Construction: ItemWarehouse");
    }

    ItemWarehouse::~ItemWarehouse() { M_HP_LOG_DBG("Subsystem Destruction: ItemWarehouse"); }

    misc::NotNull<ItemWarehouse *> ItemWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: ItemWarehouse");
            Create();
        }

        return misc::NotNull<ItemWarehouse *>(instanceUPtr_.get());
    }

    void ItemWarehouse::Create()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<ItemWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Create() after Construction: ItemWarehouse");
        }
    }

    void ItemWarehouse::Destroy() { instanceUPtr_.reset(); }

} // namespace item
} // namespace heroespath
