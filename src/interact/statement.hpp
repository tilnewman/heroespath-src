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
namespace player
{
    class Party;
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
            using StrVec_t = std::vector<std::string>;

            inline const std::string Random(const std::vector<std::string> & V)
            {
                return ((V.empty()) ? std::string() : std::string(misc::Vector::SelectRandom(V)));
            }

            template <typename T>
            inline const std::string Random(const std::vector<std::vector<T>> & VV)
            {
                return ((VV.empty()) ? std::string(T()) : Random(misc::Vector::SelectRandom(VV)));
            }

            const std::string RandomWithEmpty(const StrVec_t &);

            inline const std::string RandomOrEmpty(const StrVec_t & STR_VEC)
            {
                return ((misc::random::Bool()) ? "" : Random(STR_VEC));
            }

            inline const std::string PeriodOrBang()
            {
                return ((misc::random::Bool()) ? std::string(".") : std::string("!"));
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

            inline const StrVec_t PartyNames()
            {
                return { "strangers", "outlanders", "travelers", "wanderers", "adventurers" };
            }

            const StrVec_t Combinations(
                const StrVec_t & V1, const StrVec_t & V2, const std::string & SEPARATOR = " ");

            bool DoesPartyHaveBeasts(const player::Party &);
            bool DoesPartyHaveWolfens(const player::Party &);

            inline const std::string BeastRaceName(const player::Party & party)
            {
                return (
                    (DoesPartyHaveWolfens(party)) ? std::string("wolfen") : std::string("dragon"));
            }

            const std::string CapFirstLetter(const std::string &);

            // Not using the real list because not all sound right in conversation.
            inline const StrVec_t RaceNames()
            {
                return { "Gnome", "Pixie",   "Wolfen",      "Dragon", "Goblin",       "Troll",
                         "Orc",   "Newt",    "CaveCrawler", "Hydra",  "LizardWalker", "Minotaur",
                         "Ogre",  "Demon",   "Griffin",     "Cobra",  "Werewolf",     "Wyvern",
                         "Giant", "Ghoul",   "Witch",       "Golem",  "Pig",          "Yack",
                         "Goat",  "Monster", "Toad" };
            }

            // Not using the real list because not all sound right in conversation.
            inline const StrVec_t RoleNames()
            {
                return { "Archer",   "Bard",    "Beastmaster", "Cleric",      "Knight",
                         "Sorcerer", "Thief",   "Mugger",      "Drunk",       "Berserker",
                         "Shaman",   "Soldier", "Ranger",      "Blacksmith",  "Farmer",
                         "Friar",    "Tart",    "Loon",        "Foot-Licker", "Harpy",
                         "Mother",   "Father" };
            }

            inline const std::string RandomRaceOrRole()
            {
                return ((misc::random::Bool()) ? Random(RaceNames()) : Random(RoleNames()));
            }

            inline const StrVec_t TimesOfDay()
            {
                return { "day", "night", "morning", "afternoon", "evening" };
            }

            inline const StrVec_t WeatherAdjectives()
            {
                return { "nice",       "lovely",    "pretty",    "pleasant",
                         "delightful", "gorgeous",  "beautiful", "wretched",
                         "miserable",  "cheerless", "bleak",     "drab",
                         "grim",       "dreary",    "dismal",    "gloomy" };
            }

            inline const StrVec_t JokeAdjectives()
            {
                return { "drunken",
                         "blind",
                         "one-legged",
                         "one-eyed",
                         "idiot",
                         "murdering",
                         "nine-fingured",
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
                         "forwnward",
                         "sick",
                         "bearded",
                         "lily-liver'd",
                         "pigeon-liver'd"
                         "mustard crusted",
                         "villainous",
                         "fork-tounged",
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

            inline const StrVec_t JokeDescriptions()
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
