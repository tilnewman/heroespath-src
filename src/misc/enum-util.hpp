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
#include "misc/log.hpp"
#include "misc/nameof.hpp"
#include "misc/vector-map.hpp"

#include <algorithm>
#include <exception>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{

namespace enum_helpers
{

    const std::string MakeLowerCaseCopy(const std::string & STR);

    const std::vector<std::string> BitwiseEnumFromStringSplit(const std::string & STR_ORIG);

    EnumUnderlying_t ReturnValueOfStringOr(
        const misc::VectorMap<std::string, EnumUnderlying_t> & STRING_TO_VALUE_MAP,
        const std::string & STRING_TO_FIND,
        const EnumUnderlying_t TO_RETURN_IF_NOT_FOUND);

    EnumUnderlying_t WordsToBitFlags(
        const std::vector<std::string> & WORDS_TO_FIND,
        const misc::VectorMap<std::string, EnumUnderlying_t> & WORDS_TO_BIT_FLAG_MAP);

    //

    template <typename EnumWrapper_t>
    class CountingEnum
    {
    public:
        CountingEnum() = delete;

        static constexpr EnumUnderlying_t LargestValidValue() { return (EnumWrapper_t::Count - 1); }

        static typename EnumWrapper_t::Enum FromString(const std::string & STR_ORIG)
        {
            const auto STR_TRIMMED_LOWER { MakeLowerCaseCopy(STR_ORIG) };

            if (nameToValueMap_.Empty())
            {
                ClearAndPopulateNameToValueMap();
            }

            return static_cast<typename EnumWrapper_t::Enum>(
                ReturnValueOfStringOr(nameToValueMap_, STR_TRIMMED_LOWER, EnumWrapper_t::Count));
        }

    private:
        static void ClearAndPopulateNameToValueMap()
        {
            nameToValueMap_.Clear();
            nameToValueMap_.Reserve(256);

            for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
            {
                const std::string ENUM_TRIMMED_LOWER_STRING { MakeLowerCaseCopy(
                    EnumWrapper_t::ToString(static_cast<typename EnumWrapper_t::Enum>(index))) };

                nameToValueMap_.Append(std::make_pair(ENUM_TRIMMED_LOWER_STRING, index));
            }

            std::sort(std::begin(nameToValueMap_), std::end(nameToValueMap_));
        }

        static misc::VectorMap<std::string, EnumUnderlying_t> nameToValueMap_;
    };

    template <typename EnumWrapper_t>
    misc::VectorMap<std::string, EnumUnderlying_t> CountingEnum<EnumWrapper_t>::nameToValueMap_;

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
            if (ENUM_VALUE == EnumWrapper_t::None)
            {
                if (HOW.will_none_return_empty == NoneEmpty::Yes)
                {
                    return "";
                }
                else
                {
                    if (HOW.will_wrap == Wrap::Yes)
                    {
                        return "(None)";
                    }
                    else
                    {
                        return "None";
                    }
                }
            }

            const std::string POPULATED_STR { EnumWrapper_t::ToStringPopulate(
                ENUM_VALUE, HOW.separator) };

            if ((HOW.will_wrap == Wrap::Yes) && !POPULATED_STR.empty())
            {
                return "(" + POPULATED_STR + ")";
            }
            else
            {
                return POPULATED_STR;
            }
        }

        static typename EnumWrapper_t::Enum FromString(const std::string & STR)
        {
            // these are the words in the original STR that MIGHT correspond to bit flags of
            // this enum, the strings returned by this function have all been trimmed and made
            // lower case
            const auto FROM_STR_SPLIT_VEC { BitwiseEnumFromStringSplit(STR) };

            if (FROM_STR_SPLIT_VEC.empty())
            {
                return EnumWrapper_t::None;
            }

            if (nameToBitFlagMap_.Empty())
            {
                ClearAndPopulateNameToBitFlagMap();
            }

            // loop over all words that might correspond to bit flags (FROM_STR_SPLIT_VEC) and
            // set all that match a flag (nameToBitFlagMap_)
            return static_cast<typename EnumWrapper_t::Enum>(
                WordsToBitFlags(FROM_STR_SPLIT_VEC, nameToBitFlagMap_));
        }

    private:
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
            nameToBitFlagMap_.Clear();
            nameToBitFlagMap_.Reserve(4096);

            EnumUnderlying_t flag { 1 };

            while (flag <= EnumWrapper_t::Last)
            {
                const std::string SINGLE_BIT_TO_STRING_ORIG { ToString(
                    static_cast<typename EnumWrapper_t::Enum>(flag),
                    EnumStringHow(Wrap::No, "", NoneEmpty::No)) };

                const std::string SINGLE_BIT_TO_STRING_TRIMMED_LOWER { MakeLowerCaseCopy(
                    SINGLE_BIT_TO_STRING_ORIG) };

                nameToBitFlagMap_.Append(std::make_pair(SINGLE_BIT_TO_STRING_TRIMMED_LOWER, flag));

                flag <<= 1;
            }

            std::sort(std::begin(nameToBitFlagMap_), std::end(nameToBitFlagMap_));
        }

        static EnumUnderlying_t largestValidValue_;

        // all the words (trimmed and lower-case) that correspond to one bit position in this
        // enum
        static misc::VectorMap<std::string, EnumUnderlying_t> nameToBitFlagMap_;
    };

    template <typename EnumWrapper_t>
    EnumUnderlying_t BitFieldEnum<EnumWrapper_t>::largestValidValue_ { 0 };

    template <typename EnumWrapper_t>
    misc::VectorMap<std::string, EnumUnderlying_t> BitFieldEnum<EnumWrapper_t>::nameToBitFlagMap_;

} // namespace enum_helpers

// helper template for common enumeration operations
template <typename EnumWrapper_t>
struct EnumUtil
{
    static const std::string TypeName()
    {
        return std::string(NAMEOF_TYPE_T(EnumWrapper_t)).append("::Enum");
    }

    // this may look redundant but callers can pass in the enum type (EnumWrapper::Enum)
    // and get the underlying type (EnumUnderlying_t) back
    static constexpr EnumUnderlying_t ToUnderlyingType(const EnumUnderlying_t ENUM_VALUE)
    {
        return ENUM_VALUE;
    }

    static constexpr bool IsValid(const EnumUnderlying_t ENUM_VALUE)
    {
        if constexpr (misc::are_same_v<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>)
        {
            return (ENUM_VALUE <= enum_helpers::CountingEnum<EnumWrapper_t>::LargestValidValue());
        }
        else
        {
            return (ENUM_VALUE <= enum_helpers::BitFieldEnum<EnumWrapper_t>::LargestValidValue());
        }
    }

    static constexpr bool IsValidAndNonZero(const EnumUnderlying_t ENUM_VALUE)
    {
        return ((ENUM_VALUE != 0) && IsValid(ENUM_VALUE));
    }

    static constexpr EnumUnderlying_t LargestValidValue()
    {
        if constexpr (misc::are_same_v<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>)
        {
            return enum_helpers::CountingEnum<EnumWrapper_t>::LargestValidValue();
        }
        else
        {
            return enum_helpers::BitFieldEnum<EnumWrapper_t>::LargestValidValue();
        }
    }

    static const std::string
        ToString(const EnumUnderlying_t ENUM_VALUE, const EnumStringHow & HOW = EnumStringHow())
    {
        if constexpr (misc::are_same_v<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>)
        {
            // this just prevents a visual studio warning
            const auto IGNORED(HOW);
            return EnumWrapper_t::ToString(static_cast<typename EnumWrapper_t::Enum>(ENUM_VALUE));
        }
        else
        {
            return enum_helpers::BitFieldEnum<EnumWrapper_t>::ToString(
                static_cast<typename EnumWrapper_t::Enum>(ENUM_VALUE), HOW);
        }
    }

    // if EnumWrapper_t does not provide its own Name() then use ToString() by default
    static const std::string Name(const EnumUnderlying_t ENUM_VALUE)
    {
        return ToString(ENUM_VALUE);
    }

    // case insensitive, returns Count on error
    static typename EnumWrapper_t::Enum FromString(const std::string & STR)
    {
        if constexpr (misc::are_same_v<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>)
        {
            return enum_helpers::CountingEnum<EnumWrapper_t>::FromString(STR);
        }
        else
        {
            return enum_helpers::BitFieldEnum<EnumWrapper_t>::FromString(STR);
        }
    }

    // Responsible for common operations of enums that are used as bit flags
    static constexpr std::size_t CountBitsSet(const EnumUnderlying_t ENUM_VALUE)
    {
        std::size_t count { 0 };
        EnumUnderlying_t flag { 1 };

        while (flag <= EnumWrapper_t::Last)
        {
            if (ENUM_VALUE & flag)
            {
                ++count;
            }

            flag <<= 1;
        }

        return count;
    }
};

} // namespace heroespath

#endif // HEROESPATH_MISC_ENUM_UTIL_HPP_INCLUDED
