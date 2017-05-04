//
// ownership-chance-types.cpp
//
#include "ownership-chance-types.hpp"


namespace game
{
namespace non_player
{
namespace ownership
{
namespace chance
{

    ItemChances::ItemChances(const CountChanceMap_t &    NUM_OWNED_MAP,
                             const float                 CHANCE_EQUIPPED,
                             const MaterialChanceMap_t & MAT_CH_MAP_PRI,
                             const MaterialChanceMap_t & MAT_CH_MAP_SEC)
    :
        chance_equipped (CHANCE_EQUIPPED),
        num_owned_map   (NUM_OWNED_MAP),
        mat_map_pri     (MAT_CH_MAP_PRI),
        mat_map_sec     (MAT_CH_MAP_SEC)
    {
        if (num_owned_map.empty())
            num_owned_map[0] = 1.0f;
    }


    ItemChances::ItemChances(const float                 CHANCE_OWNED,
                             const float                 CHANCE_EQUIPPED,
                             const MaterialChanceMap_t & MAT_CH_MAP_PRI,
                             const MaterialChanceMap_t & MAT_CH_MAP_SEC)
    :
        chance_equipped (CHANCE_EQUIPPED),
        num_owned_map   (),
        mat_map_pri     (MAT_CH_MAP_PRI),
        mat_map_sec     (MAT_CH_MAP_SEC)
    {
        SetCountChanceSingle(CHANCE_OWNED);
    }


    ItemChances::ItemChances(const float                CHANCE_OWNED,
                             const float                CHANCE_EQUIPPED,
                             const item::material::Enum MATERIAL_PRIMARY,
                             const item::material::Enum MATERIAL_SECONDARY)
    :
        chance_equipped (CHANCE_EQUIPPED),
        num_owned_map   (),
        mat_map_pri     (),
        mat_map_sec     ()
    {
        SetCountChanceSingle(CHANCE_OWNED);
        mat_map_pri[MATERIAL_PRIMARY] = 1.0f;
        mat_map_sec[MATERIAL_SECONDARY] = 1.0f;
    }


    std::size_t ItemChances::CountOwned() const
    {
        if (num_owned_map.empty())
            return 0;
        else
            return MappedRandomFloatChance<std::size_t>(num_owned_map);
    }


    item::material::Enum ItemChances::RandomMaterialPri() const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((mat_map_pri.empty() == false), "game::non_player::ownership::chances::ItemChances::RandomMaterialPri() called when mat_map_pri is empty.");
        return MappedRandomFloatChance<item::material::Enum>(mat_map_pri);
    }


    item::material::Enum ItemChances::RandomMaterialSec() const
    {
        if (mat_map_sec.empty())
            return item::material::Nothing;
        else
            return MappedRandomFloatChance<item::material::Enum>(mat_map_sec);
    }


    void ItemChances::SetCountChanceSingleCertain()
    {
        num_owned_map.clear();
        SetCountChanceSingle(1.0f);
    }


    void ItemChances::SetCountChanceSingle(const float CHANCE)
    {
        num_owned_map.clear();
        SetCountChance(1, CHANCE);
        SetCountChance(0, 1.0f - CHANCE);
    }


    void ItemChances::SetCountChanceIncrement(const float CHANCE)
    {
        const std::size_t MAX_ITERATIONS(1000);
        std::size_t i(0);
        while(i<MAX_ITERATIONS)
        {
            if (sfml_util::IsRealClose<float>(num_owned_map[++i], 0.0f))
            {
                num_owned_map[i] = 1.0f;
                break;
            }
        };

        M_ASSERT_OR_LOGANDTHROW_SS((i < MAX_ITERATIONS), "game::non_player::ownership::ItemChances::SetCountChanceIncrement(" << CHANCE << ") reached the sentinel of " << MAX_ITERATIONS << " iterations.  Something is very wrong...");
    }


    ClothingChances::ClothingChances(const float                 SHIRT,
                                     const float                 GLOVES,
                                     const float                 PANTS,
                                     const float                 BOOTS,
                                     const float                 VEST,
                                     const CoverChanceMap_t &    COVER_CHANCE_MAP,
                                     const MaterialChanceMap_t & MAT_CH_MAP_PRI,
                                     const MaterialChanceMap_t & MAT_CH_MAP_SEC)
    :
        shirt    (SHIRT, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC),
        gloves   (GLOVES,1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC),
        pants    (PANTS, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC),
        boots    (BOOTS, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC),
        vest     (VEST,  1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC),
        cover_map(COVER_CHANCE_MAP)
    {}


    item::armor::cover_type::Enum ClothingChances::RandomCoverType() const
    {
        std::map<item::armor::cover_type::Enum, float> coverChanceMap;

        for (auto const & NEXT_CHANCE_PAIR : cover_map)
            if (NEXT_CHANCE_PAIR.second.IsOwned())
                coverChanceMap[NEXT_CHANCE_PAIR.first] = utilz::random::Float();

        if (coverChanceMap.size() == 1)
        {
            return coverChanceMap.begin()->first;
        }
        else if (coverChanceMap.size() > 1)
        {
            float highestChance(0.0f);
            item::armor::cover_type::Enum highestEnum(coverChanceMap.begin()->first);
            for (auto const & NEXT_CHANCE_PAIR : coverChanceMap)
                if (NEXT_CHANCE_PAIR.second > highestChance)
                    highestEnum = NEXT_CHANCE_PAIR.first;

            return highestEnum;
        }
        else
            return item::armor::cover_type::Count;
    }


    ArmorItemChances::ArmorItemChances(const float                  CHANCE_OWNED,
                                       const ArmorTypeChanceMap_t & ARMOR_TYPE_CH_MAP,
                                       const MaterialChanceMap_t &  MAT_CH_MAP_PRI,
                                       const MaterialChanceMap_t &  MAT_CH_MAP_SEC)
    :
        ItemChances(CHANCE_OWNED, 1.0f, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC),
        type_map   (ARMOR_TYPE_CH_MAP)
    {}


    ArmorItemChances::ArmorItemChances(const float                        CHANCE_OWNED,
                                       const item::armor::base_type::Enum ARMOR_BASE_TYPE,
                                       const item::material::Enum         MATERIAL_PRIMARY,
                                       const item::material::Enum         MATERIAL_SECONDARY)
    :
        ItemChances(CHANCE_OWNED, 1.0f, MATERIAL_PRIMARY, MATERIAL_SECONDARY),
        type_map   ()
    {
        type_map[ARMOR_BASE_TYPE] = 1.0f;
    }


    ArmorChances::ArmorChances(const ArmorItemChances & AVENTAIL,
                               const ArmorItemChances & SHIRT,
                               const ArmorItemChances & BRACERS,
                               const ArmorItemChances & GAUNTLETS,
                               const ArmorItemChances & PANTS,
                               const ArmorItemChances & BOOTS,
                               const HelmChanceMap_t &  HELM_MAP,
                               const CoverChanceMap_t & COVER_MAP,
                               const ShieldChanceMap_t& SHIELD_MAP)
    :
        aventail    (AVENTAIL),
        shirt       (SHIRT),
        bracers     (BRACERS),
        gauntlets   (GAUNTLETS),
        pants       (PANTS),
        boots       (BOOTS),
        helm_map    (HELM_MAP),
        cover_map   (COVER_MAP),
        shield_map  (SHIELD_MAP)
    {}


    KnifeItemChances::KnifeItemChances(const float                 CHANCE_OWNED,
                                       const float                 IS_EQUIPPED,
                                       const float                 IS_DAGGER,
                                       const SizeChanceMap_t &     SIZE_MAP,
                                       const MaterialChanceMap_t & MAT_CH_MAP_PRI,
                                       const MaterialChanceMap_t & MAT_CH_MAP_SEC)
    :
        ItemChances(CHANCE_OWNED, IS_EQUIPPED, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC),
        is_dagger  (IS_DAGGER),
        size_map   (SIZE_MAP)
    {}


    StaffItemChances::StaffItemChances(const float                 CHANCE_OWNED,
                                       const float                 IS_EQUIPPED,
                                       const float                 IS_QUARTERSTAFF,
                                       const MaterialChanceMap_t & MAT_CH_MAP_PRI,
                                       const MaterialChanceMap_t & MAT_CH_MAP_SEC)
    :
        ItemChances    (CHANCE_OWNED, IS_EQUIPPED, MAT_CH_MAP_PRI, MAT_CH_MAP_SEC),
        is_quarterstaff(IS_QUARTERSTAFF)
    {}


    InventoryChances::InventoryChances(const item::Coin_t       COINS_MIN,
                                       const item::Coin_t       COINS_MAX,
                                       const ClothingChances &  CLOTHES_CHANCES,
                                       const WeaponChances &    WEAPON_CHANCES,
                                       const ArmorChances &     ARMOR_CHANCES,
                                       const ItemChancePair_t & MISC_ITEM_CHANCES)
        :
            coins_min (COINS_MIN),
            coins_max (COINS_MAX),
            armor     (ARMOR_CHANCES),
            weapon    (WEAPON_CHANCES),
            clothes   (CLOTHES_CHANCES),
            misc_items(MISC_ITEM_CHANCES)
        {}



    WeaponChances::WeaponChances(const bool                     HAS_CLAWS,
                                 const bool                     HAS_BITE,
                                 const bool                     HAS_FISTS,
                                 const bool                     HAS_TENDRILS,
                                 const bool                     HAS_BREATH,
                                 const KnifeItemChances &       KNIFE,
                                 const StaffItemChances &       STAFF,
                                 const AxeChanceMap_t &         AXE_MAP,
                                 const ClubChanceMap_t &        CLUB_MAP,
                                 const WhipChanceMap_t &        WHIP_MAP,
                                 const SwordChanceMap_t &       SWORD_MAP,
                                 const ProjectileChanceMap_t &  PROJECTILE_MAP,
                                 const BladedStaffChanceMap_t & BLADEDSTAFF_MAP)
        :
            has_claws       (HAS_CLAWS),
            has_bite        (HAS_BITE),
            has_fists       (HAS_FISTS),
            has_tendrils    (HAS_TENDRILS),
            has_breath      (HAS_BREATH),
            knife           (KNIFE),
            staff           (STAFF),
            axe_map         (AXE_MAP),
            club_map        (CLUB_MAP),
            whip_map        (WHIP_MAP),
            sword_map       (SWORD_MAP),
            projectile_map  (PROJECTILE_MAP),
            bladedstaff_map (BLADEDSTAFF_MAP)
        {}

}
}
}
}
