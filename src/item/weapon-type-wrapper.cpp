// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// weapon-type-wrapper.cpp
//
#include "weapon-type-wrapper.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"
#include "misc/vector-map.hpp"

#include <exception>
#include <numeric>
#include <vector>

namespace heroespath
{
namespace item
{
    namespace weapon
    {

        const std::string WeaponTypeWrapper::DAGGER_NAME_ { "Dagger" };
        const std::string WeaponTypeWrapper::QUARTERSTAFF_NAME_ { "Quarterstaff" };
        const std::string WeaponTypeWrapper::BOW_GENERAL_NAME_ { "Bow" };

        WeaponTypeWrapper::WeaponTypeWrapper(const std::string & SYSTEM_NAME)
            : generalName_("")
            , specificName_("")
            , systemName_(SYSTEM_NAME)
            , readableName_("")
            , type_(weapon_type::None)
            , isDagger_(false)
            , variant_()
            , elementTypes_()
        {
            // This is the default constructor and it needs to support construction with all default
            // parameters that leave the object in an invalid state, so this if/return allows for
            // that.
            if (SYSTEM_NAME.empty())
            {
                return;
            }

            const auto SYSTEM_NAME_LOWERCASE { misc::ToLowerCopy(SYSTEM_NAME) };

            if (SYSTEM_NAME_LOWERCASE == misc::ToLowerCopy(QUARTERSTAFF_NAME_))
            {
                type_ = weapon_type::Staff;
                variant_ = true;
                SetNamesAndVerify("'name' constructor when name=Quarterstaff");
            }
            else if (
                SYSTEM_NAME_LOWERCASE == misc::ToLowerCopy(weapon_type::Name(weapon_type::Staff)))
            {
                type_ = weapon_type::Staff;
                variant_ = false;
                SetNamesAndVerify("'name' constructor when name=Staff");
            }
            else if (false == SetupWithKnifeOrDaggerName(SYSTEM_NAME_LOWERCASE))
            {
                if (false
                    == SetupWithSpecificTypeName<sword_type>(
                        SYSTEM_NAME_LOWERCASE, weapon_type::Sword))
                {
                    if (false
                        == SetupWithSpecificTypeName<axe_type>(
                            SYSTEM_NAME_LOWERCASE, weapon_type::Axe))
                    {
                        if (false
                            == SetupWithSpecificTypeName<club_type>(
                                SYSTEM_NAME_LOWERCASE, weapon_type::Club))
                        {
                            if (false
                                == SetupWithSpecificTypeName<whip_type>(
                                    SYSTEM_NAME_LOWERCASE, weapon_type::Whip))
                            {
                                if (false
                                    == SetupWithSpecificTypeName<body_part>(
                                        SYSTEM_NAME_LOWERCASE, weapon_type::BodyPart))
                                {
                                    if (false
                                        == SetupWithSpecificTypeName<projectile_type>(
                                            SYSTEM_NAME_LOWERCASE, weapon_type::Projectile))
                                    {
                                        SetupWithSpecificTypeName<bladedstaff_type>(
                                            SYSTEM_NAME_LOWERCASE, weapon_type::BladedStaff);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            std::ostringstream errorSS;
            errorSS << "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(name=\"" << SYSTEM_NAME
                    << "\") ";

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (IsValidCompleteCheck()),
                errorSS.str() << " after this 'name' constructor the object was invalid: "
                              << ToString());
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const KnifeOrDagger, const bool IS_DAGGER)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Knife)
            , isDagger_(IS_DAGGER)
            , variant_()
            , elementTypes_()
        {
            SetNamesAndVerify("after knife/dagger constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const StaffOrQuarterstaff, const bool IS_QUARTERSTAFF)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Staff)
            , isDagger_(false)
            , variant_(IS_QUARTERSTAFF)
            , elementTypes_()
        {
            SetNamesAndVerify("after is_quarterstaff constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const item::body_part::Enum BODY_PART)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::BodyPart)
            , isDagger_(false)
            , variant_(BODY_PART)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                ((BODY_PART != body_part::Count) && (BODY_PART != body_part::Skin)),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(body_part="
                    << ((body_part::Count == BODY_PART) ? "Count" : body_part::ToString(BODY_PART))
                    << ") -that body_part is invalid.");

            SetNamesAndVerify("after body_part constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const sword_type::Enum SWORD_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Sword)
            , isDagger_(false)
            , variant_(SWORD_TYPE)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SWORD_TYPE != sword_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(sword_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after sword_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const axe_type::Enum AXE_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Axe)
            , isDagger_(false)
            , variant_(AXE_TYPE)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (AXE_TYPE != axe_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(axe_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after axe_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const club_type::Enum CLUB_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Club)
            , isDagger_(false)
            , variant_(CLUB_TYPE)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (CLUB_TYPE != club_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(club_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after club_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const whip_type::Enum WHIP_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Whip)
            , isDagger_(false)
            , variant_(WHIP_TYPE)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (WHIP_TYPE != whip_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(whip_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after whip_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const projectile_type::Enum PROJ_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::Projectile)
            , isDagger_(false)
            , variant_(PROJ_TYPE)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (PROJ_TYPE != projectile_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(projectile_type=Count) -Count "
                "is invalid.");

            SetNamesAndVerify("after projectile_type constructor");
        }

        WeaponTypeWrapper::WeaponTypeWrapper(const bladedstaff_type::Enum BSTAFF_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(weapon_type::BladedStaff)
            , isDagger_(false)
            , variant_(BSTAFF_TYPE)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (BSTAFF_TYPE != bladedstaff_type::Count),
                "item::weapon::WeaponTypeWrapper::WeaponTypeWrapper(bladedstaff_type=Count) -Count "
                "is invalid.");

            SetNamesAndVerify("after bladedstaff_type constructor");
        }

        const std::string WeaponTypeWrapper::DetailsKeyName() const
        {
            return boost::algorithm::replace_all_copy(specificName_, " ", "");
        }

        const std::string WeaponTypeWrapper::ToString() const
        {
            if (*this == WeaponTypeWrapper())
            {
                return "(empty)";
            }

            misc::VectorMap<std::string, std::string> namesMap;
            namesMap[boost::algorithm::erase_all_copy(generalName_, " ")] += "general\\";
            namesMap[boost::algorithm::erase_all_copy(specificName_, " ")] += "specific\\";
            namesMap[boost::algorithm::erase_all_copy(systemName_, " ")] += "system\\";
            namesMap[boost::algorithm::erase_all_copy(readableName_, " ")] += "readable\\";

            std::ostringstream ss;
            for (const auto & VALUE_NAME_PAIR : namesMap)
            {
                ss << "\"" << VALUE_NAME_PAIR.first << "\"=" << VALUE_NAME_PAIR.second << ", ";
            }

            ss << "type=" << weapon_type::ToString(type_, EnumStringHow(Wrap::Yes))
               << std::boolalpha << ", " << ((IsStaff()) ? "Staff," : "")
               << ((IsQuarterstaff()) ? "Quarterstaff," : "")
               << ((IsBodyPart()) ? (body_part::ToString(BodyPartType()) + ",") : "")
               << ((IsBite()) ? "Bite," : "") << ((IsFists()) ? "Fists," : "")
               << ((IsKnife()) ? "Knife," : "") << ((IsDagger()) ? "Dagger," : "")
               << ((IsSword()) ? sword_type::ToString(SwordType()) + "," : "")
               << ((IsAxe()) ? axe_type::ToString(AxeType()) + "," : "")
               << ((IsClub()) ? club_type::ToString(ClubType()) + "," : "")
               << ((IsWhip()) ? whip_type::ToString(WhipType()) + "," : "")
               << ((IsProjectile()) ? projectile_type::ToString(ProjectileType()) + "," : "")
               << ((IsBladedStaff()) ? bladedstaff_type::ToString(BladedStaffType()) + "," : "")
               << " variant_=" << variant_.which() << ", ";

            if ((elementTypes_.size() != 1) || (elementTypes_.at(0) != element_type::None))
            {
                ss << ", element_types={";

                if (elementTypes_.empty())
                {
                    ss << "empty/invalid";
                }
                else
                {
                    for (std::size_t i(0); i < elementTypes_.size(); ++i)
                    {
                        if (i > 0)
                        {
                            ss << ",";
                        }

                        const auto ELEMENT_TYPE { elementTypes_.at(i) };

                        ss << element_type::ToString(
                            ELEMENT_TYPE, EnumStringHow(Wrap::Yes, "&", NoneEmpty::No));
                    }
                }

                ss << "}";
            }

            return ss.str();
        }

        const std::string WeaponTypeWrapper::ImageFilename(
            const std::string & SEPARATOR, const std::string & EXTENSION) const
        {
            std::ostringstream ss;

            // remove any spaces from the generalName_
            const auto GENERAL_NAME_FILENAME_VERSION { boost::algorithm::replace_all_copy(
                generalName_, " ", "") };

            ss << GENERAL_NAME_FILENAME_VERSION;

            if ((systemName_ != GENERAL_NAME_FILENAME_VERSION) && (systemName_ != "Bite"))
            {
                ss << SEPARATOR << systemName_;
            }

            ss << EXTENSION;
            return misc::ToLowerCopy(ss.str());
        }

        bool WeaponTypeWrapper::IsGeneralNameAlmostSpecificName() const
        {
            namespace ba = boost::algorithm;

            return (
                misc::ToLowerCopy(ba::replace_all_copy(generalName_, " ", ""))
                == misc::ToLowerCopy(specificName_));
        }

        const std::vector<WeaponTypeWrapper> WeaponTypeWrapper::MakeCompleteSet()
        {
            std::vector<WeaponTypeWrapper> wrappers;

            wrappers.emplace_back(WeaponTypeWrapper(StaffOrQuarterstaff(), false));
            wrappers.emplace_back(WeaponTypeWrapper(StaffOrQuarterstaff(), true));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Bite));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Claws));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Fists));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Tentacles));
            wrappers.emplace_back(WeaponTypeWrapper(body_part::Breath));
            wrappers.emplace_back(WeaponTypeWrapper(KnifeOrDagger(), false));
            wrappers.emplace_back(WeaponTypeWrapper(KnifeOrDagger(), true));
            AppendSpecificSet<sword_type>(wrappers);
            AppendSpecificSet<axe_type>(wrappers);
            AppendSpecificSet<club_type>(wrappers);
            AppendSpecificSet<whip_type>(wrappers);
            AppendSpecificSet<projectile_type>(wrappers);
            AppendSpecificSet<bladedstaff_type>(wrappers);

            return wrappers;
        }

        name_material_type::Enum WeaponTypeWrapper::NameMaterialType() const
        {
            if (IsBodyPart())
            {
                return name_material_type::BodyPart;
            }
            else if (
                IsBlowpipe() || (IsProjectile() && (ProjectileType() != projectile_type::Crossbow))
                || (WhipType() == whip_type::Bullwhip))
            {
                return name_material_type::Decoration;
            }
            else if (
                (type_ & weapon_type::Staff) || (ProjectileType() == projectile_type::Crossbow))
            {
                return name_material_type::Reinforced;
            }
            else if (IsSpear())
            {
                return name_material_type::Tipped;
            }
            else if (
                IsSword() || IsAxe() || IsClub() || (type_ & weapon_type::Knife) || IsWhip()
                || (IsBladedStaff() && (type_ & weapon_type::Bladed)))
            {
                return name_material_type::Handle;
            }
            else
            {
                // should never get here if valid
                return name_material_type::Count;
            }
        }

        bool
            WeaponTypeWrapper::SetupWithKnifeOrDaggerName(const std::string & SYSTEM_NAME_LOWERCASE)
        {
            const auto KNIFE_NAME { weapon_type::ToString(weapon_type::Knife) };

            const auto IS_KNIFE { SYSTEM_NAME_LOWERCASE == misc::ToLowerCopy(KNIFE_NAME) };

            const auto IS_DAGGER { SYSTEM_NAME_LOWERCASE == misc::ToLowerCopy(DAGGER_NAME_) };

            if (IS_KNIFE || IS_DAGGER)
            {
                type_ = weapon_type::Knife;
                isDagger_ = IS_DAGGER;
                SetNamesAndVerify("'name' constructor when name=Knife/Dagger");
                return true;
            }
            else
            {
                return false;
            }
        }

        void WeaponTypeWrapper::SetNamesAndVerify(const std::string & CALLER_CONTEXT_DESCRIPTION)
        {
            elementTypes_.clear();

            switch (type_)
            {
                case weapon_type::BodyPart:
                {
                    const auto BODY_PART_TYPE { BodyPartType() };
                    generalName_ = body_part::ToString(BODY_PART_TYPE);
                    specificName_ = generalName_;
                    systemName_ = generalName_;
                    readableName_ = generalName_;
                    type_ = static_cast<weapon_type::Enum>(type_ | weapon_type::Melee);
                    break;
                }

                case weapon_type::Staff:
                {
                    if (boost::get<bool>(variant_))
                    {
                        generalName_ = QUARTERSTAFF_NAME_;
                        specificName_ = QUARTERSTAFF_NAME_;
                        systemName_ = QUARTERSTAFF_NAME_;
                        readableName_ = QUARTERSTAFF_NAME_;
                        elementTypes_ = { element_type::Frost, element_type::Honor };
                    }
                    else
                    {
                        generalName_ = weapon_type::Name(weapon_type::Staff);
                        specificName_ = generalName_;
                        systemName_ = generalName_;
                        readableName_ = generalName_;
                        elementTypes_ = { element_type::Shadow, element_type::Honor };
                    }

                    type_ = static_cast<weapon_type::Enum>(
                        type_ | weapon_type::Staff | weapon_type::Melee);

                    break;
                }

                case weapon_type::Sword:
                {
                    const auto SWORD_TYPE { SwordType() };

                    generalName_ = weapon_type::Name(type_);
                    specificName_ = sword_type::Name(SWORD_TYPE);
                    systemName_ = sword_type::ToString(SWORD_TYPE);

                    if ((SWORD_TYPE == sword_type::Broadsword)
                        || (SWORD_TYPE == sword_type::Longsword)
                        || (SWORD_TYPE == sword_type::Shortsword))
                    {
                        readableName_ = specificName_;
                    }
                    else
                    {
                        readableName_ = specificName_ + " " + generalName_;
                    }

                    switch (SWORD_TYPE)
                    {
                        case sword_type::Gladius:
                        {
                            elementTypes_ = { element_type::Honor };
                            break;
                        }
                        case sword_type::Cutlass:
                        {
                            elementTypes_ = { element_type::Honor };
                            break;
                        }
                        case sword_type::Falcata:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case sword_type::Rapier:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Frost | element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case sword_type::Saber:
                        {
                            elementTypes_ = { element_type::Shadow };
                            break;
                        }
                        case sword_type::Broadsword:
                        {
                            elementTypes_ = { element_type::Honor };
                            break;
                        }
                        case sword_type::Longsword:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Frost | element_type::Fire);

                            break;
                        }
                        case sword_type::Knightlysword:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Frost | element_type::Honor);

                            break;
                        }
                        case sword_type::Flamberg:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case sword_type::Claymore:
                        {
                            elementTypes_ = element_type::ValidCombinations();
                            break;
                        }
                        case sword_type::Shortsword:
                        case sword_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    type_ = static_cast<weapon_type::Enum>(
                        type_ | weapon_type::Melee | weapon_type::Sword | weapon_type::Bladed
                        | weapon_type::Pointed);

                    break;
                }

                case weapon_type::Axe:
                {
                    const auto AXE_TYPE { AxeType() };

                    generalName_ = weapon_type::Name(type_);

                    // you would think this should be axe_type::Name() called here, but the names
                    // are so distinct that we use ToString() instead, intentionally.
                    specificName_ = axe_type::ToString(AXE_TYPE);
                    systemName_ = specificName_;

                    if (AXE_TYPE == axe_type::Sickle)
                    {
                        readableName_ = specificName_;
                    }
                    else
                    {
                        readableName_ = specificName_ + " " + generalName_;
                    }

                    switch (AXE_TYPE)
                    {
                        case axe_type::Sickle:
                        {
                            elementTypes_ = { element_type::Shadow };
                            break;
                        }
                        case axe_type::Battleaxe:
                        {
                            elementTypes_ = { element_type::Fire, element_type::Frost };
                            break;
                        }
                        case axe_type::Waraxe:
                        {
                            elementTypes_ = { element_type::Honor, element_type::Shadow };
                            break;
                        }
                        case axe_type::Handaxe:
                        case axe_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    type_ = static_cast<weapon_type::Enum>(
                        type_ | weapon_type::Bladed | weapon_type::Axe | weapon_type::Melee);

                    break;
                }

                case weapon_type::Whip:
                {
                    const auto WHIP_TYPE { WhipType() };
                    generalName_ = weapon_type::Name(type_);
                    specificName_ = whip_type::Name(WHIP_TYPE);
                    systemName_ = whip_type::ToString(WHIP_TYPE);
                    readableName_ = specificName_;

                    switch (WHIP_TYPE)
                    {
                        case whip_type::Flail:
                        {
                            elementTypes_ = { element_type::Honor, element_type::Shadow };
                            break;
                        }
                        case whip_type::MaceAndChain:
                        {
                            elementTypes_ = { element_type::Fire, element_type::Frost };
                            break;
                        }
                        case whip_type::Bullwhip:
                        case whip_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    type_ = static_cast<weapon_type::Enum>(
                        type_ | weapon_type::Melee | weapon_type::Whip);

                    break;
                }

                case weapon_type::Club:
                {
                    const auto CLUB_TYPE { ClubType() };

                    generalName_ = weapon_type::Name(type_);
                    specificName_ = club_type::ToString(ClubType());
                    systemName_ = specificName_;

                    if (CLUB_TYPE == club_type::Spiked)
                    {
                        readableName_ = specificName_ + " " + generalName_;
                    }
                    else
                    {
                        readableName_ = specificName_;
                    }

                    switch (CLUB_TYPE)
                    {
                        case club_type::Spiked:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case club_type::Mace:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Frost | element_type::Honor);

                            break;
                        }
                        case club_type::Warhammer:
                        {
                            elementTypes_ = element_type::ValidCombinations();
                            break;
                        }
                        case club_type::Maul:
                        case club_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    type_ = static_cast<weapon_type::Enum>(
                        type_ | weapon_type::Melee | weapon_type::Club);

                    break;
                }

                case weapon_type::Projectile:
                {
                    const auto PROJECTILE_TYPE { ProjectileType() };

                    switch (PROJECTILE_TYPE)
                    {
                        case projectile_type::Blowpipe:
                        {
                            generalName_ = projectile_type::Name(PROJECTILE_TYPE);
                            break;
                        }
                        case projectile_type::Sling:
                        {
                            generalName_ = projectile_type::Name(PROJECTILE_TYPE);
                            break;
                        }
                        case projectile_type::Shortbow:
                        {
                            generalName_ = BOW_GENERAL_NAME_;
                            break;
                        }
                        case projectile_type::Longbow:
                        {
                            generalName_ = BOW_GENERAL_NAME_;
                            elementTypes_ = { element_type::Fire, element_type::Frost };
                            break;
                        }
                        case projectile_type::Crossbow:
                        {
                            generalName_ = projectile_type::Name(PROJECTILE_TYPE);
                            elementTypes_ = { element_type::Honor, element_type::Shadow };
                            break;
                        }
                        case projectile_type::CompositeBow:
                        {
                            generalName_ = BOW_GENERAL_NAME_;
                            elementTypes_ = element_type::ValidCombinations();
                            break;
                        }
                        case projectile_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    specificName_ = projectile_type::Name(PROJECTILE_TYPE);
                    systemName_ = projectile_type::ToString(PROJECTILE_TYPE);
                    readableName_ = specificName_;

                    type_ = static_cast<weapon_type::Enum>(type_ | weapon_type::Projectile);

                    break;
                }

                case weapon_type::Knife:
                {
                    if (isDagger_)
                    {
                        generalName_ = DAGGER_NAME_;
                        specificName_ = DAGGER_NAME_;
                        systemName_ = DAGGER_NAME_;
                        readableName_ = DAGGER_NAME_;

                        elementTypes_ = element_type::ValidCombinations(
                            element_type::Fire | element_type::Frost | element_type::Honor);
                    }
                    else
                    {
                        generalName_ = weapon_type::Name(type_);
                        specificName_ = generalName_;
                        systemName_ = weapon_type::ToString(type_);
                        readableName_ = generalName_;

                        elementTypes_ = element_type::ValidCombinations(
                            element_type::Fire | element_type::Frost | element_type::Shadow);
                    }

                    type_ = static_cast<weapon_type::Enum>(
                        type_ | weapon_type::Bladed | weapon_type::Knife | weapon_type::Melee
                        | weapon_type::Pointed);

                    break;
                }

                case weapon_type::BladedStaff:
                {
                    const auto BLADEDSTAFF_TYPE { BladedStaffType() };
                    generalName_ = weapon_type::Name(weapon_type::BladedStaff);
                    specificName_ = bladedstaff_type::Name(BLADEDSTAFF_TYPE);
                    systemName_ = bladedstaff_type::ToString(BLADEDSTAFF_TYPE);

                    // skip prepending the general name because most players will know these items
                    // by their specfificName_
                    readableName_ = specificName_;

                    switch (BLADEDSTAFF_TYPE)
                    {
                        case bladedstaff_type::Scythe:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case bladedstaff_type::Pike:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Frost | element_type::Honor);

                            break;
                        }
                        case bladedstaff_type::Partisan:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Fire | element_type::Honor);

                            break;
                        }
                        case bladedstaff_type::Halberd:
                        {
                            elementTypes_ = element_type::ValidCombinations(
                                element_type::Fire | element_type::Frost | element_type::Honor);

                            break;
                        }
                        case bladedstaff_type::Spear:
                        case bladedstaff_type::ShortSpear:
                        case bladedstaff_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    const auto POINTED_TYPE { (
                        (BLADEDSTAFF_TYPE == bladedstaff_type::Scythe) ? weapon_type::None
                                                                       : weapon_type::Pointed) };

                    const auto BLADED_TYPE { (
                        ((BLADEDSTAFF_TYPE == bladedstaff_type::Spear)
                         || (BLADEDSTAFF_TYPE == bladedstaff_type::ShortSpear))
                            ? weapon_type::None
                            : weapon_type::Bladed) };

                    type_ = static_cast<weapon_type::Enum>(
                        type_ | weapon_type::Melee | weapon_type::BladedStaff | POINTED_TYPE
                        | BLADED_TYPE);

                    break;
                }

                case weapon_type::None:
                case weapon_type::Melee:
                case weapon_type::Bladed:
                case weapon_type::Pointed:
                default:
                {
                    break;
                }
            }

            elementTypes_.emplace_back(element_type::None);

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (IsValidCompleteCheck()),
                "item::weapon::WeaponTypeWrapper::SetNamesAndVerify("
                    << CALLER_CONTEXT_DESCRIPTION
                    << ") found the object was invalid when finished:  " << ToString());
        }

        bool WeaponTypeWrapper::IsValidCompleteCheck() const
        {
            if (elementTypes_.empty())
            {
                return false;
            }

            if (generalName_.empty() || specificName_.empty() || systemName_.empty()
                || readableName_.empty())
            {
                return false;
            }

            if (weapon_type::None == type_)
            {
                return false;
            }

            if (type_ & weapon_type::BodyPart)
            {
                return (variant_.which() == BODY_PART_INDEX)
                    && (body_part::Count != boost::get<body_part::Enum>(variant_))
                    && (body_part::Skin != boost::get<body_part::Enum>(variant_));
            }
            else if (type_ & weapon_type::Sword)
            {
                return (variant_.which() == SWORD_INDEX)
                    && (sword_type::Count != boost::get<sword_type::Enum>(variant_));
            }
            else if (type_ & weapon_type::Axe)
            {
                return (variant_.which() == AXE_INDEX)
                    && (axe_type::Count != boost::get<axe_type::Enum>(variant_));
            }
            else if (type_ & weapon_type::Whip)
            {
                return (variant_.which() == WHIP_INDEX)
                    && (whip_type::Count != boost::get<whip_type::Enum>(variant_));
            }
            else if (type_ & weapon_type::Club)
            {
                return (variant_.which() == CLUB_INDEX)
                    && (club_type::Count != boost::get<club_type::Enum>(variant_));
            }
            else if (type_ & weapon_type::Projectile)
            {
                return (variant_.which() == PROJECTILE_INDEX)
                    && (projectile_type::Count != boost::get<projectile_type::Enum>(variant_));
            }
            else if (type_ & weapon_type::BladedStaff)
            {
                return (variant_.which() == BLADEDSTAFF_INDEX)
                    && (bladedstaff_type::Count != boost::get<bladedstaff_type::Enum>(variant_));
            }
            else if (type_ & weapon_type::Knife)
            {
                return true;
            }
            else if (type_ & weapon_type::Staff)
            {
                return (variant_.which() == QUARTERSTAFF_INDEX);
            }
            else
            {
                return false;
            }
        }

    } // namespace weapon
} // namespace item
} // namespace heroespath
