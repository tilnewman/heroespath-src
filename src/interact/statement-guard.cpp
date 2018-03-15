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
// statement-guard.hpp
//
#include "statement-guard.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string GuardTalk::Compose(const Mood MOOD, const player::Party & PARTY)
        {
            using namespace compose;

            if ((misc::random::Int(6) == 0) && (DoesPartyHaveBeasts(PARTY)))
            {
                return Random({ "I'll have to check if ", "I don't know if " })
                    + Random({ "that ", "your " }) + BeastRaceName(PARTY)
                    + Random({ " is allowed.",
                               " is allowed here.",
                               " is permitted.",
                               " is permitted here." });
            }

            if (MOOD == Mood::Kind)
            {
                return ComposeKind();
            }
            else
            {
                return ComposeMean();
            }
        }

        const std::string GuardTalk::ComposeMean()
        {
            using namespace compose;

            switch (misc::random::Int(6))
            {
                case 0:
                {
                    return "None of you out-rank " + Random({ "me.", "me here." });
                }
                case 1:
                {
                    return "Move along" + Random({ ".", ", all of you.", ".  Move along." });
                }

                case 2:
                {
                    return Random({ "I'll report ", "I'm reporting " })
                        + Random({ "all of you ", "you " })
                        + Random({ "to the ", "directly to the ", "straight to the " })
                        + Random(RankHigh()) + PeriodOrBang();
                }
                case 3:
                {
                    return Random({ "You'll have to ", "You'll all have to " })
                        + Random({ "wait for the ", "wait here for the " }) + Random(RankHigh())
                        + ".";
                }
                case 4:
                {
                    return "Not now.  I'm on duty.";
                }
                case 5:
                {
                    return Random({ "I can't believe ",
                                    "Hard to believe ",
                                    "It's a shame ",
                                    "Shame " })
                        + "that they make me protect " + Random(PartyNames())
                        + Random({ " like you", " like all of you" }) + PeriodOrBang();
                }
                default:
                {
                    return "Step "
                        + Random({
                              "away",
                              "back",
                              "to the side",
                          })
                        + PeriodOrBang();
                }
            }
        }

        const std::string GuardTalk::ComposeKind()
        {
            using namespace compose;

            switch (misc::random::Int(7))
            {
                case 0:
                {
                    return "(salutes)";
                }
                case 1:
                {
                    return Random({ "Have any of you ", "Any of you ", "Have you ", "Has anyone " })
                        + Random({ "seen ", "heard from ", "spotted " })
                        + Random({ "that ", "that pathetic " }) + Random(RankLow()) + "?";
                }
                case 2:
                {
                    return Random({ "Have any of you ", "Any of you ", "Have you ", "Has anyone " })
                        + Random({ "seen the ", "heard from the ", "spotted the " })
                        + Random(RankHigh()) + "?";
                }
                case 3:
                {
                    return Random({ "I'm waiting for the ", "I take orders from the ", "" })
                        + Random(RankHigh()) + ".";
                }
                case 4:
                {
                    return Random(RankLow()) + Random({ "!", "!  Get over here!", "!  Report!" });
                }
                case 5:
                {
                    return Random({ "All you ", "All of you ", "You " }) + Random(PartyNames())
                        + Random({ " will have to wait for ", " can talk to " })
                        + Random(RankHigh()) + ".";
                }
                case 6:
                {
                    return Random({ "I take orders from the ",
                                    "I take my orders from the ",
                                    "Orders come from the " })
                        + Random(RankHigh()) + PeriodOrBang();
                }
                default:
                {
                    return Random({ "Yes, ", "Yes, yes, ", "Of course " })
                        + Random({ "the guard ", "this guard ", "guards " })
                        + " will protect even you " + Random(PartyNames()) + ".";
                }
            };
        }
    } // namespace talk
} // namespace interact
} // namespace heroespath
