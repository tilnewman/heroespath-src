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
// statement-town.hpp
//
#include "statement-town.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string TownTalk::Compose(
            const Mood MOOD, const player::Party & PARTY, const map::Level::Enum LEVEL)
        {
            if (MOOD == Mood::Kind)
            {
                return ComposeKind(PARTY, LEVEL);
            }
            else
            {
                return ComposeMean(PARTY, LEVEL);
            }
        }

        const std::string
            TownTalk::ComposeMean(const player::Party & PARTY, const map::Level::Enum LEVEL)
        {
            using namespace compose;

            auto const TownName{ [LEVEL]() { return map::Level::ToString(LEVEL); } };

            if ((misc::random::Int(6) == 0) && (DoesPartyHaveBeasts(PARTY)))
            {
                return "They let you " + Random({ "walk around", "in here", "in " + TownName() })
                    + " with " + Random(Combinations({ "that" }, { "beast", "creature", "thing" }))
                    + Random({ "?", "?!" });
            }

            if ((misc::random::Int(6) == 0) && (DoesPartyHaveBeasts(PARTY)))
            {
                return Random({ "They ", "The guard ", "The guards " })
                    + Random({ "allowed ", "let " })
                    + Random({ "you in here ", "you in this town " }) + "with that beast"
                    + Random({ "?", "?!" });
            }

            if (misc::random::Bool())
            {
                return Random({ "You're not from ", "Not from " })
                    + Random({ "these parts", "around here", TownName() })
                    + Random({ " are you?", " eh?" });
            }
            else
            {
                return Random({ "I guess ", "So " })
                    + Random({ "they",
                               "the guard",
                               "the guards",
                               "the " + TownName() + " guard",
                               "the " + TownName() + " guards" })
                    + Random({ " will let anyone ", " let anyone " })
                    + Random({ "in", "through the gate", "through the gates" })
                    + Random({ " these days", " anymore", " nowadays" }) + PeriodOrBang();
            }
        }

        const std::string TownTalk::ComposeKind(const player::Party &, const map::Level::Enum LEVEL)
        {
            using namespace compose;

            auto const TownName{ [LEVEL]() { return map::Level::ToString(LEVEL); } };

            switch (misc::random::Int(5))
            {
                case 0:
                {
                    return "Ever " + Random({ "been to ", "seen " }) + "such a "
                        + Random(WeatherAdjectives()) + Random({ " place?", " town?" });
                }
                case 1:
                {
                    return Random({ "New to ", "You all must be new to " }) + TownName() + ", eh?";
                }
                case 2:
                {
                    return "Welcome to " + TownName()
                        + Random({ " " + Random(PartyNames()) + PeriodOrBang(), PeriodOrBang() });
                }
                case 3:
                {
                    return Random({ "Are you ", "You " }) + "with the "
                        + Random({ "town", TownName() }) + " guard?";
                }
                case 4:
                {
                    return CapFirstLetter(Random(WeatherAdjectives())) + " " + Random(TimesOfDay())
                        + PrependIf(Random({ "here", "" }), " ")
                        + PrependIf(Random({ "in " + TownName(), "" }), " ")
                        + Random({ ".  Isn't it?", ".  Don't you think?", PeriodOrBang() });
                }
                default:
                {
                    return "This is " + Random({ "such a ", "a " }) + Random(WeatherAdjectives())
                        + Random({ " place.", " town." });
                }
            }
        };
    } // namespace talk
} // namespace interact
} // namespace heroespath
