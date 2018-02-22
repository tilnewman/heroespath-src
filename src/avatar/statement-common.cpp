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
namespace avatar
{
namespace talk
{

    const std::string CommonTalk::Compose(const player::Party & party)
    {
        using namespace compose;

        if ((misc::random::Int(10) == 0) && (DoesPartyHaveWolfens(party)))
        {
            return
                Random(StrVec_t{ "Nice ", "Good " }) +
                Random(StrVec_t{ "dog", "doggy" }) +
                Random(StrVec_t{ ".  Easy", ".  Easy now", ".  Down", ".  Sit", ".  Roll over" }) +
                PeriodOrBang();
        }

        if ((misc::random::Int(5) == 0) && (DoesPartyHaveBeasts(party)))
        {
            auto const RAND_NUM{ misc::random::Int(4) };
            if (RAND_NUM == 0)
            {
                return
                    AppendIf(Random(StrVec_t{ "Wow" + PeriodOrBang(), "Whoa" + PeriodOrBang(), "Well I'll be", "Yikes", "" })) +
                    Random(StrVec_t{ "That's  quite a ", "That's some ", "Some " }) +
                    Random(StrVec_t{ "pet", "pet you've got there", "pet you have", "pet following you around" }) +
                    PeriodOrBang();
            }
            else if (RAND_NUM == 1)
            {
                return
                    Random(StrVec_t{ "You don't ", "Don't " }) +
                    Random(StrVec_t{ "see ", "see many who travel with ", "see many who keep " }) +
                    Random(StrVec_t{ "such ", "those ", "" }) +
                    Random(StrVec_t{ "beasts ", "critters", "creatures" }) +
                    Random(StrVec_t{ "anymore.", "these days.", "nowadays." });
            }
            else if (RAND_NUM == 2)
            {
                return
                    Random(StrVec_t{
                        "Beasts!  ",
                        "Wild Beasts!  ",
                        CapFirstLetter(BeastRaceName(party)) + "!  ",
                        "Wild " + CapFirstLetter(BeastRaceName(party)) + "!  " }) +
                        Random(StrVec_t{ "Help!", "Help!  Guards!" });
            }
            else if (RAND_NUM == 3)
            {
                return
                    "Take your " +
                    Random(StrVec_t{
                        "wretched",
                        "disgusting",
                        "wild",
                        "dangerous",
                        "stinking",
                        "horrible" }) +
                    Random(StrVec_t{ " beasts", " creatures", " pets", " pests" }) +
                    Random(StrVec_t{ " somewhere else", " elsewhere" }) +
                    PeriodOrBang();
            }
            else
            {
                return
                    Random(StrVec_t{ "Does ", "Will " }) +
                    Random(StrVec_t{ "that ", "your " }) +
                    Random(StrVec_t{ BeastRaceName(party), "pet", "critter" }) +
                    " bite?";
            }
        }

        switch(misc::random::Int(23))
        {
            case 0:
            {
                return CapFirstLetter(Random(PartyNames())) + Random(StrVec_t{ "?", "!" });
            }
            
            case 1:
            {
                return
                    "Six " +
                    Random(PartyNames()) +
                    Random(StrVec_t{ "?  Well ", "?  Greetings, " }) +
                    Random(StrVec_t{ "what is it ", "what do you " }) +
                    Random(StrVec_t{ "all ", "six " }) +
                    "want?";
            }
            case 2:
            {
                return
                    CapFirstLetter(Random(Combinations(
                        WeatherAdjectives(),
                        { "morning", "afternoon", "evening" }))) +
                    " " +
                    Random(PartyNames()) +
                    ".";
            }
            case 3:
            {
                return
                    RandomOrEmpty(StrVec_t{ "Just another ", "Yet another " }) +
                    Random(WeatherAdjectives()) +
                    " " +
                    Random(TimesOfDay()) +
                    ".";
            }
            case 4:
            {
                return
                    Random(StrVec_t{ "Pleased ", "A pleasure " }) +
                    "to meet " +
                    Random(StrVec_t{ "all ", "all six ", "the six " }) +
                    Random(StrVec_t{ "of you ", "of you " + Random(PartyNames()) + " " }) +
                    Random(StrVec_t{ "this ", "on this " }) +
                    AppendIf(RandomOrEmpty(WeatherAdjectives()), " ") + 
                    Random(TimesOfDay()) +
                    PeriodOrBang();
            }
            case 5:
            {
                return
                    Random(StrVec_t{ "Um, ", "" }) +
                    Random(StrVec_t({ "Huh?", "What?", "Eh?" }));
            }
            case 6:
            {
                return
                    Random(StrVec_t{ "Well? ", "" }) +
                    Random(StrVec_t{ "Out with it", "Come on, out with it", "Come on then, out with it" }) +
                    PeriodOrBang();
            }
            case 7:
            {
                return
                    Random(StrVec_t{
                        "A fair greeting ",
                        "Greetings ",
                        "Howdy ",
                        "Well met ",
                        "Hello there " }) +
                    Random(PartyNames()) +
                    PeriodOrBang();
            }
            case 8:
            {
                return "Yes?" + Random(StrVec_t{ "  What?", "  Eh?", "  Me?", "" });
            }
            case 9:
            {
                return
                    Random(StrVec_t{ "Greetings" + PeriodOrBang() + "  What ", "So, what ", "What " }) +
                    Random(StrVec_t{ "do ", "could " }) +
                    Random(StrVec_t{ "all of ", "" }) +
                    "you " +
                    Random(StrVec_t{ "guys ", Random(PartyNames()) + " ", "" }) +
                    Random(StrVec_t{ "want?", "want from me?", "want of me?" });
            }
            case 10:
            {
                return Random(StrVec_t{ "Well?  What now?", "So what now?", "What now?" });
            }
            case 11:
            {
                return Random(StrVec_t{ "Peace!  ", "Mercy!  ", "Be merciful!  ", "" }) +
                    "I " +
                    Random(StrVec_t{ "don't have any ", "got no ", "have only a few ", "don't carry " }) +
                    "coins!";
            }
            case 12:
            {
                return
                    Random(StrVec_t{ "As I live and breathe.  ", "Stars upon me.  ", "" }) +
                    Random(StrVec_t{ "Six ", "Six " + Random(PartyNames()) + " " }) +
                    "as I've never seen" +
                    PeriodOrBang();
            }
            case 13:
            {
                return
                    Random(StrVec_t{ "Six ", "A friendly six ", "Six kindly " }) +
                    Random(StrVec_t{ "greetings", "hellos" }) +
                    Random(StrVec_t{ " to you", " to you all", "" }) +
                    PeriodOrBang();
            }
            case 14:
            {
                return
                    Random(StrVec_t{ "Begone, all of you", "Begone with all of you" }) +
                    PeriodOrBang();
            }
            case 15:
            {
                return
                    "Leave me " +
                    Random(StrVec_t{ "be", "alone", "in peace" }) +
                    PrependIf(Random(StrVec_t{ "you", "" }), " ") +
                    PrependIf(Random(StrVec_t{ "freaks", Random(PartyNames()), "" }), " ") +
                    PeriodOrBang();
            }
            case 16:
            {
                return
                    Random(StrVec_t{ "Away with ", "Begone " }) +
                    Random(StrVec_t{ "all six of ", "" }) +
                    Random(StrVec_t{ "you", "you " + Random(PartyNames()) }) +
                    PeriodOrBang();
            }
            case 17:
            {
                return
                    Random(StrVec_t{
                        "Warmest greetings, ",
                        "Greetings, ",
                        "A fine hello, ",
                        "Regards, ",
                        "A friendly hello, ",
                        "A kindly hello, " }) +
                    Random(StrVec_t{ "and now goodbye", "and goodbye" }) +
                    PeriodOrBang();
            }
            case 18:
            {
                return
                    Random(StrVec_t{
                        "Warmest greetings.  ",
                        "Greetings.  ",
                        "A fine hello.  ",
                        "Regards to you.  ",
                        "A friendly hello.  " }) +
                    "Now leave me alone" +
                    PeriodOrBang();
            }
            case 19:
            {
                return
                    Random(StrVec_t{
                        "I've no time for ",
                        "I'm far too important for ",
                        "I'm too busy for " }) +
                    Random(StrVec_t{ "the likes of you", "you" }) +
                    Random(StrVec_t{ " " + Random(PartyNames()), "" }) +
                    PeriodOrBang();
            }
            case 20:
            {
                return
                    Random(StrVec_t{ "No no.  Not ", "Not " }) +
                    Random(StrVec_t{ "right", "just" }) +
                    " now.";
            }
            case 21:
            {
                return "Not now.  Not ever" + PeriodOrBang();
            }
            case 22:
            {
                return
                    "Good day " +
                    Random(StrVec_t{ "to you", "to all of you", "to all six of you" }) + 
                    PeriodOrBang();
            }
            default:
            {
                return
                    Random(StrVec_t{ "Go bother ", "Bother ", "Go irritate ", "Irritate " }) +
                    Random(StrVec_t{ "somebody ", "someone " }) +
                    "else" +
                    PeriodOrBang();
            }
        }
    };

}
}
}
