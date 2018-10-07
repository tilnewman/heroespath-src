// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// spell-enum.cpp
//
#include "spell-enum.hpp"

#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"

namespace heroespath
{
namespace spell
{

    const std::string Spells::ToString(const Spells::Enum E)
    {
        switch (E)
        {
            case Sparks:
            {
                return "Sparks";
            }
            case Bandage:
            {
                return "Bandage";
            }
            case Sleep:
            {
                return "Sleep";
            }
            case Awaken:
            {
                return "Awaken";
            }
            case Trip:
            {
                return "Trip";
            }
            case Lift:
            {
                return "Lift";
            }
            case Daze:
            {
                return "Daze";
            }
            case Panic:
            {
                return "Panic";
            }
            case ClearMind:
            {
                return "ClearMind";
            }
            case Poison:
            {
                return "Poison";
            }
            case Antidote:
            {
                return "Antidote";
            }
            case PoisonCloud:
            {
                return "PoisonCloud";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "ToString");
            }
        }
    }

    const std::string Spells::Name(const Spells::Enum E)
    {
        switch (E)
        {
            case Sparks:
            {
                return "Sparks";
            }
            case Bandage:
            {
                return "Bandage";
            }
            case Sleep:
            {
                return "Sleep";
            }
            case Awaken:
            {
                return "Awaken";
            }
            case Trip:
            {
                return "Trip";
            }
            case Lift:
            {
                return "Lift";
            }
            case Daze:
            {
                return "Daze";
            }
            case Panic:
            {
                return "Panic";
            }
            case ClearMind:
            {
                return "Clear Mind";
            }
            case Poison:
            {
                return "Poison";
            }
            case Antidote:
            {
                return "Antidote";
            }
            case PoisonCloud:
            {
                return "Poison Cloud";
            }
            case Count:
            default:
            {
                ThrowInvalidValueForFunction(E, "Name");
            }
        }
    }

    const std::string Spells::ShortDesc(const Spells::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-spell-" << ToString(E) << "-short-desc";
        return misc::ConfigFile::Instance()->Value(keySS.str());
    }

    const std::string Spells::ExtraDesc(const Spells::Enum E)
    {
        std::ostringstream keySS;
        keySS << "heroespath-spell-" << ToString(E) << "-extra-desc";
        return misc::ConfigFile::Instance()->Value(keySS.str());
    }

    const std::string Spells::ImageFilename(const Spells::Enum E)
    {
        return boost::algorithm::to_lower_copy(ToString(E) + ".png");
    }

} // namespace spell
} // namespace heroespath
