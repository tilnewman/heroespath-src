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
// statement-common.hpp
//
#include "statement-common.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string CommonTalk::Compose(const player::Party & party)
        {
            using namespace compose;

            if ((misc::random::Int(10) == 0) && (DoesPartyHaveWolfens(party)))
            {
                return Random({ "Nice ", "Good " }) + Random({ "dog", "doggy" })
                    + Random({ ".  Easy", ".  Easy now", ".  Down", ".  Sit", ".  Roll over" })
                    + PeriodOrBang();
            }

            if ((misc::random::Int(5) == 0) && (DoesPartyHaveBeasts(party)))
            {
                auto const RAND_NUM{ misc::random::Int(4) };
                if (RAND_NUM == 0)
                {
                    return AppendIf(Random({ "Wow" + PeriodOrBang(),
                                             "Whoa" + PeriodOrBang(),
                                             "Well I'll be",
                                             "Yikes",
                                             "" }))
                        + Random({ "That's  quite a ", "That's some ", "Some " })
                        + Random({ "pet",
                                   "pet you've got there",
                                   "pet you have",
                                   "pet following you around" })
                        + PeriodOrBang();
                }
                else if (RAND_NUM == 1)
                {
                    return Random({ "You don't ", "Don't " })
                        + Random({ "see ", "see many who travel with ", "see many who keep " })
                        + Random({ "such ", "those ", "" })
                        + Random({ "beasts ", "critters", "creatures" })
                        + Random({ "anymore.", "these days.", "nowadays." });
                }
                else if (RAND_NUM == 2)
                {
                    return Random({ "Beasts!  ",
                                    "Wild Beasts!  ",
                                    CapFirstLetter(BeastRaceName(party)) + "!  ",
                                    "Wild " + CapFirstLetter(BeastRaceName(party)) + "!  " })
                        + Random({ "Help!", "Help!  Guards!" });
                }
                else if (RAND_NUM == 3)
                {
                    return "Take your "
                        + Random({ "wretched",
                                   "disgusting",
                                   "wild",
                                   "dangerous",
                                   "stinking",
                                   "horrible" })
                        + Random({ " beasts", " creatures", " pets", " pests" })
                        + Random({ " somewhere else", " elsewhere" }) + PeriodOrBang();
                }
                else
                {
                    return Random({ "Does ", "Will " }) + Random({ "that ", "your " })
                        + Random({ BeastRaceName(party), "pet", "critter" }) + " bite?";
                }
            }

            switch (misc::random::Int(23))
            {
                case 0:
                {
                    return CapFirstLetter(Random(PartyNames())) + Random({ "?", "!" });
                }

                case 1:
                {
                    return "Six " + Random(PartyNames()) + Random({ "?  Well ", "?  Greetings, " })
                        + Random({ "what is it ", "what do you " }) + Random({ "all ", "six " })
                        + "want?";
                }
                case 2:
                {
                    return CapFirstLetter(Random(Combinations(
                               WeatherAdjectives(), { "morning", "afternoon", "evening" })))
                        + " " + Random(PartyNames()) + ".";
                }
                case 3:
                {
                    return RandomOrEmpty({ "Just another ", "Yet another " })
                        + Random(WeatherAdjectives()) + " " + Random(TimesOfDay()) + ".";
                }
                case 4:
                {
                    return Random({ "Pleased ", "A pleasure " }) + "to meet "
                        + Random({ "all ", "all six ", "the six " })
                        + Random({ "of you ", "of you " + Random(PartyNames()) + " " })
                        + Random({ "this ", "on this " })
                        + AppendIf(RandomOrEmpty(WeatherAdjectives()), " ") + Random(TimesOfDay())
                        + PeriodOrBang();
                }
                case 5:
                {
                    return Random({ "Um, ", "" }) + Random({ "Huh?", "What?", "Eh?" });
                }
                case 6:
                {
                    return Random({ "Well? ", "" })
                        + Random({ "Out with it",
                                   "Come on, out with it",
                                   "Come on then, out with it" })
                        + PeriodOrBang();
                }
                case 7:
                {
                    return Random({ "A fair greeting ",
                                    "Greetings ",
                                    "Howdy ",
                                    "Well met ",
                                    "Hello there " })
                        + Random(PartyNames()) + PeriodOrBang();
                }
                case 8:
                {
                    return "Yes?" + Random({ "  What?", "  Eh?", "  Me?", "" });
                }
                case 9:
                {
                    return Random(
                               { "Greetings" + PeriodOrBang() + "  What ", "So, what ", "What " })
                        + Random({ "do ", "could " }) + Random({ "all of ", "" }) + "you "
                        + Random({ "guys ", Random(PartyNames()) + " ", "" })
                        + Random({ "want?", "want from me?", "want of me?" });
                }
                case 10:
                {
                    return Random({ "Well?  What now?", "So what now?", "What now?" });
                }
                case 11:
                {
                    return Random({ "Peace!  ", "Mercy!  ", "Be merciful!  ", "" }) + "I "
                        + Random(
                               { "don't have any ", "got no ", "have only a few ", "don't carry " })
                        + "coins!";
                }
                case 12:
                {
                    return Random({ "As I live and breathe.  ", "Stars upon me.  ", "" })
                        + Random({ "Six ", "Six " + Random(PartyNames()) + " " })
                        + "as I've never seen" + PeriodOrBang();
                }
                case 13:
                {
                    return Random({ "Six ", "A friendly six ", "Six kindly " })
                        + Random({ "greetings", "hellos" })
                        + Random({ " to you", " to you all", "" }) + PeriodOrBang();
                }
                case 14:
                {
                    return Random({ "Begone, all of you", "Begone with all of you" })
                        + PeriodOrBang();
                }
                case 15:
                {
                    return "Leave me " + Random({ "be", "alone", "in peace" })
                        + PrependIf(Random({ "you", "" }), " ")
                        + PrependIf(Random({ "freaks", Random(PartyNames()), "" }), " ")
                        + PeriodOrBang();
                }
                case 16:
                {
                    return Random({ "Away with ", "Begone " }) + Random({ "all six of ", "" })
                        + Random({ "you", "you " + Random(PartyNames()) }) + PeriodOrBang();
                }
                case 17:
                {
                    return Random({ "Warmest greetings, ",
                                    "Greetings, ",
                                    "A fine hello, ",
                                    "Regards, ",
                                    "A friendly hello, ",
                                    "A kindly hello, " })
                        + Random({ "and now goodbye", "and goodbye" }) + PeriodOrBang();
                }
                case 18:
                {
                    return Random({ "Warmest greetings.  ",
                                    "Greetings.  ",
                                    "A fine hello.  ",
                                    "Regards to you.  ",
                                    "A friendly hello.  " })
                        + "Now leave me alone" + PeriodOrBang();
                }
                case 19:
                {
                    return Random({ "I've no time for ",
                                    "I'm far too important for ",
                                    "I'm too busy for " })
                        + Random({ "the likes of you", "you" })
                        + Random({ " " + Random(PartyNames()), "" }) + PeriodOrBang();
                }
                case 20:
                {
                    return Random({ "No no.  Not ", "Not " }) + Random({ "right", "just" })
                        + " now.";
                }
                case 21:
                {
                    return "Not now.  Not ever" + PeriodOrBang();
                }
                case 22:
                {
                    return "Good day " + Random({ "to you", "to all of you", "to all six of you" })
                        + PeriodOrBang();
                }
                default:
                {
                    return Random({ "Go bother ", "Bother ", "Go irritate ", "Irritate " })
                        + Random({ "somebody ", "someone " }) + "else" + PeriodOrBang();
                }
            }
        };
    } // namespace talk
} // namespace interact
} // namespace heroespath
