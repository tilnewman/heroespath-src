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
// statement-monk.hpp
//
#include "statement-monk.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string MonkTalk::Compose()
        {
            using namespace compose;

            switch (misc::random::Int(18))
            {
                case 0:
                {
                    return CapFirstLetter(Random(MonkThings())) + " is "
                        + Random(StrVec_t{ "a", "your" }) + " choice.  "
                        + Random(StrVec_t{ "What have you chosen?", "What will you choose?" });
                }
                case 1:
                {
                    return "May " + Random(MonkThings()) + Random(StrVec_t{ " be with ", " find " })
                        + Random(StrVec_t{ "you.", "you all." });
                }
                case 2:
                {
                    return "Peace " + Random(StrVec_t{ "be with ", "to " })
                        + Random(StrVec_t{ "you.", "you all." });
                }
                case 3:
                {
                    return "This is a peaceful place.";
                }
                case 4:
                {
                    return "This is a place " + Random(StrVec_t{ "for ", "of " })
                        + Random(MonkThings()) + ".";
                }
                case 5:
                {
                    return "You " + Random(StrVec_t{ "have found ", "speak with " }) + "a humble "
                        + Random(StrVec_t{ "brother.", "monk." });
                }
                case 6:
                {
                    return Random(StrVec_t{ "All are ", "You are all " }) + "welcome here.";
                }
                case 7:
                {
                    return "You have come to the right place.";
                }
                case 8:
                {
                    return "Your lives have led you " + Random(StrVec_t{ "here.  ", "to me.  " })
                        + Random(StrVec_t{ "Think ", "Meditate " }) + "on "
                        + Random(StrVec_t{ "it.", "that." });
                }
                case 9:
                {
                    return "I see only brothers and sisters before me.";
                }
                case 10:
                {
                    return "I " + Random(StrVec_t{ "follow the ", "seek the " })
                        + Random(StrVec_t{ "righteous ", "one true " })
                        + Random(StrVec_t{ "path.", "way." });
                }
                case 11:
                {
                    return Random(StrVec_t{ "The ", "My ", "Our " }) + "master is away.";
                }
                case 12:
                {
                    return "Find "
                        + Random(StrVec_t{ "peace ", "your true calling ", "your path " })
                        + Random(StrVec_t{ "here in ", "here with us among " })
                        + Random(StrVec_t{ "the monks.", "the order.", "our order." });
                }
                case 13:
                {
                    return Random(StrVec_t{
                               "Have you found ", "Are you looking for ", "Are you seeking " })
                        + Random(MonkThings()) + "?";
                }
                case 14:
                {
                    return "Are you considering " + Random(StrVec_t{ "a life ", "life " })
                        + Random(StrVec_t{ "among ", "with " })
                        + Random(StrVec_t{ "us?", "the monks?", "the brothers?" });
                }
                case 15:
                {
                    return "If I speak of " + Random(MonkThings()) + ", will you "
                        + Random(StrVec_t{ "really listen?", "truly hear?" });
                }
                case 16:
                {
                    return Random(StrVec_t{ "May our paths ", "Our paths " })
                        + Random(StrVec_t{ "cross", "join" })
                        + Random(StrVec_t{ " peacefully.", "." });
                }
                case 17:
                {
                    return Random(StrVec_t{ "May " + Random(MonkThings()),
                                            CapFirstLetter(Random(MonkThings())) })
                        + Random(StrVec_t{ " follow you.", " be with you." });
                }
                default:
                {
                    return Random(StrVec_t{ "Do you dream of ", "Have you found " })
                        + Random(MonkThings()) + "?";
                }
            }
        };
    } // namespace talk
} // namespace interact
} // namespace heroespath
