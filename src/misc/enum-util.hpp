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
#include "misc/boost-string-includes.hpp"
#include "misc/enum-common.hpp"
#include "misc/log.hpp"
#include "misc/strings.hpp"

#include <boost/type_index.hpp>

#include <algorithm>
#include <exception>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{

namespace enum_helpers
{

    inline const std::string TrimAndMakeLowerCase(const std::string & STR)
    {
        return boost::algorithm::trim_copy(boost::algorithm::to_lower_copy(STR));
    }

    inline const std::vector<std::string> BitwiseEnumFromStringSplit(const std::string & STR_ORIG)
    {
        const auto STR_TRIMMED_AND_LOWERCASE { TrimAndMakeLowerCase(STR_ORIG) };

        if (STR_TRIMMED_AND_LOWERCASE.empty())
        {
            return {};
        }

        std::string seperatorChars;

        for (const auto CHAR : STR_TRIMMED_AND_LOWERCASE)
        {
            // spaces/dashes/colons are not valid separators because some names will have them
            if ((misc::IsAlphaOrDigit(CHAR) == false) && (CHAR != ' ') && (CHAR != '-')
                && (CHAR != ':'))
            {
                seperatorChars.push_back(CHAR);
            }
        }

        return misc::SplitByChars(STR_TRIMMED_AND_LOWERCASE, misc::SplitHow(seperatorChars));
    }

    inline EnumUnderlying_t ReturnValueOfStringOr(
        const std::map<std::string, EnumUnderlying_t> & STRING_TO_VALUE_MAP,
        const std::string & STRING_TO_FIND,
        const EnumUnderlying_t TO_RETURN_IF_NOT_FOUND)
    {
        const auto FOUND_ITER { std::find_if(
            std::begin(STRING_TO_VALUE_MAP),
            std::end(STRING_TO_VALUE_MAP),
            [&](const auto & NAME_VALUE_PAIR) {
                return (STRING_TO_FIND == NAME_VALUE_PAIR.first);
            }) };

        if (FOUND_ITER == std::end(STRING_TO_VALUE_MAP))
        {
            return TO_RETURN_IF_NOT_FOUND;
        }
        else
        {
            return FOUND_ITER->second;
        }
    }

    inline EnumUnderlying_t WordsToBitFlags(
        const std::vector<std::string> & WORDS_TO_FIND,
        const std::map<std::string, EnumUnderlying_t> & WORDS_TO_BIT_FLAG_MAP)
    {
        EnumUnderlying_t result { 0 };

        for (const auto & WORD_TO_FIND : WORDS_TO_FIND)
        {
            const auto FOUND_ITER { std::find_if(
                std::begin(WORDS_TO_BIT_FLAG_MAP),
                std::end(WORDS_TO_BIT_FLAG_MAP),
                [&](const auto & NAME_VALUE_PAIR) {
                    return (WORD_TO_FIND == NAME_VALUE_PAIR.first);
                }) };

            if (FOUND_ITER != std::end(WORDS_TO_BIT_FLAG_MAP))
            {
                result |= FOUND_ITER->second;
            }
        }

        return result;
    }

    template <typename EnumWrapper_t>
    class CountingEnum
    {
    public:
        CountingEnum() = delete;

        static constexpr EnumUnderlying_t LargestValidValue() { return (EnumWrapper_t::Count - 1); }

        static typename EnumWrapper_t::Enum FromString(const std::string & STR_ORIG)
        {
            const auto STR_TRIMMED_LOWER { TrimAndMakeLowerCase(STR_ORIG) };

            if (nameToValueMap_.empty())
            {
                ClearAndPopulateNameToValueMap();
            }

            return static_cast<typename EnumWrapper_t::Enum>(
                ReturnValueOfStringOr(nameToValueMap_, STR_TRIMMED_LOWER, EnumWrapper_t::Count));
        }

    private:
        static void ClearAndPopulateNameToValueMap()
        {
            nameToValueMap_.clear();

            for (EnumUnderlying_t index(0); index < EnumWrapper_t::Count; ++index)
            {
                const std::string ENUM_TRIMMED_LOWER_STRING { TrimAndMakeLowerCase(
                    EnumWrapper_t::ToString(static_cast<typename EnumWrapper_t::Enum>(index))) };

                nameToValueMap_[ENUM_TRIMMED_LOWER_STRING] = index;
            }
        }

        static std::map<std::string, EnumUnderlying_t> nameToValueMap_;
    };

    template <typename EnumWrapper_t>
    std::map<std::string, EnumUnderlying_t> CountingEnum<EnumWrapper_t>::nameToValueMap_;

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

            if (nameToBitFlagMap_.empty())
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
            nameToBitFlagMap_.clear();

            EnumUnderlying_t flag { 1 };

            while (flag <= EnumWrapper_t::Last)
            {
                const std::string SINGLE_BIT_TO_STRING_ORIG { ToString(
                    static_cast<typename EnumWrapper_t::Enum>(flag),
                    EnumStringHow(Wrap::No, "", NoneEmpty::No)) };

                const std::string SINGLE_BIT_TO_STRING_TRIMMED_LOWER { TrimAndMakeLowerCase(
                    SINGLE_BIT_TO_STRING_ORIG) };

                nameToBitFlagMap_[SINGLE_BIT_TO_STRING_TRIMMED_LOWER] = flag;

                flag <<= 1;
            }
        }

        static EnumUnderlying_t largestValidValue_;

        // all the words (trimmed and lower-case) that correspond to one bit position in this
        // enum
        static std::map<std::string, EnumUnderlying_t> nameToBitFlagMap_;
    };

    template <typename EnumWrapper_t>
    EnumUnderlying_t BitFieldEnum<EnumWrapper_t>::largestValidValue_ { 0 };

    template <typename EnumWrapper_t>
    std::map<std::string, EnumUnderlying_t> BitFieldEnum<EnumWrapper_t>::nameToBitFlagMap_;

} // namespace enum_helpers

// helper template for common enumeration operations
template <typename EnumWrapper_t>
struct EnumUtil
{
    static const std::string TypeName()
    {
        return boost::typeindex::type_id<EnumWrapper_t>().pretty_name() + "::Enum";
    }

    // this may look redundant but callers can pass in the enum type (EnumWrapper::Enum)
    // and get the underlying type (EnumUnderlying_t) back
    static constexpr EnumUnderlying_t ToUnderlyingType(const EnumUnderlying_t ENUM_VALUE)
    {
        return ENUM_VALUE;
    }

    static constexpr bool IsValid(const EnumUnderlying_t ENUM_VALUE)
    {
        if constexpr (std::is_same<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>::value)
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
        if constexpr (std::is_same<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>::value)
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
        if constexpr (std::is_same<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>::value)
        {
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
        if constexpr (std::is_same<typename EnumWrapper_t::EnumBase_t, EnumCounting_t>::value)
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
