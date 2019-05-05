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

#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace helpers
{

    using StrNumPair_t = std::pair<std::string, EnumUnderlying_t>;
    using StrNumPairVec_t = std::vector<StrNumPair_t>;

    const std::string MakeLowerCaseCopy(const std::string & STR);

    const std::vector<std::string> BitwiseEnumFromStringSplit(const std::string & STR_ORIG);

    void sortStrNumPairVec(StrNumPairVec_t & v);

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

            if (nameToValueMap_.empty())
            {
                ClearAndPopulateNameToValueMap();
            }

            for (const auto & PAIR : nameToValueMap_)
            {
                if (PAIR.first == STR_TRIMMED_LOWER)
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
                const std::string ENUM_TRIMMED_LOWER_STRING { MakeLowerCaseCopy(
                    EnumWrapper_t::ToString(static_cast<typename EnumWrapper_t::Enum>(index))) };

                nameToValueMap_.emplace_back(std::make_pair(ENUM_TRIMMED_LOWER_STRING, index));
            }

            sortStrNumPairVec(nameToValueMap_);
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
            EnumUnderlying_t result { 0 };
            for (const auto & WORD_TO_FIND : FROM_STR_SPLIT_VEC)
            {
                EnumUnderlying_t bitFlag { 0 };
                for (const auto & PAIR : nameToBitFlagMap_)
                {
                    if (PAIR.first == WORD_TO_FIND)
                    {
                        bitFlag = PAIR.second;
                        break;
                    }
                }
                result |= bitFlag;
            }

            return static_cast<typename EnumWrapper_t::Enum>(result);
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
            nameToBitFlagMap_.reserve(sizeof(EnumUnderlying_t) * 8);

            EnumUnderlying_t flag { 1 };

            while (flag <= EnumWrapper_t::Last)
            {
                const std::string SINGLE_BIT_TO_STRING_ORIG { ToString(
                    static_cast<typename EnumWrapper_t::Enum>(flag),
                    EnumStringHow(Wrap::No, "", NoneEmpty::No)) };

                const std::string SINGLE_BIT_TO_STRING_TRIMMED_LOWER { MakeLowerCaseCopy(
                    SINGLE_BIT_TO_STRING_ORIG) };

                nameToBitFlagMap_.emplace_back(
                    std::make_pair(SINGLE_BIT_TO_STRING_TRIMMED_LOWER, flag));

                flag <<= 1;
            }

            sortStrNumPairVec(nameToBitFlagMap_);
        }

        static EnumUnderlying_t largestValidValue_;

        // all the words (trimmed and lower-case) that correspond to one bit position in this
        // enum
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
    // this may look redundant but callers can pass in the enum type (EnumWrapper::Enum)
    // and get the underlying type (EnumUnderlying_t) back
    static constexpr EnumUnderlying_t ToUnderlyingType(const EnumUnderlying_t ENUM_VALUE) noexcept
    {
        return ENUM_VALUE;
    }

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

    template <typename T = typename EnumWrapper_t::EnumBase_t>
    static std::enable_if_t<misc::are_same_v<T, EnumCounting_t>, const std::string>
        ToString(const EnumUnderlying_t ENUM_VALUE, const EnumStringHow & = EnumStringHow())
    {
        return EnumWrapper_t::ToString(static_cast<typename EnumWrapper_t::Enum>(ENUM_VALUE));
    }

    template <typename T = typename EnumWrapper_t::EnumBase_t>
    static std::enable_if_t<!misc::are_same_v<T, EnumCounting_t>, const std::string>
        ToString(const EnumUnderlying_t ENUM_VALUE, const EnumStringHow & HOW = EnumStringHow())
    {
        return helpers::BitFieldEnum<EnumWrapper_t>::ToString(
            static_cast<typename EnumWrapper_t::Enum>(ENUM_VALUE), HOW);
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
            return helpers::CountingEnum<EnumWrapper_t>::FromString(STR);
        }
        else
        {
            return helpers::BitFieldEnum<EnumWrapper_t>::FromString(STR);
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
