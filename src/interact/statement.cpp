// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// statement.cpp
//
#include "statement.hpp"

#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "misc/strings.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string CategoryToString(const Category CATEGORY)
        {
            switch (CATEGORY)
            {
                case Category::Common:
                {
                    return "Common";
                }
                case Category::Town:
                {
                    return "Town";
                }
                case Category::Child:
                {
                    return "Child";
                }
                case Category::Monk:
                {
                    return "Monk";
                }
                case Category::Drunk:
                {
                    return "Drunk";
                }
                case Category::Guard:
                default:
                {
                    return "Guard";
                }
            }
        }

        namespace compose
        {

            const std::string RandomWithEmpty(const std::vector<std::string> & STR_VEC)
            {
                std::vector<std::string> v(STR_VEC);
                v.emplace_back("");
                return misc::RandomSelect(STR_VEC);
            }

            const std::vector<std::string> Combinations(
                const std::vector<std::string> & V1,
                const std::vector<std::string> & V2,
                const std::string & SEPARATOR)
            {
                std::vector<std::string> v;

                for (const auto & S1 : V1)
                {
                    for (const auto & S2 : V2)
                    {
                        v.emplace_back((S1 + SEPARATOR).append(S2));
                    }
                }

                return v;
            }

            bool DoesPartyHaveBeasts(const creature::PlayerParty & party)
            {
                for (const auto & CHARACTER_PTR : party.Characters())
                {
                    if (CHARACTER_PTR->IsBeast())
                    {
                        return true;
                    }
                }

                return false;
            }

            bool DoesPartyHaveWolfens(const creature::PlayerParty & party)
            {
                for (const auto & CHARACTER_PTR : party.Characters())
                {
                    if (CHARACTER_PTR->Race() == creature::race::Wolfen)
                    {
                        return true;
                    }
                }

                return false;
            }

            const std::string CapFirstLetter(const std::string & STRING)
            {
                if (STRING.empty())
                {
                    return "";
                }

                std::string result { STRING };
                misc::ToUpper(result.front());
                return result;
            }

        } // namespace compose
    } // namespace talk
} // namespace interact
} // namespace heroespath
