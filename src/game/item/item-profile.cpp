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
// item-profile.cpp
//
#include "item-profile.hpp"

#include "game/creature/enchantment-factory.hpp"
#include "game/item/item-profile-warehouse.hpp"

#include <sstream>


namespace game
{
namespace item
{

    ItemProfile::ItemProfile()
    :
        baseName_   (""),
        category_   (category::None),
        armor_      (armor_type::NotArmor),
        weapon_     (weapon_type::NotAWeapon),
        unique_     (unique_type::NotUnique),
        misc_       (misc_type::NotMisc),
        set_        (set_type::NotASet),
        named_      (named_type::NotNamed),
        element_    (element_type::None),
        isPixie_    (false),
        shield_	    (armor::shield_type::Count),
        helm_	    (armor::helm_type::Count),
        base_	    (armor::base_type::Count),
        cover_	    (armor::cover_type::Count),
        isAventail_ (false),
        isBracer_   (false),
        isShirt_    (false),
        isBoots_    (false),
        isPants_    (false),
        isGauntlets_(false),
        sword_	    (weapon::sword_type::Count),
        axe_	    (weapon::axe_type::Count),
        club_	    (weapon::club_type::Count),
        whip_	    (weapon::whip_type::Count),
        proj_	    (weapon::projectile_type::Count),
        bstaff_	    (weapon::bladedstaff_type::Count),
        size_	    (sfml_util::Size::Medium),
        isKnife_	(false),
        isDagger_	(false),
        isStaff_	(false),
        isQStaff_	(false),
        matPri_     (material::Nothing),
        matSec_     (material::Nothing),
        armorType_  (armor::base_type::Count),
        role_       (creature::role::Count),
        score_      (0),
        summonInfo_ (),
        religious_  (0.0f)
    {}


    ItemProfile::ItemProfile(const std::string &     BASE_NAME,
                             const category::Enum    CATEGORY,
                             const armor_type::Enum  ARMOR,
                             const weapon_type::Enum WEAPON,
                             const unique_type::Enum UNIQUE,
                             const misc_type::Enum   MISC,
                             const set_type::Enum    SET,
                             const named_type::Enum  NAMED)
    :
        ItemProfile()
    {
        baseName_   = BASE_NAME;
        category_   = CATEGORY;
        armor_      = ARMOR;
        weapon_     = WEAPON;
        unique_     = UNIQUE;
        misc_       = MISC;
        set_        = SET;
        named_      = NAMED;
    }


    void ItemProfile::SetSummoningAndAdjustScore(const creature::SummonInfo & SUMMON_INFO)
    {
        summonInfo_ = SUMMON_INFO;

        //Use a creature's rank min/max to establish a kind of combined power/worth/value
        //summon score, then append that to the summoning item's score.

        auto const CREATURE_RANK_PAIR{
            creature::race::RaceRoleRanks(SUMMON_INFO.Race(), SUMMON_INFO.Role()) };

        auto const CREATURE_RANK_AVG{
            static_cast<double>(CREATURE_RANK_PAIR.first + CREATURE_RANK_PAIR.second) / 2.0 };

        auto const SUMMON_COUNT{ static_cast<double>(SUMMON_INFO.Count()) };

        auto const SUMMON_SCORE{ static_cast<int>( std::sqrt(
            static_cast<double>(CREATURE_RANK_AVG * SUMMON_COUNT)) * 150.0) };

        score_ += SUMMON_SCORE;
    }


    const std::string ItemProfile::ToString() const
    {
        std::ostringstream ss;

        if (baseName_.empty() == false)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "base_name=" << baseName_;
        }

        if (category_ != 0)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "category="
                << item::category::ToString(category_, true);
        }

        if (armor_ != 0)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "armor_type="
                << item::armor_type::ToString(armor_, true);
        }

        if (weapon_ != 0)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "weapon_type="
                << item::weapon_type::ToString(weapon_, true);
        }

        if ((unique_ != unique_type::NotUnique) && (unique_ != unique_type::Count))
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "unique_type="
                << unique_type::ToString(unique_);
        }

        if ((misc_ != misc_type::NotMisc) && (misc_ != misc_type::Count))
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "misc_type="
                << misc_type::ToString(misc_);
        }

        if ((set_ != set_type::NotASet) && (set_ != set_type::Count))
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "set_type="
                << set_type::ToString(set_);
        }

        if ((named_ != named_type::NotNamed) && (named_ != named_type::Count))
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "named_type="
                << named_type::ToString(named_);
        }

        if (element_ != item::element_type::None)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "element_type="
                << element_type::ToString(element_);
        }

        if (isPixie_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Pixie)";
        }

        if (shield_ != armor::shield_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "shield_type="
                << armor::shield_type::ToString(shield_);
        }

        if (helm_ != armor::helm_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "helm_type="
                << armor::helm_type::ToString(helm_);
        }

        if (base_ != armor::base_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "base_type="
                << armor::base_type::ToString(base_);
        }

        if (cover_ != armor::cover_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "cover_type="
                << armor::cover_type::ToString(cover_);
        }

        if (isAventail_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Aventail)";
        }

        if (isBracer_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Bracer)";
        }

        if (isShirt_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Shirt)";
        }

        if (isBoots_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Boots)";
        }

        if (isPants_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Pants)";
        }

        if (isGauntlets_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Gauntlets)";
        }

        if (sword_ != weapon::sword_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "sword_type="
                << weapon::sword_type::ToString(sword_);
        }

        if (axe_ != weapon::axe_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "axe_type="
                << weapon::axe_type::ToString(axe_);
        }

        if (club_ != weapon::club_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "club_type="
                << weapon::club_type::ToString(club_);
        }

        if (whip_ != weapon::whip_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "whip_type="
                << weapon::whip_type::ToString(whip_);
        }

        if (proj_ != weapon::projectile_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "proj_type="
                << weapon::projectile_type::ToString(proj_);
        }

        if (bstaff_ != weapon::bladedstaff_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "bstaff_type="
                << weapon::bladedstaff_type::ToString(bstaff_);
        }

        if (isKnife_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Knife)";
        }

        if (isDagger_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Dagger)";
        }

        if (isStaff_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(Staff)";
        }

        if (isQStaff_)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "(QuarterStaff)";
        }

        if (size_ != sfml_util::Size::Medium)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "size="
                << sfml_util::Size::ToString(size_);
        }

        if (((matPri_ == material::Nothing) && (matSec_ == material::Nothing)) == false)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "mat="
                << ((matPri_ == material::Count) ? "(count)" : material::ToString(matPri_))
                << ","
                << ((matSec_ == material::Count) ? "(count)" : material::ToString(matSec_));
        }

        if (armorType_ != armor::base_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "armorTypeRestricted="
                << armor::base_type::ToString(armorType_);
        }

        if (role_ != creature::role::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "role_restriction="
                << creature::role::ToString(role_);
        }

        if (score_ != 0)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "score=" << score_;
        }

        if (summonInfo_.OriginType() != creature::origin_type::Count)
        {
            ss << ((ss.str().empty()) ? "" : ", ")
                << creature::origin_type::ToString(summonInfo_.OriginType())
                << " summoning a "
                << creature::race::RaceRoleName(summonInfo_.Race(), summonInfo_.Role());
        }

        if (religious_ > 0.0f)
        {
            ss << ((ss.str().empty()) ? "" : ", ") << "religious_ratio=" << religious_;
        }

        return ss.str();
    }


    void ItemProfile::SetUnique(const unique_type::Enum  E,
                                const material::Enum     MATERIAL_PRIMARY,
                                const material::Enum     MATERIAL_SECONDARY,
                                const element_type::Enum ELEMENT_TYPE)
    {
        unique_ = E;
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;
        misc_ = unique_type::MiscType(E);
        isPixie_ = false;
        religious_ = unique_type::ReligiousRatio(E);

        auto const IS_WEAPON{ ((E == unique_type::ViperFangFingerclaw) ||
                               (E == unique_type::ScorpionStingerFingerclaw) ||
                               (E == unique_type::RazorFingerclaw)) };

        score_ = (ItemProfileWarehouse::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(E, MATERIAL_PRIMARY) +
                  creature::EnchantmentFactory::Instance()->TreasureScore(ELEMENT_TYPE,
                                                                          IS_WEAPON,
                                                                          MATERIAL_PRIMARY));
    }


    void ItemProfile::SetMisc(const misc_type::Enum E,
                              const bool            IS_PIXIE,
                              const material::Enum  MATERIAL_PRIMARY,
                              const material::Enum  MATERIAL_SECONDARY,
                              const set_type::Enum  SET_TYPE)
    {
        using namespace item;

        misc_ = E;
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;
        isPixie_ = IS_PIXIE;

        if (misc::IsRealZero(religious_))
        {
            religious_ = misc_type::ReligiousRatio(E);
        }

        if (0 == score_)
        {
            score_ += ItemProfileWarehouse::Score(MATERIAL_PRIMARY, MATERIAL_SECONDARY);
        }

        score_ += creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE);

        if (E == misc_type::Cape)
        {
            category_ = static_cast<category::Enum>(category_ |
                                                    category::Armor |
                                                    category::Equippable);
            armor_ = armor_type::Covering;
            cover_ = armor::cover_type::Cape;
            score_ += ItemProfileWarehouse::Score(armor::cover_type::Cape);
        }
        else if (E == misc_type::Cloak)
        {
            category_ = static_cast<category::Enum>(category_ |
                                                    category::Armor |
                                                    category::Equippable |
                                                    category::Wearable);
            armor_ = armor_type::Covering;
            cover_ = armor::cover_type::Cloak;
            score_ += ItemProfileWarehouse::Score(armor::cover_type::Cloak);
        }
        else if (E == misc_type::LockPicks)
        {
            EquippableHelper(200);
        }
        else if ((E == misc_type::Fingerclaw) ||
                 (E == misc_type::Amulet) ||
                 (E == misc_type::Armband) ||
                 (E == misc_type::Veil) ||
                 (E == misc_type::Litch_Hand) ||
                 (E == misc_type::Braid))
        {
            EquippableHelper(0);
        }
        else if (E == misc_type::Spider_Eggs)
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
            score_ += 500;
        }
        else if (E == misc_type::Summoning_Statue)
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
            score_ += 100;
        }
        else if ((E == item::misc_type::Wand) ||
                 (E == item::misc_type::Petrified_Snake) ||
                 (E == item::misc_type::Mummy_Hand) ||
                 (E == item::misc_type::Shard) ||
                 (E == item::misc_type::Orb) ||
                 (E == item::misc_type::Scepter) ||
                 (E == item::misc_type::Icicle) ||
                 (E == item::misc_type::Finger) ||
                 (E == item::misc_type::Unicorn_Horn) ||
                 (E == item::misc_type::Devil_Horn) ||
                 (E == item::misc_type::Staff))
        {
            category_ = static_cast<category::Enum>(category_ |
                                                    category::AllowsCasting |
                                                    category::Equippable);
            score_ += 100;

            if (E == item::misc_type::Staff)
            {
                isStaff_ = true;

                category_ = static_cast<category::Enum>(category_ |
                                                        category::Weapon |
                                                        category::Equippable |
                                                        category::TwoHanded);

                weapon_ = static_cast<weapon_type::Enum>(weapon_type::Staff |
                                                         weapon_type::Melee);
            }
        }
        else if ((E == item::misc_type::DrumLute) ||
                 (E == item::misc_type::Lyre) ||
                 (E == item::misc_type::Hurdy_Gurdy) ||
                 (E == item::misc_type::Pipe_And_Tabor) ||
                 (E == item::misc_type::Viol) ||
                 (E == item::misc_type::Recorder))
        {
            category_ = static_cast<category::Enum>(category_ | category::Equippable);
            role_ = creature::role::Bard;
            score_ += 100;
        }
        else if ((E == item::misc_type::Bust) ||
                 (E == item::misc_type::Figurine_Blessed) ||
                 (E == item::misc_type::Doll_Blessed))
        {
            category_ = static_cast<category::Enum>(category_ | category::Blessed);
            score_ += 50;
        }
        else if ((E == item::misc_type::Figurine_Cursed) ||
                 (E == item::misc_type::Doll_Cursed))
        {
            category_ = static_cast<category::Enum>(category_ | category::Cursed);
            score_ += 50;
        }
        else if ((E == item::misc_type::Puppet) ||
                 (E == item::misc_type::Dried_Head))
        {
            score_ += 50;
        }
        else if ((E == item::misc_type::Goblet) ||
                 (E == item::misc_type::Balm_Pot) ||
                 (E == item::misc_type::Egg) ||
                 (E == item::misc_type::Embryo) ||
                 (E == item::misc_type::Seeds))
        {
            category_ = static_cast<category::Enum>(category_ | category::Useable);
            score_ += 150;
        }
        else if (E == item::misc_type::Ring)
        {
            category_ = static_cast<category::Enum>(category_ |
                                                    category::Equippable |
                                                    category::Wearable);
            score_ += 20;
        }
    }


    void ItemProfile::SetShield(const armor::shield_type::Enum E,
                                const material::Enum           MATERIAL_PRIMARY,
                                const material::Enum           MATERIAL_SECONDARY,
                                const named_type::Enum         NAMED_TYPE,
                                const set_type::Enum           SET_TYPE,
                                const element_type::Enum       ELEMENT_TYPE)
    {
        shield_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable |
                                                category::OneHanded);

        armor_ = item::armor_type::Sheild;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              false);
    }


    void ItemProfile::SetHelm(const armor::helm_type::Enum E,
                              const material::Enum         MATERIAL_PRIMARY,
                              const material::Enum         MATERIAL_SECONDARY,
                              const named_type::Enum       NAMED_TYPE,
                              const set_type::Enum         SET_TYPE,
                              const element_type::Enum     ELEMENT_TYPE)
    {
        helm_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Helm;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              false);
    }


    void ItemProfile::SetCover(const armor::cover_type::Enum E,
                               const material::Enum          MATERIAL_PRIMARY,
                               const material::Enum          MATERIAL_SECONDARY,
                               const named_type::Enum        NAMED_TYPE,
                               const set_type::Enum          SET_TYPE,
                               const element_type::Enum      ELEMENT_TYPE,
                               const bool               IS_PIXIE)
    {
        cover_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Covering;

        SetHelper(MATERIAL_PRIMARY,
                  MATERIAL_SECONDARY,
                  NAMED_TYPE,
                  SET_TYPE,
                  ELEMENT_TYPE,
                  IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              false);
    }


    void ItemProfile::SetAventail(const armor::base_type::Enum E,
                                  const material::Enum         MATERIAL_PRIMARY,
                                  const material::Enum         MATERIAL_SECONDARY,
                                  const named_type::Enum       NAMED_TYPE,
                                  const set_type::Enum         SET_TYPE,
                                  const element_type::Enum     ELEMENT_TYPE)
    {
        isAventail_ = true;
        base_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Aventail;

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              false);
    }


    void ItemProfile::SetBracer(const armor::base_type::Enum E,
                                const material::Enum         MATERIAL_PRIMARY,
                                const material::Enum         MATERIAL_SECONDARY,
                                const named_type::Enum       NAMED_TYPE,
                                const set_type::Enum         SET_TYPE,
                                const element_type::Enum     ELEMENT_TYPE,
                                const bool                   IS_PIXIE)
    {
        isBracer_ = true;
        base_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Bracer;

        SetHelper(MATERIAL_PRIMARY,
                  MATERIAL_SECONDARY,
                  NAMED_TYPE,
                  SET_TYPE,
                  ELEMENT_TYPE,
                  IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              false);
    }


    void ItemProfile::SetShirt(const armor::base_type::Enum E,
                               const material::Enum         MATERIAL_PRIMARY,
                               const material::Enum         MATERIAL_SECONDARY,
                               const named_type::Enum       NAMED_TYPE,
                               const set_type::Enum         SET_TYPE,
                               const element_type::Enum     ELEMENT_TYPE,
                               const bool                   IS_PIXIE)
    {
        isShirt_ = true;
        base_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Shirt;

        SetHelper(MATERIAL_PRIMARY,
                  MATERIAL_SECONDARY,
                  NAMED_TYPE,
                  SET_TYPE,
                  ELEMENT_TYPE,
                  IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              false);
    }


    void ItemProfile::SetBoots(const armor::base_type::Enum E,
                               const material::Enum         MATERIAL_PRIMARY,
                               const material::Enum         MATERIAL_SECONDARY,
                               const named_type::Enum       NAMED_TYPE,
                               const set_type::Enum         SET_TYPE,
                               const element_type::Enum     ELEMENT_TYPE,
                               const bool                   IS_PIXIE)
    {
        isBoots_ = true;
        base_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Boots;

        SetHelper(MATERIAL_PRIMARY,
                  MATERIAL_SECONDARY,
                  NAMED_TYPE,
                  SET_TYPE,
                  ELEMENT_TYPE,
                  IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              false);
    }


    void ItemProfile::SetPants(const armor::base_type::Enum E,
                               const material::Enum         MATERIAL_PRIMARY,
                               const material::Enum         MATERIAL_SECONDARY,
                               const named_type::Enum       NAMED_TYPE,
                               const set_type::Enum         SET_TYPE,
                               const element_type::Enum     ELEMENT_TYPE,
                               const bool                   IS_PIXIE)
    {
        isPants_ = true;
        base_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Pants;

        SetHelper(MATERIAL_PRIMARY,
                  MATERIAL_SECONDARY,
                  NAMED_TYPE,
                  SET_TYPE,
                  ELEMENT_TYPE,
                  IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              false);
    }


    void ItemProfile::SetGauntlets(const armor::base_type::Enum E,
                                   const material::Enum         MATERIAL_PRIMARY,
                                   const material::Enum         MATERIAL_SECONDARY,
                                   const named_type::Enum       NAMED_TYPE,
                                   const set_type::Enum         SET_TYPE,
                                   const element_type::Enum     ELEMENT_TYPE,
                                   const bool                   IS_PIXIE)
    {
        isGauntlets_ = true;
        base_ = E;

        category_ = static_cast<category::Enum>(category::Armor |
                                                category::Equippable |
                                                category::Wearable);

        armor_ = item::armor_type::Gauntlets;

        SetHelper(MATERIAL_PRIMARY,
                  MATERIAL_SECONDARY,
                  NAMED_TYPE,
                  SET_TYPE,
                  ELEMENT_TYPE,
                  IS_PIXIE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              false);
    }


    void ItemProfile::SetSword(const weapon::sword_type::Enum E,
                               const material::Enum           MATERIAL_PRIMARY,
                               const material::Enum           MATERIAL_SECONDARY,
                               const named_type::Enum         NAMED_TYPE,
                               const set_type::Enum           SET_TYPE,
                               const element_type::Enum       ELEMENT_TYPE)
    {
        using namespace item;
        using namespace item::weapon;

        sword_ = E;

        auto const HANDED{ [E]()
            {
                if ((E == sword_type::Claymore) ||
                    (E == sword_type::Longsword) ||
                    (E == sword_type::Knightlysword))
                {
                    return category::TwoHanded;
                }
                else
                {
                    return category::OneHanded;
                }
            }() };

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                HANDED);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Melee |
                                                 weapon_type::Sword |
                                                 weapon_type::Bladed |
                                                 weapon_type::Pointed);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetAxe(const weapon::axe_type::Enum E,
                             const material::Enum         MATERIAL_PRIMARY,
                             const material::Enum         MATERIAL_SECONDARY,
                             const named_type::Enum       NAMED_TYPE,
                             const set_type::Enum         SET_TYPE,
                             const element_type::Enum     ELEMENT_TYPE)
    {
        using namespace item::weapon;

        axe_ = E;

        auto const HANDED{ [E]()
            {
                if ((E == axe_type::Battleaxe) ||
                    (E == axe_type::Waraxe))
                {
                    return category::TwoHanded;
                }
                else
                {
                    return category::OneHanded;
                }
            }() };

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                HANDED);


        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Bladed |
                                                 weapon_type::Axe |
                                                 weapon_type::Melee);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetClub(const weapon::club_type::Enum E,
                              const material::Enum          MATERIAL_PRIMARY,
                              const material::Enum          MATERIAL_SECONDARY,
                              const named_type::Enum        NAMED_TYPE,
                              const set_type::Enum          SET_TYPE,
                              const element_type::Enum      ELEMENT_TYPE)
    {
        using namespace item::weapon;

        club_ = E;

        auto const HANDED{ [E]()
            {
                if ((E == club_type::Spiked))
                {
                    return category::OneHanded;
                }
                else
                {
                    return category::TwoHanded;
                }
            }() };

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                HANDED);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Melee |
                                                 weapon_type::Club);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetWhip(const weapon::whip_type::Enum E,
                              const material::Enum          MATERIAL_PRIMARY,
                              const material::Enum          MATERIAL_SECONDARY,
                              const named_type::Enum        NAMED_TYPE,
                              const set_type::Enum          SET_TYPE,
                              const element_type::Enum      ELEMENT_TYPE)
    {
        using namespace item::weapon;

        whip_ = E;

        auto const HANDED{ [E]()
            {
                if ((E == whip_type::Flail))
                {
                    return category::TwoHanded;
                }
                else
                {
                    return category::OneHanded;
                }
            }() };

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                HANDED);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Melee |
                                                 weapon_type::Whip);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetProjectile(const weapon::projectile_type::Enum E,
                                    const material::Enum                MATERIAL_PRIMARY,
                                    const material::Enum                MATERIAL_SECONDARY,
                                    const named_type::Enum              NAMED_TYPE,
                                    const set_type::Enum                SET_TYPE,
                                    const element_type::Enum            ELEMENT_TYPE)
    {
        using namespace item;
        using namespace item::weapon;

        proj_ = E;

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::TwoHanded);

        auto const WEAPON_TYPE{ [E]()
            {
                if (E == projectile_type::Blowpipe) { return weapon_type::Blowpipe; }
                if (E == projectile_type::Sling)    { return weapon_type::Sling; }
                if (E == projectile_type::Crossbow) { return weapon_type::Crossbow; }
                return weapon_type::Bow;
            }() };

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Projectile | WEAPON_TYPE);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetBladedStaff(const weapon::bladedstaff_type::Enum E,
                                      const material::Enum                 MATERIAL_PRIMARY,
                                      const material::Enum                 MATERIAL_SECONDARY,
                                      const named_type::Enum               NAMED_TYPE,
                                      const set_type::Enum                 SET_TYPE,
                                      const element_type::Enum             ELEMENT_TYPE)
    {
        using namespace item;
        using namespace item::weapon;

        bstaff_ = E;

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::TwoHanded);

        auto const BLADDED_TYPE{ ((E == bladedstaff_type::Spear) ? 0 : weapon_type::Bladed) };
        auto const POINTED_TYPE{ ((E == bladedstaff_type::Scythe) ? 0 : weapon_type::Pointed) };
        auto const SPEAR_TYPE{ ((E == bladedstaff_type::Spear) ? weapon_type::Spear : 0) };

        weapon_ = static_cast<weapon_type::Enum>(BLADDED_TYPE |
                                                 weapon_type::BladedStaff |
                                                 weapon_type::Melee |
                                                 POINTED_TYPE |
                                                 SPEAR_TYPE);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::Score(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetKnife(const sfml_util::Size::Enum E,
                               const material::Enum        MATERIAL_PRIMARY,
                               const material::Enum        MATERIAL_SECONDARY,
                               const named_type::Enum      NAMED_TYPE,
                               const set_type::Enum        SET_TYPE,
                               const element_type::Enum    ELEMENT_TYPE,
                               const bool                  IS_PIXIE)
    {
        isKnife_ = true;
        size_ = E;

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::OneHanded);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Bladed |
                                                 weapon_type::Knife |
                                                 weapon_type::Melee |
                                                 weapon_type::Pointed);

        SetHelper(MATERIAL_PRIMARY,
                  MATERIAL_SECONDARY,
                  NAMED_TYPE,
                  SET_TYPE,
                  ELEMENT_TYPE,
                  IS_PIXIE);

        score_ = ItemProfileWarehouse::ScoreKnife(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetDagger(const sfml_util::Size::Enum E,
                                const material::Enum        MATERIAL_PRIMARY,
                                const material::Enum        MATERIAL_SECONDARY,
                                const named_type::Enum      NAMED_TYPE,
                                const set_type::Enum        SET_TYPE,
                                const element_type::Enum    ELEMENT_TYPE,
                                const bool                  IS_PIXIE)
    {
        isDagger_ = true;
        size_ = E;

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::OneHanded);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Bladed |
                                                 weapon_type::Knife |
                                                 weapon_type::Melee |
                                                 weapon_type::Pointed);

        SetHelper(MATERIAL_PRIMARY,
                  MATERIAL_SECONDARY,
                  NAMED_TYPE,
                  SET_TYPE,
                  ELEMENT_TYPE,
                  IS_PIXIE);

        score_ = ItemProfileWarehouse::ScoreDagger(E);

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetStaff(const material::Enum     MATERIAL_PRIMARY,
                               const material::Enum     MATERIAL_SECONDARY,
                               const named_type::Enum   NAMED_TYPE,
                               const set_type::Enum     SET_TYPE,
                               const element_type::Enum ELEMENT_TYPE)
    {
        isStaff_ = true;

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::TwoHanded);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Staff |
                                                 weapon_type::Melee);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::ScoreStaff();

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetQuarterStaff(const material::Enum     MATERIAL_PRIMARY,
                                      const material::Enum     MATERIAL_SECONDARY,
                                      const named_type::Enum   NAMED_TYPE,
                                      const set_type::Enum     SET_TYPE,
                                      const element_type::Enum ELEMENT_TYPE)
    {
        isQStaff_ = true;

        category_ = static_cast<category::Enum>(category::Weapon |
                                                category::Equippable |
                                                category::TwoHanded);

        weapon_ = static_cast<weapon_type::Enum>(weapon_type::Staff |
                                                 weapon_type::Melee);

        SetHelper(MATERIAL_PRIMARY, MATERIAL_SECONDARY, NAMED_TYPE, SET_TYPE, ELEMENT_TYPE);

        score_ = ItemProfileWarehouse::ScoreQuarterStaff();

        score_ += ScoreHelper(MATERIAL_PRIMARY,
                              MATERIAL_SECONDARY,
                              NAMED_TYPE,
                              SET_TYPE,
                              ELEMENT_TYPE,
                              true);
    }


    void ItemProfile::SetHelper(const material::Enum     MATERIAL_PRIMARY,
                                const material::Enum     MATERIAL_SECONDARY,
                                const named_type::Enum   NAMED_ENUM,
                                const set_type::Enum     SET_ENUM,
                                const element_type::Enum ELEMENT_TYPE,
                                const bool               IS_PIXIE)
    {
        matPri_ = MATERIAL_PRIMARY;
        matSec_ = MATERIAL_SECONDARY;
        named_ = NAMED_ENUM;
        set_ = SET_ENUM;
        element_ = ELEMENT_TYPE;
        isPixie_ = IS_PIXIE;
        role_ = set_type::Role(SET_ENUM);
    }


    int ItemProfile::ScoreHelper(const item::material::Enum     MATERIAL_PRI,
                                 const item::material::Enum     MATERIAL_SEC,
                                 const item::named_type::Enum   NAMED_TYPE,
                                 const item::set_type::Enum     SET_TYPE,
                                 const item::element_type::Enum ELEMENT_TYPE,
                                 const bool                     IS_WEAPON) const
    {
        if ((MATERIAL_PRI == item::material::Nothing) ||
            (MATERIAL_PRI == item::material::Count))
        {
            return 0;
        }
        else
        {
            return (ItemProfileWarehouse::Score(MATERIAL_PRI, MATERIAL_SEC) +
                    creature::EnchantmentFactory::Instance()->TreasureScore(NAMED_TYPE,
                                                                            MATERIAL_PRI,
                                                                            IS_WEAPON,
                                                                            ! IS_WEAPON) +
                    creature::EnchantmentFactory::Instance()->TreasureScore(SET_TYPE) +
                    creature::EnchantmentFactory::Instance()->TreasureScore(ELEMENT_TYPE,
                                                                            IS_WEAPON,
                                                                            MATERIAL_PRI));
        }
    }


    void ItemProfile::EquippableHelper(const int SCORE_BONUS)
    {
        category_ = static_cast<category::Enum>(category_ | category::Equippable);
        score_ += SCORE_BONUS;
    }

}
}
