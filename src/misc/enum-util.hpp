// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
#define HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
//
// enum-util.hpp
//
//  Note:  enum-common.hpp is meant to be included in *.hpp only, and enum-util.hpp is meant to be
//         included in *.cpp files only.  Breaking this rule will not break the build, but the whole
//         point of splitting enum functions into those two files was to speed build times and make
//         creating and including enums as light-weight as possible.  So while you CAN include
//         enum-util.hpp in a header file, it would defeat the design and slow down the build.
//
#include "misc/enum-common.hpp"
#include "misc/strings.hpp"
#include "misc/type-helpers.hpp"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace helpers
{

    using StrNumPair_t = std::pair<std::string, EnumUnderlying_t>;
    using StrNumPairVec_t = std::vector<StrNumPair_t>;

    template <typename EnumWrapper_t>
    class CountingEnum
    {
    public:
        CountingEnum() = delete;

        static constexpr EnumUnderlying_t LargestValidValue() { return (EnumWrapper_t::Count - 1); }

        // case-insensitive
        static typename EnumWrapper_t::Enum FromString(const std::string_view STR_VIEW)
        {
            if (nameToValueMap_.empty())
            {
                ClearAndPopulateNameToValueMap();
            }

            for (const auto & PAIR : nameToValueMap_)
            {
                if (misc::AreEqualCaseInsensitive(PAIR.first, STR_VIEW))
                {
                    return static_cast<typename EnumWrapper_t::Enum>(PAIR.second);
                }
            }

            return EnumWrapper_t::Count;
        }

    private:
        static void ClearAndPopulateNameToValueMap()
        {
            nameToValueMap_.clear();
            nameToValueMap_.reserve(static_cast<std::size_t>(EnumWrapper_t::Count));

            for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
            {
                const auto ENUM = static_cast<typename EnumWrapper_t::Enum>(index);
                nameToValueMap_.emplace_back(std::make_pair(NAMEOF_ENUM_STR(ENUM), index));
            }

            // only sorting to speed linear lookup later
            std::sort(std::begin(nameToValueMap_), std::end(nameToValueMap_));
        }

        static StrNumPairVec_t nameToValueMap_;
    };

    template <typename EnumWrapper_t>
    StrNumPairVec_t CountingEnum<EnumWrapper_t>::nameToValueMap_;

    //

    template <typename EnumWrapper_t>
    class BitFieldEnum
    {
    public:
        BitFieldEnum() = delete;

        static constexpr EnumUnderlying_t LargestValidValue()
        {
            if (0 == largestValidValue_)
            {
                largestValidValue_ = CalcLargestValidValue();
            }

            return largestValidValue_;
        }

        static const std::string
            ToString(const EnumUnderlying_t ENUM_VALUE, const EnumStringHow & HOW = EnumStringHow())
        {
            std::string str;
            str.reserve(16);

            if (HOW.will_wrap == Wrap::Yes)
            {
                str += '(';
            }

            if (ENUM_VALUE == EnumWrapper_t::None)
            {
                if (HOW.will_none_return_empty == NoneEmpty::No)
                {
                    str += "None";
                }
            }
            else
            {
                ToStringPopulate(str, ENUM_VALUE, HOW.separator);
            }

            if (str.compare("(") == 0)
            {
                str.clear();
            }
            else if (HOW.will_wrap == Wrap::Yes)
            {
                str += ')';
            }

            return str;
        }

        // case-insensitive
        static typename EnumWrapper_t::Enum FromString(const std::string_view STR_VIEW)
        {
            if (nameToBitFlagMap_.empty())
            {
                ClearAndPopulateNameToBitFlagMap();
            }

            EnumUnderlying_t result { 0 };

            // Can't simply split on !IsAlphaOrDigit() because some of the enum names have
            // underscores/dashes/etc.  Typical bitfield enum separators are '/' and ','.
            auto isSeparator = [](const char CH) constexpr noexcept
            {
                return (!misc::IsAlphaOrDigit(CH) && (CH != '_') && (CH != '-'));
            };

            auto setBitByName = [&](const auto FIRST, const auto LAST) {
                for (const auto & PAIR : nameToBitFlagMap_)
                {
                    if (misc::AreEqualCaseInsensitive(FIRST, LAST, PAIR.first))
                    {
                        result |= PAIR.second;
                        break;
                    }
                }

                return false;
            };

            misc::FindStringsBetweenSeparatorsAndCall(
                std::begin(STR_VIEW), std::end(STR_VIEW), isSeparator, setBitByName);

            return static_cast<typename EnumWrapper_t::Enum>(result);
        }

    private:
        static void ToStringPopulateAppendIf(
            const std::size_t STR_SIZE_ORIG,
            std::string & str,
            const EnumUnderlying_t ENUM_VALUE,
            const std::string_view SEP,
            const std::string_view BIT_NAME,
            EnumUnderlying_t & singleBitToCheck)
        {
            if (ENUM_VALUE & singleBitToCheck)
            {
                if (str.size() != STR_SIZE_ORIG)
                {
                    str += SEP;
                }

                str += BIT_NAME;
            }

            singleBitToCheck <<= 1;
        }

        static void ToStringPopulate(
            std::string & str,
            const EnumUnderlying_t ENUM_VALUE,
            const std::string_view SEPARATOR_ORIG)
        {
            const auto ORIG_SIZE = str.size();

            std::string separator(SEPARATOR_ORIG);

            if constexpr (misc::are_same_v<
                              typename EnumWrapper_t::SeparatorPolicy_t,
                              SeparatorAlwaysSlash>)
            {
                separator = "/";
            }

            EnumUnderlying_t singleBitToCheck { 1 };

            if constexpr (EnumWrapper_t::Last > NAMEOF_ENUM_RANGE_MAX)
            {
                for (const auto & NAME : EnumWrapper_t::TO_STRINGS)
                {
                    ToStringPopulateAppendIf(
                        ORIG_SIZE, str, ENUM_VALUE, separator, NAME, singleBitToCheck);
                }
            }
            else
            {
                while (singleBitToCheck <= EnumWrapper_t::Last)
                {
                    ToStringPopulateAppendIf(
                        ORIG_SIZE,
                        str,
                        ENUM_VALUE,
                        separator,
                        NAMEOF_ENUM(static_cast<typename EnumWrapper_t::Enum>(singleBitToCheck)),
                        singleBitToCheck);
                }
            }
        }

        static constexpr EnumUnderlying_t CalcLargestValidValue()
        {
            EnumUnderlying_t finalValue { 1 };
            EnumUnderlying_t shiftingValue { 1 };

            while (shiftingValue <= EnumWrapper_t::Last)
            {
                finalValue |= shiftingValue;
                shiftingValue <<= 1;
            }

            return finalValue;
        }

        static void ClearAndPopulateNameToBitFlagMap()
        {
            nameToBitFlagMap_.clear();
            nameToBitFlagMap_.reserve(sizeof(EnumUnderlying_t) * 8);

            EnumUnderlying_t flag { 1 };

            const auto TO_STRING_HOW = EnumStringHow(Wrap::No, "", NoneEmpty::No);

            while (flag <= EnumWrapper_t::Last)
            {
                nameToBitFlagMap_.emplace_back(std::make_pair(
                    ToString(static_cast<typename EnumWrapper_t::Enum>(flag), TO_STRING_HOW),
                    flag));

                flag <<= 1;
            }

            // only sorting to speed linear lookup later
            std::sort(std::begin(nameToBitFlagMap_), std::end(nameToBitFlagMap_));
        }

        static EnumUnderlying_t largestValidValue_;
        static StrNumPairVec_t nameToBitFlagMap_;
    };

    template <typename EnumWrapper_t>
    EnumUnderlying_t BitFieldEnum<EnumWrapper_t>::largestValidValue_ { 0 };

    template <typename EnumWrapper_t>
    StrNumPairVec_t BitFieldEnum<EnumWrapper_t>::nameToBitFlagMap_;

} // namespace helpers

// helper template for common enumeration operations
template <typename EnumWrapper_t>
struct EnumUtil
{
    static constexpr bool IsValid(const EnumUnderlying_t ENUM_VALUE) noexcept
    {
        if constexpr (misc::are_same_v<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>)
        {
            return (ENUM_VALUE <= helpers::CountingEnum<EnumWrapper_t>::LargestValidValue());
        }
        else
        {
            return (ENUM_VALUE <= helpers::BitFieldEnum<EnumWrapper_t>::LargestValidValue());
        }
    }

    static constexpr bool IsValidAndNonZero(const EnumUnderlying_t ENUM_VALUE) noexcept
    {
        return ((ENUM_VALUE != 0) && IsValid(ENUM_VALUE));
    }

    static constexpr EnumUnderlying_t LargestValidValue()
    {
        if constexpr (misc::are_same_v<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>)
        {
            return helpers::CountingEnum<EnumWrapper_t>::LargestValidValue();
        }
        else
        {
            return helpers::BitFieldEnum<EnumWrapper_t>::LargestValidValue();
        }
    }

    template <typename T = EnumWrapper_t>
    static constexpr std::
        enable_if_t<misc::are_same_v<typename T::EnumBase_t, EnumCounting_t>, std::string_view>
        ToString(const typename T::Enum ENUM)
    {
        return NAMEOF_ENUM(ENUM);
    }

    template <typename T = EnumWrapper_t>
    static std::
        enable_if_t<misc::are_same_v<typename T::EnumBase_t, EnumCounting_t>, const std::string>
        ToStringStd(const typename T::Enum ENUM)
    {
        return NAMEOF_ENUM_STR(ENUM);
    }

    template <typename T = EnumWrapper_t>
    static std::enable_if_t<
        misc::are_same_v<typename T::EnumBase_t, EnumCounting_t>,
        const std::string_view>
        Name(const typename T::Enum ENUM)
    {
        return EnumWrapper_t::Name(ENUM);
    }

    template <typename T = EnumWrapper_t>
    static std::
        enable_if_t<misc::are_same_v<typename T::EnumBase_t, EnumCounting_t>, const std::string>
        NameStd(const typename T::Enum ENUM)
    {
        return std::string(EnumWrapper_t::Name(ENUM));
    }

    // this is how all bitwise enums implement their ToString() function and is used everywhere
    template <typename T = EnumWrapper_t>
    static std::
        enable_if_t<!misc::are_same_v<typename T::EnumBase_t, EnumCounting_t>, const std::string>
        ToString(const typename T::Enum ENUM, const EnumStringHow & HOW = EnumStringHow())
    {
        return helpers::BitFieldEnum<T>::ToString(ENUM, HOW);
    }

    // if EnumWrapper_t does not provide its own Name() then use ToString() by default
    template <typename T = EnumWrapper_t>
    static std::
        enable_if_t<!misc::are_same_v<typename T::EnumBase_t, EnumCounting_t>, const std::string>
        Name(const typename T::Enum ENUM, const EnumStringHow & HOW = EnumStringHow())
    {
        return T::Name(ENUM, HOW);
    }

    // case insensitive, returns Count on error
    static typename EnumWrapper_t::Enum FromString(const std::string_view STR_VIEW)
    {
        if constexpr (misc::are_same_v<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>)
        {
            return helpers::CountingEnum<EnumWrapper_t>::FromString(STR_VIEW);
        }
        else
        {
            return helpers::BitFieldEnum<EnumWrapper_t>::FromString(STR_VIEW);
        }
    }

    static constexpr std::size_t CountBits(const EnumUnderlying_t ENUM_VALUE)
    {
        return misc::CountBits(static_cast<std::uint32_t>(ENUM_VALUE));
    }
};

} // namespace heroespath

#endif // HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
