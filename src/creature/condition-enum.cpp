// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// condition-enum.cpp
//
#include "condition-enum.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"

#include <exception>
#include <sstream>

namespace heroespath
{
namespace creature
{

    const std::string Conditions::ToString(const Conditions::Enum E)
    {
        switch (E)
        {
            case Conditions::Good:
            {
                return "Good";
            }
            case Conditions::Bold:
            {
                return "Bold";
            }
            case Conditions::Heroic:
            {
                return "Heroic";
            }
            case Conditions::Daunted:
            {
                return "Daunted";
            }
            case Conditions::Panic:
            {
                return "Panic";
            }
            case Conditions::Dazed:
            {
                return "Dazed";
            }
            case Conditions::Tripped:
            {
                return "Tripped";
            }
            case Conditions::AsleepNatural:
            {
                return "AsleepNatural";
            }
            case Conditions::Poisoned:
            {
                return "Poisoned";
            }
            case Conditions::Pounced:
            {
                return "Pounced";
            }
            case Conditions::AsleepMagical:
            {
                return "AsleepMagical";
            }
            case Conditions::Unconscious:
            {
                return "Unconscious";
            }
            case Conditions::Stone:
            {
                return "Stone";
            }
            case Conditions::Dead:
            {
                return "Dead";
            }
            case Conditions::Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "ToString");
            }
        }
    }

    const std::string Conditions::Name(const Conditions::Enum E)
    {
        if (E == Conditions::AsleepNatural)
        {
            return "Asleep";
        }
        else if (E == Conditions::AsleepMagical)
        {
            return "Magical Sleep";
        }
        else
        {
            return ToString(E);
        }
    }

    const std::string Conditions::Desc(const Conditions::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-creature-condition-" << ToString(E) << "-desc";
        return misc::ConfigFile::Instance()->Value(keySS.str());
    }

    const std::string Conditions::ImageFilename(const Conditions::Enum E)
    {
        return boost::algorithm::to_lower_copy(ToString(E) + ".png");
    }

    namespace condition
    {
        // see comments in header
        const std::size_t Severity::ALL { 0 };
        const std::size_t Severity::GOOD { 1 };
        const std::size_t Severity::LEAST_BENEFITIAL { 2 };
        const std::size_t Severity::MOST_BENEFITIAL { 1000 };
        const std::size_t Severity::LEAST_HARMFUL { 1001 };
        const std::size_t Severity::MOST_HARMFUL { 2000 };

        // see comments in header
        std::size_t Severity::Get(const Conditions::Enum E)
        {
            switch (E)
            {
                case Conditions::Good:
                {
                    return GOOD;
                }
                case Conditions::Bold:
                {
                    return 100;
                }
                case Conditions::Heroic:
                {
                    return 400;
                }
                case Conditions::Daunted:
                {
                    return 1001;
                }
                case Conditions::Panic:
                {
                    return 1050;
                }
                case Conditions::Dazed:
                {
                    return 1100;
                }
                case Conditions::Tripped:
                {
                    return 1200;
                }
                case Conditions::AsleepNatural:
                {
                    return 1300;
                }
                case Conditions::Poisoned:
                {
                    return 1400;
                }
                case Conditions::Pounced:
                {
                    return 1450;
                }
                case Conditions::AsleepMagical:
                {
                    return 1500;
                }
                case Conditions::Unconscious:
                {
                    return 1800;
                }
                case Conditions::Stone:
                {
                    return 1900;
                }
                case Conditions::Dead:
                {
                    return 2000;
                }
                case Conditions::Count:
                default:
                {
                    std::ostringstream ss;
                    ss << "creature::Conditions::Enum::Severity(" << E << ")_InvalidValueError.";

                    throw std::range_error(ss.str());
                }
            }
        }
    } // namespace condition
} // namespace creature
} // namespace heroespath
