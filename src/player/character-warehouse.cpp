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
// character-warehouse.cpp (player)
//
#include "character-warehouse.hpp"

#include "creature/creature.hpp"
#include "log/log-macros.hpp"

namespace heroespath
{
namespace player
{

    std::unique_ptr<CharacterWarehouse> CharacterWarehouse::instanceUPtr_{ nullptr };

    CharacterWarehouse::CharacterWarehouse()
        : warehouse_()
    {
        M_HP_LOG_DBG("Singleton Construction: Player CharacterWarehouse");
    }

    CharacterWarehouse::~CharacterWarehouse()
    {
        M_HP_LOG_DBG("Singleton Destruction: Player CharacterWarehouse");
    }

    CharacterWarehouse * CharacterWarehouse::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Singleton Instance() before Acquire(): Player CharacterWarehouse");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void CharacterWarehouse::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<CharacterWarehouse>();
        }
        else
        {
            M_HP_LOG_ERR("Singleton Acquire() after Construction: Player CharacterWarehouse");
        }
    }

    void CharacterWarehouse::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_),
            "player::CharacterWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    const creature::CreaturePtr_t
        CharacterWarehouse::Store(const creature::CreaturePtr_t CHARACTER_PTR)
    {
        return warehouse_.Store(CHARACTER_PTR);
    }

    void CharacterWarehouse::Free(const creature::CreaturePtr_t CHARACTER_PTR)
    {
        warehouse_.Free(CHARACTER_PTR);
    }

    void CharacterWarehouse::Free(creature::CreaturePVec_t & creaturePVec)
    {
        warehouse_.Free(creaturePVec);
    }

} // namespace player
} // namespace heroespath
