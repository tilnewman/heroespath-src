// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// statement-child.hpp
//
#include "statement-child.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string ChildTalk::Compose(const Mood MOOD, const creature::PlayerParty & PARTY)
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

        const std::string ChildTalk::ComposeMean(const creature::PlayerParty & PARTY)
        {
            using namespace compose;

            if ((misc::Random(6) == 0) && (DoesPartyHaveBeasts(PARTY)))
            {
                return "A " + BeastRaceName(PARTY) + "!";
            }

            switch (misc::Random(5))
            {
                case 0:
                {
                    return Random({ "I'm not supposed to", "Not supposed to", "I shouldn't" })
                        + " talk " + Random({ "with", "to" })
                        + Random({ " strangers", " " + Random(PartyNames()) }) + PeriodOrBang();
                }
                case 1: { return "You're not my " + Random(Parents()) + PeriodOrBang();
                }
                case 2:
                {
                    return "I'll run and tell " + Random({ "my ", "" }) + Random(Parents())
                        + Random({ " on you", "" }) + PeriodOrBang();
                }
                case 3:
                {
                    return CapFirstLetter(Random(Parents()))
                        + Random({ "", "!  Strangers", "!  " + Random(PartyNames()) }) + "!";
                }
                case 4: { return "I'm just a kid" + PeriodOrBang();
                }
                default: { return "Strangers!";
                }
            }
        }

        const std::string ChildTalk::ComposeKind(const creature::PlayerParty & PARTY)
        {
            using namespace compose;

            if ((misc::Random(6) == 0) && (DoesPartyHaveBeasts(PARTY)))
            {
                return "A " + BeastRaceName(PARTY) + "!";
            }

            switch (misc::Random(9))
            {
                case 0:
                {
                    const std::string COUNT { Random({ "a ", "two ", "three " }) };
                    return "I have " + COUNT + Random({ "", Random(JokeAdjectives()) + " " })
                        + Random(Relations()) + ((COUNT == "a") ? "" : "s") + ".";
                }
                case 1:
                {
                    return "I have a " + Random(ParentsAndRelations()) + " "
                        + Random(JokeDescriptions()) + ".";
                }
                case 2:
                {
                    return "My " + Random(ParentsAndRelations()) + " is "
                        + Random(WeatherAdjectives()) + ".";
                }
                case 3:
                {
                    return "Do you have "
                        + Random({ "a " + Random(Relations()), "any " + Random(Relations()) + "s" })
                        + "?";
                }
                case 4: { return "Are you the one " + Random(JokeDescriptions()) + "?";
                }
                case 5:
                {
                    return "My " + Random(ParentsAndRelations()) + " says you're the one "
                        + Random(JokeDescriptions()) + PeriodOrBang();
                }
                case 6: { return "You're " + Random(WeatherAdjectives()) + PeriodOrBang();
                }
                case 7:
                {
                    return "Wanna play "
                        + Random({ "hide and seek",
                                   "lick the fish",
                                   "chase the pixie",
                                   "guess what you're eating",
                                   "who can spit the farthest",
                                   "who can eat the most pond scum" })
                        + "?" + Random({ "  Bet you I'll win!", "" });
                }
                case 8:
                {
                    const std::string DAREME { "Dare me to" };
                    const std::string WANNA { "Wanna" };

                    const std::string PROP { Random(
                        { "Dare you to", DAREME, WANNA, "Let's", "Bet you can't", "I'm gonna " }) };

                    const auto WAS_QUESTION { (PROP == WANNA) || (PROP == DAREME) };

                    return PROP + " "
                        + Random({ "throw rocks at a town guard",
                                   "eat cowpies",
                                   "pee in the town well",
                                   "light a monk's robe on fire",
                                   "put a bullfrog up my sister's skirt",
                                   "tease the king",
                                   "eat this mushroom",
                                   "lick this toad",
                                   "kiss my ugly " + Random(Relations()),
                                   "take the pants off a sleeping drunkard",
                                   "steal from my " + Random(ParentsAndRelations()),
                                   "eat from a witch's garden",
                                   "put boogers in the baker's dough" })
                        + ((WAS_QUESTION) ? std::string("?") : std::string("."));
                }
                default: { return "Tell me a story about " + Random(RaceNames()) + "s.";
                }
            }
        }

    } // namespace talk
} // namespace interact
} // namespace heroespath
