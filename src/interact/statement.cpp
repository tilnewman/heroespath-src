// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software.  If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// statement.cpp
//
#include "statement.hpp"
#include "creature/creature.hpp"
#include "player/party.hpp"

#include <boost/algorithm/string.hpp>

namespace heroespath
{
namespace interact
{
    namespace talk
    {
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

            bool DoesPartyHaveBeasts(const player::Party & party)
            {
                for (auto const CHARACTER_PTR : party.Characters())
                {
                    if (CHARACTER_PTR->IsBeast())
                    {
                        return true;
                    }
                }

                return false;
            }

            bool DoesPartyHaveWolfens(const player::Party & party)
            {
                for (auto const CHARACTER_PTR : party.Characters())
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
