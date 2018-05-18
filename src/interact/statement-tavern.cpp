// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// statement-tavern.hpp
//
#include "statement-tavern.hpp"
#include "creature/creature.hpp"
#include <vector>

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string TavernTalk::Compose(const Mood MOOD, const creature::PlayerParty & PARTY)
        {
            if (MOOD == Mood::Kind)
            {
                return ComposeKind(PARTY);
            }
            else
            {
                return ComposeMean(PARTY);
            }
        }

        const std::string TavernTalk::ComposeMean(const creature::PlayerParty & PARTY)
        {
            using namespace compose;

            if ((misc::random::Int(10) == 0) && (DoesPartyHaveBeasts(PARTY)))
            {
                auto const BEAST_RACE_NAME{ (
                    (DoesPartyHaveWolfens(PARTY)) ? std::string("wolfen ")
                                                  : std::string("dragon ")) };

                return "Is that " + std::string(BEAST_RACE_NAME) + "allowed in here?";
            }

            switch (misc::random::Int(5))
            {
                case 1:
                {
                    return Random({ "Do I look like the ", "Do I look like the " })
                        + Random(TavernServers()) + "?";
                }
                case 2:
                {
                    return Random({ "I'm here to ", "I'd rather " })
                        + Random({ "drink my ", "drown my " }) + Random({ "woes ", "sorrows " })
                        + "alone.";
                }
                case 3:
                {
                    return Random({ "Why do they ", "They shouldn't " })
                        + Random({ "allow ", "let " }) + "your kind in here.";
                }
                case 4:
                {
                    return "What are " + AppendIf(RandomOrEmpty({ "all", "all of" }), " ")
                        + "you looking at" + Random({ "?", "?!" });
                }
                default:
                {
                    return "You " + Random(PartyNames()) + " talk'n to me" + Random({ "?", "?!" });
                }
            }
        }

        const std::string TavernTalk::ComposeKind(const creature::PlayerParty & PARTY)
        {
            using namespace compose;

            if ((misc::random::Int(10) == 0) && (DoesPartyHaveBeasts(PARTY)))
            {
                auto const BEAST_RACE_NAME{ (
                    (DoesPartyHaveWolfens(PARTY)) ? std::string("wolfen ")
                                                  : std::string("dragon ")) };

                auto const FIRST_PART{ Random(
                    { "Does your " + std::string(BEAST_RACE_NAME),
                      "Nice " + std::string(BEAST_RACE_NAME) + ".  Does he" }) };

                std::string secondPart{ "" };
                switch (misc::random::Int(6))
                {
                    case 0:
                    {
                        secondPart = Random({ "burp", "fart", "cough up", "hack up", "poop" }) + " "
                            + ((DoesPartyHaveWolfens(PARTY)) ? "hairballs" : "lava");

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
                        secondPart = Random({ "like ", "drink " }) + Random(TavernDrinks());
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

            switch (misc::random::Int(20))
            {
                case 0:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + "Who said that?";
                }
                case 1:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + "Have you heard the one about the " + Random(RaceNames()) + " "
                        + Random(JokeDescriptions()) + "?";
                }
                case 2:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + "I've had too much wine women and song.  "
                        + AppendIf(RandomOrEmpty(TavernNoises())) + "So don't sing so loud.";
                }
                case 3:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises())) + "How dry I am.  "
                        + AppendIf(RandomOrEmpty(TavernNoises())) + "Nobody cares...";
                }
                case 4:
                {
                    return AppendIf(RandomOrEmpty(TavernNoises()))
                        + "Have you heard the one about the " + Random(JokeAdjectives()) + " "
                        + Random(RoleNames()) + "?";
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
                    return "You look like you could use "
                        + Random(std::vector<StrVec_t>{
                              { "a drink", "a beer", "an ale" },
                              { "some of this "
                                + Random(Combinations(TavernDrinkAdjectives(), TavernDrinks())) } })
                        + ".";
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
                default:
                {
                    return Random(TavernNoises()) + PrependIf(RandomOrEmpty(TavernNoises()))
                        + PrependIf(RandomOrEmpty(TavernNoises()));
                }
            }
        }
    } // namespace talk
} // namespace interact
} // namespace heroespath
