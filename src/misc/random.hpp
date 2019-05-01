// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MISC_RANDOM_HPP_INCLUDED
#define HEROESPATH_MISC_RANDOM_HPP_INCLUDED
//
// random.hpp
//
#include "misc/assertlogandthrow.hpp"
#include "misc/nameof.hpp"
#include "misc/real.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <type_traits>

namespace heroespath
{
namespace misc
{
    namespace helpers
    {

        class MersenneTwister19937
        {
            using Engine_t = std::mt19937_64;
            static Engine_t engine_;
            static constexpr unsigned long long WARM_UP_COUNT_ = 10000;

            //  This is the quickest and simplest way to seed, and it makes recording the seed
            //  for replay and troubleshooting possible.  The mt19937 is an amazing algorithm but
            //  you will never see it when seeding like this.  This will make the output less
            //  than perfectly uniform (slightly) and reduce the period (dramatically), but not in
            //  any way typical users/uses would ever see.  This is still lightyears better than
            //  srand(time(NULL)) and rand() could ever be, and if you needed better than this, you
            //  would probably not be using mt19937 anyway...
            static const std::string SeedPoorQualityButLoggable();

            // This seed method brings out the best in mt19937, but now std::see_seq that is
            // limiting your entropy, and you cannot simply log the seed and get all the benefits of
            // replay testing.
            static const std::string SeedHighQualityButNotLoggable();

        public:
            MersenneTwister19937() = delete;
            static void Setup();
            static Engine_t & Engine() noexcept { return engine_; }
        };

        template <typename T, typename = std::enable_if_t<are_arithmetic_nobool_v<T>>>
        T RandomImpl(const T THE_MIN, const T THE_MAX)
        {
            if constexpr (are_floating_point_v<T>)
            {
                std::uniform_real_distribution<T> realDistribution(THE_MIN, THE_MAX);
                return realDistribution(helpers::MersenneTwister19937::Engine());
            }
            else
            {
                std::uniform_int_distribution<T> intDistribution(THE_MIN, THE_MAX);
                return intDistribution(helpers::MersenneTwister19937::Engine());
            }
        }

    } // namespace helpers

    // this is really just an error handling wrapper around RandomImpl()
    template <typename T, typename = std::enable_if_t<are_arithmetic_nobool_v<T>>>
    T Random(const T THE_MIN, const T THE_MAX_ORIG)
    {
        // std::uniform_int_distribution doesn't work with single byte types
        if constexpr (sizeof(T) == 1)
        {
            return static_cast<T>(
                Random(static_cast<int>(THE_MIN), static_cast<int>(THE_MAX_ORIG)));
        }
        else
        {
            if (THE_MIN > THE_MAX_ORIG)
            {
                return Random(THE_MAX_ORIG, THE_MIN);
            }

            if (IsRealClose(THE_MIN, THE_MAX_ORIG))
            {
                return THE_MIN;
            }

            // std::uniform_real_distribution is over the half-closed interval [MIN, MAX) so we need
            // to advance the MAX or else it will be impossible to return the MAX.
            T theMaxFinal { NextIfReal(THE_MAX_ORIG) };

            // std::uniform_whatever_distribution will crashes/undefined behavior if the difference
            // (MAX - MIN) overflows, so handle that special case here
            if constexpr (are_signed_v<T>)
            {
                const T THE_MAX_FINAL_LIMIT { (THE_MIN + std::numeric_limits<T>::max()) };

                if ((THE_MIN < T(0)) && (THE_MAX_FINAL_LIMIT < theMaxFinal))
                {
                    M_HP_LOG_ERR(
                        "Random<"
                        << NAMEOF_TYPE_T(T) << ">(MIN=" << THE_MIN << ", MAX=" << THE_MAX_ORIG
                        << ") failing because the difference between (MAX-MIN) overflows for T="
                        << NAMEOF_TYPE_T(T)
                        << ".  So instead, generating a random number between [MIN, "
                           "MIN+numeric_limit::max] or ["
                        << THE_MIN << ", " << THE_MAX_FINAL_LIMIT << "].");

                    theMaxFinal = THE_MAX_FINAL_LIMIT;
                }
            }

            return helpers::RandomImpl(THE_MIN, theMaxFinal);
        }
    }

    template <typename T>
    T Random(const T THE_MAX)
    {
        return Random(T(0), THE_MAX);
    }

    inline bool RandomBool() { return (Random(1) == 1); }

    template <
        typename Iterator_t,
        typename = std::enable_if_t<are_random_access_iterator_v<Iterator_t>>>
    void RandomShuffle(const Iterator_t BEGIN, const Iterator_t END)
    {
        std::shuffle(BEGIN, END, helpers::MersenneTwister19937::Engine());
    }

    template <
        typename Container_t,
        typename = std::enable_if_t<are_random_access_iterator_v<typename Container_t::iterator>>>
    void RandomShuffle(Container_t & container)
    {
        RandomShuffle(std::begin(container), std::end(container));
    }

    template <typename Container_t>
    std::size_t RandomIndex(const Container_t & CONTAINER)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CONTAINER.empty() == false),
            "Given an empty " << NAMEOF_TYPE_T(Container_t) << " container.");

        return Random(CONTAINER.size() - static_cast<std::size_t>(1));
    }

    template <
        typename Container_t,
        typename = std::enable_if_t<are_random_access_iterator_v<typename Container_t::iterator>>>
    typename Container_t::value_type & RandomSelect(Container_t & container)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (container.empty() == false),
            "Given an empty " << NAMEOF_TYPE_T(Container_t) << " container. (non-const version)");

        const auto RANDOM_INDEX { RandomIndex(container) };

        const auto RANDOM_INDEX_DIFF_T {
            static_cast<typename Container_t::iterator::difference_type>(RANDOM_INDEX)
        };

        return *(std::begin(container) + RANDOM_INDEX_DIFF_T);
    }

    template <
        typename Container_t,
        typename = std::enable_if_t<are_random_access_iterator_v<typename Container_t::iterator>>>
    const typename Container_t::value_type & RandomSelect(const Container_t & CONTAINER)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CONTAINER.empty() == false),
            "Given an empty " << NAMEOF_TYPE_T(Container_t) << " container. (const version)");

        const auto RANDOM_INDEX { RandomIndex(CONTAINER) };

        const auto RANDOM_INDEX_DIFF_T {
            static_cast<typename Container_t::iterator::difference_type>(RANDOM_INDEX)
        };

        return *(std::begin(CONTAINER) + RANDOM_INDEX_DIFF_T);
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_RANDOM_HPP_INCLUDED
