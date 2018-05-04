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
#ifndef HEROESPATH_ITEM_ARMOR_TYPE_WRAPPER_HPP_INCLUDED
#define HEROESPATH_ITEM_ARMOR_TYPE_WRAPPER_HPP_INCLUDED
//
// armor-type-wrapper.hpp
//
#include "item/armor-types.hpp"
#include "item/item-type-enum.hpp"
#include "misc/boost-serialize-includes.hpp"

#include <boost/variant.hpp>

#include <string>
#include <tuple>

namespace heroespath
{
namespace item
{
    namespace armor
    {

        // Responsible generating all type and name information about a SINGLE armor item. Instances
        // of this class can be constructed from either the required enum and
        // base_type::Enum, or from the unique name.  Unfortunately these classes are
        // serialized so they require a default constructor that leaves the object in an invalid
        // state.
        class ArmorTypeWrapper
        {
        public:
            explicit ArmorTypeWrapper(
                const std::string & SYSTEM_NAME = "",
                const base_type::Enum BASE_TYPE = base_type::Count,
                const bool WILL_FORCE_PLAIN_BASE_TYPE_IF_REQUIRED = false);

            explicit ArmorTypeWrapper(
                const armor_type::Enum,
                const base_type::Enum BASE_TYPE = base_type::Count,
                const bool WILL_MAKE_INVALID_IGNORING_BASE_TYPE = false);

            explicit ArmorTypeWrapper(const body_part::Enum);

            explicit ArmorTypeWrapper(const cover_type::Enum COVER_TYPE);

            ArmorTypeWrapper(const helm_type::Enum HELM_TYPE);

            explicit ArmorTypeWrapper(const shield_type::Enum SHIELD_TYPE);

            const std::string GeneralName() const { return generalName_; }
            const std::string SpecificName() const { return specificName_; }
            const std::string SystemName() const { return systemName_; }
            const std::string ReadableName() const { return readableName_; }
            const std::string DetailsKeyName() const;

            const std::string ToString() const;

            bool IsTypeValid() const { return (armor_type::NotArmor != type_); }

            bool WasMadeInvalidWithoutBaseType() const
            {
                return intentionallyMadeInvalidWithoutBaseType_;
            }

            const ElementEnumVec_t
                ElementTypesWithGivenBaseType(const base_type::Enum BASE_TYPE) const
            {
                return MakeElementTypesWithGivenBaseType(BASE_TYPE);
            }

            armor_type::Enum Type() const { return type_; }

            base_type::Enum BaseType() const { return base_; }

            const ElementEnumVec_t ElementTypes() const { return elementTypes_; }

            bool IsSkin() const { return (armor_type::Skin == type_); }

            bool IsShield() const
            {
                return (armor_type::Shield == type_) && (ShieldType() != shield_type::Count);
            }

            bool IsHelm() const
            {
                return (armor_type::Helm == type_) && (HelmType() != helm_type::Count);
            }

            bool IsCover() const
            {
                return (armor_type::Covering == type_) && (CoverType() != cover_type::Count);
            }

            bool IsGauntlets() const
            {
                return (armor_type::Gauntlets == type_) && (base_type::Count != base_);
            }

            bool IsPants() const
            {
                return (armor_type::Pants == type_) && (base_type::Count != base_);
            }

            bool IsBoots() const
            {
                return (armor_type::Boots == type_) && (base_type::Count != base_);
            }

            bool IsShirt() const
            {
                return (armor_type::Shirt == type_) && (base_type::Count != base_);
            }

            bool IsBracers() const
            {
                return (armor_type::Bracers == type_) && (base_type::Count != base_);
            }

            bool IsAventail() const
            {
                return (armor_type::Aventail == type_) && (base_type::Count != base_);
            }

            cover_type::Enum CoverType() const
            {
                return GetSpecificTypeIfValid<cover_type>(armor_type::Covering, COVER_INDEX_);
            }

            helm_type::Enum HelmType() const
            {
                return GetSpecificTypeIfValid<helm_type>(armor_type::Helm, HELM_INDEX_);
            }

            shield_type::Enum ShieldType() const
            {
                return GetSpecificTypeIfValid<shield_type>(armor_type::Shield, SHIELD_INDEX_);
            }

            const std::string
                ImageFilename(const std::string & SEPARATOR, const std::string & EXTENSION) const;

            bool IsGeneralNameAlmostSpecificName() const;

            static const std::vector<ArmorTypeWrapper> MakeCompleteSet();

            friend bool operator==(const ArmorTypeWrapper & L, const ArmorTypeWrapper & R);
            friend bool operator<(const ArmorTypeWrapper & L, const ArmorTypeWrapper & R);

        private:
            template <typename T>
            static void MakeSpecificSetWithoutBaseType(std::vector<ArmorTypeWrapper> & wrappers)
            {
                for (int i(0); i < T::Count; ++i)
                {
                    auto const SPECIFIC_ARMOR_ENUM{ static_cast<typename T::Enum>(i) };
                    wrappers.emplace_back(ArmorTypeWrapper(SPECIFIC_ARMOR_ENUM));
                }
            }

            static void MakeSpecificSetsWithBaseType(std::vector<ArmorTypeWrapper> & wrappers);

            template <typename T>
            typename T::Enum
                GetSpecificTypeIfValid(const armor_type::Enum ARMOR_TYPE, const int INDEX) const
            {
                if ((ARMOR_TYPE == type_) && (variant_.which() == INDEX))
                {
                    return boost::get<typename T::Enum>(variant_);
                }
                else
                {
                    return T::Count;
                }
            }

            template <typename T>
            bool SetupWithSpecificTypeName(
                const std::string & SYSTEM_NAME,
                const armor_type::Enum ARMOR_TYPE,
                const base_type::Enum BASE_TYPE)
            {
                auto const SYSTEM_NAME_LOWERCASE{ boost::algorithm::to_lower_copy(SYSTEM_NAME) };

                for (int i(0); i < T::Count; ++i)
                {
                    auto const SPECIFIC_TYPE_ENUM{ static_cast<typename T::Enum>(i) };

                    auto const SPECIFIC_TYPE_STR_LOWERCASE{ boost::algorithm::to_lower_copy(
                        T::ToString(SPECIFIC_TYPE_ENUM)) };

                    if (SPECIFIC_TYPE_STR_LOWERCASE == SYSTEM_NAME_LOWERCASE)
                    {
                        type_ = ARMOR_TYPE;
                        base_ = BASE_TYPE;
                        variant_ = SPECIFIC_TYPE_ENUM;

                        std::ostringstream ss;
                        ss << "after SetupWithSpecificTypeName<"
                           << boost::typeindex::type_id<T>().pretty_name()
                           << "::Enum>(system_name_lowercase=" << SYSTEM_NAME_LOWERCASE
                           << ", armor_type_tostring_lowercase=" << SPECIFIC_TYPE_STR_LOWERCASE
                           << ")";

                        SetNamesAndVerify(ss.str());
                        return true;
                    }
                }

                return false;
            }

            void SetNamesAndVerify(const std::string & CALLER_CONTEXT_DESCRIPTION);
            bool IsValidCompleteCheck() const;
            void SetupElementTypes();

            const ElementEnumVec_t MakeElementTypesWithGivenBaseType(
                const base_type::Enum BASE_TYPE = base_type::Count) const;

        public:
            static const std::string GLOVES_NAME_;

        private:
            static const int COVER_INDEX_ = 0;
            static const int HELM_INDEX_ = 1;
            static const int SHIELD_INDEX_ = 2;

            // a readable (gui visible) category kind of name, i.e. "Helm" for all
            // armor::helm_type::Enums
            std::string generalName_;

            // a readable (gui visible) unique name among all with a common generalName_, i.e.
            // "Mail Coif" when helm_type::MailCoif
            std::string specificName_;

            // a non-readable (contains no spaces) name that is unique system wide, not just unique
            // per generalName_, i.e. "MailCoif" when helm_type::MailCoif
            std::string systemName_;

            // a readable (gui visible) full name that is often specificName_ + " " + generalName_,
            // i.e. "Mail Coif" when helm_type::MailCoif, will include base_type if valid and not
            // "Plain" so when armor_type::Shirt and base_type::Mail then readableName_="Mail Shirt"
            std::string readableName_;

            armor_type::Enum type_;
            base_type::Enum base_;
            boost::variant<cover_type::Enum, helm_type::Enum, shield_type::Enum> variant_;

            // these are the "standard" element enchantments that are allowed for this weapon if not
            // also a set_type or named_type.
            ElementEnumVec_t elementTypes_;

            bool intentionallyMadeInvalidWithoutBaseType_;

        private:
            friend class boost::serialization::access;
            template <typename Archive>
            void serialize(Archive & ar, const unsigned int)
            {
                ar & generalName_;
                ar & specificName_;
                ar & systemName_;
                ar & readableName_;
                ar & type_;
                ar & base_;
                ar & variant_;
                ar & elementTypes_;
                ar & intentionallyMadeInvalidWithoutBaseType_;
            }
        };

        inline bool operator==(const ArmorTypeWrapper & L, const ArmorTypeWrapper & R)
        {
            // elementTypes_ intentionally not considered

            return std::tie(
                       L.generalName_,
                       L.specificName_,
                       L.systemName_,
                       L.readableName_,
                       L.type_,
                       L.base_,
                       L.variant_,
                       L.intentionallyMadeInvalidWithoutBaseType_)
                == std::tie(
                       R.generalName_,
                       R.specificName_,
                       R.systemName_,
                       R.readableName_,
                       R.type_,
                       R.base_,
                       R.variant_,
                       R.intentionallyMadeInvalidWithoutBaseType_);
        }

        inline bool operator!=(const ArmorTypeWrapper & L, const ArmorTypeWrapper & R)
        {
            return !(L == R);
        }

        inline bool operator<(const ArmorTypeWrapper & L, const ArmorTypeWrapper & R)
        {
            // elementTypes_ intentionally not considered

            return std::tie(
                       L.generalName_,
                       L.specificName_,
                       L.systemName_,
                       L.readableName_,
                       L.type_,
                       L.base_,
                       L.variant_,
                       L.intentionallyMadeInvalidWithoutBaseType_)
                < std::tie(
                       R.generalName_,
                       R.specificName_,
                       R.systemName_,
                       R.readableName_,
                       R.type_,
                       R.base_,
                       R.variant_,
                       R.intentionallyMadeInvalidWithoutBaseType_);
        }

    } // namespace armor
} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_ARMOR_TYPE_WRAPPER_HPP_INCLUDED
