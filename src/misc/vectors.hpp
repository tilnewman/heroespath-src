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
#include "misc/random.hpp"

#include <algorithm>
#include <numeric>

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

        // appends SOURCE into dest, stable unless WILL_UNIQUE
        template <typename T>
        static void Append(const T & SOURCE, T & dest, const SortOpt SORT_OPTION = SortOpt::None)
        {
            dest.reserve(dest.size() + SOURCE.size());
            dest.insert(std::end(dest), std::begin(SOURCE), std::end(SOURCE));

            if (SORT_OPTION == SortOpt::SortAndUnique)
            {
                std::sort(std::begin(dest), std::end(dest));
                dest.erase(std::unique(std::begin(dest), std::end(dest)), std::end(dest));
            }
        }

        // appends SOURCE into dest using move so that the source is left in an undefined but safe
        // to destruct state, stable unless WILL_UNIQUE, source will stay the same size but the
        // values inside will be changed,  I did some tests that showed this to be about 2x faster
        // with strings
        template <typename T>
        static void AppendMove(T && source, T & dest, const SortOpt SORT_OPTION = SortOpt::None)
        {
            dest.reserve(dest.size() + source.size());

            dest.insert(
                std::end(dest),
                std::make_move_iterator(std::begin(source)),
                std::make_move_iterator(std::end(source)));

            if (SORT_OPTION == SortOpt::SortAndUnique)
            {
                std::sort(std::begin(dest), std::end(dest));
                dest.erase(std::unique(std::begin(dest), std::end(dest)), std::end(dest));
            }
        }

        template <typename T>
        static T Combine(const T & A, const T & B, const SortOpt SORT_OPTION = SortOpt::None)
        {
            T result;
            result.reserve(A.size() + B.size());
            result.insert(std::end(result), std::begin(A), std::end(A));
            result.insert(std::end(result), std::begin(B), std::end(B));

            if (SORT_OPTION == SortOpt::SortAndUnique)
            {
                std::sort(std::begin(result), std::end(result));
                result.erase(std::unique(std::begin(result), std::end(result)), std::end(result));
            }

            return result;
        }

        template <typename T>
        static const T Exclude(const T & ORIG, const typename T::value_type & TO_EXCLUDE)
        {
            if (ORIG.empty())
            {
                return ORIG;
            }

            auto result(ORIG);

            result.erase(
                std::remove(std::begin(result), std::end(result), TO_EXCLUDE), std::end(result));

            return result;
        }

        template <typename T>
        static const T Exclude(const T & ORIG, const T & TO_EXCLUDE)
        {
            if (ORIG.empty() || TO_EXCLUDE.empty())
            {
                return ORIG;
            }

            T result;
            result.reserve(ORIG.size());

            std::copy_if(
                std::begin(ORIG),
                std::end(ORIG),
                back_inserter(result),
                [&TO_EXCLUDE](const auto & FROM_ORIG) {
                    return (
                        std::find(std::begin(TO_EXCLUDE), std::end(TO_EXCLUDE), FROM_ORIG)
                        == std::end(TO_EXCLUDE));
                });

            return result;
        }

        template <typename T>
        static typename T::value_type Sum(const T & CONTAINER)
        {
            if (CONTAINER.empty())
            {
                return static_cast<typename T::value_type>(0);
            }

            return std::accumulate(
                std::begin(CONTAINER),
                std::end(CONTAINER),
                typename T::value_type(0),
                [](const auto SUBTOTAL, const auto VALUE) { return SUBTOTAL + VALUE; });
        }

        template <typename T>
        static typename T::value_type Average(const T & CONTAINER)
        {
            if (CONTAINER.empty())
            {
                return static_cast<typename T::value_type>(0);
            }

            return static_cast<typename T::value_type>(
                static_cast<double>(Sum(CONTAINER)) / static_cast<double>(CONTAINER.size()));
        }

        template <typename T>
        static typename T::value_type StandardDeviation(const T & CONTAINER)
        {
            return StandardDeviation(CONTAINER, CONTAINER.size(), Average(CONTAINER));
        }

        template <typename T>
        static typename T::value_type StandardDeviation(
            const T & CONTAINER, const std::size_t COUNT, const typename T::value_type AVERAGE_ORIG)
        {
            if ((CONTAINER.size() <= 3) || (COUNT < 3) || (COUNT >= CONTAINER.size()))
            {
                return 0;
            }

            using Math_t = long double;

            const auto AVERAGE { static_cast<Math_t>(AVERAGE_ORIG) };

            const Math_t DEVIATION_SUM { std::accumulate(
                std::begin(CONTAINER),
                std::begin(CONTAINER) + static_cast<std::ptrdiff_t>(COUNT),
                0.0L,
                [&](const Math_t SO_FAR, const auto VALUE) {
                    return SO_FAR + std::pow((static_cast<Math_t>(VALUE) - AVERAGE), 2);
                }) };

            return static_cast<typename T::value_type>(
                std::sqrt(DEVIATION_SUM / static_cast<Math_t>(COUNT - 1)));
        }

        template <typename T>
        static bool OrderlessCompareEqual(const T & A, const T & B)
        {
            if (A.size() != B.size())
            {
                return false;
            }

            auto a { A };
            auto b { B };

            std::sort(std::begin(a), std::end(a));
            std::sort(std::begin(b), std::end(b));

            return (a == b);
        }

        template <typename T>
        static bool OrderlessCompareLess(const T & A, const T & B)
        {
            auto a { A };
            auto b { B };

            std::sort(std::begin(a), std::end(a));
            std::sort(std::begin(b), std::end(b));

            return (a < b);
        }
    };

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_VECTOROPERATIONS_HPP_INCLUDED
