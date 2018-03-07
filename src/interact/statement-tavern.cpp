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
// statement-tavern.hpp
//
#include "statement-tavern.hpp"
#include "player/character.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string TavernTalk::Compose(const player::Party & party)
        {
            using namespace compose;

            if ((misc::random::Int(10) == 0) && (DoesPartyHaveBeasts(party)))
            {
                auto const BEAST_RACE_NAME{ (
                    (DoesPartyHaveWolfens(party)) ? std::string("wolfen ")
                                                  : std::string("dragon ")) };

                if ((misc::random::Int(7) == 0) && (DoesPartyHaveBeasts(party)))
                {
                    return "Is that " + std::string(BEAST_RACE_NAME) + "allowed in here?";
                }

                auto const FIRST_PART{ Random(
                    { "Does your " + std::string(BEAST_RACE_NAME),
                      "Nice " + std::string(BEAST_RACE_NAME) + ".  Does he" }) };

                std::string secondPart{ "" };
                switch (misc::random::Int(6))
                {
                    case 0:
                    {
                        secondPart = Random({ "burp", "fart", "cough up", "hack up", "poop" }) + " "
                            + ((DoesPartyHaveWolfens(party)) ? "hairballs" : "lava");

                        break;
                    }
                    case 1:
                    {
                        secondPart = "eat " + Random(RaceNames()) + "s";
                        break;
                    }
                    case 2:
                    {
                        secondPart = "like " + Random(RoleNames()) + "s";
                        break;
                    }
                    case 3:
                    {
                        secondPart = "house borken";
                        break;
                    }
                    case 4:
                    {
                        secondPart + Random({ "like ", "drink " }) + Random(TavernDrinks());
                        break;
                    }
                    case 5:
                    {
                        secondPart = "drink " + Random(TavernDrinks()) + " with us";
                        break;
                    }
                    default:
                    {
                        secondPart = "sing drinking songs" + Random({ " with us", " too", "" });
                        break;
                    }
                }

                return AppendIf(RandomOrEmpty(TavernNoises())) + FIRST_PART + secondPart + "?";
            }

            switch (misc::random::Int(24))
            {
                case 0:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + "Who said that?";
                }
                case 1:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + "I've had too much wine women and song.  "
                        + AppendIf(RandomOrEmpty(TavernNoises())) + "So don't sing so loud.";
                }
                case 2:
                {
                    return Random({ "Do I look like the ", "Do I look like the " })
                        + Random(TavernServers()) + "?";
                }
                case 3:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + "How dry I am.  "
                        + AppendIf(RandomOrEmpty(TavernNoises())) + "Nobody cares...";
                }
                case 4:
                {
                    return Random({ "I'm here to ", "I'd rather " })
                        + Random({ "drink my ", "drown my " }) + Random({ "woes ", "sorrows " })
                        + "alone.";
                }
                case 5:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + "Why did "
                        + Random({ "he", "she" }) + " leave me "
                        + Random(std::vector<StrVec_t>{
                              Combinations(
                                  { "for my" }, { "brother", "sister", "cousin", "friend" }),
                              Combinations({ "for that", "for a" }, { RandomRaceOrRole() }) })
                        + "?" + PrependIf(RandomOrEmpty(TavernNoises()));
                }
                case 6:
                {
                    return Random(TavernNoises()) + "  I really "
                        + Random({ "love", "hate", "pity" }) + " all of you"
                        + PrependIf(RandomOrEmpty(PartyNames()), " ") + ".";
                }
                case 7:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + CapFirstLetter(Random(TavernServers())) + "!  "
                        + AppendIf(RandomOrEmpty(TavernNoises())) + "Another round of "
                        + Random(TavernDrinks()) + " for me and my six new friends here.";
                }
                case 8:
                {
                    return Random({ "Why do they ", "They shouldn't " })
                        + Random({ "allow ", "let " }) + "your kind in here.";
                }
                case 9:
                {
                    return "Ninety-nine bottles of beer on the wall...";
                }
                case 10:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + "Who's ready for another song?!";
                }
                case 11:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + "Watch me "
                        + Random({ "balance", "pour" }) + " this " + Random(TavernDrinks()) + " on "
                        + Random({ "his", "her", "my", "your", "that guy's" }) + " head.";
                }
                case 12:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + CapFirstLetter(Random(TavernServers())) + "!"
                        + PrependIf(RandomOrEmpty(TavernNoises())) + "  More "
                        + Random(TavernDrinks()) + "!";
                }
                case 13:
                {
                    return "They " + Random({ "serve", "serve only", "pour", "pour only" })
                        + " the finest " + Random(RaceNames()) + " piss around here"
                        + PeriodOrBang();
                }
                case 14:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + "Watch how much of this "
                        + AppendIf(RandomOrEmpty(TavernDrinkAdjectives()), " ")
                        + Random(TavernDrinks()) + " I can "
                        + Random({ "guzzle", "drink", "chug", "spill", "piss on the floor" })
                        + PeriodOrBang();
                }
                case 15:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + Random({ "Hey, who ", "Who " })
                        + "stole my " + AppendIf(RandomOrEmpty(TavernDrinkAdjectives()), " ")
                        + Random(TavernDrinks()) + "?";
                }
                case 16:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + "Let's have another"
                        + Random({ " drinking ", " " }) + "song!";
                }
                case 17:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + "Why did the "
                        + Random(JokeAdjectives()) + " " + Random(RoleNames()) + " cross the road?";
                }
                case 18:
                {
                    return "Hey.  " + AppendIf(RandomOrEmpty(TavernNoises()))
                        + Random({ "Hey you.", "Hey." }) + "  Watch me "
                        + Random({ "drink", "spill" }) + " this "
                        + AppendIf(RandomOrEmpty(TavernDrinkAdjectives()), " ")
                        + Random(TavernDrinks()) + ".";
                }
                case 19:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + Random({ "I've had", "I think I've had" }) + " too much of this "
                        + AppendIf(RandomOrEmpty(TavernDrinkAdjectives()), " ")
                        + Random(TavernDrinks()) + ".";
                }
                case 20:
                {
                    return "What are " + AppendIf(RandomOrEmpty({ "all", "all of" }), " ")
                        + "you looking at" + Random({ "?", "?!" });
                }
                case 21:
                {
                    return "You " + Random(PartyNames()) + " talk'n to me" + Random({ "?", "?!" });
                }
                case 22:
                {
                    return "You look like you could use "
                        + Random(std::vector<StrVec_t>{
                              { "a drink", "a beer", "an ale" },
                              { "some of this "
                                + Random(Combinations(TavernDrinkAdjectives(), TavernDrinks())) } })
                        + ".";
                }
                case 23:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + "Have you heard the one about the " + Random(JokeAdjectives()) + " "
                        + Random(RoleNames()) + "?";
                }
                case 24:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + "Have you heard the one about the " + Random(RaceNames()) + " "
                        + Random(JokeDescriptions()) + "?";
                }
                default:
                {
                    return Random(TavernNoises()) + PrependIf(RandomOrEmpty(TavernNoises()))
                        + PrependIf(RandomOrEmpty(TavernNoises()));
                }
            }
        };
    } // namespace talk
} // namespace interact
} // namespace heroespath
