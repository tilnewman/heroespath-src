// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// npc-warehouse.cpp
//
#include "npc-warehouse.hpp"

#include "game/npc.hpp"
#include "log/log-macros.hpp"

namespace heroespath
{
namespace game
{

    std::unique_ptr<NpcWarehouse> NpcWarehouse::instanceUPtr_;

    NpcWarehouse::NpcWarehouse()
        : warehouse_()
    {
        M_HP_LOG_DBG("Subsystem Construction: NpcWarehouse");
    }

    NpcWarehouse::~NpcWarehouse() { M_HP_LOG_DBG("Subsystem Destruction: NpcWarehouse"); }

    misc::NotNull<NpcWarehouse *> NpcWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() before Acquire(): NpcWarehouse");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void NpcWarehouse::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<NpcWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: NpcWarehouse");
        }
    }

    void NpcWarehouse::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "creature::NpcWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

} // namespace game
} // namespace heroespath
