// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// creature.cpp
//
#include "creature.hpp"

#include "creature/condition-algorithms.hpp"
#include "creature/condition-holder.hpp"
#include "creature/condition.hpp"
#include "creature/title-holder.hpp"
#include "gui/creature-image-paths.hpp"
#include "item/algorithms.hpp"
#include "item/armor-enum.hpp"
#include "item/item-factory.hpp"
#include "item/item-profile-factory.hpp"
#include "item/item-profile.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/random.hpp"
#include "misc/real.hpp"
#include "misc/strings.hpp"
#include "misc/vectors.hpp"
#include "song/song-holder.hpp"
#include "song/song.hpp"
#include "spell/spell-holder.hpp"
#include "spell/spell.hpp"

#include <algorithm>

namespace heroespath
{
namespace creature
{

    const std::string Creature::ITEM_ACTION_SUCCESS_STR_ { "" };

    Creature::Creature(
        const bool IS_PLAYER,
        const std::string_view NAME,
        const sex::Enum SEX,
        const race::Enum & RACE,
        const role::Enum & ROLE,
        const StatSet & STATS,
        const std::string_view IMAGE_FILENAME,
        const Health_t & HEALTH,
        const Rank_t & RANK,
        const Experience_t & EXPERIENCE,
        const Mana_t & MANA)
        : isPlayer_(IS_PLAYER)
        , name_(NAME)
        , imageFilename_(IMAGE_FILENAME)
        , imageFullPath_("")
        , sex_(SEX)
        , bodyType_(BodyType::Make_FromRaceAndRole(RACE, ROLE))
        , race_(RACE)
        , role_(ROLE)
        , conditionsVec_({ Conditions::Good })
        , titlesVec_()
        , inventory_()
        , dateTimeCreated_(gui::DateTime::CurrentDateTime())
        , spellsVec_()
        , achievements_(ROLE)
        , heldWeaponsPVec_()
        , lastSpellCastNum_(0)
        , songsVec_()
        , lastSongPlayedNum_(0)
        , healthCurrent_(HEALTH)
        , healthNormal_(HEALTH)
        , rank_(RANK)
        , experience_(EXPERIENCE)
        , actualSet_()
        , bonusSet_()
        , enchantmentsPVec_()
    {
        actualSet_.Get(Traits::Mana).CurrAndNormSet(MANA.GetAsTrait());
        actualSet_.Get(Traits::Strength).CurrAndNormSet(STATS.Str().GetAsTrait());
        actualSet_.Get(Traits::Accuracy).CurrAndNormSet(STATS.Acc().GetAsTrait());
        actualSet_.Get(Traits::Charm).CurrAndNormSet(STATS.Cha().GetAsTrait());
        actualSet_.Get(Traits::Luck).CurrAndNormSet(STATS.Lck().GetAsTrait());
        actualSet_.Get(Traits::Speed).CurrAndNormSet(STATS.Spd().GetAsTrait());
        actualSet_.Get(Traits::Intelligence).CurrAndNormSet(STATS.Int().GetAsTrait());

        ReCalculateTraitBonuses();

        std::string errorMessage;

        M_HP_ASSERT_OR_LOG_AND_THROW_SS(
            (item::ItemFactory::EquipBodyPartWeapon(misc::NotNull<Creature *>(this), errorMessage)),
            "Error while trying to equip body part weapon: " << errorMessage);

        M_HP_ASSERT_OR_LOG_AND_THROW_SS(
            (item::ItemFactory::EquipSkinArmor(misc::NotNull<Creature *>(this), errorMessage)),
            "Error while trying to equip body part armor: " << errorMessage);
    }

    const std::string Creature::ImagePath() const
    {
        if (imageFilename_.empty())
        {
            imageFilename_ = gui::CreatureImagePaths::FilenameRandom(*this);
        }

        if (imageFullPath_.empty())
        {
            imageFullPath_ = gui::CreatureImagePaths::PathFromFilename(imageFilename_);
        }

        return imageFullPath_;
    }

    const std::string Creature::NameOrRaceAndRole(const bool IS_FIRST_LETTER_CAPS) const
    {
        std::string result((isPlayer_) ? Name() : race::RaceRoleName(race_, role_));

        if (!IS_FIRST_LETTER_CAPS)
        {
            misc::ToLower(result);
        }

        return result;
    }

    const std::string Creature::NameAndRaceAndRole(const bool IS_FIRST_LETTER_CAPS) const
    {
        std::string result;
        result.reserve(64);

        result += Name();

        if (result != RaceNameForced())
        {
            result += " the ";
            result += race::RaceRoleName(race_, role_);
        }

        if (!IS_FIRST_LETTER_CAPS)
        {
            misc::ToLower(result);
        }

        return result;
    }

    const std::string Creature::RankClassName() const
    {
        if (race_ == creature::race::Dragon)
        {
            return NAMEOF_ENUM_STR(DragonClass());
        }
        else if (race_ == creature::race::Wolfen)
        {
            return NAMEOF_ENUM_STR(WolfenClassType());
        }
        else
        {
            return NAMEOF_ENUM_STR(RankClass());
        }
    }

    float Creature::RankRatio() const
    {
        const float GRANDMASTER_RANK_F {
            misc::ConfigFile::Instance()->ValueOrDefault<float>("rankclass-Master-rankmax") + 1.0f
        };

        auto rankRatio { rank_.GetAs<float>() / GRANDMASTER_RANK_F };
        if (rankRatio > 1.0f)
        {
            rankRatio = 1.0f;
        }

        return rankRatio;
    }

    Health_t Creature::HealthCurrentAdj(const Health_t & HEALTH_ADJ)
    {
        healthCurrent_ += HEALTH_ADJ;

        if (healthCurrent_ < 0_health)
        {
            healthCurrent_ = 0_health;
        }

        if (healthCurrent_ > healthNormal_)
        {
            healthCurrent_ = healthNormal_;
        }

        return healthCurrent_;
    }

    Health_t Creature::HealthNormalAdj(const Health_t & HEALTH_ADJ)
    {
        healthNormal_ += HEALTH_ADJ;

        if (healthNormal_ < 1_health)
        {
            healthNormal_ = 1_health;
        }

        if (healthNormal_ < healthCurrent_)
        {
            healthCurrent_ = healthNormal_;
        }

        return healthNormal_;
    }

    void Creature::TitleAdd(const Titles::Enum ENUM, const bool ALLOW_CHANGES)
    {
        if (ALLOW_CHANGES)
        {
            title::Holder::Get(ENUM)->Change(CreaturePtr_t(this));
        }

        titlesVec_.emplace_back(ENUM);
    }

    const TitlePVec_t Creature::TitlesPVec() const
    {
        TitlePVec_t titlePVec;

        for (const auto NEXT_TITLE_ENUM : titlesVec_)
        {
            titlePVec.emplace_back(title::Holder::Get(NEXT_TITLE_ENUM));
        }

        return titlePVec;
    }

    const std::string Creature::HealthPercentStr(const bool WILL_APPEND_SYMBOL) const
    {
        std::string result;

        result.reserve(8);

        result += '0';

        const float HEALTH_RATIO(HealthRatio());
        if (!misc::IsRealClose(HEALTH_RATIO, 0.0f))
        {
            const auto HEALTH_INT { static_cast<int>(HEALTH_RATIO * 100.0f) };

            if (HEALTH_INT < 1)
            {
                result = "1";
            }
            else if (HEALTH_INT < 100)
            {
                result = std::to_string(HEALTH_INT);
            }
            else
            {
                result = "100";
            }
        }

        if (WILL_APPEND_SYMBOL)
        {
            result += '%';
        }

        return result;
    }

    bool Creature::ConditionAdd(const Conditions::Enum ENUM, const bool ALLOW_CHANGES)
    {
        if (std::find(conditionsVec_.begin(), conditionsVec_.end(), Conditions::Good)
            != conditionsVec_.end())
        {
            // prevent multiple 'Good' conditions
            if (ENUM == Conditions::Good)
            {
                return false;
            }
            else
            {
                // remove (all) the 'Good' conditions before adding the 'not so good' condition
                conditionsVec_.erase(
                    std::remove(conditionsVec_.begin(), conditionsVec_.end(), Conditions::Good),
                    conditionsVec_.end());
            }
        }

        // verify the condition is not already in the list
        if (conditionsVec_.end() == std::find(conditionsVec_.begin(), conditionsVec_.end(), ENUM))
        {
            conditionsVec_.emplace_back(ENUM);

            // make the change to the creature
            if (ALLOW_CHANGES)
            {
                condition::Holder::Get(ENUM)->InitialChange(CreaturePtr_t(this));
                ReCalculateTraitBonuses();
            }

            return true;
        }

        return false;
    }

    bool Creature::ConditionRemove(const Conditions::Enum ENUM)
    {
        CondEnumVec_t conditionsToRemoveVec;
        for (const auto NEXT_CONDITION_ENUM : conditionsVec_)
        {
            if (NEXT_CONDITION_ENUM == ENUM)
            {
                conditionsToRemoveVec.emplace_back(NEXT_CONDITION_ENUM);
            }
        }

        auto wasAnyConditionRemoved { false };
        for (const auto NEXT_CONDITION_TO_REMOVE_ENUM : conditionsToRemoveVec)
        {
            wasAnyConditionRemoved = true;
            condition::Holder::Get(NEXT_CONDITION_TO_REMOVE_ENUM)->FinalChange(CreaturePtr_t(this));

            const auto FOUND_ITER { std::find(
                conditionsVec_.begin(), conditionsVec_.end(), NEXT_CONDITION_TO_REMOVE_ENUM) };

            if (FOUND_ITER != std::end(conditionsVec_))
            {
                conditionsVec_.erase(FOUND_ITER);
            }
        }

        if (conditionsVec_.size() == 0)
        {
            ConditionAdd(Conditions::Good);
        }

        if (wasAnyConditionRemoved)
        {
            ReCalculateTraitBonuses();
        }

        return wasAnyConditionRemoved;
    }

    std::size_t Creature::ConditionRemoveAll()
    {
        const std::size_t ORIG_COND_COUNT(conditionsVec_.size());

        // undo the changes made by the conditions that will be removed
        for (const auto NEXT_COND_ENUM : conditionsVec_)
        {
            condition::Holder::Get(NEXT_COND_ENUM)->FinalChange(CreaturePtr_t(this));
        }

        conditionsVec_.clear();
        ConditionAdd(Conditions::Good);
        ReCalculateTraitBonuses();
        return ORIG_COND_COUNT;
    }

    const ConditionPVec_t Creature::ConditionsPVec() const
    {
        ConditionPVec_t conditionPVec;

        for (const auto NEXT_CONDITION_ENUM : conditionsVec_)
        {
            conditionPVec.emplace_back(condition::Holder::Get(NEXT_CONDITION_ENUM));
        }

        return conditionPVec;
    }

    bool Creature::HasCondition(const Conditions::Enum ENUM) const
    {
        for (const auto NEXT_CONDITION_ENUM : conditionsVec_)
        {
            if (NEXT_CONDITION_ENUM == ENUM)
            {
                return true;
            }
        }

        return false;
    }

    bool Creature::HasConditionNotAThreatTemp() const
    {
        return (
            HasCondition(Conditions::Dazed) || HasCondition(Conditions::Tripped)
            || HasCondition(Conditions::Pounced));
    }

    bool Creature::HasConditionNotAThreatPerm(const UnconOpt UNCON_OPTION) const
    {
        return (
            HasCondition(Conditions::Dead) || HasCondition(Conditions::Stone)
            || ((UNCON_OPTION == UnconOpt::Include) && HasCondition(Conditions::Unconscious)));
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
            CondEnumVec_t tempCondVec(conditionsVec_);

            std::sort(
                tempCondVec.begin(),
                tempCondVec.end(),
                [](const Conditions::Enum A, const Conditions::Enum B) {
                    return condition::Severity::Get(A) > condition::Severity::Get(B);
                });

            return condition::Severity::Get(tempCondVec[0]);
        }
    }

    bool Creature::HasMagicalCondition() const
    {
        for (const auto NEXT_CONDITION_ENUM : conditionsVec_)
        {
            if (condition::Holder::Get(NEXT_CONDITION_ENUM)->IsMagical())
            {
                return true;
            }
        }

        return false;
    }

    const std::string
        Creature::ConditionNames(const std::size_t MAX_TO_LIST, const size_t MIN_SEVERITY)
    {
        return creature::condition::Algorithms::Names(
            conditionsVec_,
            misc::JoinHow(misc::JoinOpt::Ellipsis, MAX_TO_LIST),
            MIN_SEVERITY,
            condition::Algorithms::SortOpt::Descending);
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

        if (HasCondition(Conditions::Tripped))
        {
            return RESPONSE_PREFIX + "tripped" + RESPONSE_POSTFIX;
        }

        if (HasCondition(Conditions::Pounced))
        {
            return RESPONSE_PREFIX + "pounced" + RESPONSE_POSTFIX;
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
        const auto IS_ALLOWED_STR { ItemIsAddAllowed(ITEM_PTR) };
        if (IS_ALLOWED_STR == ITEM_ACTION_SUCCESS_STR_)
        {
            inventory_.ItemAdd(ITEM_PTR);
            EnchantmentsApplyOrRemoveByType(
                ITEM_PTR->Enchantments(), EnchantmentType::WhenHeld, true);
        }
        else
        {
            M_HP_LOG_ERR(
                "creature::ItemAdd(creature={" << ToString() << "}, item={" << ITEM_PTR->ToString()
                                               << "}) item add failed.  Reason=" << IS_ALLOWED_STR
                                               << ".");
        }

        return IS_ALLOWED_STR;
    }

    const std::string Creature::ItemIsAddAllowed(const item::ItemPtr_t ITEM_PTR) const
    {
        std::string str;
        str.reserve(128);

        if (ITEM_PTR->IsBodyPart())
        {
            // always allow bodypart items
            str = ITEM_ACTION_SUCCESS_STR_;
        }
        else if (isPlayer_ && ((inventory_.Weight() + ITEM_PTR->Weight()) > WeightCanCarry()))
        {
            // verify strength/weight

            str = "item weighs ";
            str += ITEM_PTR->Weight().ToString();
            str += " which is ";
            str += ((inventory_.Weight() + ITEM_PTR->Weight()) - WeightCanCarry()).ToString();
            str += " too heavy";
        }
        else
        {
            str = ITEM_ACTION_SUCCESS_STR_;
        }

        return str;
    }

    void Creature::ItemRemove(const item::ItemPtr_t ITEM_PTR)
    {
        EnchantmentsApplyOrRemoveByType(ITEM_PTR->Enchantments(), EnchantmentType::WhenHeld, false);
        inventory_.ItemRemove(ITEM_PTR);
    }

    const std::string Creature::ItemEquip(const item::ItemPtr_t ITEM_PTR)
    {
        const auto IS_ALLOWED_STR { ItemIsEquipAllowed(ITEM_PTR) };
        if (IS_ALLOWED_STR == ITEM_ACTION_SUCCESS_STR_)
        {
            EnchantmentsApplyOrRemoveByType(
                ITEM_PTR->Enchantments(), EnchantmentType::WhenEquipped, true);

            inventory_.ItemEquip(ITEM_PTR);
            SetHeldWeaponsToBest();
        }
        else
        {
            M_HP_LOG_ERR(
                "creature::ItemEquip(creature={"
                << ToString() << "}, item={" << ITEM_PTR->ToString()
                << "}) item equip failed.  Reason=" << IS_ALLOWED_STR << ".");
        }

        return IS_ALLOWED_STR;
    }

    const std::string Creature::ItemIsEquipAllowed(const item::ItemPtr_t ITEM_PTR) const
    {
        if (ITEM_PTR->IsBodyPart())
        {
            return ITEM_ACTION_SUCCESS_STR_;
        }

        if (ITEM_PTR->HasCategory(item::Category::Broken))
        {
            return "Can't equip because it is broken.";
        }
        else if (!ITEM_PTR->IsEquipable())
        {
            std::string result;
            result.reserve(128);

            result += ("Not an equipable item.");

            if (ITEM_PTR->HasCategory(item::Category::Useable))
            {
                result += " (Try using it instead...)";
            }

            if (ITEM_PTR->IsQuest())
            {
                result += " (Have patience, this item will prove useful in time...)";
            }

            return result;
        }
        else if (
            IsPixie() && (ITEM_PTR->IsPixie() == false)
            && (ITEM_PTR->MustBePixieVersionForPixiesToEquip()))
        {
            std::string result;
            result.reserve(128);

            result += ("Can't equip because it is not made for pixies.");

            if (ITEM_PTR->MustBePixieVersionForPixiesToEquip()
                && item::Misc::HasPixieVersion(ITEM_PTR->MiscType()))
            {
                result += "  Pixies can never equip this item because none are made for pixies.";
            }
            else
            {
                result += "  This kind of item must be "
                          "specially made for a pixie before it can be equipped.";
            }

            return result;
        }
        else if (!IsPixie() && ITEM_PTR->IsPixie())
        {
            return "Can't equip because it is a mini verso made only for Pixies.";
        }
        else if (ITEM_PTR->IsSet())
        {
            const auto REQUIRED_ROLE { item::Set::RoleRestriction(ITEM_PTR->SetType()) };
            if (REQUIRED_ROLE != Role())
            {
                return "Can't equip because it is part of a magical set intended for "
                    + std::string(role::Name(REQUIRED_ROLE)) + "s.";
            }
        }

        // collect all the remaining reasons why the equip is not possible into one ss
        std::string equipFailReasonStr;
        equipFailReasonStr.reserve(255);

        auto separatorIfNotEmpty { [](const std::string & S) -> const std::string {
            if (S.empty())
            {
                return "";
            }
            else
            {
                return "  ";
            }
        } };

        // TODO? verify strength limit of the item?

        // verify role, such as Sorcerers not being able to use swords
        const std::string CAN_EQUIP_BY_ROLE_STR(ItemIsEquipAllowedByRaceAndRole(ITEM_PTR));
        if (CAN_EQUIP_BY_ROLE_STR != ITEM_ACTION_SUCCESS_STR_)
        {
            equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr) + CAN_EQUIP_BY_ROLE_STR;
        }

        // verify invalid duplicates
        if (ITEM_PTR->HasCategory(item::Category::OneHanded)
            && (inventory_.CountItemOfCategoryEquipped(item::Category::OneHanded) > 2))
        {
            equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
            equipFailReasonStr += "Can't equip three one-handed items.";
        }

        if (ITEM_PTR->HasCategory(item::Category::TwoHanded))
        {
            item::ItemPVec_t twoHandedNonBodyPartWeapons;

            for (const auto & PTR : inventory_.ItemsEquipped())
            {
                if (PTR->IsWeapon() && PTR->IsTwoHanded() && !PTR->IsBodyPart())
                {
                    twoHandedNonBodyPartWeapons.emplace_back(PTR);
                }
            }

            if (!twoHandedNonBodyPartWeapons.empty())
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += "Can't equip multiple two-handed items. (";
                equipFailReasonStr += item::Algorithms::Names(twoHandedNonBodyPartWeapons);
                equipFailReasonStr += " is already equipped)";
            }
        }

        if (ITEM_PTR->IsMisc())
        {
            const auto MISC_TYPE { ITEM_PTR->MiscType() };

            if ((MISC_TYPE == item::Misc::Wand)
                && (inventory_.CountItemOfMiscTypeEquipped(item::Misc::Wand) > 0))
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += "Can't equip more than one wand at a time.";
            }

            // verify valid duplicates upper limits
            if ((MISC_TYPE == item::Misc::Ring)
                && (inventory_.CountItemOfMiscTypeEquipped(item::Misc::Ring) < Body().NumArms()))
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += "Can't equip more than one ring per hand.";
            }

            if ((MISC_TYPE == item::Misc::MaskMourners) || (MISC_TYPE == item::Misc::MaskRascal))
            {
                std::size_t equippedMaskCount { 0 };

                if (inventory_.CountItemOfMiscTypeEquipped(item::Misc::MaskMourners))
                {
                    ++equippedMaskCount;
                }

                if (inventory_.CountItemOfMiscTypeEquipped(item::Misc::MaskRascal))
                {
                    ++equippedMaskCount;
                }

                if (Body().NumHeads() <= equippedMaskCount)
                {
                    equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                    equipFailReasonStr += "Can only equip one mask per head.";
                }
            }

            if ((MISC_TYPE == item::Misc::Ring) && (Body().HasFingers() == false))
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += RaceName();
                equipFailReasonStr += "'s can't equip rings because they have no fingers.";
            }
        }

        if (ITEM_PTR->IsArmor())
        {
            const auto ARMOR_TYPE { ITEM_PTR->ArmorType() };

            if ((ARMOR_TYPE == item::Armor::Bracer) && (Body().HasArms() == false))
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += "Can't equip bracers without arms.";
            }

            if ((ARMOR_TYPE == item::Armor::Gauntlet) && (Body().HasFingers() == false))
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += "Can't equip gauntlets without fingers.";
            }

            if ((ARMOR_TYPE == item::Armor::Gauntlet) && (Body().HasArms() == false))
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += "Can't equip gauntlets without arms.";
            }

            if ((ARMOR_TYPE == item::Armor::Helm) && (Body().HasHead() == false))
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += RaceName();
                equipFailReasonStr += "'s can't equip helms because they have no head.";
            }

            if ((ARMOR_TYPE == item::Armor::Helm) && (Body().HasHorns() == true))
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += RaceName();
                equipFailReasonStr += "'s can't equip helms beaus they have horns.";
            }

            if ((ARMOR_TYPE == item::Armor::Aventail)
                && (item::Algorithms::FindByArmorType(inventory_.ItemsEquipped(), item::Armor::Helm)
                        .empty()))
            {
                equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                equipFailReasonStr += "Can't equip an aventail without first equipping a helm.";
            }

            std::size_t armorEquipLimit { 1 };

            std::string equipLimitErrorStr;
            equipLimitErrorStr.reserve(255);

            if ((ARMOR_TYPE == item::Armor::Bracer) || (ARMOR_TYPE == item::Armor::Gauntlet))
            {
                armorEquipLimit = Body().NumArms() / 2;

                equipLimitErrorStr += separatorIfNotEmpty(equipLimitErrorStr);

                equipLimitErrorStr
                    += "(Bracer and gauntlets are pairs of items.  You can only equip one "
                       "set per two arms.  ";

                equipLimitErrorStr += Name() + " has ";
                equipLimitErrorStr += std::to_string(Body().NumArms());
                equipLimitErrorStr += " arms.)";
            }
            else if (ARMOR_TYPE == item::Armor::Shield)
            {
                armorEquipLimit = Body().NumArms();

                equipLimitErrorStr += separatorIfNotEmpty(equipLimitErrorStr);
                equipLimitErrorStr += "(Can only equip one shield per arm.  ";
                equipLimitErrorStr += Name();
                equipLimitErrorStr += " has ";
                equipLimitErrorStr += std::to_string(Body().NumArms());
                equipLimitErrorStr += " arms.)";
            }
            else if ((ARMOR_TYPE == item::Armor::Boot) || (ARMOR_TYPE == item::Armor::Pant))
            {
                armorEquipLimit = Body().NumLegs() / 2;

                equipLimitErrorStr += separatorIfNotEmpty(equipLimitErrorStr);
                equipLimitErrorStr += "(Can only equip one boots or pants per two legs.  ";
                equipLimitErrorStr += Name();
                equipLimitErrorStr += " has ";
                equipLimitErrorStr += std::to_string(Body().NumLegs());
                equipLimitErrorStr += " legs.)";
            }
            else if (ARMOR_TYPE == item::Armor::Helm)
            {
                armorEquipLimit = Body().NumHeads();

                equipLimitErrorStr += separatorIfNotEmpty(equipLimitErrorStr);
                equipLimitErrorStr += "(Can only equip one helm per head.  ";
                equipLimitErrorStr += Name();
                equipLimitErrorStr += " has ";
                equipLimitErrorStr += std::to_string(Body().NumHeads());
                equipLimitErrorStr += " heads.)";
            }
            else if (ARMOR_TYPE == item::Armor::Aventail)
            {
                armorEquipLimit = inventory_.CountItemOfArmorTypeEquipped(item::Armor::Helm);

                equipLimitErrorStr += separatorIfNotEmpty(equipLimitErrorStr);
                equipLimitErrorStr += "(Can only equip one aventail per helm.  ";
                equipLimitErrorStr += Name();
                equipLimitErrorStr += " has ";
                equipLimitErrorStr += std::to_string(armorEquipLimit);
                equipLimitErrorStr += " helms.)";
            }

            const auto & ARMOR_INFO { ITEM_PTR->ArmorInfo() };

            if (ARMOR_INFO.IsCover())
            {
                // can't equip Robe+Cloak, but can equip Vest with either Robe or Cloak, and
                // can also equip Cape with Vest
                const auto HAS_EQUIPPED_VEST { inventory_.HasCoverTypeEquipped(
                    item::Covers::Vest) };

                const auto HAS_EQUIPPED_CAPE { inventory_.HasCoverTypeEquipped(
                    item::Covers::Cape) };

                const auto HAS_EQUIPPED_ROBE { inventory_.HasCoverTypeEquipped(
                    item::Covers::Robe) };

                const auto HAS_EQUIPPED_CLOAK { inventory_.HasCoverTypeEquipped(
                    item::Covers::Cloak) };

                if (ARMOR_INFO.IsMinor<item::Covers>(item::Covers::Vest) && HAS_EQUIPPED_VEST)
                {
                    equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                    equipLimitErrorStr += "Can't equip vest because already has a vest equipped.";
                }
                else if (ARMOR_INFO.IsMinor<item::Covers>(item::Covers::Cape))
                {
                    if (HAS_EQUIPPED_CAPE)
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);

                        equipLimitErrorStr
                            += "Can't equip cape because already has a cape equipped.";
                    }

                    if (HAS_EQUIPPED_ROBE || HAS_EQUIPPED_CLOAK)
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);

                        equipLimitErrorStr
                            += "Can't equip cape because already has a robe or cloak equipped.";
                    }
                }
                else if (ARMOR_INFO.IsMinor<item::Covers>(item::Covers::Robe))
                {
                    if (HAS_EQUIPPED_ROBE)
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                        equipLimitErrorStr
                            += "Can't equip robe because already has a robe equipped.";
                    }

                    if (HAS_EQUIPPED_CAPE)
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                        equipLimitErrorStr
                            += "Can't equip robe because already has a cape equipped.";
                    }

                    if (HAS_EQUIPPED_CLOAK)
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                        equipLimitErrorStr
                            += "Can't equip robe because already has a cloak equipped.";
                    }
                }
                else if (ARMOR_INFO.IsMinor<item::Covers>(item::Covers::Cloak))
                {
                    if (HAS_EQUIPPED_CLOAK)
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                        equipLimitErrorStr
                            += "Can't equip cloak because already has a cloak equipped.";
                    }

                    if (HAS_EQUIPPED_CAPE)
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                        equipLimitErrorStr
                            += "Can't equip cloak because already has a cape equipped.";
                    }

                    if (HAS_EQUIPPED_ROBE)
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                        equipLimitErrorStr
                            += "Can't equip cloak because already has a robe equipped.";
                    }
                }
            }
            else
            {
                const auto ALREADY_EQUIPPED_COUNT { inventory_.CountItemOfArmorTypeEquipped(
                    ARMOR_TYPE) };

                if (ALREADY_EQUIPPED_COUNT > armorEquipLimit)
                {
                    const auto ARMOR_TYPE_STR { NAMEOF_ENUM(ARMOR_TYPE) };

                    if (0 == armorEquipLimit)
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                        equipLimitErrorStr += "Can't equip any ";
                        equipLimitErrorStr += ARMOR_TYPE_STR;
                        equipLimitErrorStr += '.';
                    }
                    else
                    {
                        equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                        equipLimitErrorStr += "Already equipped ";
                        equipLimitErrorStr += std::to_string(ALREADY_EQUIPPED_COUNT);
                        equipLimitErrorStr += ' ';
                        equipLimitErrorStr += ARMOR_TYPE_STR;
                        equipLimitErrorStr += ", which is the limit.";
                    }

                    equipFailReasonStr += separatorIfNotEmpty(equipFailReasonStr);
                    equipLimitErrorStr += equipLimitErrorStr;
                }
            }
        }

        if (equipFailReasonStr.empty())
        {
            return ITEM_ACTION_SUCCESS_STR_;
        }
        else
        {
            return boost::algorithm::erase_last_copy(equipFailReasonStr, ", ");
        }
    }

    const std::string
        Creature::ItemIsEquipAllowedByRaceAndRole(const item::ItemPtr_t ITEM_PTR) const
    {
        std::string str(ITEM_ACTION_SUCCESS_STR_);
        str.reserve(64);

        const bool IS_MILLITARY_ROLE
            = ((role::Knight == role_) || (role::Captain == role_) || (role::Warlord == role_)
               || (role::Chieftain == role_) || (role::Soldier == role_));

        const auto MILLITARY_ROLE_NAMES = "Soldier, Captain, Warlord, Knight, Chieftain";

        if (ITEM_PTR->IsWeapon())
        {
            const auto & WEAPON_INFO { ITEM_PTR->WeaponInfo() };

            if ((role::Bard == role_) || (role::Archer == role_))
            {
                if (WEAPON_INFO.IsMinor<item::Axes>(item::Axes::Waraxe)
                    || WEAPON_INFO.IsMinor<item::Clubs>(item::Clubs::Warhammer)
                    || WEAPON_INFO.IsBladedstaff())
                {
                    str += role::Name(role_);
                    str += "s cannot equip axes, clubs, spears, or bladedstaffs made for war.";
                    return str;
                }
            }

            if (role::Thief == role_)
            {
                if (ITEM_PTR->IsTwoHanded()
                    && !WEAPON_INFO.IsMinor<item::Projectiles>(item::Projectiles::Sling)
                    && !WEAPON_INFO.IsMinor<item::Projectiles>(item::Projectiles::Blowpipe))
                {
                    str += "Thieves cannot equip two-handed weapons except for Slings and "
                           "Blowpipes.";

                    return str;
                }

                // if (ITEM_PTR->HasWeaponType(item::Weapon::Axe)
                //    || ITEM_PTR->HasWeaponType(item::Weapon::Sword)
                //    || ITEM_PTR->HasWeaponType(item::Weapon::Club))
                //{
                //    return std::string("Thieves cannot equip ")
                //        .append(item::Weapon::ToString(ITEM_PTR->WeaponType()))
                //        .append("s.");
                //}
            }

            if (!IS_MILLITARY_ROLE)
            {
                if (WEAPON_INFO.IsBladedstaff() && !WEAPON_INFO.IsEitherSpear())
                {
                    str += "Only ";
                    str += MILLITARY_ROLE_NAMES;
                    str += " may equip non-simple-spear bladedstaffs.";
                    return str;
                }

                if (WEAPON_INFO.IsMinor<item::Swords>(item::Swords::Knightlysword)
                    || WEAPON_INFO.IsMinor<item::Swords>(item::Swords::Claymore))
                {
                    str += "Only ";
                    str += MILLITARY_ROLE_NAMES;
                    str += " may equip Knightlyswords and Claymore swords.";
                    return str;
                }
            }
        }

        if (ITEM_PTR->IsArmor())
        {
            const auto & ARMOR_INFO { ITEM_PTR->ArmorInfo() };

            if (!ITEM_PTR->IsBodyPart() && ((race::Dragon == race_) || (race::Wolfen == race_)))
            {
                str += NAMEOF_ENUM(race_);
                str += " can only equip body parts.";
                return str;
            }

            if (!IS_MILLITARY_ROLE)
            {
                if (ARMOR_INFO.IsMinor<item::Shields>(item::Shields::Pavis))
                {
                    str += "Only ";
                    str += MILLITARY_ROLE_NAMES;
                    str += " may equip Pavis shields.";
                    return str;
                }

                if (ARMOR_INFO.IsMinor<item::Helms>(item::Helms::Great))
                {
                    str += "Only ";
                    str += MILLITARY_ROLE_NAMES;
                    str += " may equip Great helms.";
                    return str;
                }

                if (ARMOR_INFO.Form() == item::Forms::Plate)
                {
                    str += "Only ";
                    str += MILLITARY_ROLE_NAMES;
                    str += " may equip Plate armor.";
                    return str;
                }
            }
        }

        if (item::Misc::IsMusicalInstrument(ITEM_PTR->MiscType()) && (role::Bard != role_))
        {
            str += "Only Bards may equip musical instruments.";
            return str;
        }

        if (ITEM_PTR->IsSet() && (item::Set::RoleRestriction(ITEM_PTR->SetType()) != role_))
        {
            str += "Only ";
            str += NAMEOF_ENUM(item::Set::RoleRestriction(ITEM_PTR->SetType()));
            str += "s may equip items from the ";
            str += item::Set::Name(ITEM_PTR->SetType());
            str += " Set.";
            return str;
        }

        if ((role::Cleric == role_) || (role::Sorcerer == role_))
        {
            if (ITEM_PTR->IsWeapon())
            {
                const auto & WEAPON_INFO { ITEM_PTR->WeaponInfo() };

                if (!WEAPON_INFO.IsBlowpipe() && !WEAPON_INFO.IsAnyKnife()
                    && !WEAPON_INFO.IsMinor<item::Projectiles>(item::Projectiles::Sling)
                    && !WEAPON_INFO.IsAnyStaff() && !WEAPON_INFO.IsWhip())
                {
                    str += NAMEOF_ENUM(role_);
                    str += "s can only equip staffs, knives, whips, slings, and blowpipes.";
                    return str;
                }
            }

            if (ITEM_PTR->IsArmor())
            {
                const auto MAT_PRI { ITEM_PTR->MaterialPrimary() };

                if ((MAT_PRI != item::Material::Cloth) && (MAT_PRI != item::Material::Leather)
                    && (MAT_PRI != item::Material::Silk) && (MAT_PRI != item::Material::Fur))
                {
                    str += NAMEOF_ENUM(role_);
                    str += "s can only equip armor made from cloth, silk, fur, or leather.";
                    return str;
                }
            }
        }

        return str;
    }

    const std::string Creature::ItemUnEquip(const item::ItemPtr_t ITEM_PTR)
    {
        const auto IS_ITEM_UNEQUIP_ALLOWED_STR { IsItemUnqeuipAllowed(ITEM_PTR) };
        if (IS_ITEM_UNEQUIP_ALLOWED_STR != ITEM_ACTION_SUCCESS_STR_)
        {
            return IS_ITEM_UNEQUIP_ALLOWED_STR;
        }

        EnchantmentsApplyOrRemoveByType(
            ITEM_PTR->Enchantments(), EnchantmentType::WhenEquipped, false);

        inventory_.ItemUnEquip(ITEM_PTR);
        SetHeldWeaponsToBestIfInvalidated();
        return ITEM_ACTION_SUCCESS_STR_;
    }

    const std::string Creature::IsItemUnqeuipAllowed(const item::ItemPtr_t ITEM_PTR)
    {
        if (ITEM_PTR->IsBodyPart())
        {
            return "BodyPartWeapons cannot be unequipped.";
        }
        else
        {
            return ITEM_ACTION_SUCCESS_STR_;
        }
    }

    void Creature::IncrementHeldWeapons()
    {
        // compose a vec of weapon vecs that can be considered equipped current weapon sets
        const auto WEAPONS_PVEC_VEC { ComposeWeaponsList() };

        if (heldWeaponsPVec_.empty())
        {
            if (WEAPONS_PVEC_VEC.empty())
            {
                heldWeaponsPVec_.clear();
            }
            else
            {
                heldWeaponsPVec_ = WEAPONS_PVEC_VEC[0];
            }
        }
        else
        {
            // find where in the weaponsPVecVec the currWeapons_ is
            const auto NUM_WEAPONS_SVECS(WEAPONS_PVEC_VEC.size());
            for (std::size_t i(0); i < NUM_WEAPONS_SVECS; ++i)
            {
                if (heldWeaponsPVec_ == WEAPONS_PVEC_VEC[i])
                {
                    if (i == (NUM_WEAPONS_SVECS - 1))
                    {
                        heldWeaponsPVec_.clear();
                    }
                    else
                    {
                        heldWeaponsPVec_ = WEAPONS_PVEC_VEC[i + 1];
                    }

                    return;
                }
            }

            // if not in the vec then choose the first (most desirable) set of weapons
            heldWeaponsPVec_ = WEAPONS_PVEC_VEC[0];
        }
    }

    void Creature::SetHeldWeaponsToBest()
    {
        // compose a vec of weapon vecs that can be considered equipped current weapon sets
        const auto WEAPONS_PVEC_VEC { ComposeWeaponsList() };

        if (WEAPONS_PVEC_VEC.empty())
        {
            heldWeaponsPVec_.clear();
        }
        else
        {
            heldWeaponsPVec_ = WEAPONS_PVEC_VEC[0];
        }
    }

    void Creature::SetHeldWeaponsToBestIfInvalidated()
    {
        auto EQUIPPED_ITEMS_PVEC { Inventory().ItemsEquipped() };

        // determine if the current weapon set is valid (equipped)
        for (const auto & HELD_ITEM_PTR : heldWeaponsPVec_)
        {
            const auto IS_HELD_ITEM_EQUIPPED { std::find_if(
                                                   EQUIPPED_ITEMS_PVEC.begin(),
                                                   EQUIPPED_ITEMS_PVEC.end(),
                                                   [HELD_ITEM_PTR](const auto EQUIPPED_ITEM_PTR) {
                                                       return HELD_ITEM_PTR == EQUIPPED_ITEM_PTR;
                                                   })
                                               != EQUIPPED_ITEMS_PVEC.end() };

            if (IS_HELD_ITEM_EQUIPPED == false)
            {
                SetHeldWeaponsToBest();
                break;
            }
        }
    }

    std::size_t Creature::HeldWeaponsCount() const
    {
        std::size_t count { 0 };

        // wands do not count as weapons
        for (const auto & NEXT_ITEM_PTR : heldWeaponsPVec_)
        {
            if (NEXT_ITEM_PTR->MiscType() != item::Misc::Wand)
            {
                ++count;
            }
        }

        return count;
    }

    bool Creature::IsHoldingProjectileWeapon() const
    {
        for (const auto & NEXT_ITEM_PTR : heldWeaponsPVec_)
        {
            if (NEXT_ITEM_PTR->WeaponInfo().IsProjectile())
            {
                return true;
            }
        }

        return false;
    }

    const std::string Creature::WeaponsString() const
    {
        // use a temp to avoid re-ordering the original heldWeaponsPVec_
        auto tempPVec { heldWeaponsPVec_ };

        // sort by 'most damage'
        if (tempPVec.size() > 1)
        {
            std::sort(tempPVec.begin(), tempPVec.end(), [](const auto & A, const auto & B) {
                return (A->DamageMin() + A->DamageMax()) > (B->DamageMin() + B->DamageMax());
            });
        }

        std::string weaponstr;
        weaponstr.reserve(tempPVec.size() * 32);

        for (const auto & NEXT_ITEM_PTR : tempPVec)
        {
            if (!weaponstr.empty())
            {
                weaponstr += ", ";
            }

            weaponstr += NEXT_ITEM_PTR->Name();
        }

        return weaponstr;
    }

    const std::string Creature::ArmorString() const
    {
        item::ItemPVec_t armorItemsPVec;
        for (const auto & NEXT_ITEM_PTR : Inventory().ItemsEquipped())
        {
            if (NEXT_ITEM_PTR->IsArmor())
            {
                armorItemsPVec.emplace_back(NEXT_ITEM_PTR);
            }
        }

        // sort by armor rating
        if (armorItemsPVec.size() > 1)
        {
            std::sort(
                armorItemsPVec.begin(),
                armorItemsPVec.end(),
                [](const auto ITEM_PTR_A, const auto ITEM_PTR_B) {
                    return ITEM_PTR_A->ArmorRating() > ITEM_PTR_B->ArmorRating();
                });
        }

        std::string armorStr;
        armorStr.reserve(armorItemsPVec.size() * 32);

        for (const auto & NEXT_ITEM_PTR : armorItemsPVec)
        {
            if (!armorStr.empty())
            {
                armorStr += ", ";
            }

            armorStr += NEXT_ITEM_PTR->Name();
        }

        return armorStr;
    }

    Armor_t Creature::ArmorRating() const { return inventory_.ArmorRating(rank_); }

    const std::string Creature::CanCastSpellsStr(const bool WILL_PREFIX_AND_POSTFIX) const
    {
        const std::string RESPONSE_PREFIX((WILL_PREFIX_AND_POSTFIX) ? "Cannot cast because " : "");
        const std::string RESPONSE_POSTFIX((WILL_PREFIX_AND_POSTFIX) ? "." : "");

        if (role::Knight == role_)
        {
            return RESPONSE_PREFIX + "knights cannot cast spells" + RESPONSE_POSTFIX;
        }

        const std::string CAN_TAKE_ACTION_STR(CanTakeActionStr(false));

        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return RESPONSE_PREFIX + CAN_TAKE_ACTION_STR + RESPONSE_POSTFIX;
        }

        if (spellsVec_.empty())
        {
            return RESPONSE_PREFIX + std::string(sex::HeSheIt(sex_, false)) + " knows no spells"
                + RESPONSE_POSTFIX;
        }

        if (TraitWorking(Traits::Mana) <= 0)
        {
            return RESPONSE_PREFIX + std::string(sex::HeSheIt(sex_, false)) + " has no mana left"
                + RESPONSE_POSTFIX;
        }

        if (inventory_.HasCastingItemEquipped() == false)
        {
            return RESPONSE_PREFIX + std::string(creature::sex::HeSheIt(sex_, false))
                + " has no wand or other casting item" + RESPONSE_POSTFIX;
        }

        return "";
    }

    bool Creature::CanCastSpellByEffectType(const combat::EffectType::Enum ENUM) const
    {
        return CanCastSpellByEffectType(combat::EffectTypeVec_t(1, ENUM));
    }

    bool Creature::CanCastSpellByEffectType(const combat::EffectTypeVec_t & EFFECT_TYPE_VEC) const
    {
        for (const auto NEXT_SPELL_ENUM : spellsVec_)
        {
            const auto NEXT_EFFECT_TYPE_SOURCE { spell::Holder::Get(NEXT_SPELL_ENUM)->Effect() };

            for (const auto NEXT_EFFECT_TYPE_TEST : EFFECT_TYPE_VEC)
            {
                if (NEXT_EFFECT_TYPE_SOURCE == NEXT_EFFECT_TYPE_TEST)
                {
                    return true;
                }
            }
        }

        return false;
    }

    const spell::SpellPVec_t Creature::SpellsPVec() const
    {
        spell::SpellPVec_t spellsPVec;

        for (const auto NEXT_SPELL_TYPE : spellsVec_)
        {
            spellsPVec.emplace_back(spell::Holder::Get(NEXT_SPELL_TYPE));
        }

        return spellsPVec;
    }

    bool Creature::SpellAdd(const spell::Spells::Enum ENUM)
    {
        if (std::find(spellsVec_.begin(), spellsVec_.end(), ENUM) == spellsVec_.end())
        {
            spellsVec_.emplace_back(ENUM);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Creature::SpellRemove(const spell::Spells::Enum ENUM)
    {
        const auto FOUND_ITER { std::find(spellsVec_.begin(), spellsVec_.end(), ENUM) };

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

    const std::string Creature::CanPlaySongsStr(const bool WILL_PREFIX_AND_POSTFIX) const
    {
        const std::string RESPONSE_PREFIX(
            (WILL_PREFIX_AND_POSTFIX) ? "Cannot play song because " : "");

        const std::string RESPONSE_POSTFIX((WILL_PREFIX_AND_POSTFIX) ? "." : "");

        if (role_ != role::Bard)
        {
            return RESPONSE_PREFIX + "only Bards can play magical songs" + RESPONSE_POSTFIX;
        }

        const std::string CAN_TAKE_ACTION_STR(CanTakeActionStr(false));

        if (CAN_TAKE_ACTION_STR.empty() == false)
        {
            return RESPONSE_PREFIX + CAN_TAKE_ACTION_STR + RESPONSE_POSTFIX;
        }

        if (songsVec_.empty())
        {
            return RESPONSE_PREFIX + std::string(sex::HeSheIt(sex_, false)) + " knows no songs"
                + RESPONSE_POSTFIX;
        }

        if (TraitWorking(Traits::Mana) <= 0)
        {
            return RESPONSE_PREFIX + std::string(sex::HeSheIt(sex_, false)) + " has no mana left"
                + RESPONSE_POSTFIX;
        }

        if (inventory_.HasMusicalInstrumentEquipped() == false)
        {
            return RESPONSE_PREFIX + std::string(sex::HeSheIt(sex_, false))
                + " has no musical instrument equipped" + RESPONSE_POSTFIX;
        }

        return "";
    }

    bool Creature::CanPlaySongsByEffectType(const combat::EffectType::Enum ENUM) const
    {
        return CanPlaySongsByEffectType(combat::EffectTypeVec_t(1, ENUM));
    }

    bool Creature::CanPlaySongsByEffectType(const combat::EffectTypeVec_t & EFFECT_TYPE_VEC) const
    {
        for (const auto NEXT_SONG_ENUM : songsVec_)
        {
            const auto NEXT_EFFECT_TYPE_SOURCE { song::Holder::Get(NEXT_SONG_ENUM)->Effect() };

            for (const auto NEXT_EFFECT_TYPE_TEST : EFFECT_TYPE_VEC)
            {
                if (NEXT_EFFECT_TYPE_SOURCE == NEXT_EFFECT_TYPE_TEST)
                {
                    return true;
                }
            }
        }

        return false;
    }

    const song::SongPVec_t Creature::SongsPVec() const
    {
        song::SongPVec_t songsPVec;

        for (const auto NEXT_SONG_ENUM : songsVec_)
        {
            songsPVec.emplace_back(song::Holder::Get(NEXT_SONG_ENUM));
        }

        return songsPVec;
    }

    bool Creature::SongAdd(const song::Songs::Enum ENUM)
    {
        if (std::find(songsVec_.begin(), songsVec_.end(), ENUM) == songsVec_.end())
        {
            songsVec_.emplace_back(ENUM);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Creature::SongRemove(const song::Songs::Enum ENUM)
    {
        const auto FOUND_ITER { std::find(songsVec_.begin(), songsVec_.end(), ENUM) };

        if (FOUND_ITER == songsVec_.end())
        {
            return false;
        }
        else
        {
            songsVec_.erase(FOUND_ITER);
            return true;
        }
    }

    const std::string Creature::ToString() const
    {
        std::string result;
        result.reserve(2048);

        result += '\"';
        result += name_;
        result += '\"';
        result += ", ";
        result += NAMEOF_ENUM(sex_);
        result += ", ";
        result += race::Name(race_);

        result += ", ";
        result += role::Name(role_);
        result += ", ";
        result += actualSet_.StatsString(false);
        result += ", health=";
        result += healthCurrent_.ToString();
        result += '/';
        result += healthNormal_.ToString();
        result += ", mana=";
        result += std::to_string(TraitWorking(Traits::Mana));
        result += '/';
        result += std::to_string(TraitNormal(Traits::Mana));
        result += ", rank=";
        result += Rank().ToString();
        result += ", exp=";
        result += Exp().ToString();
        result += ", body[";
        result += bodyType_.ToString();
        result += "], conds=";

        for (const auto NEXT_CONDITION_ENUM : conditionsVec_)
        {
            result += condition::Holder::Get(NEXT_CONDITION_ENUM)->Name();
            result += ',';
        }

        result += " titles=";
        for (const auto NEXT_TITLE_ENUM : titlesVec_)
        {
            result += title::Holder::Get(NEXT_TITLE_ENUM)->Name();
            result += ',';
        }

        result += " inventory=";
        result += inventory_.ToString();

        return result;
    }

    void Creature::EnchantmentApplyOrRemove(
        const EnchantmentPtr_t ENCHANTMENT_PTR, const bool WILL_APPLY)
    {
        if (WILL_APPLY)
        {
            enchantmentsPVec_.emplace_back(ENCHANTMENT_PTR);
            ReCalculateTraitBonuses();
        }
        else
        {
            enchantmentsPVec_.erase(
                std::remove(enchantmentsPVec_.begin(), enchantmentsPVec_.end(), ENCHANTMENT_PTR),
                enchantmentsPVec_.end());

            ReCalculateTraitBonuses();
        }
    }

    Weight_t Creature::WeightCanCarry() const
    {
        auto base { Weight_t::value_type(5000) };
        if (race::Gnome == race_)
        {
            base = 2500;
        }
        else if (race::Pixie == race_)
        {
            base = 1000;
        }
        else if (race::Human != race_)
        {
            base = 3000;
        }

        auto multiplier { Weight_t::value_type(1000) };
        if (race::Gnome == race_)
        {
            multiplier = 500;
        }
        else if (race::Pixie == race_)
        {
            multiplier = 250;
        }
        else if (race::Human != race_)
        {
            multiplier = 750;
        }

        auto divisor { Weight_t::value_type(2) };
        if (race::Gnome == race_)
        {
            divisor = 4;
        }
        else if (race::Pixie == race_)
        {
            divisor = 5;
        }
        else if (race::Human != race_)
        {
            divisor = 3;
        }

        return Weight_t::Make(
            base + ((Strength().GetAs<Weight_t::value_type>() * multiplier) / divisor));
    }

    void Creature::ReCalculateTraitBonuses()
    {
        for (EnumUnderlying_t i(0); i < Traits::StatCount; ++i)
        {
            const auto NEXT_TRAIT_ENUM { static_cast<Traits::Enum>(i) };

            int traitPercent { 0 };

            for (const auto NEXT_COND_ENUM : conditionsVec_)
            {
                traitPercent += condition::Holder::Get(NEXT_COND_ENUM)
                                    ->Traits()
                                    .GetCopy(NEXT_TRAIT_ENUM)
                                    .Current();
            }

            for (const auto & NEXT_ENCHANTMENT_PTR : enchantmentsPVec_)
            {
                traitPercent += NEXT_ENCHANTMENT_PTR->Traits().GetCopy(NEXT_TRAIT_ENUM).Current();
            }

            traitPercent += bonusSet_.GetCopy(NEXT_TRAIT_ENUM).Normal();

            bonusSet_.Get(NEXT_TRAIT_ENUM).CurrentSet(traitPercent);
        }
    }

    Trait_t Creature::TraitWorking(const Traits::Enum ENUM) const
    {
        const auto ACTUAL { static_cast<float>(TraitCurrent(ENUM))
                            + (TraitBonusActualAsRatio(ENUM)
                               * static_cast<float>(TraitCurrent(ENUM))) };

        return (ACTUAL < 0.0f) ? 0 : static_cast<Trait_t>(ACTUAL);
    }

    Trait_t Creature::TraitBonusNormalAdj(const Traits::Enum ENUM, const Trait_t ADJ)
    {
        const auto NEW_NORMAL { bonusSet_.Get(ENUM).NormalAdj(ADJ) };
        ReCalculateTraitBonuses();
        return NEW_NORMAL;
    }

    bool Creature::TraitBonusTest(const Traits::Enum ENUM) const
    {
        return (misc::Random(100) < bonusSet_.GetCopy(ENUM).Current());
    }

    const TraitSet Creature::TraitsWorking() const
    {
        TraitSet set;
        for (EnumUnderlying_t i(0); i < Traits::Count; ++i)
        {
            const auto NEXT_TRAIT_ENUM { static_cast<Traits::Enum>(i) };
            set.Get(NEXT_TRAIT_ENUM).NormalSet(TraitNormal(NEXT_TRAIT_ENUM));
            set.Get(NEXT_TRAIT_ENUM).CurrentSet(TraitWorking(NEXT_TRAIT_ENUM));
        }

        return set;
    }

    const std::string
        Creature::TraitModifiedString(const Traits::Enum ENUM, const bool WILL_WRAP) const
    {
        const auto WORKING { TraitWorking(ENUM) };
        const auto NORMAL { TraitNormal(ENUM) };

        std::string result;
        result.reserve(16);

        if (WORKING != NORMAL)
        {
            if (WILL_WRAP)
            {
                result += '(';
            }

            if (WORKING > NORMAL)
            {
                result += '+';
            }

            result += std::to_string(WORKING - NORMAL);

            if (WILL_WRAP)
            {
                result += ')';
            }
        }

        return result;
    }

    void Creature::StatTraitsModify(const StatSet & STAT_SET)
    {
        TraitNormalAdj(Traits::Strength, STAT_SET.Str().GetAsTrait());
        TraitNormalAdj(Traits::Accuracy, STAT_SET.Acc().GetAsTrait());
        TraitNormalAdj(Traits::Charm, STAT_SET.Cha().GetAsTrait());
        TraitNormalAdj(Traits::Luck, STAT_SET.Lck().GetAsTrait());
        TraitNormalAdj(Traits::Speed, STAT_SET.Spd().GetAsTrait());
        TraitNormalAdj(Traits::Intelligence, STAT_SET.Int().GetAsTrait());
    }

    void Creature::BeforeSerialize() { inventory_.BeforeSerialize(); }

    void Creature::AfterSerialize() { inventory_.AfterSerialize(); }

    void Creature::AfterDeserialize()
    {
        inventory_.AfterDeserialize();
        SetHeldWeaponsToBest();
    }

    const item::ItemPVecVec_t Creature::ComposeWeaponsList() const
    {
        item::ItemPVecVec_t weaponsPVecVec;
        const auto EQUIPPED_ITEMS_PVEC { Inventory().ItemsEquipped() };

        // the vec of non-bodypart equipped weapons is always first
        item::ItemPVec_t ssNonBPWeaponsPVec;
        for (const auto & NEXT_ITEM_PTR : EQUIPPED_ITEMS_PVEC)
        {
            if ((NEXT_ITEM_PTR->IsWeapon()) && (NEXT_ITEM_PTR->IsBodyPart() == false))
            {
                ssNonBPWeaponsPVec.emplace_back(NEXT_ITEM_PTR);
            }
        }

        ssNonBPWeaponsPVec = item::Algorithms::FindByBroken(
            ssNonBPWeaponsPVec, item::Algorithms::BrokenOpt::Discard);

        if (ssNonBPWeaponsPVec.empty() == false)
        {
            weaponsPVecVec.emplace_back(ssNonBPWeaponsPVec);
        }

        //...if the creature is a spellcaster then wands are considered a separate weapon
        // set/vec
        if (CanCastSpells())
        {
            const item::ItemPVec_t NOT_BROKEN_WANDS_PVEC { item::Algorithms::FindByBroken(
                item::Algorithms::FindByMiscType(EQUIPPED_ITEMS_PVEC, item::Misc::Wand),
                item::Algorithms::BrokenOpt::Discard) };

            if (NOT_BROKEN_WANDS_PVEC.empty() == false)
            {
                weaponsPVecVec.emplace_back(NOT_BROKEN_WANDS_PVEC);
            }
        }

        // the equipped body part weapons that are not bite/claws are always second
        item::ItemPVec_t ssBPNonBCWeaponsPVec;
        for (const auto & NEXT_ITEM_PTR : EQUIPPED_ITEMS_PVEC)
        {
            if (NEXT_ITEM_PTR->IsWeapon())
            {
                const auto & WEAPON_INFO { NEXT_ITEM_PTR->WeaponInfo() };

                if (!WEAPON_INFO.IsMinor<item::BodyPartWeapons>(item::BodyPartWeapons::Bite)
                    && !WEAPON_INFO.IsMinor<item::BodyPartWeapons>(item::BodyPartWeapons::Claws))
                {
                    ssBPNonBCWeaponsPVec.emplace_back(NEXT_ITEM_PTR);
                }
            }
        }

        if (ssBPNonBCWeaponsPVec.empty() == false)
        {
            weaponsPVecVec.emplace_back(ssBPNonBCWeaponsPVec);
        }

        // the equipped body part weapons that ARE bite/claws always come after
        item::ItemPVec_t ssBPBCWeaponsPVec;
        for (const auto & NEXT_ITEM_PTR : EQUIPPED_ITEMS_PVEC)
        {
            if (NEXT_ITEM_PTR->IsWeapon())
            {
                const auto & WEAPON_INFO { NEXT_ITEM_PTR->WeaponInfo() };

                if (WEAPON_INFO.IsMinor<item::BodyPartWeapons>(item::BodyPartWeapons::Bite)
                    || WEAPON_INFO.IsMinor<item::BodyPartWeapons>(item::BodyPartWeapons::Claws))
                {
                    ssBPBCWeaponsPVec.emplace_back(NEXT_ITEM_PTR);
                }
            }
        }

        if (ssBPBCWeaponsPVec.empty() == false)
        {
            weaponsPVecVec.emplace_back(ssBPBCWeaponsPVec);
        }

        return weaponsPVecVec;
    }

    bool operator==(const Creature & L, const Creature & R)
    {
        // imageFilename_ can be random so don't include it in comparisons
        // imageFullPath_ changes between load/save so don't include it in comparisons

        // short-circuit on the most likely member to differ
        if (L.dateTimeCreated_ != R.dateTimeCreated_)
        {
            return false;
        }

        if (std::tie(
                L.name_,
                L.sex_,
                L.bodyType_,
                L.race_,
                L.role_,
                L.achievements_,
                L.lastSpellCastNum_,
                L.lastSongPlayedNum_,
                L.healthCurrent_,
                L.healthNormal_,
                L.rank_,
                L.experience_,
                L.actualSet_,
                L.bonusSet_)
            != std::tie(
                R.name_,
                R.sex_,
                R.bodyType_,
                R.race_,
                R.role_,
                R.achievements_,
                R.lastSpellCastNum_,
                R.lastSongPlayedNum_,
                R.healthCurrent_,
                R.healthNormal_,
                R.rank_,
                R.experience_,
                R.actualSet_,
                R.bonusSet_))
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

        if (misc::Vector::OrderlessCompareEqual(L.songsVec_, R.songsVec_) == false)
        {
            return false;
        }

        if (misc::Vector::OrderlessCompareEqual(L.enchantmentsPVec_, R.enchantmentsPVec_) == false)
        {
            return false;
        }

        return misc::Vector::OrderlessCompareEqual(L.spellsVec_, R.spellsVec_);
    }

    bool operator<(const Creature & L, const Creature & R)
    {
        // imageFilename_ can be random so don't include it in comparisons
        // imageFullPath_ changes between load/save so don't include it in comparisons

        // this function's compare is intentionally ordered to control the way creatures are
        // sorted in lists, so don't re-order these compares

        // short-circuit compare by name_ and dateTimeCreated_ first
        if (L.name_ != R.name_)
        {
            return L.name_ < R.name_;
        }

        if (std::tie(
                L.sex_,
                L.race_,
                L.role_,
                L.rank_,
                L.experience_,
                L.achievements_,
                L.lastSpellCastNum_,
                L.lastSongPlayedNum_,
                L.healthCurrent_,
                L.healthNormal_,
                L.actualSet_,
                L.bonusSet_,
                L.bodyType_,
                L.dateTimeCreated_)
            < std::tie(
                R.sex_,
                R.race_,
                R.role_,
                R.rank_,
                R.experience_,
                R.achievements_,
                R.lastSpellCastNum_,
                R.lastSongPlayedNum_,
                R.healthCurrent_,
                R.healthNormal_,
                R.actualSet_,
                R.bonusSet_,
                R.bodyType_,
                R.dateTimeCreated_))
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.titlesVec_, R.titlesVec_))
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.conditionsVec_, R.conditionsVec_))
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.songsVec_, R.songsVec_))
        {
            return true;
        }

        if (misc::Vector::OrderlessCompareLess(L.enchantmentsPVec_, R.enchantmentsPVec_))
        {
            return true;
        }

        return misc::Vector::OrderlessCompareLess(L.spellsVec_, R.spellsVec_);
    }

    void Creature::EnchantmentsApplyOrRemoveByType(
        const EnchantmentPVec_t & PVEC, const EnchantmentType::Enum TYPE, const bool WILL_APPLY)
    {
        for (const auto & NEXT_ENCHANTMENT_PTR : PVEC)
        {
            if (NEXT_ENCHANTMENT_PTR->Type() & TYPE)
            {
                EnchantmentApplyOrRemove(NEXT_ENCHANTMENT_PTR, WILL_APPLY);
            }
        }
    }

    bool Creature::HasEnchantmentType(const EnchantmentType::Enum TYPE_ENUM) const
    {
        for (const auto & NEXT_ENCHANTMENT_PTR : enchantmentsPVec_)
        {
            if (NEXT_ENCHANTMENT_PTR->Type() & TYPE_ENUM)
            {
                return true;
            }
        }

        return false;
    }
} // namespace creature
} // namespace heroespath
