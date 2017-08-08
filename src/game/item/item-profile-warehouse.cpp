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
// item-profile-warehouse.cpp
//
#include "item-profile-warehouse.hpp"

#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"


namespace game
{
namespace item
{

    std::unique_ptr<ItemProfileWarehouse> ItemProfileWarehouse::instanceUPtr_{ nullptr };


    ItemProfileWarehouse::ItemProfileWarehouse()
    :
        vec_()
    {
        M_HP_LOG_DBG("Singleton Construction: ItemProfileWarehouse");
    }


    ItemProfileWarehouse::~ItemProfileWarehouse()
    {
        M_HP_LOG_DBG("Singleton Destruction: ItemProfileWarehouse");
    }


    ItemProfileWarehouse * ItemProfileWarehouse::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): ItemProfileWarehouse");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void ItemProfileWarehouse::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new ItemProfileWarehouse);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: ItemProfileWarehouse");
        }
    }


    void ItemProfileWarehouse::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "game::creature::ItemProfileWarehouse::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    void ItemProfileWarehouse::Setup()
    {
        std::size_t count{ 0 };

        for (int i(1); i < item::unique_type::Count; ++i)
        {
            auto const NEXT_UNIQUE_ENUM{ static_cast<item::unique_type::Enum>(i) };

            auto const ELEMENT_TYPE_COMBS_VEC{ item::element_type::Combinations(
                item::unique_type::ElementTypes(NEXT_UNIQUE_ENUM)) };

            for (auto const NEXT_ELEMENT_TYPE : ELEMENT_TYPE_COMBS_VEC)
            {
                auto const MATERIALS{ item::unique_type::Materials(NEXT_UNIQUE_ENUM) };

                M_ASSERT_OR_LOGANDTHROW_SS((MATERIALS.first.empty() == false),
                    "game::item::ItemProfileWarehouse::Setup() failed to find any primary "
                    << "materials for unique_type="
                    << item::unique_type::ToString(NEXT_UNIQUE_ENUM));

                auto didAddProfile{ false };

                for (auto const NEXT_MATERIAL_PRIMARY : MATERIALS.first)
                {
                    if (MATERIALS.second.empty())
                    {
                        ItemProfile p;
                        
                        p.SetUnique(NEXT_UNIQUE_ENUM,
                                    NEXT_MATERIAL_PRIMARY,
                                    item::material::Nothing,
                                    NEXT_ELEMENT_TYPE);

                        p.SetMisc(item::unique_type::MiscType(NEXT_UNIQUE_ENUM),
                                                              NEXT_MATERIAL_PRIMARY,
                                                              item::material::Nothing);

                        vec_.push_back(p);
                        didAddProfile = true;
                        ++count;
                    }
                    else
                    {
                        for (auto const NEXT_MATERIAL_SECONDARY : MATERIALS.second)
                        {
                            if (NEXT_MATERIAL_PRIMARY == NEXT_MATERIAL_SECONDARY)
                            {
                                continue;
                            }

                            ItemProfile p;
                            
                            p.SetUnique(NEXT_UNIQUE_ENUM,
                                        NEXT_MATERIAL_PRIMARY,
                                        NEXT_MATERIAL_SECONDARY,
                                        NEXT_ELEMENT_TYPE);

                            p.SetMisc(item::unique_type::MiscType(NEXT_UNIQUE_ENUM),
                                                              NEXT_MATERIAL_PRIMARY,
                                                              NEXT_MATERIAL_SECONDARY);

                            vec_.push_back(p);
                            didAddProfile = true;
                            ++count;
                        }
                    }
                }

                M_ASSERT_OR_LOGANDTHROW_SS((didAddProfile),
                    "game::item::ItemProfileWarehouse::Setup() failed to find any valid "
                    << "material combinations for unique_type="
                    << item::unique_type::ToString(NEXT_UNIQUE_ENUM));
            }
        }

        count += SetupProfilesForMiscType(item::misc_type::LockPicks);
        count += SetupProfilesForMiscType(item::misc_type::Spider_Eggs);



        M_HP_LOG("ItemProfileWarehouse Setup created " << count << " profiles.");
    }


    std::size_t ItemProfileWarehouse::SetupProfilesForMiscType(const item::misc_type::Enum E)
    {
        std::size_t count{ 0 };
        auto const MATERIALS{ item::misc_type::Materials(E) };

        M_ASSERT_OR_LOGANDTHROW_SS((MATERIALS.first.empty() == false),
            "game::item::ItemProfileWarehouse::SetupProfilesForMiscType() failed to find any "
            << " primary materials for misc_type=" << item::misc_type::ToString(E));

        auto didAddProfile{ false };

        for (auto const NEXT_MATERIAL_PRIMARY : MATERIALS.first)
        {
            if (MATERIALS.second.empty())
            {
                ItemProfile p;
                p.SetMisc(E, NEXT_MATERIAL_PRIMARY, item::material::Nothing);
                vec_.push_back(p);
                didAddProfile = true;
                ++count;
            }
            else
            {
                for (auto const NEXT_MATERIAL_SECONDARY : MATERIALS.second)
                {
                    if (NEXT_MATERIAL_PRIMARY == NEXT_MATERIAL_SECONDARY)
                    {
                        continue;
                    }

                    ItemProfile p;
                    p.SetMisc(E, NEXT_MATERIAL_PRIMARY, NEXT_MATERIAL_SECONDARY);
                    vec_.push_back(p);
                    didAddProfile = true;
                    ++count;
                }
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS((didAddProfile),
            "game::item::ItemProfileWarehouse::SetupProfilesForMiscType() failed to find any"
            << " valid material combinations for misc_type=" << item::misc_type::ToString(E));

        return count;
    }

}
}
