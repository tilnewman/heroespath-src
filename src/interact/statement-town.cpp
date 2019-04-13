// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
            const Mood MOOD, const creature::PlayerParty & PARTY, const map::Level::Enum LEVEL)
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
            TownTalk::ComposeMean(const creature::PlayerParty & PARTY, const map::Level::Enum LEVEL)
        {
            using namespace compose;

            const auto TownName { [LEVEL]() { return map::Level::ToString(LEVEL); } };

            if ((misc::Random(6) == 0) && (DoesPartyHaveBeasts(PARTY)))
            {
                return "They let you " + Random({ "walk around", "in here", "in " + TownName() })
                    + " with " + Random(Combinations({ "that" }, { "beast", "creature", "thing" }))
                    + Random({ "?", "?!" });
            }

            if ((misc::Random(6) == 0) && (DoesPartyHaveBeasts(PARTY)))
            {
                return Random({ "They ", "The guard ", "The guards " })
                    + Random({ "allowed ", "let " })
                    + Random({ "you in here ", "you in this town " }) + "with that beast"
                    + Random({ "?", "?!" });
            }

            if (misc::RandomBool())
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

        const std::string
            TownTalk::ComposeKind(const creature::PlayerParty &, const map::Level::Enum LEVEL)
        {
            using namespace compose;

            const auto TownName { [LEVEL]() { return map::Level::ToString(LEVEL); } };

            switch (misc::Random(5))
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
        }
    } // namespace talk
} // namespace interact
} // namespace heroespath
