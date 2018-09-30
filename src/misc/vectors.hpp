// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_VECTOROPERATIONS_HPP_INCLUDED
#define HEROESPATH_MISC_VECTOROPERATIONS_HPP_INCLUDED
//
// vectors.hpp
//  A set of helper functions for working with vectors
//
#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace misc
{

    // simple wrapper for vector helper functions
    struct Vector
    {

        enum class SortOpt
        {
            None,
            SortAndUnique
        };

        // appends A into B, stable unless WILL_UNIQUE
        template <typename T>
        static void Append(
            const std::vector<T> & A_VEC,
            std::vector<T> & b_vec,
            const SortOpt SORT_OPTION = SortOpt::None)
        {
            b_vec.insert(std::end(b_vec), std::begin(A_VEC), std::end(A_VEC));

            if (SORT_OPTION == SortOpt::SortAndUnique)
            {
                std::sort(b_vec.begin(), b_vec.end());
                b_vec.erase(std::unique(b_vec.begin(), b_vec.end()), b_vec.end());
            }
        }

        // appends A into B using move so that the source is left in an undefined but safe to
        // destruct state, stable unless WILL_UNIQUE
        template <typename T>
        static void AppendMove(
            const std::vector<T> & SOURCE,
            std::vector<T> & dest,
            const SortOpt SORT_OPTION = SortOpt::None)
        {
            dest.insert(
                std::end(dest),
                std::make_move_iterator(std::begin(SOURCE)),
                std::make_move_iterator(std::end(SOURCE)));

            if (SORT_OPTION == SortOpt::SortAndUnique)
            {
                std::sort(dest.begin(), dest.end());
                dest.erase(std::unique(dest.begin(), dest.end()), dest.end());
            }
        }

        template <typename T>
        static std::vector<T> AndCopy(
            const std::vector<T> & A_VEC,
            const std::vector<T> & B_VEC,
            const SortOpt SORT_OPTION = SortOpt::None)
        {
            std::vector<T> finalVec { A_VEC };
            Append(B_VEC, finalVec, SORT_OPTION);
            return finalVec;
        }

        template <typename T>
        static const std::vector<T> Exclude(const std::vector<T> & ORIG_VEC, const T toExclude)
        {
            std::vector<T> v { toExclude };
            return Exclude(ORIG_VEC, v);
        }

        template <typename T>
        static const std::vector<T>
            Exclude(const std::vector<T> & ORIG, const std::vector<T> & EXCLUDED)
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
                [&EXCLUDED](const auto & FROM_ORIG) {
                    return (
                        std::find(EXCLUDED.begin(), EXCLUDED.end(), FROM_ORIG) == EXCLUDED.end());
                });

            return finalVec;
        }

        template <typename T>
        static void ShuffleVec(std::vector<T> & v)
        {
            if (v.size() > 1)
            {
                std::shuffle(v.begin(), v.end(), random::MersenneTwister::engine);
            }
        }

        template <typename T>
        static T & SelectRandom(std::vector<T> & vector)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (vector.empty() == false),
                "misc::Vector::SelectRandom(non-const) was given an empty vector.");

            return vector[misc::random::SizeT(vector.size() - 1)];
        }

        template <typename T>
        static const T & SelectRandom(const std::vector<T> & VECTOR)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (VECTOR.empty() == false),
                "misc::Vector::SelectRandom(const) was given an empty vector.");

            return VECTOR[misc::random::SizeT(VECTOR.size() - 1)];
        }

        enum JoinOpt : unsigned
        {
            None = 0,
            Wrap = 1 << 0,
            And = 1 << 1,
            Ellipsis = 1 << 2
        };

        template <typename T>
        static const std::string Join(
            const std::vector<T> & VEC,
            const std::size_t MAX_COUNT = 0,
            const JoinOpt OPTIONS = JoinOpt::None,
            const std::string (*TO_STRING_FUNC)(const T &) = [](const T & X) -> const std::string {
                std::ostringstream ss;
                ss << X;
                return ss.str();
            },
            bool (*ONLY_IF_FUNC)(const T) = [](const T) -> bool { return true; })
        {
            const std::size_t VEC_ELEMENT_COUNT { VEC.size() };
            if (VEC_ELEMENT_COUNT == 0)
            {
                return ""; // skip wrapping on empty case
            }

            std::size_t toJoinCount { 0 };
            std::vector<T> toJoinVec;
            for (auto const & NEXT_ELEMENT : VEC)
            {
                if (ONLY_IF_FUNC(NEXT_ELEMENT))
                {
                    toJoinVec.emplace_back(NEXT_ELEMENT);
                    if ((MAX_COUNT != 0) && (++toJoinCount == MAX_COUNT))
                    {
                        break;
                    }
                }
            }

            const std::size_t TO_JOIN_ELEMENT_COUNT { toJoinVec.size() };
            if (TO_JOIN_ELEMENT_COUNT == 0)
            {
                return ""; // skip wrapping on empty case
            }

            std::ostringstream ss;
            for (std::size_t i(0); i < TO_JOIN_ELEMENT_COUNT; ++i)
            {
                if (i != 0)
                {
                    ss << ", ";
                }

                if ((OPTIONS & JoinOpt::And) && (TO_JOIN_ELEMENT_COUNT > 2) && (i >= 2)
                    && ((TO_JOIN_ELEMENT_COUNT - 1) == i))
                {
                    ss << "and ";
                }

                ss << TO_STRING_FUNC(toJoinVec[i]);

                if ((OPTIONS & JoinOpt::Ellipsis) && (TO_JOIN_ELEMENT_COUNT < VEC_ELEMENT_COUNT)
                    && ((TO_JOIN_ELEMENT_COUNT - 1) == i))
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

        template <typename T>
        static T SumVec(const std::vector<T> & VALUES)
        {
            return std::accumulate(
                std::begin(VALUES),
                std::end(VALUES),
                T(0),
                [](auto const SUBTOTAL, auto const VALUE) { return SUBTOTAL + VALUE; });
        }

        template <typename T>
        static T Average(const std::vector<T> & VALUES)
        {
            if (VALUES.empty())
            {
                return static_cast<T>(0);
            }

            return static_cast<T>(
                static_cast<double>(SumVec(VALUES)) / static_cast<double>(VALUES.size()));
        }

        template <typename T>
        static T StandardDeviation(const std::vector<T> & VALUES)
        {
            return StandardDeviation(VALUES, VALUES.size(), Average(VALUES));
        }

        template <typename T>
        static T StandardDeviation(
            const std::vector<T> & VALUES,
            const std::size_t COUNT,
            const T AVERAGE_ORIG,
            const bool WILL_IGNORE_FIRST_VALUE = false)
        {
            if ((COUNT <= 1) || ((COUNT - 1) > VALUES.size()))
            {
                return 0;
            }

            auto const AVERAGE_DOUBLE { static_cast<double>(AVERAGE_ORIG) };

            double deviationSum { 0.0 };

            std::size_t i { ((WILL_IGNORE_FIRST_VALUE) ? 1U : 0U) };
            for (; i < (COUNT - 1); ++i)
            {
                auto const NEXT_VALUE { static_cast<double>(VALUES[i]) };
                deviationSum += std::pow((NEXT_VALUE - AVERAGE_DOUBLE), 2);
            }

            return static_cast<T>(std::sqrt(deviationSum / static_cast<double>(COUNT - 1)));
        }

        template <typename T>
        static bool OrderlessCompareEqual(const std::vector<T> & A, const std::vector<T> & B)
        {
            if (A.size() != B.size())
            {
                return false;
            }

            auto a { A };
            auto b { B };

            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());

            return (a == b);
        }

        template <typename T>
        static bool OrderlessCompareLess(const std::vector<T> & A, const std::vector<T> & B)
        {
            auto a { A };
            auto b { B };

            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());

            return (a < b);
        }

        template <typename T>
        struct MinMaxAvgSum
        {
            MinMaxAvgSum(const T MIN, const T MAX, const T AVG, const T SUM)
                : min(MIN)
                , max(MAX)
                , avg(AVG)
                , sum(SUM)
            {}

            explicit MinMaxAvgSum(const std::vector<T> & VALUES)
                : min(0)
                , max(0)
                , avg(0)
                , sum(0)
            {
                if (VALUES.empty() == false)
                {
                    min = VALUES.front();
                    max = min;

                    for (auto const VALUE : VALUES)
                    {
                        if (VALUE < min)
                        {
                            min = VALUE;
                        }

                        if (VALUE > max)
                        {
                            max = VALUE;
                        }

                        sum += VALUE;
                    }

                    avg = static_cast<T>(
                        static_cast<double>(sum) / static_cast<double>(VALUES.size()));
                }
            }

            T min;
            T max;
            T avg;
            T sum;
        };
    };
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_VECTOROPERATIONS_HPP_INCLUDED
