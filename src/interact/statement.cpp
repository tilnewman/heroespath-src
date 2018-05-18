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
#include "misc/boost-string-includes.hpp"

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

            const std::string RandomWithEmpty(const StrVec_t & STR_VEC)
            {
                StrVec_t v(STR_VEC);
                v.emplace_back("");
                return misc::Vector::SelectRandom(STR_VEC);
            }

            const StrVec_t Combinations(
                const StrVec_t & V1, const StrVec_t & V2, const std::string & SEPARATOR)
            {
                StrVec_t v;

                for (auto const & S1 : V1)
                {
                    for (auto const & S2 : V2)
                    {
                        v.emplace_back((S1 + SEPARATOR).append(S2));
                    }
                }

                return v;
            }

            bool DoesPartyHaveBeasts(const creature::PlayerParty & party)
            {
                for (auto const & CHARACTER_PTR : party.Characters())
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
                for (auto const & CHARACTER_PTR : party.Characters())
                {
                    if (CHARACTER_PTR->Race() == creature::race::Wolfen)
                    {
                        return true;
                    }
                }

                return false;
            }

            const std::string CapFirstLetter(const std::string & S)
            {
                if (S.empty())
                {
                    return "";
                }
                else
                {
                    std::string s{ S };
                    s[0] = boost::algorithm::to_upper_copy(std::string(1, S[0]))[0];
                    return s;
                }
            }

        } // namespace compose
    } // namespace talk
} // namespace interact
} // namespace heroespath
