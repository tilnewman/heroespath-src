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
// armor-ratings.cpp
//
#include "armor-ratings.hpp"

#include "game/item/item.hpp"
#include "game/item/armor-factory.hpp"
#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"


namespace game
{
namespace item
{

    std::unique_ptr<ArmorRatings> ArmorRatings::instanceUPtr_{ nullptr };


    ArmorRatings::ArmorRatings()
    :
        clothesCloth_             (0),
        clothesSoftLeather_       (0),
        clothesHardLeather_       (0),
        armoredLesserSoftLeather_ (0),
        armoredLesserSteel_       (0),
        armoredLesserDiamond_     (0),
        armoredGreaterSoftLeather_(0),
        armoredGreaterSteel_      (0),
        armoredGreaterDiamond_    (0)
    {
        M_HP_LOG_DBG("Singleton Construction: ArmorRatings");
    }


    ArmorRatings::~ArmorRatings()
    {
        M_HP_LOG_DBG("Singleton Destruction: ArmorRatings");
    }


    ArmorRatings * ArmorRatings::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): ArmorRatings");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void ArmorRatings::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_ = std::make_unique<ArmorRatings>();
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: ArmorRatings");
        }
    }


    void ArmorRatings::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "sfml_util::ArmorRatings::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    void ArmorRatings::Setup()
    {
        clothesCloth_       = ClothesSetRating(item::material::Cloth);
        clothesSoftLeather_ = ClothesSetRating(item::material::SoftLeather);
        clothesHardLeather_ = ClothesSetRating(item::material::HardLeather);

        armoredLesserSoftLeather_ = LesserArmorSetRating(item::material::SoftLeather);
        armoredLesserSteel_       = LesserArmorSetRating(item::material::Steel);
        armoredLesserDiamond_     = LesserArmorSetRating(item::material::Diamond);

        armoredGreaterSoftLeather_ = GreaterArmorSetRating(item::material::SoftLeather);
        armoredGreaterSteel_       = GreaterArmorSetRating(item::material::Steel);
        armoredGreaterDiamond_     = GreaterArmorSetRating(item::material::Diamond);
    }


    stats::Trait_t ArmorRatings::ClothesSetRating(
        const item::material::Enum MATERIAL_ENUM) const
    {
        using namespace item::armor;

        auto const SKIN_PTR{ ArmorFactory::Make_Skin(material::Flesh, 1, false) };
        auto const SHIRT_PTR{ ArmorFactory::Make_Shirt(base_type::Plain) };
        auto const PANTS_PTR{ ArmorFactory::Make_Pants(base_type::Plain, material::Cloth) };
        auto const BOOTS_PTR{ ArmorFactory::Make_Boots(base_type::Plain, MATERIAL_ENUM) };
        auto const VEST_PTR{ ArmorFactory::Make_Cover(cover_type::Vest, MATERIAL_ENUM) };
        auto const CLOAK_PTR{ ArmorFactory::Make_Cover(cover_type::Cloak, MATERIAL_ENUM) };
        auto const GLOVES_PTR{ ArmorFactory::Make_Gauntlets(base_type::Plain, MATERIAL_ENUM) };

        return GetTotalArmorRating( ItemPVec_t{ SKIN_PTR,
                                                SHIRT_PTR,
                                                PANTS_PTR,
                                                BOOTS_PTR,
                                                VEST_PTR,
                                                CLOAK_PTR,
                                                GLOVES_PTR });
    }


    stats::Trait_t ArmorRatings::LesserArmorSetRating(
        const item::material::Enum E) const
    {
        using namespace item::armor;

        auto const SHIELD_PTR{ ArmorFactory::Make_Shield(shield_type::Buckler,
            ((E == material::SoftLeather) ? material::Wood : E)) };

        auto const HELM_PTR{ ArmorFactory::Make_Helm(((E == material::SoftLeather) ?
            helm_type::Leather : helm_type::Kettle), E) };

        auto const BASE_TYPE{ ((E == material::SoftLeather) ?
            base_type::Plain : base_type::Mail) };

        auto const GAUNTLETS_PTR{ ArmorFactory::Make_Gauntlets(BASE_TYPE, E) };
        auto const PANTS_PTR{ ArmorFactory::Make_Pants(BASE_TYPE, E) };
        auto const BOOTS_PTR{ ArmorFactory::Make_Boots(BASE_TYPE, E) };
        auto const SHIRT_PTR{ ArmorFactory::Make_Shirt(BASE_TYPE) };
        auto const BRACER_PTR{ ArmorFactory::Make_Bracer(BASE_TYPE, E) };
        auto const AVENTAIL_PTR{ ArmorFactory::Make_Aventail(BASE_TYPE, E) };
        auto const COVER_PTR{ ArmorFactory::Make_Cover(cover_type::Cloak, material::SoftLeather) };
        auto const SKIN_PTR{ ArmorFactory::Make_Skin(material::Flesh, 1, false) };

        return GetTotalArmorRating(ItemPVec_t{ SHIELD_PTR,
                                               HELM_PTR,
                                               GAUNTLETS_PTR,
                                               PANTS_PTR,
                                               BOOTS_PTR,
                                               SHIRT_PTR,
                                               BRACER_PTR,
                                               AVENTAIL_PTR,
                                               COVER_PTR,
                                               SKIN_PTR } );
    }


    stats::Trait_t ArmorRatings::GreaterArmorSetRating(
        const item::material::Enum E) const
    {
        using namespace item::armor;

        auto const SHIELD_PTR{ ArmorFactory::Make_Shield(shield_type::Pavis,
            ((E == material::SoftLeather) ? material::Wood : E)) };

        auto const HELM_PTR{ ArmorFactory::Make_Helm(((E == material::SoftLeather) ?
            helm_type::Leather : helm_type::Great), E) };

        auto const BASE_TYPE{ ((E == material::SoftLeather) ?
            base_type::Plain : base_type::Plate) };

        auto const GAUNTLETS_PTR{ ArmorFactory::Make_Gauntlets(BASE_TYPE, E, E) };
        auto const PANTS_PTR{ ArmorFactory::Make_Pants(BASE_TYPE, E) };
        auto const BOOTS_PTR{ ArmorFactory::Make_Boots(BASE_TYPE, E, E) };
        auto const SHIRT_PTR{ ArmorFactory::Make_Shirt(BASE_TYPE, E) };
        auto const BRACER_PTR{ ArmorFactory::Make_Bracer(BASE_TYPE, E, E) };
        auto const AVENTAIL_PTR{ ArmorFactory::Make_Aventail(BASE_TYPE, E, E) };
        auto const COVER_PTR{ ArmorFactory::Make_Cover(cover_type::Cloak, material::HardLeather) };
        auto const SKIN_PTR{ ArmorFactory::Make_Skin(material::Flesh, 1, false) };

        return GetTotalArmorRating(ItemPVec_t{ SHIELD_PTR,
                                               HELM_PTR,
                                               GAUNTLETS_PTR,
                                               PANTS_PTR,
                                               BOOTS_PTR,
                                               SHIRT_PTR,
                                               BRACER_PTR,
                                               AVENTAIL_PTR,
                                               COVER_PTR,
                                               SKIN_PTR } );
    }


    stats::Trait_t ArmorRatings::GetTotalArmorRating(const ItemPVec_t & ITEM_PVEC) const
    {
        stats::Trait_t totalArmorRating{ 0 };

        for (auto const NEXT_ITEM_PTR : ITEM_PVEC)
        {
            totalArmorRating += NEXT_ITEM_PTR->ArmorRating();
        }

        return totalArmorRating;
    }

}
}
