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
#include "misc/real.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <limits>
#include <random>
#include <tuple>
#include <typeinfo>

#include <boost/type_index.hpp>

namespace heroespath
{
namespace misc
{
    namespace random_helpers
    {

        class MersenneTwister19937
        {
        public:
            using Engine_t = std::mt19937_64;

            MersenneTwister19937() = delete;

            static void Setup()
            {
                std::random_device randomDevice;

                // Seed Method #1 Complete
                // This seed method brings out the best in mt19937, but now it is std::see_seq that
                // is limiting your entropy.
                //
                // std::array<std::random_device::result_type, engine_.state_size> initialSeed;
                // std::generate_n(initialSeed.data(), initialSeed.size(), std::ref(randomDevice));
                // std::seed_seq seedSequence(initialSeed.begin(), initialSeed.end());
                // engine_.seed(seedSequence);
                // const std::string SEED_STR { "(way too big to log)" };

                // Seed Method #2 Quick and Dirty
                //  This is the quickest and simplest way to seed, and it makes recording the seed
                //  for replay and troubleshooting easy.  The MT19937 is an amazing algorithm but
                //  you will never see it when seeding like this.  This will make the output less
                //  than perfectly uniform and reduce the period dramatically, but not in any way
                //  casual users would ever see.  This is still lightyears better than
                //  srand(time(NULL)) and rand() could ever be.
                //
                const auto SEED { randomDevice() };
                std::seed_seq seedSequence { SEED };
                engine_.seed(seedSequence);
                const std::string SEED_STR { std::to_string(SEED) };

                engine_.discard(WARM_UP_COUNT_);

                M_HP_LOG(
                    "Random Number Generator Setup:  engine=std::mt19937_64, seed="
                    << SEED_STR
                    << ", seed_size=" << (seedSequence.size() * sizeof(std::seed_seq::result_type))
                    << "bytes, warm_up_count=" << std::to_string(WARM_UP_COUNT_)
                    << ", first_random_number<"
                    << boost::typeindex::type_id<Engine_t::result_type>().pretty_name()
                    << ">=" << engine_());
            }

            static Engine_t & Engine() noexcept { return engine_; }

        private:
            static Engine_t engine_;
            static const unsigned long long WARM_UP_COUNT_ { 1 << 20 };
        };

        template <typename T, typename = std::enable_if_t<are_arithmetic_nobool_v<T>>>
        T RandomImpl(const T THE_MIN, const T THE_MAX)
        {
            if constexpr (are_floating_point_v<T>)
            {
                std::uniform_real_distribution<T> realDistribution(THE_MIN, THE_MAX);
                return realDistribution(random_helpers::MersenneTwister19937::Engine());
            }
            else
            {
                std::uniform_int_distribution<T> intDistribution(THE_MIN, THE_MAX);
                return intDistribution(random_helpers::MersenneTwister19937::Engine());
            }
        }

    } // namespace random_helpers

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

            // TODO
            // if (THE_MIN == THE_MAX_ORIG)
            if (IsRealClose(THE_MIN, THE_MAX_ORIG))
            {
                return THE_MIN;
            }

            // std::uniform_real_distribution is over the half-closed interval [MIN, MAX) so we need
            // to advance the MAX or else it will be impossible to return the MAX.
            T theMaxFinal { NextAfterIfReal(THE_MAX_ORIG) };

            // std::uniform_whatever_distribution will crashes/undefined behavior if the difference
            // (MAX - MIN) overflows, so handle that special case here
            if constexpr (std::is_signed_v<T>)
            {
                const T THE_MAX_FINAL_LIMIT { (THE_MIN + std::numeric_limits<T>::max()) };

                if ((THE_MIN < T(0)) && (THE_MAX_FINAL_LIMIT < theMaxFinal))
                {
                    M_HP_LOG_ERR(
                        "Unable to generate random number with the [MIN,MAX] given because the "
                        "difference (MAX-MIN) overflows.  Will generate a random number over a "
                        "reduced "
                        "range instead."
                        << M_HP_STREAM_TYPE(T) << M_HP_STREAM_VAR(THE_MIN)
                        << M_HP_STREAM_VAR(THE_MAX_ORIG) << M_HP_STREAM_VAR(theMaxFinal)
                        << ((IsRealClose(THE_MAX_ORIG, theMaxFinal)) ? "(same)" : "(different)")
                        << M_HP_STREAM_VAR(THE_MAX_FINAL_LIMIT));

                    theMaxFinal = THE_MAX_FINAL_LIMIT;
                }
            }

            return random_helpers::RandomImpl(THE_MIN, theMaxFinal);
        }
    }

    template <typename T>
    T Random(const T THE_MAX)
    {
        return Random(T(0), THE_MAX);
    }

    template <typename T>
    T Random()
    {
        return Random(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max());
    }

    inline bool RandomBool() { return (Random(1) == 1); }

    template <typename Iterator_t, typename = std::enable_if_t<are_random_access_v<Iterator_t>>>
    void RandomShuffle(const Iterator_t BEGIN, const Iterator_t END)
    {
        std::shuffle(BEGIN, END, random_helpers::MersenneTwister19937::Engine());
    }

    template <
        typename Container_t,
        typename = std::enable_if_t<are_random_access_v<typename Container_t::iterator>>>
    void RandomShuffle(Container_t & container)
    {
        RandomShuffle(std::begin(container), std::end(container));
    }

    template <typename Container_t>
    std::size_t RandomIndex(const Container_t & CONTAINER)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CONTAINER.empty() == false),
            "Given an empty " << boost::typeindex::type_id<Container_t>().pretty_name()
                              << " container.");

        return Random(CONTAINER.size() - static_cast<std::size_t>(1));
    }

    template <
        typename Container_t,
        typename = std::enable_if_t<are_random_access_v<typename Container_t::iterator>>>
    typename Container_t::value_type & RandomSelect(Container_t & container)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (container.empty() == false),
            "Given an empty " << boost::typeindex::type_id<Container_t>().pretty_name()
                              << " container. (non-const version)");

        const auto RANDOM_INDEX { RandomIndex(container) };

        const auto RANDOM_INDEX_DIFF_T {
            static_cast<typename Container_t::iterator::difference_type>(RANDOM_INDEX)
        };

        return *(std::begin(container) + RANDOM_INDEX_DIFF_T);
    }

    template <
        typename Container_t,
        typename = std::enable_if_t<are_random_access_v<typename Container_t::iterator>>>
    const typename Container_t::value_type & RandomSelect(const Container_t & CONTAINER)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (CONTAINER.empty() == false),
            "Given an empty " << boost::typeindex::type_id<Container_t>().pretty_name()
                              << " container. (const version)");

        const auto RANDOM_INDEX { RandomIndex(CONTAINER) };

        const auto RANDOM_INDEX_DIFF_T {
            static_cast<typename Container_t::iterator::difference_type>(RANDOM_INDEX)
        };

        return *(std::begin(CONTAINER) + RANDOM_INDEX_DIFF_T);
    }

} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_RANDOM_HPP_INCLUDED
