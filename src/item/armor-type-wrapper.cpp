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
// armor-type-wrapper.cpp
//
#include "armor-type-wrapper.hpp"

#include "misc/assertlogandthrow.hpp"

#include <exception>
#include <numeric>
#include <sstream>

namespace heroespath
{
namespace item
{
    namespace armor
    {

        const std::string ArmorTypeWrapper::GLOVES_NAME_{ "Gloves" };

        ArmorTypeWrapper::ArmorTypeWrapper(
            const std::string & SYSTEM_NAME,
            const base_type::Enum BASE_TYPE,
            const bool WILL_FORCE_PLAIN_BASE_TYPE_IF_REQUIRED)
            : generalName_("")
            , specificName_("")
            , systemName_(SYSTEM_NAME)
            , readableName_("")
            , type_(armor_type::NotArmor)
            , base_(BASE_TYPE)
            , variant_()
        {
            // This is the default constructor and it needs to support construction with all default
            // parameters that leave the object in an invalid state, so this if/return allows for
            // that.
            if (SYSTEM_NAME.empty())
            {
                return;
            }

            // if (generalName_.empty()) //no need for the check before the first attempt to set it
            {
                SetupWithSpecificTypeName<cover_type>(SYSTEM_NAME, armor_type::Covering, BASE_TYPE);
            }

            if (generalName_.empty())
            {
                SetupWithSpecificTypeName<helm_type>(SYSTEM_NAME, armor_type::Helm, BASE_TYPE);
            }

            if (generalName_.empty())
            {
                SetupWithSpecificTypeName<shield_type>(SYSTEM_NAME, armor_type::Shield, BASE_TYPE);
            }

            for (int i(0); i <= armor_type::Skin; ++i)
            {
                auto const ARMOR_TYPE_ENUM{ static_cast<armor_type::Enum>(i) };
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

                    if ((ARMOR_TYPE_ENUM == armor_type::Gauntlets) && (base_type::Plain == base_))
                    {
                        generalName_ = GLOVES_NAME_;
                    }
                    else
                    {
                        generalName_ = SYSTEM_NAME;
                    }

                    specificName_ = generalName_;
                    systemName_ = generalName_;
                    readableName_ = generalName_;
                    type_ = ARMOR_TYPE_ENUM;

                    break;
                }
            }

            std::ostringstream ss;
            ss << "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(name=" << SYSTEM_NAME
               << ", orig_base_type="
               << ((BASE_TYPE == base_type::Count) ? "Count" : base_type::ToString(BASE_TYPE))
               << ", final_base_type = "
               << ((base_ == base_type::Count) ? "Count" : base_type::ToString(base_)) << ") ";

            if (SYSTEM_NAME == GLOVES_NAME_)
            {
                generalName_ = SYSTEM_NAME;
                specificName_ = SYSTEM_NAME;
                systemName_ = SYSTEM_NAME;
                readableName_ = SYSTEM_NAME;
                type_ = armor_type::Gauntlets;

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (BASE_TYPE == base_type::Plain),
                    ss.str() << "which matches \"gloves\" but the base_type specified was not "
                                "base_type::plain.");

                base_ = BASE_TYPE;
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                (generalName_.empty() == false),
                ss.str() << "but that name failed to be recognized by any armor category.");

            ss << "and that set armor_type=" << armor_type::ToString(type_)
               << ", and variant_.which()=" << variant_.which() << ", ";

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                ss.str() << "but and that name was recognized but resulted in IsValid()==false.  "
                            "ToString()="
                         << ToString());

            auto const ORIG_GENERAL_NAME{ generalName_ };
            auto const ORIG_SPECIFIC_NAME{ specificName_ };
            auto const ORIG_SYSTEM_NAME{ systemName_ };
            auto const ORIG_READABLE_NAME{ readableName_ };

            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                ((ORIG_GENERAL_NAME == generalName_) && (ORIG_SPECIFIC_NAME == specificName_)
                 && (ORIG_SYSTEM_NAME == systemName_) && (ORIG_READABLE_NAME == readableName_)),
                ss.str() << "but the generated names did not match general=" << ORIG_GENERAL_NAME
                         << "/" << generalName_ << ", specific=" << ORIG_SPECIFIC_NAME << "/"
                         << specificName_ << ", system=" << ORIG_SYSTEM_NAME << "/" << systemName_
                         << ", readable=\"" << ORIG_READABLE_NAME << "\" \"" << readableName_
                         << "\".");
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
        {
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(armor_type="
                    << armor_type::ToString(ARMOR_TYPE) << ", base_type="
                    << ((BASE_TYPE == base_type::Count) ? "Count" : base_type::ToString(BASE_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        ArmorTypeWrapper::ArmorTypeWrapper(const body_part::Enum BODY_PART)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(armor_type::Skin)
            , base_(base_type::Count)
            , variant_()
        {
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(body_part="
                    << ((BODY_PART == body_part::Count) ? "Count" : body_part::ToString(BODY_PART))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        ArmorTypeWrapper::ArmorTypeWrapper(const cover_type::Enum COVER_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(armor_type::Covering)
            , base_(base_type::Count)
            , variant_()
        {
            variant_ = COVER_TYPE;
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(body_part="
                    << ((COVER_TYPE == cover_type::Count) ? "Count"
                                                          : cover_type::ToString(COVER_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        ArmorTypeWrapper::ArmorTypeWrapper(const helm_type::Enum HELM_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(armor_type::Helm)
            , base_(base_type::Count)
            , variant_()
        {
            variant_ = HELM_TYPE;
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(body_part="
                    << ((HELM_TYPE == helm_type::Count) ? "Count" : helm_type::ToString(HELM_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        ArmorTypeWrapper::ArmorTypeWrapper(const shield_type::Enum SHIELD_TYPE)
            : generalName_("")
            , specificName_("")
            , systemName_("")
            , readableName_("")
            , type_(armor_type::Shield)
            , base_(base_type::Count)
            , variant_()
        {
            variant_ = SHIELD_TYPE;
            SetNames();

            M_ASSERT_OR_LOGANDTHROW_SS(
                (IsValid()),
                "item::armor::ArmorTypeWrapper::ArmorTypeWrapper(shield_type="
                    << ((SHIELD_TYPE == shield_type::Count) ? "Count"
                                                            : shield_type::ToString(SHIELD_TYPE))
                    << ") but the resulting object was not valid.  ToString()=" << ToString());
        }

        void ArmorTypeWrapper::SetNames()
        {
            if (IsSkin())
            {
                generalName_ = armor_type::ToString(type_);
                specificName_ = generalName_;
                systemName_ = generalName_;
                readableName_ = generalName_;
            }
            if (IsShield())
            {
                auto const SHIELD_TYPE{ ShieldType() };
                generalName_ = armor_type::ToString(type_);
                specificName_ = shield_type::ToString(SHIELD_TYPE);
                systemName_ = specificName_;
                readableName_ = specificName_ + " " + generalName_;
            }
            else if (IsHelm())
            {
                auto const HELM_TYPE{ HelmType() };
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
            }
            else if (IsCover())
            {
                auto const COVER_TYPE{ CoverType() };
                generalName_ = cover_type::ToString(COVER_TYPE);
                specificName_ = generalName_;
                systemName_ = generalName_;
                readableName_ = generalName_;
            }
            else if (
                IsGauntlets() || IsPants() || IsBoots() || IsShirt() || IsBracers() || IsAventail())
            {
                auto const IS_GLOVES{ IsGauntlets() && (base_type::Plain == base_) };

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
            }
        }

        bool ArmorTypeWrapper::IsValid() const
        {
            if (armor_type::NotArmor == type_)
            {
                return false;
            }
            else if (
                generalName_.empty() || specificName_.empty() || systemName_.empty()
                || readableName_.empty())
            {
                return false;
            }
            else if (
                (armor_type::Gauntlets == type_) && (base_type::Plain == base_)
                && (GLOVES_NAME_ != specificName_))
            {
                return false;
            }
            if ((armor_type::DoesRequireBaseType(type_)) && (base_type::Count == base_))
            {
                return false;
            }
            else
            {
                const std::vector<bool> BOOLS{ IsShield(), IsHelm(),  IsGauntlets(), IsPants(),
                                               IsBoots(),  IsShirt(), IsBracers(),   IsAventail(),
                                               IsCover(),  IsSkin() };

                const int COUNT{ std::accumulate(
                    std::begin(BOOLS),
                    std::end(BOOLS),
                    0,
                    [](auto const SUBTOTAL, auto const NEXT_BOOL) {
                        return SUBTOTAL + ((NEXT_BOOL) ? 1 : 0);
                    }) };

                return (COUNT == 1);
            }
        }

        const std::string ArmorTypeWrapper::DetailsKeyName() const
        {
            auto const SPECIFIC_NAME_WITHOUT_SPACES{ boost::algorithm::replace_all_copy(
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

            std::ostringstream ss;
            ss << "generalName_=\"" << generalName_ << "\", specificName_=\"" << specificName_
               << "\", systemName_=\"" << systemName_ << "\", readableName_=\"" << readableName_
               << "\", type=" << armor_type::ToString(type_)
               << ", base=" << ((base_type::Count == base_) ? "Count" : base_type::ToString(base_))
               << ", variant_.which()=" << variant_.which() << ", type=("
               << ((IsShield()) ? shield_type::ToString(ShieldType()) : "") << ","
               << ((IsHelm()) ? helm_type::ToString(HelmType()) : "") << ","
               << ((IsGauntlets()) ? "gauntlets" : "") << "," << ((IsPants()) ? "pants" : "") << ","
               << ((IsBoots()) ? "boots" : "") << "," << ((IsShirt()) ? "shirt" : "") << ","
               << ((IsBracers()) ? "bracers" : "") << "," << ((IsAventail()) ? "aventail" : "")
               << "," << ((IsCover()) ? cover_type::ToString(CoverType()) : "") << ","
               << ((IsSkin()) ? "skin" : "") << ")";

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
                auto const GENERAL_NAME_FILENAME_VERSION{ boost::algorithm::replace_all_copy(
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

        void
            ArmorTypeWrapper::MakeSpecificSetsWithBaseType(std::vector<ArmorTypeWrapper> & wrappers)
        {
            for (int a(0); a < armor_type::Count; ++a)
            {
                auto const ARMOR_ENUM{ static_cast<armor_type::Enum>(a) };

                if (armor_type::DoesRequireBaseType(ARMOR_ENUM))
                {
                    for (int b(0); b < base_type::Count; ++b)
                    {
                        auto const BASE_ENUM{ static_cast<base_type::Enum>(b) };
                        wrappers.emplace_back(ArmorTypeWrapper(ARMOR_ENUM, BASE_ENUM));
                    }
                }
            }
        }

    } // namespace armor
} // namespace item
} // namespace heroespath
