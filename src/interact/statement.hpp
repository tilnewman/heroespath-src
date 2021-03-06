// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_STATEMENT_HPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENT_HPP_INCLUDED
//
// pose-enum.hpp
//
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "misc/vectors.hpp"

#include <algorithm>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{
    class PlayerParty;
}
namespace interact
{
    namespace talk
    {

        enum class Mood
        {
            Kind,
            Mean
        };

        enum class Category
        {
            Common,
            Town,
            Child,
            Monk,
            Drunk,
            Guard
        };

        const std::string CategoryToString(const Category);

        using CategoryVec_t = std::vector<Category>;

        namespace compose
        {
            inline const std::string Random(const std::vector<std::string> & V)
            {
                return ((V.empty()) ? std::string() : std::string(misc::RandomSelect(V)));
            }

            template <typename T>
            inline const std::string Random(const std::vector<std::vector<T>> & VV)
            {
                return ((VV.empty()) ? std::string(T()) : Random(misc::RandomSelect(VV)));
            }

            const std::string RandomWithEmpty(const std::vector<std::string> &);

            inline const std::string RandomOrEmpty(const std::vector<std::string> & STR_VEC)
            {
                return ((misc::RandomBool()) ? "" : Random(STR_VEC));
            }

            inline const std::string PeriodOrBang()
            {
                return ((misc::RandomBool()) ? std::string(".") : std::string("!"));
            }

            inline const std::string
                PrependIf(const std::string & S, const std::string & PREFIX = "  ")
            {
                return ((S.empty()) ? S : PREFIX + S);
            }

            inline const std::string
                AppendIf(const std::string & S, const std::string & POSTFIX = "  ")
            {
                return ((S.empty()) ? S : S + POSTFIX);
            }

            inline const std::vector<std::string> PartyNames()
            {
                return { "strangers", "outlanders", "travelers", "wanderers", "adventurers" };
            }

            const std::vector<std::string> Combinations(
                const std::vector<std::string> & V1,
                const std::vector<std::string> & V2,
                const std::string & SEPARATOR = " ");

            bool DoesPartyHaveBeasts(const creature::PlayerParty &);
            bool DoesPartyHaveWolfens(const creature::PlayerParty &);

            inline const std::string BeastRaceName(const creature::PlayerParty & party)
            {
                return (
                    (DoesPartyHaveWolfens(party)) ? std::string("wolfen") : std::string("dragon"));
            }

            const std::string CapFirstLetter(const std::string &);

            // Not using the real list because not all sound right in conversation.
            inline const std::vector<std::string> RaceNames()
            {
                return { "Gnome", "Pixie",   "Wolfen",      "Dragon", "Goblin",       "Troll",
                         "Orc",   "Newt",    "CaveCrawler", "Hydra",  "LizardWalker", "Minotaur",
                         "Ogre",  "Demon",   "Griffin",     "Cobra",  "Werewolf",     "Wyvern",
                         "Giant", "Ghoul",   "Witch",       "Golem",  "Pig",          "Yack",
                         "Goat",  "Monster", "Toad" };
            }

            // Not using the real list because not all sound right in conversation.
            inline const std::vector<std::string> RoleNames()
            {
                return { "Archer",   "Bard",    "Beastmaster", "Cleric",      "Knight",
                         "Sorcerer", "Thief",   "Mugger",      "Drunk",       "Berserker",
                         "Shaman",   "Soldier", "Ranger",      "Blacksmith",  "Farmer",
                         "Friar",    "Tart",    "Loon",        "Foot-Licker", "Harpy",
                         "Mother",   "Father" };
            }

            inline const std::string RandomRaceOrRole()
            {
                return ((misc::RandomBool()) ? Random(RaceNames()) : Random(RoleNames()));
            }

            inline const std::vector<std::string> TimesOfDay()
            {
                return { "day", "night", "morning", "afternoon", "evening" };
            }

            inline const std::vector<std::string> WeatherAdjectives()
            {
                return { "nice",       "lovely",    "pretty",    "pleasant",
                         "delightful", "gorgeous",  "beautiful", "wretched",
                         "miserable",  "cheerless", "bleak",     "drab",
                         "grim",       "dreary",    "dismal",    "gloomy" };
            }

            inline const std::vector<std::string> JokeAdjectives()
            {
                return { "drunken",
                         "blind",
                         "one-legged",
                         "one-eyed",
                         "idiot",
                         "murdering",
                         "nine-fingered",
                         "nine-toed",
                         "shy",
                         "dying",
                         "dwarf",
                         "rich",
                         "poor",
                         "ugly",
                         "back-bearded",
                         "thumb-sucking",
                         "cross-eyed",
                         "slack-jawed",
                         "mud covered"
                         "pig-eyed",
                         "stock-eyed",
                         "three-inch",
                         "frownward",
                         "sick",
                         "bearded",
                         "lily-liver'd",
                         "pigeon-liver'd"
                         "mustard crusted",
                         "villainous",
                         "fork-tongued",
                         "rank",
                         "googly-eyed",
                         "sodden-witted",
                         "beslubbering",
                         "logger-headed",
                         "puking",
                         "beef-witted",
                         "boil-brained",
                         "dizzy",
                         "flea-covered",
                         "foolish",
                         "villainous",
                         "spotted",
                         "club-footed" };
            }

            inline const std::vector<std::string> JokeDescriptions()
            {
                return { "who ate his dog",         "who jumped down the well",
                         "who can't count",         "who can't blink",
                         "who kissed a monk",       "who saw the queen naked",
                         "who slapped the king",    "who picked his friend's nose",
                         "who drank goblin pizzel", "who stepped in dragon droppings",
                         "who sucks his thumb",     "who found a singing frog",
                         "who pee'd in the well",   "who eats worms",
                         "who didn't have a nose",  "who could spit over the castle" };
            }
        } // namespace compose
    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENT_HPP_INCLUDED
