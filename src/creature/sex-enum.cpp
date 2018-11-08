// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// sex-enum.hpp
//
#include "sex-enum.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace creature
{

    const std::string sex::ToString(const sex::Enum SEX_TYPE)
    {
        switch (SEX_TYPE)
        {
            case Unknown:
            {
                return "Unknown";
            }
            case Male:
            {
                return "Male";
            }
            case Female:
            {
                return "Female";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(SEX_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    const std::string sex::HeSheIt(const sex::Enum SEX_TYPE, const bool WILL_CAPITALIZE)
    {
        switch (SEX_TYPE)
        {
            case Unknown:
            {
                if (WILL_CAPITALIZE)
                    return "It";
                else
                    return "it";
            }
            case Male:
            {
                if (WILL_CAPITALIZE)
                    return "He";
                else
                    return "he";
            }
            case Female:
            {
                if (WILL_CAPITALIZE)
                    return "She";
                else
                    return "she";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(SEX_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    const std::string sex::HisHerIts(
        const sex::Enum SEX_TYPE, const bool WILL_CAPITALIZE, const bool WILL_POSSESSIVE_HER)
    {
        switch (SEX_TYPE)
        {
            case Unknown:
            {
                if (WILL_CAPITALIZE)
                    return "Its";
                else
                    return "its";
            }
            case Male:
            {
                if (WILL_CAPITALIZE)
                    return "His";
                else
                    return "his";
            }
            case Female:
            {
                if (WILL_CAPITALIZE)
                    return std::string("Her").append((WILL_POSSESSIVE_HER) ? "s" : "");
                else
                    return std::string("her").append((WILL_POSSESSIVE_HER) ? "s" : "");
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(SEX_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

    const std::string sex::HimHerIt(const sex::Enum SEX_TYPE, const bool WILL_CAPITALIZE)
    {
        switch (SEX_TYPE)
        {
            case Unknown:
            {
                if (WILL_CAPITALIZE)
                    return "It";
                else
                    return "it";
            }
            case Male:
            {
                if (WILL_CAPITALIZE)
                    return "Him";
                else
                    return "him";
            }
            case Female:
            {
                if (WILL_CAPITALIZE)
                    return "Her";
                else
                    return "her";
            }
            case Count:
            {
                return "(Count)";
            }
            default:
            {
                M_HP_LOG_ERR(
                    "enum_value=" << static_cast<EnumUnderlying_t>(SEX_TYPE)
                                  << " is invalid. (count=" << static_cast<EnumUnderlying_t>(Count)
                                  << ")");

                return "";
            }
        }
    }

} // namespace creature
} // namespace heroespath
