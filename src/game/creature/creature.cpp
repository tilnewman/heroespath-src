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
// creature.cpp
//
#include "creature.hpp"

#include "game/game-data-file.hpp"
#include "game/log-macros.hpp"
#include "game/spell/spell-warehouse.hpp"
#include "game/spell/spell-base.hpp"
#include "game/item/item.hpp"
#include "game/item/algorithms.hpp"
#include "game/creature/conditions.hpp"
#include "game/creature/condition.hpp"
#include "game/creature/condition-warehouse.hpp"
#include "game/creature/condition-algorithms.hpp"
#include "game/creature/title-warehouse.hpp"

#include "misc/real.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/vectors.hpp"
#include "misc/boost-string-includes.hpp"

#include <algorithm>


namespace game
{
namespace creature
{

    std::size_t       Creature::globalSerialNumber_(0);
    const std::string Creature::ITEM_ACTION_SUCCESS_STR_("");


    Creature::Creature(const std::string &         NAME,
                       const sex::Enum             SEX,
                       const BodyType &            BODY_TYPE,
                       const creature::Race &      RACE,
                       const creature::Role &      ROLE,
                       const stats::StatSet &      STATS,
                       const stats::Health_t       HEALTH,
                       const stats::Rank_t         RANK,
                       const stats::Exp_t          EXPERIENCE,
                       const ConditionEnumVec_t &  CONDITIONS_VEC,
                       const TitleEnumVec_t &      TITLE_VEC,
                       const item::Inventory &     INVENTORY,
                       const sfml_util::DateTime & DATE_TIME,
                       const std::string &         IMAGE_FILENAME,
                       const spell::SpellVec_t &   SPELL_VEC,
                       const stats::Mana_t         MANA)
    :
        name_           (NAME),
        imageFilename_  (IMAGE_FILENAME),
        sex_            (SEX),
        bodyType_       (BODY_TYPE),
        race_           (RACE),
        role_           (ROLE),
        stats_          (STATS),
        serialNumber_   (globalSerialNumber_++),
        healthCurrent_  (HEALTH),
        healthNormal_   (HEALTH),
        rank_           (RANK),
        experience_     (EXPERIENCE),
        conditionsVec_  (CONDITIONS_VEC),
        titlesVec_      (TITLE_VEC),
        inventory_      (INVENTORY),
        dateTimeCreated_(DATE_TIME),
        spellsVec_      (SPELL_VEC),
        achievements_   (),
        currWeaponsPVec_(),
        manaCurrent_    (MANA),
        manaNormal_     (MANA)
    {
        //set the default condition if not already there
        if (conditionsVec_.empty())
        {
            ConditionAdd(Conditions::Good);
        }

        ReCalculateStats();
    }


    Creature::~Creature()
    {}


    const std::string Creature::NameOrRaceAndRole(const bool IS_FIRST_LETTER_CAPS) const
    {
        std::ostringstream ss;
        if (IsPlayerCharacter())
        {
            ss << Name();
        }
        else
        {
            ss << Race().Name();

            if (Role().Which() != role::Wolfen)
            {
                ss << " " << Role().Name();
            }
        }

        if (IS_FIRST_LETTER_CAPS)
        {
            return ss.str();
        }
        else
        {
            return boost::algorithm::to_lower_copy(ss.str());
        }
    }


    const std::string Creature::NameAndRaceAndRole(const bool IS_FIRST_LETTER_CAPS) const
    {
        auto const NAME_STR(Name());
        auto const RACE_STR(Race().Name());
        auto const ROLE_STR(Role().Name());

        std::ostringstream ss;

        if (NAME_STR == RACE_STR)
        {
            ss << NAME_STR;
        }
        else
        {
            ss << NAME_STR << " the " << RACE_STR;
        }

        if (RACE_STR != ROLE_STR)
        {
            ss << " " << ROLE_STR;
        }

        if (IS_FIRST_LETTER_CAPS)
        {
            return ss.str();
        }
        else
        {
            return boost::algorithm::to_lower_copy(ss.str());
        }
    }


    void Creature::ReCalculateStats()
    {
        stats_.ResetCurrentAndActualToNormal();

        auto const CONDITIONS_PVEC{ ConditionsPVec() };

        for (auto const NEXT_COND_PTR : CONDITIONS_PVEC)
            stats_.ModifyCurrentAndActual(NEXT_COND_PTR->StatMult());
    }


    const std::string Creature::RankClassName() const
    {
        if (Race().Which() == creature::race::Dragon)
        {
            return creature::dragon_class::Name(DragonClass());
        }
        else if (Race().Which() == creature::race::Wolfen)
        {
            return creature::wolfen_class::Name(WolfenClass());
        }
        else
        {
            return rank_class::ToString(RankClass());
        }
    }


    float Creature::RankRatio() const
    {
        const float GRANDMASTER_RANK(GameDataFile::Instance()->
            GetCopyFloat("heroespath-rankclass-Master-rankmax") + 1.0f);

        auto rankRatio{ static_cast<float>(rank_) / GRANDMASTER_RANK };
        if (rankRatio > 1.0f)
        {
            rankRatio = 1.0f;
        }

        return rankRatio;
    }


    void Creature::TitleAdd(const Titles::Enum E, const bool ALLOW_CHANGES)
    {
        if (ALLOW_CHANGES)
        {
            title::Warehouse::Get(E)->Change(this);
        }

        titlesVec_.push_back(E);
    }


    const TitlePVec_t Creature::TitlesPVec() const
    {
        TitlePVec_t titlePVec;

        for (auto const NEXT_TITLE_ENUM : titlesVec_)
            titlePVec.push_back( title::Warehouse::Get(NEXT_TITLE_ENUM) );

        return titlePVec;
    }


    const std::string Creature::HealthPercentStr(const bool WILL_APPEND_SYMBOL) const
    {
        std::ostringstream ss;

        const float HEALTH_RATIO(HealthRatio());
        if (misc::IsRealClose(HEALTH_RATIO, 0.0f))
        {
            ss << "0";
        }
        else
        {
            const int HEALTH_INT(static_cast<int>(HEALTH_RATIO * 100.0f));

            if (HEALTH_INT < 1)
                ss << "1";
            else if (HEALTH_INT < 100)
                ss << HEALTH_INT;
            else
                ss << "100";
        }

        if (WILL_APPEND_SYMBOL)
            ss << "%";

        return ss.str();
    }


    bool Creature::ConditionAdd(const Conditions::Enum E, const bool ALLOW_CHANGES)
    {
        const ConditionEnumVecCIter_t COND_GOOD_ITER( std::find(conditionsVec_.begin(),
                                                                conditionsVec_.end(),
                                                                Conditions::Good) );

        bool willAdd(false);
        if (COND_GOOD_ITER == conditionsVec_.end())
        {
            willAdd = true;
        }
        else
        {
            //prevent multiple 'Good' conditions
            if (E == Conditions::Good)
            {
                return false;
            }
            else
            {
                //remove (all) the 'Good' conditions before adding the 'not so good' condition
                conditionsVec_.erase(std::remove(conditionsVec_.begin(), conditionsVec_.end(), Conditions::Good), conditionsVec_.end());
                willAdd = true;
            }
        }

        if (willAdd)
        {
            //verify the condition is not already in the list
            if (conditionsVec_.end() == std::find(conditionsVec_.begin(), conditionsVec_.end(), E))
            {
                conditionsVec_.push_back(E);

                //make the change to the creature
                if (ALLOW_CHANGES)
                {
                    condition::Warehouse::Get(E)->InitialChange(this);
                    ReCalculateStats();
                }

                return true;
            }
        }

        return false;
    }


    bool Creature::ConditionRemove(const Conditions::Enum E)
    {
        ConditionEnumVec_t conditionsToRemoveVec;
        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
        {
            if (NEXT_CONDITION_ENUM == E)
            {
                conditionsToRemoveVec.push_back(NEXT_CONDITION_ENUM);
            }
        }

        auto wasAnyConditionRemoved{ false };
        for (auto const NEXT_CONDITION_TO_REMOVE_ENUM : conditionsToRemoveVec)
        {
            wasAnyConditionRemoved = true;
            condition::Warehouse::Get(NEXT_CONDITION_TO_REMOVE_ENUM)->FinalUndo(this);
            conditionsVec_.erase(std::find(conditionsVec_.begin(), conditionsVec_.end(), NEXT_CONDITION_TO_REMOVE_ENUM));
        }

        if (conditionsVec_.size() == 0)
        {
            ConditionAdd(Conditions::Good);
        }

        ReCalculateStats();

        return wasAnyConditionRemoved;
    }


    std::size_t Creature::ConditionRemoveAll()
    {
        const std::size_t ORIG_COND_COUNT(conditionsVec_.size());

        //undo the changes made by the conditions that will be removed
        for (auto const NEXT_COND_ENUM : conditionsVec_)
        {
            condition::Warehouse::Get(NEXT_COND_ENUM)->FinalUndo(this);
        }

        conditionsVec_.clear();
        ConditionAdd(Conditions::Good);
        ReCalculateStats();
        return ORIG_COND_COUNT;
    }


    const ConditionPVec_t Creature::ConditionsPVec() const
    {
        ConditionPVec_t conditionPVec;

        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
            conditionPVec.push_back( condition::Warehouse::Get(NEXT_CONDITION_ENUM) );

        return conditionPVec;
    }


    bool Creature::HasCondition(const Conditions::Enum E) const
    {
        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
        {
            if (NEXT_CONDITION_ENUM == E)
            {
                return true;
            }
        }

        return false;
    }


    bool Creature::HasConditionNotAThreatTemp() const
    {
        return (HasCondition(Conditions::Dazed) ||
                HasCondition(Conditions::Tripped) ||
                HasCondition(Conditions::Unconscious) ||
                HasCondition(Conditions::AsleepNatural) ||
                HasCondition(Conditions::AsleepMagical));
    }


    bool Creature::HasConditionNotAThreatPerm() const
    {
        return (HasCondition(Conditions::Dead) || HasCondition(Conditions::Stone));
    }


    std::size_t Creature::GetWorstConditionSeverity() const
    {
        if (conditionsVec_.empty())
        {
            return 0;
        }
        else if (conditionsVec_.size() == 1)
        {
            return condition::Severity::Get(conditionsVec_[0]);
        }
        else
        {
            ConditionEnumVec_t tempCondVec(conditionsVec_);

            std::sort(tempCondVec.begin(),
                      tempCondVec.end(),
                      []
                      (const Conditions::Enum A, const Conditions::Enum B)
                      { return condition::Severity::Get(A) > condition::Severity::Get(B); });

            return condition::Severity::Get(tempCondVec[0]);
        }
    }


    bool Creature::HasMagicalCondition() const
    {
        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
        {
            if (condition::Warehouse::Get(NEXT_CONDITION_ENUM)->IsMagical())
            {
                return true;
            }
        }

        return false;
    }


    const std::string Creature::ConditionNames(const std::size_t MAX_TO_LIST,
                                               const size_t      MIN_SEVERITY)
    {
        return creature::condition::Algorithms::Names(conditionsVec_,
                                                      false,
                                                      false,
                                                      MAX_TO_LIST,
                                                      true,
                                                      MIN_SEVERITY,
                                                      true);
    }


    const std::string Creature::CanTakeActionStr(const bool WILL_PREFIX_AND_POSTFIX) const
    {
        const std::string RESPONSE_PREFIX((WILL_PREFIX_AND_POSTFIX) ? "Cannot act because " : "");
        const std::string RESPONSE_POSTFIX((WILL_PREFIX_AND_POSTFIX) ? "." : "");

        if (HasCondition(Conditions::Dead))
        {
            return RESPONSE_PREFIX + "dead" + RESPONSE_POSTFIX;
        }

        if (HasCondition(Conditions::Stone))
        {
            return RESPONSE_PREFIX + "turned to stone" + RESPONSE_POSTFIX;
        }

        if (HasCondition(Conditions::Unconscious))
        {
            return RESPONSE_PREFIX + "unconscious" + RESPONSE_POSTFIX;
        }

        if (HasCondition(Conditions::Dazed))
        {
            return RESPONSE_PREFIX + "dazed" + RESPONSE_POSTFIX;
        }

        if (HasCondition(Conditions::Tripped))
        {
            return RESPONSE_PREFIX + "tripped" + RESPONSE_POSTFIX;
        }

        if (HasCondition(Conditions::AsleepNatural))
        {
            return RESPONSE_PREFIX + "asleep" + RESPONSE_POSTFIX;
        }

        if (HasCondition(Conditions::AsleepMagical))
        {
            return RESPONSE_PREFIX + "under magical sleep" + RESPONSE_POSTFIX;
        }

        return "";
    }


    const std::string Creature::ItemAdd(const item::ItemPtr_t ITEM_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_PTR != nullptr),
            "Creature::ItemAdd() was given a null ITEM_PTR.");

        const std::string IS_ALLOWED_STR( ItemIsAddAllowed(ITEM_PTR) );
        if (IS_ALLOWED_STR == ITEM_ACTION_SUCCESS_STR_)
        {
            inventory_.ItemAdd(ITEM_PTR);
        }
        else
        {
            M_HP_LOG("game::creature::ItemAdd(creature_name=\"" << Name() << "\", item_name=\""
                << ITEM_PTR->Name() << "\") item add failed:  \"" << IS_ALLOWED_STR << "\".");
        }

        return IS_ALLOWED_STR;
    }


    const std::string Creature::ItemIsAddAllowed(const item::ItemPtr_t ITEM_PTR) const
    {
        //always allow bodypart items
        if (ITEM_PTR->IsBodypart())
        {
            return ITEM_ACTION_SUCCESS_STR_;
        }

        //verify strength/weight
        if (IsPlayerCharacter() &&
            ((inventory_.Weight() + ITEM_PTR->Weight()) > WeightCanCarry()))
        {
            std::ostringstream ss;
            ss << "item is " << (inventory_.Weight() + ITEM_PTR->Weight()) - WeightCanCarry()
                << " too heavy";
            return ss.str();
        }

        //verify armor vs race
        if (ITEM_PTR->IsArmor() &&
            ((race_.Which() == race::Dragon) || (race_.Which() == race::Wolfen)))
        {
            std::ostringstream ss;
            ss << race::Name(race_.Which()) << "s cannot be given armor";
            return ss.str();
        }

        return ITEM_ACTION_SUCCESS_STR_;
    }


    void Creature::ItemRemove(const item::ItemPtr_t ITEM_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_PTR != nullptr),
            "Creature::ItemRemove() was given a null ITEM_PTR.");

        inventory_.ItemRemove(ITEM_PTR);
    }


    const std::string Creature::ItemEquip(const item::ItemPtr_t ITEM_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_PTR != nullptr),
            "Creature::ItemEquip(nullptr) was given a null ITEM_PTR.");

        const std::string IS_ALLOWED_STR( ItemIsEquipAllowed(ITEM_PTR) );
        if (IS_ALLOWED_STR == ITEM_ACTION_SUCCESS_STR_)
        {
            inventory_.ItemEquip(ITEM_PTR);
            SetCurrentWeaponsToBest();
        }

        return IS_ALLOWED_STR;
    }


    const std::string Creature::ItemIsEquipAllowed(const item::ItemPtr_t ITEM_PTR) const
    {
        if (ITEM_PTR->IsBodypart())
        {
            return ITEM_ACTION_SUCCESS_STR_;
        }

        const item::category::Enum CATEGORY(ITEM_PTR->Category());
        const item::misc_type::Enum MISC_TYPE(ITEM_PTR->MiscType());

        if (CATEGORY == item::category::Broken)
        {
            return "Can't equip the " + ITEM_PTR->Name() + " because it is broken.";
        }
        else if ((CATEGORY & item::category::Equippable) == 0)
        {
            std::ostringstream ss;
            ss << "Not an equippable item.";

            if (CATEGORY & item::category::Useable)
            {
                ss << "  (Try using it instead)";
            }

            if (CATEGORY & item::category::QuestItem)
            {
                ss << "  (have patience, this item will prove useful in time)";
            }

            return ss.str();
        }

        const std::string SEP("  ");

        //collect all the remaining reasons why the equip is not possible into one
        std::ostringstream resultSS;

        //TODO? verify strength limit of the item?

        //verify role, such as Sorcerers not being able to use swords
        const std::string CAN_EQUIP_BY_ROLE_STR( ItemIsEquipAllowedByRole(ITEM_PTR) );
        if (CAN_EQUIP_BY_ROLE_STR != ITEM_ACTION_SUCCESS_STR_)
        {
            resultSS << CAN_EQUIP_BY_ROLE_STR << SEP;
        }

        //verify invalid duplicates
        if ((CATEGORY & item::category::OneHanded) &&
            (inventory_.CountItemOfCategoryEquipped(item::category::OneHanded) > 2))
        {
            resultSS << "Can't equip three one-handed items." << SEP;
        }

        if (CATEGORY & item::category::TwoHanded)
        {
            auto const NONBODYPART_TWOHANDED_WEAPONS_SVEC{
                item::Algorithms::FindByCategory(
                    item::Algorithms::FindByCategory(
                        inventory_.ItemsEquipped(), item::category::TwoHanded),
                    item::category::BodyPart, false) };

            if (NONBODYPART_TWOHANDED_WEAPONS_SVEC.empty() == false)
            {
                resultSS << "Can't equip multiple two-handed items. ("
                    << item::Algorithms::Names(NONBODYPART_TWOHANDED_WEAPONS_SVEC, false, false)
                    << " is already equipped)" << SEP;
            }
        }

        if ((MISC_TYPE == item::misc_type::Wand) &&
            (inventory_.CountItemOfMiscTypeEquipped(item::misc_type::Wand) > 0))
        {
            resultSS << "Can't equip more than one wand at a time." << SEP;
        }

        //verify valid duplicates upper limits
        if ((MISC_TYPE == item::misc_type::Ring) &&
            (inventory_.CountItemOfMiscTypeEquipped(item::misc_type::Ring) == 10))
        {
            resultSS << "Can't equip more than ten rings at once." << SEP;
        }

        if ((MISC_TYPE == item::misc_type::Mask) &&
            (inventory_.CountItemOfMiscTypeEquipped(item::misc_type::Mask) > 0))
        {
            resultSS << "Can't wear more than one mask at the same time." << SEP;
        }

        if ((MISC_TYPE == item::misc_type::Ring) && (Body().HasFingers() == false))
        {
            resultSS << race_.Name() << "'s can't wear rings because they have no fingers." << SEP;
        }

        const item::armor_type::Enum ARMOR_TYPE(ITEM_PTR->ArmorType());

        if ((CATEGORY & item::category::Armor) ||
            (ARMOR_TYPE == item::armor_type::Boots) ||
            (ARMOR_TYPE == item::armor_type::Pants) ||
            (ARMOR_TYPE == item::armor_type::Shirt))
        {
            if ((ARMOR_TYPE & item::armor_type::Bracer) && (Body().HasArms() == false))
            {
                resultSS << "Can't wear bracers without arms." << SEP;
            }

            if ((ARMOR_TYPE & item::armor_type::Gauntlets) && (Body().HasFingers() == false))
            {
                resultSS << "Can't wear guantlets without fingers." << SEP;
            }

            if (((ARMOR_TYPE & item::armor_type::Helm) > 0) && (Body().HasHead() == false))
            {
                resultSS << race_.Name() << "'s can't wear helms because they have no head." << SEP;
            }

            if (((ARMOR_TYPE & item::armor_type::Helm) > 0) && (Body().HasHorns() == true))
            {
                resultSS << race_.Name() << "'s can't wear helms becaus they have horns." << SEP;
            }

            if ((ARMOR_TYPE & item::armor_type::Aventail) &&
                (item::Algorithms::FindByArmorType(inventory_.ItemsEquipped(),
                    item::armor_type::Helm).empty()))
            {
                resultSS << "Can't wear an aventail without first wearing a helm." << SEP;
            }

            std::size_t armorEquipLimit{ 1 };

            if ((ARMOR_TYPE & item::armor_type::Bracer) ||
                (ARMOR_TYPE & item::armor_type::Gauntlets))
            {
                armorEquipLimit = Body().NumArms() / 2;
            }

            if (ARMOR_TYPE & item::armor_type::Sheild)
            {
                armorEquipLimit = Body().NumArms();
            }

            if ((ARMOR_TYPE & item::armor_type::Boots) || (ARMOR_TYPE & item::armor_type::Pants))
            {
                armorEquipLimit = Body().NumLegs() / 2;
            }

            if (ARMOR_TYPE & item::armor_type::Helm)
            {
                armorEquipLimit = Body().NumHeads();
            }

            if (ARMOR_TYPE & item::armor_type::Shirt)
            {
                armorEquipLimit = 1;
            }

            if (ARMOR_TYPE & item::armor_type::Aventail)
            {
                armorEquipLimit = inventory_.CountItemOfArmorTypeEquipped(item::armor_type::Helm);
            }

            if (inventory_.CountItemOfArmorTypeEquipped(ARMOR_TYPE) >= armorEquipLimit)
            {
                const std::string ARMOR_TYPE_STR( item::armor_type::ToString(ARMOR_TYPE, false) );

                std::ostringstream ss;
                ss << "Can't equip more than " << armorEquipLimit;
                if (boost::algorithm::iends_with(ARMOR_TYPE_STR, "s"))
                {
                    ss << " pairs";
                }
                
                ss << " of " << ARMOR_TYPE_STR << "." << SEP;

                //make an exception for cover_type::Vest, which can be equipped along with the
                //other cover_types: cape, robe, and cloak -which are otherwise mutually exclusive
                if (ITEM_PTR->Armor_Info().cover == item::armor::cover_type::Vest)
                {
                    bool alraedyHasVestEquipped(false);
                    for (auto const NEXT_ITEM_PTR : inventory_.ItemsEquipped())
                    {
                        if (NEXT_ITEM_PTR->Armor_Info().cover == item::armor::cover_type::Vest)
                        {
                            alraedyHasVestEquipped = true;
                        }
                    }

                    if (alraedyHasVestEquipped)
                    {
                        resultSS << ss.str() << "  " << "(Already has a vest equipped)" << SEP;
                    }
                }
                else if ((ITEM_PTR->Armor_Info().cover != item::armor::cover_type::Vest) &&
                    (ARMOR_TYPE & item::armor_type::Covering))
                {
                    auto alreadyHasNonVestCovering{ false };
                    for (auto const NEXT_ITEM_PTR : inventory_.ItemsEquipped())
                    {
                        if ((NEXT_ITEM_PTR->ArmorType() & item::armor_type::Covering) &&
                            (NEXT_ITEM_PTR->Armor_Info().cover != item::armor::cover_type::Vest))
                        {
                            alreadyHasNonVestCovering = true;
                        }
                    }

                    if (alreadyHasNonVestCovering)
                    {
                        resultSS << ss.str() << SEP;
                    }
                }
                else
                {
                    resultSS << ss.str() << SEP;
                }
            }
        }

        if (resultSS.str().empty())
        {
            return ITEM_ACTION_SUCCESS_STR_;
        }
        else
        {
            return boost::algorithm::erase_last_copy(resultSS.str(), SEP);
        }
    }


    const std::string Creature::ItemIsEquipAllowedByRole(const item::ItemPtr_t ITEM_PTR) const
    {
        const creature::role::Enum ROLE(role_.Which());
        const creature::role::Enum EXCLUSIVE_ROLE(ITEM_PTR->ExclusiveRole());
        if ((EXCLUSIVE_ROLE != creature::role::Count) && (EXCLUSIVE_ROLE != ROLE))
        {
            std::ostringstream ss;
            ss << "Only " << creature::role::ToString(EXCLUSIVE_ROLE) << "s may use this item.";
            return ss.str();
        }

        const item::category::Enum CATEGORY(ITEM_PTR->Category());
        const item::weapon_type::Enum WEAPON_TYPE(ITEM_PTR->WeaponType());
        const item::material::Enum MATERIAL_PRI(ITEM_PTR->MaterialPrimary());
        const item::material::Enum MATERIAL_SEC(ITEM_PTR->MaterialSecondary());
        const item::misc_type::Enum MISC_TYPE(ITEM_PTR->MiscType());

        if ((ROLE != role::Knight) &&
            ((WEAPON_TYPE & item::weapon_type::BladedStaff) > 0) &&
            ((WEAPON_TYPE & item::weapon_type::Spear) == 0))
        {
            return "Only knights may use non-simple-spear bladed staff weapons.";
        }

        if ((CATEGORY & item::category::BodyPart) == 0)
        {
            if (race_.Which() == race::Dragon)
            {
                return "Dragons can only equip body parts, such as skin, horns, claws, or bite.";
            }

            if (race_.Which() == race::Wolfen)
            {
                return "Wolfens can only equip body parts, such as skin, claws, or bite.";
            }
        }

        if ((item::misc_type::IsMusicalInstrument(MISC_TYPE)) && (ROLE != role::Bard))
        {
            return "Only Bards can equip musical instruments.";
        }

        if ((ROLE == role::Cleric) || (ROLE == role::Sorcerer))
        {
            if (CATEGORY & item::category::Weapon)
            {
                if (((WEAPON_TYPE & item::weapon_type::Blowpipe) == 0) &&
                    ((WEAPON_TYPE & item::weapon_type::Crossbow) == 0) &&
                    ((WEAPON_TYPE & item::weapon_type::Knife) == 0) &&
                    ((WEAPON_TYPE & item::weapon_type::Sling) == 0) &&
                    ((WEAPON_TYPE & item::weapon_type::Staff) == 0) &&
                    ((WEAPON_TYPE & item::weapon_type::Whip) == 0))
                {
                    std::ostringstream scSS;
                    scSS << ((ROLE == role::Cleric) ? "Clerics" : "Sorcerers")
                        << " can only use staffs, knives, daggers, whips, slings, "
                        << "blowpipes and crossbows.";
                    return scSS.str();
                }
            }

            if (CATEGORY & item::category::Armor)
            {
                if (((MATERIAL_PRI & item::material::Cloth) == 0) &&
                    ((MATERIAL_PRI & item::material::SoftLeather) == 0) &&
                    ((MATERIAL_PRI & item::material::HardLeather) == 0) &&
                    ((MATERIAL_SEC & item::material::Cloth) == 0) &&
                    ((MATERIAL_SEC & item::material::SoftLeather) == 0) &&
                    ((MATERIAL_SEC & item::material::HardLeather) == 0))
                {
                    std::ostringstream scSS;
                    scSS << ((ROLE == role::Cleric) ? "Clerics" : "Sorcerers")
                        << " can only use armor made from cloth or leather.";
                    return scSS.str();
                }
            }
        }

        if (ROLE == role::Bard)
        {
            if ((WEAPON_TYPE & item::weapon_type::Axe) ||
                (WEAPON_TYPE & item::weapon_type::Club))
            {
                return "Bards cannot use axes or clubs made for war.";
            }

            if ((WEAPON_TYPE & item::weapon_type::BladedStaff) ||
                (WEAPON_TYPE & item::weapon_type::Spear))
            {
                return "Bards cannot use bladed staffs.";
            }
        }

        if (ROLE == role::Archer)
        {
            if ((WEAPON_TYPE & item::weapon_type::Axe) || (WEAPON_TYPE & item::weapon_type::Club))
            {
                return "Archers cannot use axes or clubs made for war.";
            }

            if ((WEAPON_TYPE & item::weapon_type::BladedStaff) ||
                (WEAPON_TYPE & item::weapon_type::Spear))
            {
                return "Archers cannot use bladed staffs.";
            }
        }

        if (ROLE == role::Thief)
        {
            if ((CATEGORY & item::category::TwoHanded) &&
                (CATEGORY & item::category::Weapon) &&
                ((WEAPON_TYPE & item::weapon_type::Sling) == 0) &&
                ((WEAPON_TYPE & item::weapon_type::Blowpipe) == 0))
            {
                return "Thieves cannot use two-handed weapons except for Slings and Blowpipes.";
            }

            if (((WEAPON_TYPE & item::weapon_type::Axe) > 0) ||
                ((WEAPON_TYPE & item::weapon_type::Sword) > 0) ||
                ((WEAPON_TYPE & item::weapon_type::Club) > 0))
            {
                std::ostringstream ss;
                ss << "Thieves cannot use "
                   << item::weapon_type::ToString(WEAPON_TYPE, false)<< "s.";
            }
        }

        return ITEM_ACTION_SUCCESS_STR_;
    }


    const std::string Creature::ItemUnEquip(const item::ItemPtr_t ITEM_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ITEM_PTR != nullptr),
            "Creature::ItemUnEquip() was given a null ITEM_PTR.");

        auto const IS_ITEM_UNEQUIP_ALLOWED_STR(IsItemUnqeuipAllowed(ITEM_PTR));
        if (IS_ITEM_UNEQUIP_ALLOWED_STR != ITEM_ACTION_SUCCESS_STR_)
        {
            return IS_ITEM_UNEQUIP_ALLOWED_STR;
        }

        inventory_.ItemUnEquip(ITEM_PTR);
        SetCurrentWeaponsToBestIfInvalidated();
        return ITEM_ACTION_SUCCESS_STR_;
    }


    const std::string Creature::IsItemUnqeuipAllowed(const item::ItemPtr_t ITEM_PTR)
    {
        if (ITEM_PTR->IsBodypart())
        {
            return "Bodyparts cannot be unequipped.";
        }
        else
        {
            return ITEM_ACTION_SUCCESS_STR_;
        }
    }


    const item::ItemPVec_t Creature::CurrentWeaponsInc()
    {
        //compose a vec of weapon vecs that can be considered equipped current weapon sets
        auto const WEAPONS_PVEC_VEC{ ComposeWeaponsList() };

        if (currWeaponsPVec_.empty())
        {
            if (WEAPONS_PVEC_VEC.empty())
            {
                currWeaponsPVec_.clear();
            }
            else
            {
                currWeaponsPVec_ = WEAPONS_PVEC_VEC[0];
            }

            return currWeaponsPVec_;
        }
        else
        {
            //find where in the weaponsPVecVec the currWeapons_ is
            auto const NUM_WEAPONS_SVECS(WEAPONS_PVEC_VEC.size());
            for (std::size_t i(0); i < NUM_WEAPONS_SVECS; ++i)
            {
                if (currWeaponsPVec_ == WEAPONS_PVEC_VEC[i])
                {
                    if (i == (NUM_WEAPONS_SVECS - 1))
                    {
                        currWeaponsPVec_.clear();
                    }
                    else
                    {
                        currWeaponsPVec_ = WEAPONS_PVEC_VEC[i + 1];
                    }

                    return currWeaponsPVec_;
                }
            }

            //if not in the vec then choose the first (most desirable) set of weapons
            currWeaponsPVec_ = WEAPONS_PVEC_VEC[0];
            return currWeaponsPVec_;
        }
    }


    void Creature::SetCurrentWeaponsToBest()
    {
        //compose a vec of weapon vecs that can be considered equipped current weapon sets
        auto const WEAPONS_PVEC_VEC{ ComposeWeaponsList() };

        if (WEAPONS_PVEC_VEC.empty())
        {
            currWeaponsPVec_.clear();
        }
        else
        {
            currWeaponsPVec_ = WEAPONS_PVEC_VEC[0];
        }
    }


    void Creature::SetCurrentWeaponsToBestIfInvalidated()
    {
        auto EQUIPPED_ITEMS_PVEC{ Inventory().ItemsEquipped() };

        //determine if the currrent weapon set is valid
        bool isValid(true);
        for (auto const NEXT_ITEM_PTR : currWeaponsPVec_)
        {
            auto const FOUND_CITER{ std::find_if(EQUIPPED_ITEMS_PVEC.begin(),
                                                 EQUIPPED_ITEMS_PVEC.end(),
                                                 [NEXT_ITEM_PTR]
                                                 (const item::ItemPtr_t P)
                                                 { return NEXT_ITEM_PTR == P; }) };

            if (FOUND_CITER == EQUIPPED_ITEMS_PVEC.end())
            {
                isValid = false;
                break;
            }
        }

        if (false == isValid)
        {
            SetCurrentWeaponsToBest();
        }
    }


    std::size_t Creature::WeaponsCount() const
    {
        std::size_t count(0);

        //wands do not count as weapons
        if (currWeaponsPVec_.empty() == false)
        {
            for (auto const NEXT_ITEM_PTR : currWeaponsPVec_)
            {
                if ((NEXT_ITEM_PTR->MiscType() & item::misc_type::Wand) == 0)
                {
                    ++count;
                }
            }
        }

        return count;
    }


    bool Creature::IsHoldingProjectileWeapon() const
    {
        bool isHoldingProjectileWeapon(false);

        for (auto const NEXT_ITEM_PTR : currWeaponsPVec_)
        {
            if (NEXT_ITEM_PTR->IsWeapon() &&
                (NEXT_ITEM_PTR->WeaponType() & item::weapon_type::Projectile))
            {
                isHoldingProjectileWeapon = true;
                break;
            }
        }

        return isHoldingProjectileWeapon;
    }


    const std::string Creature::WeaponsString() const
    {
        const std::string SEPARATOR(", ");

        //use a temp to avoid re-ordering the original currWeaponsPVec_
        auto tempPVec{ currWeaponsPVec_ };

        //sort by 'most damage'
        if (tempPVec.size() > 1)
        {
            std::sort(tempPVec.begin(),
                      tempPVec.end(),
                      []
                      (const item::ItemPtr_t A, const item::ItemPtr_t & B)
                      {
                        return (A->DamageMin() + A->DamageMax()) >
                               (B->DamageMin() + B->DamageMax());
                      });
        }

        std::ostringstream ss;
        for (auto const NEXT_ITEM_PTR : tempPVec)
        {
            ss << NEXT_ITEM_PTR->Name() << SEPARATOR;
        }

        return boost::algorithm::erase_last_copy(ss.str(), SEPARATOR);
    }


    const std::string Creature::ArmorString() const
    {
        item::ItemPVec_t armorItemsPVec;
        auto const EQUIPPED_ITEMS_PVEC{ Inventory().ItemsEquipped() };

        for (auto const NEXT_ITEM_PTR : EQUIPPED_ITEMS_PVEC)
        {
            if (NEXT_ITEM_PTR->Category() & item::category::Armor)
            {
                armorItemsPVec.push_back(NEXT_ITEM_PTR);
            }
        }

        //sort by armor rating
        if (armorItemsPVec.size() > 1)
        {
            std::sort(armorItemsPVec.begin(),
                      armorItemsPVec.end(),
                      []
                      (const item::ItemPtr_t A, const item::ItemPtr_t B)
                      {
                          return A->ArmorRating() > B->ArmorRating();
                      });
        }

        std::ostringstream ss;
        const std::string SEPARATOR(", ");
        for (auto const NEXT_ITEM_PTR : armorItemsPVec)
        {
            ss << NEXT_ITEM_PTR->Name() << SEPARATOR;
        }

        return boost::algorithm::erase_last_copy(ss.str(), SEPARATOR);
    }


    stats::Armor_t Creature::ArmorRating() const
    {
        return inventory_.ArmorRating();
    }


    const std::string Creature::CanCastSpellsStr(const bool WILL_PREFIX_AND_POSTFIX) const
    {
        const std::string RESPONSE_PREFIX((WILL_PREFIX_AND_POSTFIX) ? "Cannot cast because " : "");
        const std::string RESPONSE_POSTFIX((WILL_PREFIX_AND_POSTFIX) ? "." : "");

        if (role::Knight == role_.Which())
            return RESPONSE_PREFIX + "knights cannot cast spells" + RESPONSE_POSTFIX;

        const std::string CAN_TAKE_ACTION_STR(CanTakeActionStr(false));

        if (CAN_TAKE_ACTION_STR.empty() == false)
            return RESPONSE_PREFIX + CAN_TAKE_ACTION_STR + RESPONSE_POSTFIX;

        if (spellsVec_.empty())
            return RESPONSE_PREFIX + sex::HeSheIt(sex_, false) + " knows no spells" + RESPONSE_POSTFIX;

        if (ManaCurrent() <= 0)
            return RESPONSE_PREFIX + sex::HeSheIt(sex_, false) + " has no mana left" + RESPONSE_POSTFIX;

        return "";
    }


    bool Creature::CanCastSpellByType(const spell::SpellType::Enum E) const
    {
        for (auto const NEXT_SPELL_ENUM : spellsVec_)
            if (spell::Warehouse::Get(NEXT_SPELL_ENUM)->Type() == E)
                return true;

        return false;
    }


    const spell::SpellPVec_t Creature::SpellsPVec() const
    {
        spell::SpellPVec_t spellsPVec;

        for (auto const NEXT_SPELL_TYPE : spellsVec_)
            spellsPVec.push_back( spell::Warehouse::Get(NEXT_SPELL_TYPE) );

        return spellsPVec;
    }


    bool Creature::SpellAdd(const spell::Spells::Enum E)
    {
        if (std::find(spellsVec_.begin(), spellsVec_.end(), E) == spellsVec_.end())
        {
            spellsVec_.push_back(E);
            return true;
        }
        else
        {
            return false;
        }
    }


    bool Creature::SpellRemove(const spell::Spells::Enum E)
    {
        auto const FOUND_ITER{ std::find(spellsVec_.begin(), spellsVec_.end(), E) };

        if (FOUND_ITER == spellsVec_.end())
        {
            return false;
        }
        else
        {
            spellsVec_.erase(FOUND_ITER);
            return true;
        }
    }


    const std::string Creature::ToString() const
    {
        std::ostringstream ss;

        ss << "\"" << name_ << "\""
            << ", " << sex::ToString(sex_)
            << ", " << race::Name(race_.Which())
            << ", " << role::Name(role_.Which())
            << ", " << stats_.ToStringCurrent()
            << ", " << stats_.ToStringNormal()
            << ", health=" << healthCurrent_ << "/" << healthNormal_
            << ", rank=" << rank_
            << ", exp=" << experience_
            << ", body[" << bodyType_.ToString() << "]"
            << ", serial#" << serialNumber_;

        ss << ", conds=";
        for (auto const NEXT_CONDITION_ENUM : conditionsVec_)
            ss << condition::Warehouse::Get(NEXT_CONDITION_ENUM)->Name() << ",";

        ss << ", titles=";
        for (auto const NEXT_TITLE_ENUM : titlesVec_)
            ss << title::Warehouse::Get(NEXT_TITLE_ENUM)->Name() << ",";

        ss << ", inventory=" << inventory_.ToString();

        return ss.str();
    }


    void Creature::ManaCurrentAdj(const stats::Mana_t ADJ)
    {
        manaCurrent_ += ADJ;
        if (manaCurrent_ < 0)
        {
            manaCurrent_ = 0;
        }
        else if (manaCurrent_ > manaNormal_)
        {
            manaCurrent_ = manaNormal_;
        }
    }


    void Creature::ManaNormalAdj(const stats::Mana_t ADJ)
    {
        manaNormal_ += ADJ;
        if (manaNormal_ < 0)
        {
            manaNormal_ = 0;
        }
    }


    void Creature::StoreItemsInWarehouseAfterLoad()
    {
        inventory_.StoreItemsInWarehouseAfterLoad();
    }


    item::Weight_t Creature::WeightCanCarry() const
    {
        item::Weight_t base(5000);
        if (race::Gnome == race_.Which())
            base = 2500;
        else if (race::Pixie == race_.Which())
            base = 10;
        else if (race::Human != race_.Which())
            base = 3000;

        item::Weight_t multiplier(1000);
        if (race::Gnome == race_.Which())
            multiplier = 500;
        else if (race::Pixie == race_.Which())
            multiplier = 100;
        else if (race::Human != race_.Which())
            multiplier = 750;

        item::Weight_t divisor(2);
        if (race::Gnome == race_.Which())
            divisor = 4;
        else if (race::Pixie == race_.Which())
            divisor = 50;
        else if (race::Human != race_.Which())
            divisor = 3;

        return (base + ((static_cast<item::Weight_t>(stats_.Str().Current()) * multiplier) / divisor));
    }


    const item::ItemPVecVec_t Creature::ComposeWeaponsList() const
    {
        item::ItemPVecVec_t weaponsPVecVec;
        auto const EQUIPPED_ITEMS_PVEC{ Inventory().ItemsEquipped() };

        //the vec of non-bodypart equipped weapons is always first
        item::ItemPVec_t ssNonBPWeaponsPVec;
        for (auto const NEXT_ITEM_PTR : EQUIPPED_ITEMS_PVEC)
        {
            if ((NEXT_ITEM_PTR->IsWeapon()) &&
                (NEXT_ITEM_PTR->IsBodypart() == false))
            {
                ssNonBPWeaponsPVec.push_back(NEXT_ITEM_PTR);
            }
        }

        ssNonBPWeaponsPVec = item::Algorithms::FindByBroken(ssNonBPWeaponsPVec, false);

        if (ssNonBPWeaponsPVec.empty() == false)
        {
            weaponsPVecVec.push_back(ssNonBPWeaponsPVec);
        }

        //...if the creature is a spellcaster then wands are considered a separate weapon set/vec
        if (CanCastSpells())
        {
            const item::ItemPVec_t NOT_BROKEN_WANDS_PVEC{
                item::Algorithms::FindByBroken(
                    item::Algorithms::FindByMiscType(EQUIPPED_ITEMS_PVEC,
                                                     item::misc_type::Wand), false) };

            if (NOT_BROKEN_WANDS_PVEC.empty() == false)
            {
                weaponsPVecVec.push_back(NOT_BROKEN_WANDS_PVEC);
            }
        }

        //the equipped body part weapons that are not bite/claws are always second
        item::ItemPVec_t ssBPNonBCWeaponsPVec;
        for (auto const NEXT_ITEM_PTR : EQUIPPED_ITEMS_PVEC)
        {
            if ((NEXT_ITEM_PTR->IsWeapon()) &&
                NEXT_ITEM_PTR->IsBodypart() &&
                ((NEXT_ITEM_PTR->WeaponType() & item::weapon_type::Bite) == 0) &&
                ((NEXT_ITEM_PTR->WeaponType() & item::weapon_type::Claws) == 0))
            {
                ssBPNonBCWeaponsPVec.push_back(NEXT_ITEM_PTR);
            }
        }

        if (ssBPNonBCWeaponsPVec.empty() == false)
        {
            weaponsPVecVec.push_back(ssBPNonBCWeaponsPVec);
        }

        //the equipped body part weapons that ARE bite/claws always come after
        item::ItemPVec_t ssBPBCWeaponsPVec;
        for (auto const NEXT_ITEM_PTR : EQUIPPED_ITEMS_PVEC)
        {
            if ((NEXT_ITEM_PTR->IsWeapon()) &&
                NEXT_ITEM_PTR->IsBodypart() &&
                ((NEXT_ITEM_PTR->WeaponType() & item::weapon_type::Bite) ||
                (NEXT_ITEM_PTR->WeaponType() & item::weapon_type::Claws)))
            {
                ssBPBCWeaponsPVec.push_back(NEXT_ITEM_PTR);
            }
        }

        if (ssBPBCWeaponsPVec.empty() == false)
        {
            weaponsPVecVec.push_back(ssBPBCWeaponsPVec);
        }

        return weaponsPVecVec;
    }


    bool operator==(const Creature & L, const Creature & R)
    {
        if ( std::tie(L.name_,
                      L.imageFilename_,
                      L.sex_,
                      L.bodyType_,
                      L.race_,
                      L.role_,
                      L.stats_,
                      L.serialNumber_,
                      L.healthCurrent_,
                      L.healthNormal_,
                      L.rank_,
                      L.experience_,
                      L.dateTimeCreated_,
                      L.achievements_,
                      L.manaCurrent_,
                      L.manaNormal_)
                ==
                std::tie(R.name_,
                         R.imageFilename_,
                         R.sex_,
                         R.bodyType_,
                         R.race_,
                         R.role_,
                         R.stats_,
                         R.serialNumber_,
                         R.healthCurrent_,
                         R.healthNormal_,
                         R.rank_,
                         R.experience_,
                         R.dateTimeCreated_,
                         R.achievements_,
                         R.manaCurrent_,
                         R.manaNormal_) == false)
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(L.titlesVec_, R.titlesVec_) == false)
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(L.conditionsVec_, R.conditionsVec_) == false)
        {
            return false;
        }

        return misc::Vector::OrderlessCompareEqual(L.spellsVec_, R.spellsVec_);
    }


    bool operator<(const Creature & L, const Creature & R)
    {
        if ( std::tie(L.name_,
                      L.imageFilename_,
                      L.sex_,
                      L.bodyType_,
                      L.race_,
                      L.role_,
                      L.stats_,
                      L.serialNumber_,
                      L.healthCurrent_,
                      L.healthNormal_,
                      L.rank_,
                      L.experience_,
                      L.dateTimeCreated_,
                      L.achievements_,
                      L.manaCurrent_,
                      L.manaNormal_)
                <
                std::tie(R.name_,
                         R.imageFilename_,
                         R.sex_,
                         R.bodyType_,
                         R.race_,
                         R.role_,
                         R.stats_,
                         R.serialNumber_,
                         R.healthCurrent_,
                         R.healthNormal_,
                         R.rank_,
                         R.experience_,
                         R.dateTimeCreated_,
                         R.achievements_,
                         R.manaCurrent_,
                         R.manaNormal_) == true)
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.titlesVec_, R.titlesVec_) == true)
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.conditionsVec_, R.conditionsVec_) == true)
        {
            return true;
        }

        return misc::Vector::OrderlessCompareLess(L.spellsVec_, R.spellsVec_);
    }

}
}
