#ifndef GAME_NONPLAYER_CHANCEFACTORY_INCLUDED
#define GAME_NONPLAYER_CHANCEFACTORY_INCLUDED
//
// inventory-factory.hpp
//  Code responsible for creating non-player-characters items. (clothes/weapons/armor/jewelry/etc)
//
#include "game/creature/race-enum.hpp"
#include "game/creature/role-enum.hpp"
#include "game/non-player/character.hpp"
#include "game/non-player/ownership-chance-types.hpp"
#include "game/non-player/ownership-profile.hpp"
#include "game/item/weapon-info.hpp"
#include "game/game-data-file.hpp"

#include <memory>
#include <map>
#include <vector>
#include <cstddef> //for std::size_t


namespace game
{
namespace non_player
{
namespace ownership
{

    template<typename T>
    void NormalizeChanceMap(std::map<T, float> & map)
    {
        float subtotal(0.0f);
        for (auto const & NEXT_CHANCE_PAIR : map)
            subtotal += NEXT_CHANCE_PAIR.second;

        const float NORMALIZE_ADJ((1.0f - subtotal) / map.size());

        for (auto & nextChancePair : map)
            nextChancePair.second += NORMALIZE_ADJ;
    }


    //types required by the singeton implementation
    class ChanceFactory;
    using ChanceFactorySPtr_t = std::shared_ptr<ChanceFactory>;


    //A singleton class that is responsible for creating sets of items that will equip non-player characters.
    class ChanceFactory
    {
        //a wrapper for all the info needed to create weapon chances
        struct WeaponSet
        {
            WeaponSet()
            :
                count(0),//this default value intentionally invalid
                chanceMap()
            {}

            std::size_t count;
            std::map<item::weapon::WeaponInfo, float> chanceMap;
        };

        using WeaponSetVec_t = std::vector<WeaponSet>;

        //prevent copy construction
        ChanceFactory(const ChanceFactory &) =delete;

        //prevent copy assignment
        ChanceFactory & operator=(const ChanceFactory &) =delete;

        //prevent non-singleton construction
        ChanceFactory();

    public:
        virtual ~ChanceFactory();

        static ChanceFactorySPtr_t Instance();

        static const chance::InventoryChances Make(const non_player::CharacterSPtr_t & CREATURE_SPTR);

    private:
        static const chance::InventoryChances Make(const Profile &                     PROFILE,
                                                   const non_player::CharacterSPtr_t & CHARACTER_SPTR);

        static void LookupClothingMaterialChances(const Profile &                     PROFILE,
                                                  const non_player::CharacterSPtr_t & CHARACTER_SPTR,
                                                  float &                             clothChance,
                                                  float &                             softleatherChance,
                                                  float &                             hardleatherChance);

        //returns a float < 0.0f if the value found was 'remaining'
        static float GetFloatFromGameDataFile(const std::string & KEY, const float MIN, const float MAX);

        static void Make_Coins(const Profile & PROFILE,
                               item::Coin_t &  coinsMin_OutParam,
                               item::Coin_t &  coinsMax_OutParam);

        static const chance::ClothingChances Make_ClothingChances(const Profile &                     PROFILE,
                                                                  const non_player::CharacterSPtr_t & CHARACTER_SPTR);

        static const chance::WeaponChances Make_WeaponChances(const Profile &                     PROFILE,
                                                              const non_player::CharacterSPtr_t & CHARACTER_SPTR);

        static const chance::ArmorChances Make_ArmorChances(const Profile &                     PROFILE,
                                                            const non_player::CharacterSPtr_t & CHARACTER_SPTR);

        static const chance::ItemChancePair_t Make_MiscItemChances(const Profile &                    PROFILE,
                                                                  const non_player::CharacterSPtr_t & CHARACTER_SPTR);

        static const chance::ClothingChances Make_ClothingMaterialChances(const Profile &                     PROFILE,
                                                                          const non_player::CharacterSPtr_t & CHARACTER_SPTR);

        static void PopulateWeaponChances(chance::WeaponChances &,
                                          const WeaponSet &,
                                          const Profile &,
                                          const non_player::CharacterSPtr_t &);

        static void LookupPossibleArmorByRole(const Profile &,
                                              const non_player::CharacterSPtr_t &,
                                              chance::ArmorChances &);

        static void LookupPossibleWeaponsByRole(const creature::role::Enum, WeaponSetVec_t &);

        static void Make_ClothingMaterialChancesPrimary(const Profile &,
                                                        const non_player::CharacterSPtr_t &,
                                                        chance::ItemChances &);

        static void Make_MaterialChancesPrimary(const Profile &                       PROFILE,
                                                const non_player::CharacterSPtr_t &   CHARACTER_SPTR,
                                                const chance::MaterialChanceMap_t &   MATERIALS_TYPICAL,
                                                const item::Weight_t                  ITEM_WEIGHT,
                                                chance::MaterialChanceMap_t &         materialsMap_OutParam);

        static void Make_MaterialChancesSecondary(const Profile &,
                                                  const non_player::CharacterSPtr_t &,
                                                  chance::MaterialChanceMap_t &);

        static const chance::MaterialChanceMap_t Make_MaterialChanceMap(const std::string &           PREFIX,
                                                                        const std::string &           POSTFIX,
                                                                        const chance::MaterialVec_t & MATERIALS_VEC);

        static const chance::MaterialChanceMap_t Make_MaterialChanceMapCool();
        static const chance::MaterialChanceMap_t Make_MaterialChanceMapPrecious();
        static const chance::MaterialChanceMap_t Make_MaterialChanceMapMetal();

        static void Make_KnifeOrDaggerSizeChances(const creature::rank_class::Enum, chance::SizeChanceMap_t &);

        static void PopulateWeaponMaterials(const std::string &                   WEAPON_NAME,
                                            const chance::MaterialChanceMap_t &   TYPICAL_PRI_MATERIALS,
                                            const Profile &                       PROFILE,
                                            const non_player::CharacterSPtr_t &   CHARACTER_SPTR,
                                            chance::MaterialChanceMap_t &         materialsMapPri,
                                            chance::MaterialChanceMap_t &         materialsMapSec);

        static void PopulateArmorMaterials( const std::string &                   ARMOR_NAME,
                                            const chance::MaterialChanceMap_t &   TYPICAL_PRI_MATERIALS,
                                            const Profile &                       PROFILE,
                                            const non_player::CharacterSPtr_t &   CHARACTER_SPTR,
                                            chance::MaterialChanceMap_t &         materialsMapPri,
                                            chance::MaterialChanceMap_t &         materialsMapSec);

        static void PopulatMaterials(const chance::MaterialChanceMap_t &   TYPICAL_PRI_MATERIALS,
                                     const Profile &                       PROFILE,
                                     const non_player::CharacterSPtr_t &   CHARACTER_SPTR,
                                     chance::MaterialChanceMap_t &         materialsMapPri,
                                     chance::MaterialChanceMap_t &         materialsMapSec,
                                     const item::Weight_t                  WEIGHT);

        template<typename T>
        static bool SetWeaponChances(const std::string &                   WEAPON_NAME,
                                     const T                               WEAPON_ENUM,
                                     const float                           CHANCE_BASE,
                                     const Profile &                       PROFILE,
                                     const non_player::CharacterSPtr_t &   CHARACTER_SPTR,
                                     const chance::MaterialChanceMap_t &   MATERIALS_TYPICAL,
                                     std::map<T, chance::ItemChances> &    weaponChanceMap)
        {
            if (IsWeaponPossibleConsideringComplexity(WEAPON_NAME, PROFILE.complexityType) == false)
            {
                //TODO how to handle in a way that workds without leaving chances without materials?!
                //weaponChanceMap[WEAPON_ENUM].SetCountChanceSingleNoChance();
                //return false;
            }

            const float WEAPON_NUM_FLOAT  ( static_cast<float>(WEAPON_ENUM) );
            const float WEAPON_COUNT_FLOAT( static_cast<float>(T::Count) );

            float chance(CHANCE_BASE);

            //...minus a small amount depending on how valuable/rare the weapon is that is defined by its position in the enum
            const float ENUM_RATIO(WEAPON_NUM_FLOAT / WEAPON_COUNT_FLOAT);
            chance -= (ENUM_RATIO * CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_);

            //...adjust based on creature rank, where higher rank means higher chance of the more rare/valuable items
            const float ENUM_RATIO_INVERSE((WEAPON_COUNT_FLOAT + 1.0f) / (WEAPON_NUM_FLOAT + 1.0f));//add ones to prevent division by zero
            const float RANK(static_cast<float>(CHARACTER_SPTR->Rank()));
            const float RANK_GRANDMASTER(GameDataFile::Instance()->GetCopyFloat("heroespath-rankclass-Master-rankmax") + 1);
            const float RANK_RATIO(RANK / RANK_GRANDMASTER);
            chance += ((ENUM_RATIO_INVERSE * CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_) * RANK_RATIO);

            ChanceFactory::ForceMinMax(chance, CHANCE_MINIMUM_, 1.0f);
            weaponChanceMap[WEAPON_ENUM].SetCountChanceIncrementAndEquip(chance);

            PopulateWeaponMaterials(WEAPON_NAME,
                                    MATERIALS_TYPICAL,
                                    PROFILE,
                                    CHARACTER_SPTR,
                                    weaponChanceMap[WEAPON_ENUM].mat_map_pri,
                                    weaponChanceMap[WEAPON_ENUM].mat_map_sec);

            return true;
        }

        static void SetArmorChancesGeneral(chance::ArmorItemChances &           armorItemChances,
                                           const std::string &                  COMPLETE_NAME,
                                           const item::armor::base_type::Enum   TYPE,
                                           const float                          CHANCE,
                                           const Profile &                      PROFILE,
                                           const non_player::CharacterSPtr_t &  CREATURE_SPTR,
                                           const bool                           WILL_MATERIALS_INCLUDED_WOOD);

        static void SetArmorChancesSpecific(chance::ItemChances &               itemChances,
                                            const std::string &                 COMPLETE_NAME,
                                            const float                         CHANCE,
                                            const Profile &                     PROFILE,
                                            const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                            const bool                          WILL_MATERIALS_INCLUDED_WOOD);

        static void RestrictMaterialsByComplexity(const complexity_type::Enum   COMPLEXITY,
                                                  float &                       chanceCool,
                                                  chance::MaterialChanceMap_t & chanceMapCool,
                                                  float &                       chanceMetal,
                                                  chance::MaterialChanceMap_t & chanceMapMetal,
                                                  float &                       chancePrecious,
                                                  chance::MaterialChanceMap_t & chanceMapPrecious);

        static bool IsWeaponPossibleConsideringComplexity(const std::string &         WEAPON_NAME,
                                                          const complexity_type::Enum CREATURE_COMPLEXITY);

        static item::Weight_t LookupArmorWeight(const std::string & ARMOR_NAME);

        static const chance::MaterialChanceMap_t MakeTypicalArmorMaterials(const Profile &                   PROFILE,
                                                                           const non_player::CharacterSPtr_t & CREATURE_SPTR,
                                                                           const bool                        INCLUDE_WOOD);

        static inline void ForceMinMax(float & val_OutParam,
                                       const float MIN_VAL = CHANCE_MINIMUM_,
                                       const float MAX_VAL = CHANCE_MAXIMUM_)
        {
            if (val_OutParam < MIN_VAL)
                val_OutParam = MIN_VAL;
            else
                if ((MAX_VAL > 0.0f) && (val_OutParam > MAX_VAL))
                    val_OutParam = MAX_VAL;
        }

        static inline void ForceMin(float val_OutParam, const float MIN = CHANCE_MINIMUM_)
        {
            ForceMinMax(val_OutParam, MIN, 0.0f);
        }


    private:
        static const float CHANCE_MINIMUM_;
        static const float CHANCE_MAXIMUM_;
        static const float CHANCE_WEAPON_ENUM_POS_ADJ_SET_;
        static const float CHANCE_WEAPON_ENUM_POS_ADJ_INDIVIDUAL_;

        static ChanceFactorySPtr_t instance_;

        static chance::MaterialChanceMap_t materialChanceMapCool_;
        static chance::MaterialChanceMap_t materialChanceMapMetal_;
        static chance::MaterialChanceMap_t materialChanceMapPrecious_;
    };

}
}
}
#endif //GAME_NONPLAYER_CHANCEFACTORY_INCLUDED
