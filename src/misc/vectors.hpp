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
#ifndef MISC_VECTOROPERATIONS_HPP_INCLUDED
#define MISC_VECTOROPERATIONS_HPP_INCLUDED
//
// vectors.hpp
//  A set of helper functions for working with vectors
//
#include "misc/random.hpp"
#include "misc/assertlogandthrow.hpp"

#include <vector>
#include <sstream>
#include <algorithm>
#include <string>


namespace misc
{

    //simple wrapper for vector helper functions
    struct Vector
    {

        enum class SortOpt
        {
            None,
            SortAndUnique
        };


        //appends A into B, stable unless WILL_UNIQUE
        template<typename T>
        static void Append(
            const std::vector<T> & A_VEC,
            std::vector<T> &       b_vec,
            const SortOpt          SORT_OPTION = SortOpt::None)
        {
            std::copy(A_VEC.begin(), A_VEC.end(), back_inserter(b_vec));

            if (SORT_OPTION == SortOpt::SortAndUnique)
            {
                std::sort(b_vec.begin(), b_vec.end());
                b_vec.erase(std::unique(b_vec.begin(), b_vec.end()), b_vec.end());
            }
        }


        template<typename T>
        static std::vector<T> AndCopy(
            const std::vector<T> & A_VEC,
            const std::vector<T> & B_VEC,
            const SortOpt          SORT_OPTION = SortOpt::None)
        {
            std::vector<T> finalVec{ A_VEC };
            Append(B_VEC, finalVec, SORT_OPTION);
            return finalVec;
        }


        template<typename T>
        static const std::vector<T> Exclude(
            const std::vector<T> & ORIG_VEC,
            const T toExclude)
        {
            std::vector<T> v{ toExclude };
            return Exclude(ORIG_VEC, v);
        }


        template<typename T>
        static const std::vector<T> Exclude(
            const std::vector<T> & ORIG,
            const std::vector<T> & EXCLUDED)
        {
            if (EXCLUDED.empty())
            {
                return ORIG;
            }

            std::vector<T> finalVec;
            finalVec.reserve(ORIG.size());

            std::copy_if(
                ORIG.begin(),
                ORIG.end(),
                back_inserter(finalVec),
                [&EXCLUDED] (const auto & FROM_ORIG)
                {
                    return (std::find(
                        EXCLUDED.begin(),
                        EXCLUDED.end(), FROM_ORIG) == EXCLUDED.end());
                });

            return finalVec;
        }


        template<typename T>
        static bool ShuffleVec(std::vector<T> & v)
        {
            if (v.size() > 1)
            {
                std::shuffle(v.begin(), v.end(), random::MersenneTwister::engine);
                return true;
            }
            else
            {
                return false;
            }
        }


        template<typename T>
        static T SelectRandom(const std::vector<T> & V)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((V.empty() == false),
                "misc::Vector::SelectRandom() was given an empty vector.");

            if (V.size() == 1)
            {
                return V[0];
            }
            else
            {
                return V[static_cast<std::size_t>(
                    misc::random::Int(static_cast<int>(V.size()) - 1))];
            }
        }


        enum JoinOpt : unsigned
        {
            None     = 0,
            Wrap     = 1 << 0,
            And      = 1 << 1,
            Ellipsis = 1 << 2
        };


        template<typename T>
        static const std::string Join(
            const std::vector<T> & VEC,
            const std::size_t      MAX_COUNT = 0,
            const JoinOpt          OPTIONS   = JoinOpt::None,
            const std::string(*TO_STRING_FUNC)(const T) =
                [](const T x) -> const std::string
                {
                    std::ostringstream ss;
                    ss << x;
                    return ss.str();
                },
            bool(*ONLY_IF_FUNC)(const T) =
                [](const T) -> bool
                {
                    return true;
                })
        {
            const std::size_t VEC_ELEMENT_COUNT{ VEC.size() };
            if (VEC_ELEMENT_COUNT == 0)
            {
                return "";//skip wrapping on empty case
            }

            std::size_t toJoinCount{ 0 };
            std::vector<T> toJoinVec;
            for (auto const & NEXT_ELEMENT : VEC)
            {
                if (ONLY_IF_FUNC(NEXT_ELEMENT))
                {
                    toJoinVec.push_back(NEXT_ELEMENT);
                    if ((MAX_COUNT != 0) && (++toJoinCount == MAX_COUNT))
                    {
                        break;
                    }
                }
            }

            const std::size_t TO_JOIN_ELEMENT_COUNT{ toJoinVec.size() };
            if (TO_JOIN_ELEMENT_COUNT == 0)
            {
                return "";//skip wrapping on empty case
            }

            std::ostringstream ss;
            for (std::size_t i(0); i < TO_JOIN_ELEMENT_COUNT; ++i)
            {
                if (i != 0)
                {
                    ss << ", ";
                }

                if ((OPTIONS & JoinOpt::And) &&
                    (TO_JOIN_ELEMENT_COUNT > 2) &&
                    (i >= 2) &&
                    ((TO_JOIN_ELEMENT_COUNT - 1) == i))
                {
                    ss << "and ";
                }

                ss << TO_STRING_FUNC(toJoinVec[i]);

                if ((OPTIONS & JoinOpt::Ellipsis) &&
                    (TO_JOIN_ELEMENT_COUNT < VEC_ELEMENT_COUNT) &&
                    ((TO_JOIN_ELEMENT_COUNT - 1) == i))
                {
                    ss << "...";
                }
            }

            if ((OPTIONS & JoinOpt::Wrap) && (ss.str().empty() == false))
            {
                return "(" + ss.str() + ")";
            }
            else
            {
                return ss.str();
            }
        }


        template<typename T>
        static T Average(const std::vector<T> & V)
        {
            if (V.empty())
            {
                return static_cast<T>(0);
            }

            T sum{ 0 };
            for (auto const NEXT_VALUE : V)
            {
                sum += NEXT_VALUE;
            }

            return sum / static_cast<T>(V.size());
        }


        //skip the first value because that is an invalid framerate value
        template<typename T>
        static T StandardDeviation(const std::vector<T> & V,
                                   const std::size_t      COUNT,
                                   const T                AVERAGE)
        {
            if ((COUNT <= 1) || ((COUNT - 1) > V.size()))
            {
                return static_cast<T>(0);
            }

            double deviationSum{ 0.0 };

            for (std::size_t i(1); i < (COUNT - 1); ++i)
            {
                auto const NEXT_VALUE{ V[i] };
                deviationSum += std::pow(static_cast<double>(NEXT_VALUE - AVERAGE), 2);
            }

            return static_cast<T>(std::sqrt(deviationSum / static_cast<double>(COUNT - 1)));
        }


        template<typename T>
        static bool OrderlessCompareEqual(const std::vector<T> & A,
                                          const std::vector<T> & B)
        {
            if (A.size() != B.size())
            {
                return false;
            }

            for (auto const & NEXT : A)
            {
                if (std::find(B.begin(), B.end(), NEXT) == B.end())
                {
                    return false;
                }
            }

            return true;
        }


        template<typename T>
        static bool OrderlessCompareLess(const std::vector<T> & A,
                                         const std::vector<T> & B)
        {
            auto a{ A };
            auto b{ B };

            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());

            return a < b;
        }

    };

}

#endif //MISC_VECTOROPERATIONS_HPP_INCLUDED
