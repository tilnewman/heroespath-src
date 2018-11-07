// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// armor-type-wrapper.cpp
//
#include "armor-type-wrapper.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/vector-map.hpp"

#include <exception>
#include <numeric>
#include <sstream>

namespace heroespath
{
namespace item
{
    namespace armor
    {

        const std::string ArmorTypeWrapper::GLOVES_NAME_ { "Gloves" };

        ArmorTypeWrapper::ArmorTypeWrapper(
            const std::string & SYSTEM_NAME,
            const base_type::Enum BASE_TYPE,
            const bool WILL_FORCE_PLAIN_BASE_TYPE_IF_REQUIRED)
            : generalName_("")
            , specificName_("")
            , systemName_(SYSTEM_NAME)
            , readableName_("")
            , type_(armor_type::Not)
            , base_(BASE_TYPE)
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

            if (false
                == SetupWithSpecificTypeName<cover_type>(
                    SYSTEM_NAME, armor_type::Covering, BASE_TYPE))
            {
                if (false
                    == SetupWithSpecificTypeName<helm_type>(
                        SYSTEM_NAME, armor_type::Helm, BASE_TYPE))
                {
                    if (false
                        == SetupWithSpecificTypeName<shield_type>(
                            SYSTEM_NAME, armor_type::Shield, BASE_TYPE))
                    {
                        if (SYSTEM_NAME == GLOVES_NAME_)
                        {
                            type_ = armor_type::Gauntlets;
                            base_ = BASE_TYPE;
                            SetNamesAndVerify("after 'name' constructor where name was gloves");
                        }
                        else
                        {
                            for (misc::EnumUnderlying_t i(0); i <= armor_type::Skin; ++i)
                            {
                                const auto ARMOR_TYPE_ENUM { static_cast<armor_type::Enum>(i) };
                                if (armor_type::ToString(ARMOR_TYPE_ENUM) == SYSTEM_NAME)
                                {
                                    if (WILL_FORCE_PLAIN_BASE_TYPE_IF_REQUIRED
                                        && armor_type::DoesRequireBaseType(ARMOR_TYPE_ENUM)
                                        && (base_type::Count == BASE_TYPE))
                                    {
                                        base_ = base_type::Plain;
                                    }
                                    else
                                    {
                                        base_ = BASE_TYPE;
                                    }

                                    type_ = ARMOR_TYPE_ENUM;

                                    SetNamesAndVerify(
                                        "after 'name' constructor where armor_type was used");

                                    break;
                                }
                            }
                        }
                    }
                }
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (IsValidCompleteCheck()),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(name="
                    << SYSTEM_NAME << ", orig_base_type="
                    << ((BASE_TYPE == base_type::Count) ? "Count" : base_type::ToString(BASE_TYPE))
                    << ", final_base_type = "
                    << ((base_ == base_type::Count) ? "Count" : base_type::ToString(base_))
                    << ") but IsValidCompleteCheck()=false at first step check: " << ToString());
        }

        ArmorTypeWrapper::ArmorTypeWrapper(
            const armor_type::Enum ARMOR_TYPE, const armor::base_type::Enum BASE_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(ARMOR_TYPE)
            , base_(BASE_TYPE)
            , variant_()
            , elementTypes_()
        {
            SetNamesAndVerify("after armor_type/base_type constructor");
        }

        ArmorTypeWrapper::ArmorTypeWrapper(const body_part::Enum BODY_PART)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(armor_type::Skin)
            , base_(base_type::Count)
            , variant_()
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (BODY_PART == body_part::Skin),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(body_part="
                    << ((BODY_PART == body_part::Count) ? "Count" : body_part::ToString(BODY_PART))
                    << ") that body_part is not valid.");

            SetNamesAndVerify("after body_part constructor");
        }

        ArmorTypeWrapper::ArmorTypeWrapper(const cover_type::Enum COVER_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(armor_type::Covering)
            , base_(base_type::Count)
            , variant_(COVER_TYPE)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (COVER_TYPE != cover_type::Count),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(cover_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after cover_type constructor");
        }

        ArmorTypeWrapper::ArmorTypeWrapper(const helm_type::Enum HELM_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(armor_type::Helm)
            , base_(base_type::Count)
            , variant_(HELM_TYPE)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (HELM_TYPE != helm_type::Count),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(helm_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after helm_type constructor");
        }

        ArmorTypeWrapper::ArmorTypeWrapper(const shield_type::Enum SHIELD_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(armor_type::Shield)
            , base_(base_type::Count)
            , variant_(SHIELD_TYPE)
            , elementTypes_()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (SHIELD_TYPE != shield_type::Count),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(shield_type=Count) -Count is "
                "invalid.");

            SetNamesAndVerify("after shield_type constructor");
        }

        const std::string ArmorTypeWrapper::DetailsKeyName() const
        {
            const auto SPECIFIC_NAME_WITHOUT_SPACES { boost::algorithm::replace_all_copy(
                specificName_, " ", "") };

            if (armor_type::DoesRequireBaseType(type_))
            {
                return base_type::ToString(BaseType()) + SPECIFIC_NAME_WITHOUT_SPACES;
            }
            else
            {
                return SPECIFIC_NAME_WITHOUT_SPACES;
            }
        }

        const std::string ArmorTypeWrapper::ToString() const
        {
            if (*this == ArmorTypeWrapper())
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

            ss << "type=" << armor_type::ToString(type_)
               << ", base=" << ((base_type::Count == base_) ? "Count" : base_type::ToString(base_))
               << ", variant_.which()=" << variant_.which() << ", type=("
               << ((IsShield()) ? shield_type::ToString(ShieldType()) : "") << ","
               << ((IsHelm()) ? helm_type::ToString(HelmType()) : "") << ","
               << ((IsGauntlets()) ? "gauntlets" : "") << "," << ((IsPants()) ? "pants" : "") << ","
               << ((IsBoots()) ? "boots" : "") << "," << ((IsShirt()) ? "shirt" : "") << ","
               << ((IsBracers()) ? "bracers" : "") << "," << ((IsAventail()) ? "aventail" : "")
               << "," << ((IsCover()) ? cover_type::ToString(CoverType()) : "") << ","
               << ((IsSkin()) ? "skin" : "") << ")"
               << ")";

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
                            ELEMENT_TYPE,
                            misc::EnumStringHow(misc::Wrap::Yes, "&", misc::NoneEmpty::No));
                    }
                }

                ss << "}";
            }

            return ss.str();
        }

        const std::string ArmorTypeWrapper::ImageFilename(
            const std::string & SEPARATOR, const std::string & EXTENSION) const
        {
            std::ostringstream ss;

            if (IsGauntlets() && (base_type::Plain == base_))
            {
                ss << armor_type::ToString(armor_type::Gauntlets);
            }
            else
            {
                // remove any spaces from the generalName_
                const auto GENERAL_NAME_FILENAME_VERSION { boost::algorithm::replace_all_copy(
                    generalName_, " ", "") };

                ss << GENERAL_NAME_FILENAME_VERSION;

                if (systemName_ != GENERAL_NAME_FILENAME_VERSION)
                {
                    ss << SEPARATOR << systemName_;
                }
            }

            if (base_type::Count != base_)
            {
                ss << SEPARATOR << base_type::ToString(base_);
            }

            ss << EXTENSION;
            return boost::algorithm::to_lower_copy(ss.str());
        }

        bool ArmorTypeWrapper::IsGeneralNameAlmostSpecificName() const
        {
            namespace ba = boost::algorithm;

            return (
                ba::to_lower_copy(ba::replace_all_copy(generalName_, " ", ""))
                == ba::to_lower_copy(specificName_));
        }

        const std::vector<ArmorTypeWrapper> ArmorTypeWrapper::MakeCompleteSet()
        {
            std::vector<ArmorTypeWrapper> wrappers;
            wrappers.emplace_back(ArmorTypeWrapper(body_part::Skin));
            MakeSpecificSetWithoutBaseType<shield_type>(wrappers);
            MakeSpecificSetWithoutBaseType<helm_type>(wrappers);
            MakeSpecificSetWithoutBaseType<cover_type>(wrappers);
            MakeSpecificSetsWithBaseType(wrappers);
            return wrappers;
        }

        name_material_type::Enum ArmorTypeWrapper::NameMaterialType() const
        {
            if (IsSkin())
            {
                return name_material_type::BodyPart;
            }
            else if (IsCover())
            {
                return name_material_type::Claspped;
            }
            else if (IsShield() || IsHelm())
            {
                return name_material_type::Reinforced;
            }
            else if (IsGauntlets())
            {
                return name_material_type::ReinforcedWithBase;
            }
            else if (IsBoots() || IsShirt() || IsPants() || IsBracers() || IsAventail())
            {
                return name_material_type::ClasppedWithBase;
            }
            else
            {
                // should never get here if valid
                return name_material_type::Count;
            }
        }

        void
            ArmorTypeWrapper::MakeSpecificSetsWithBaseType(std::vector<ArmorTypeWrapper> & wrappers)
        {
            for (misc::EnumUnderlying_t a(0); a < armor_type::Count; ++a)
            {
                const auto ARMOR_ENUM { static_cast<armor_type::Enum>(a) };

                if (armor_type::DoesRequireBaseType(ARMOR_ENUM))
                {
                    for (misc::EnumUnderlying_t b(0); b < base_type::Count; ++b)
                    {
                        const auto BASE_ENUM { static_cast<base_type::Enum>(b) };
                        wrappers.emplace_back(ArmorTypeWrapper(ARMOR_ENUM, BASE_ENUM));
                    }
                }
            }
        }

        void ArmorTypeWrapper::SetNamesAndVerify(const std::string & CALLER_CONTEXT_DESCRIPTION)
        {
            SetupElementTypes();

            switch (type_)
            {
                case armor_type::Gauntlets:
                case armor_type::Pants:
                case armor_type::Boots:
                case armor_type::Shirt:
                case armor_type::Bracers:
                case armor_type::Aventail:
                {
                    const auto IS_GLOVES { IsGauntlets() && (base_type::Plain == base_) };

                    generalName_ = ((IS_GLOVES) ? GLOVES_NAME_ : armor_type::ToString(type_));
                    specificName_ = generalName_;
                    systemName_ = generalName_;

                    if ((base_type::Plain == base_) || (base_type::Count == base_))
                    {
                        readableName_ = generalName_;
                    }
                    else
                    {
                        readableName_ = base_type::ToString(base_) + " " + generalName_;
                    }

                    break;
                }

                case armor_type::Shield:
                {
                    const auto SHIELD_TYPE { ShieldType() };
                    generalName_ = armor_type::ToString(type_);
                    specificName_ = shield_type::ToString(SHIELD_TYPE);
                    systemName_ = specificName_;
                    readableName_ = specificName_ + " " + generalName_;
                    break;
                }
                case armor_type::Helm:
                {
                    const auto HELM_TYPE { HelmType() };
                    generalName_ = armor_type::ToString(type_);
                    specificName_ = helm_type::Name(HELM_TYPE);
                    systemName_ = helm_type::ToString(HELM_TYPE);

                    if (HELM_TYPE == helm_type::MailCoif)
                    {
                        readableName_ = specificName_;
                    }
                    else
                    {
                        readableName_ = specificName_ + " " + generalName_;
                    }

                    break;
                }
                case armor_type::Covering:
                {
                    const auto COVER_TYPE { CoverType() };
                    generalName_ = cover_type::ToString(COVER_TYPE);
                    specificName_ = generalName_;
                    systemName_ = generalName_;
                    readableName_ = generalName_;
                    break;
                }
                case armor_type::Skin:
                {
                    generalName_ = armor_type::ToString(type_);
                    specificName_ = generalName_;
                    systemName_ = generalName_;
                    readableName_ = generalName_;
                    break;
                }

                case armor_type::Not:
                case armor_type::Count:
                default:
                {
                    break;
                }
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (IsValidCompleteCheck()),
                "item::armor::ArmorTypeWrapper::SetNamesAndVerify("
                    << CALLER_CONTEXT_DESCRIPTION
                    << ") found the object was invalid when finished:  " << ToString());
        }

        bool ArmorTypeWrapper::IsValidCompleteCheck() const
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

            switch (type_)
            {
                case armor_type::Gauntlets:
                {
                    if (base_type::Plain == base_)
                    {
                        return (GLOVES_NAME_ == specificName_);
                    }
                    else
                    {
                        return (base_type::Count != base_);
                    }
                }

                case armor_type::Aventail:
                case armor_type::Bracers:
                case armor_type::Shirt:
                case armor_type::Boots:
                case armor_type::Pants:
                {
                    return (base_type::Count != base_);
                }

                case armor_type::Shield:
                {
                    return (
                        (variant_.which() == SHIELD_INDEX_)
                        && (boost::get<shield_type::Enum>(variant_) != shield_type::Count));
                }

                case armor_type::Helm:
                {
                    return (
                        (variant_.which() == HELM_INDEX_)
                        && (boost::get<helm_type::Enum>(variant_) != helm_type::Count));
                }

                case armor_type::Covering:
                {
                    return (
                        (variant_.which() == COVER_INDEX_)
                        && (boost::get<cover_type::Enum>(variant_) != cover_type::Count));
                }

                case armor_type::Skin:
                {
                    return true;
                }

                case armor_type::Not:
                case armor_type::Count:
                default:
                {
                    return false;
                }
            }
        }

        void ArmorTypeWrapper::SetupElementTypes()
        {
            elementTypes_ = MakeElementTypesWithGivenBaseType(base_);
        }

        const ElementEnumVec_t ArmorTypeWrapper::MakeElementTypesWithGivenBaseType(
            const base_type::Enum BASE_TYPE) const
        {
            ElementEnumVec_t elementTypes;

            switch (type_)
            {
                case armor_type::Bracers:
                case armor_type::Boots:
                case armor_type::Aventail:
                {
                    if ((base_type::Count != BASE_TYPE) && (base_type::Plain != BASE_TYPE))
                    {
                        elementTypes = { element_type::Fire, element_type::Frost };
                    }

                    break;
                }
                case armor_type::Gauntlets:
                case armor_type::Pants:
                case armor_type::Shirt:
                {
                    if ((base_type::Count != BASE_TYPE) && (base_type::Plain != BASE_TYPE))
                    {
                        elementTypes = { element_type::Honor, element_type::Shadow };
                    }

                    break;
                }
                case armor_type::Shield:
                {
                    switch (ShieldType())
                    {
                        case shield_type::Kite:
                        {
                            elementTypes = element_type::ValidCombinations(
                                element_type::Frost | element_type::Honor);

                            break;
                        }
                        case shield_type::Heater:
                        {
                            elementTypes = element_type::ValidCombinations(
                                element_type::Fire | element_type::Shadow);

                            break;
                        }
                        case shield_type::Pavis:
                        {
                            elementTypes = element_type::ValidCombinations();
                            break;
                        }
                        case shield_type::Buckler:
                        case shield_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    break;
                }
                case armor_type::Helm:
                {
                    switch (HelmType())
                    {
                        case helm_type::MailCoif:
                        {
                            elementTypes = { element_type::Frost };
                            break;
                        }
                        case helm_type::Kettle:
                        {
                            elementTypes = { element_type::Shadow };
                            break;
                        }
                        case helm_type::Archers:
                        {
                            elementTypes = { element_type::Honor };
                            break;
                        }
                        case helm_type::Bascinet:
                        {
                            elementTypes = { element_type::Fire };
                            break;
                        }
                        case helm_type::Great:
                        {
                            elementTypes = element_type::ValidCombinations();
                            break;
                        }
                        case helm_type::Leather:
                        case helm_type::Count:
                        default:
                        {
                            break;
                        }
                    }

                    break;
                }
                case armor_type::Covering:
                {
                    const auto COVER_TYPE { CoverType() };

                    if (cover_type::Cape == COVER_TYPE)
                    {
                        elementTypes = { element_type::Honor, element_type::Shadow };
                    }
                    else if (cover_type::Vest == COVER_TYPE)
                    {
                        elementTypes = { element_type::Fire, element_type::Frost };
                    }
                    else if (cover_type::Cloak == COVER_TYPE)
                    {
                        elementTypes = element_type::ValidCombinations();
                    }

                    break;
                }
                case armor_type::Skin:
                case armor_type::Not:
                case armor_type::Count:
                default:
                {
                    break;
                }
            }

            elementTypes.emplace_back(element_type::None);

            return elementTypes;
        }

    } // namespace armor
} // namespace item
} // namespace heroespath
