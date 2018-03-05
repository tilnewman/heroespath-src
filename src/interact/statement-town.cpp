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

        const std::string
            TownTalk::Compose(const player::Party & party, const map::Level::Enum LEVEL)
        {
            using namespace compose;

            auto const TownName{ [LEVEL]() { return map::Level::ToString(LEVEL); } };

            if ((misc::random::Int(6) == 0) && (DoesPartyHaveBeasts(party)))
            {
                return "They let you "
                    + Random(StrVec_t{ "walk around", "in here", "in " + TownName() }) + " with "
                    + Random(Combinations({ "that" }, { "beast", "creature", "thing" }))
                    + Random(StrVec_t{ "?", "?!" });
            }

            if ((misc::random::Int(6) == 0) && (DoesPartyHaveBeasts(party)))
            {
                return Random(StrVec_t{ "They ", "The guard ", "The guards " })
                    + Random(StrVec_t{ "allowed ", "let " })
                    + Random(StrVec_t{ "you in here ", "you in this town " }) + "with that beast"
                    + Random(StrVec_t{ "?", "?!" });
            }

            switch (misc::random::Int(7))
            {
                case 0:
                {
                    return Random(StrVec_t{ "You're not from ", "Not from " })
                        + Random(StrVec_t{ "these parts", "around here", TownName() })
                        + Random(StrVec_t{ " are you?", " eh?" });
                }
                case 1:
                {
                    return Random(StrVec_t{ "New to ", "You all must be new to " }) + TownName()
                        + ", eh?";
                }
                case 2:
                {
                    return "Welcome to " + TownName()
                        + Random(StrVec_t{
                              " " + Random(PartyNames()) + PeriodOrBang(), PeriodOrBang() });
                }
                case 3:
                {
                    return Random(StrVec_t{ "Are you ", "You " }) + "with the "
                        + Random(StrVec_t{ "town", TownName() }) + " guard?";
                }
                case 4:
                {
                    return CapFirstLetter(Random(WeatherAdjectives())) + " " + Random(TimesOfDay())
                        + PrependIf(Random(StrVec_t{ "here", "" }), " ")
                        + PrependIf(Random(StrVec_t{ "in " + TownName(), "" }), " ")
                        + Random(StrVec_t{ ".  Isn't it?", ".  Don't you think?", PeriodOrBang() });
                }
                case 5:
                {
                    return "This is " + Random(StrVec_t{ "such a ", "a " })
                        + Random(WeatherAdjectives()) + Random(StrVec_t{ " place.", " town." });
                }
                case 6:
                {
                    return "Ever " + Random(StrVec_t{ "been to ", "seen " }) + "such a "
                        + Random(WeatherAdjectives()) + Random(StrVec_t{ " place?", " town?" });
                }
                default:
                {
                    return Random(StrVec_t{ "I guess ", "So " })
                        + Random(StrVec_t{ "they",
                                           "the guard",
                                           "the guards",
                                           "the " + TownName() + " guard",
                                           "the " + TownName() + " guards" })
                        + Random(StrVec_t{ " will let anyone ", " let anyone " })
                        + Random(StrVec_t{ "in", "through the gate", "through the gates" })
                        + Random(StrVec_t{ " these days", " anymore", " nowadays" })
                        + PeriodOrBang();
                }
            }
        };
    } // namespace talk
} // namespace interact
} // namespace heroespath
