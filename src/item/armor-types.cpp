// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// armor-types.cpp
//
#include "armor-types.hpp"

#include "misc/log-macros.hpp"

namespace heroespath
{
namespace item
{
    namespace armor
    {

        const std::string shield_type::ToString(const shield_type::Enum SHEILD_TYPE)
        {
            switch (SHEILD_TYPE)
            {
                case Buckler:
                {
                    return "Buckler";
                }
                case Kite:
                {
                    return "Kite";
                }
                case Heater:
                {
                    return "Heater";
                }
                case Pavis:
                {
                    return "Pavis";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(ValueOutOfRangeErrorString(SHEILD_TYPE));
                    return "";
                }
            }
        }

        const std::string helm_type::ToString(const helm_type::Enum HELM_TYPE)
        {
            switch (HELM_TYPE)
            {
                case Leather:
                {
                    return "Leather";
                }
                case MailCoif:
                {
                    return "MailCoif";
                }
                case Kettle:
                {
                    return "Kettle";
                }
                case Archers:
                {
                    return "Archers";
                }
                case Bascinet:
                {
                    return "Bascinet";
                }
                case Great:
                {
                    return "Great";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(ValueOutOfRangeErrorString(HELM_TYPE));
                    return "";
                }
            };
        }

        const std::string helm_type::Name(const helm_type::Enum HELM_TYPE)
        {
            if (HELM_TYPE == MailCoif)
            {
                return "Mail Coif";
            }
            else
            {
                return ToString(HELM_TYPE);
            }
        }

        const std::string base_type::ToString(const base_type::Enum BASE_TYPE)
        {
            switch (BASE_TYPE)
            {
                case Plain:
                {
                    return "Plain";
                }
                case Mail:
                {
                    return "Mail";
                }
                case Scale:
                {
                    return "Scale";
                }
                case Plate:
                {
                    return "Plate";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(ValueOutOfRangeErrorString(BASE_TYPE));
                    return "";
                }
            };
        }

        const std::string cover_type::ToString(const cover_type::Enum COVER_TYPE)
        {
            switch (COVER_TYPE)
            {
                case Cape:
                {
                    return "Cape";
                }
                case Vest:
                {
                    return "Vest";
                }
                case Robe:
                {
                    return "Robe";
                }
                case Cloak:
                {
                    return "Cloak";
                }
                case Count:
                {
                    return "(Count)";
                }
                default:
                {
                    M_HP_LOG_ERR(ValueOutOfRangeErrorString(COVER_TYPE));
                    return "";
                }
            };
        }

    } // namespace armor
} // namespace item
} // namespace heroespath
