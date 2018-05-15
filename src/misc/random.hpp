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
#include <cmath> //for std::nextafter()
#include <limits> //for std::uniform_..._distribution() and numeric_limits<>
#include <random>
#include <typeinfo>
#include <vector>

namespace heroespath
{
namespace misc
{
    namespace random
    {

        // This single engine and seed is used for all randomness in the game engine
        struct MersenneTwister
        {
            static void Seed()
            {
                std::array<int, engine.state_size> seedData;
                std::random_device randDevice;
                std::generate_n(seedData.data(), seedData.size(), std::ref(randDevice));
                std::seed_seq seedSequence(seedData.begin(), seedData.end());
                engine.seed(seedSequence);
            }

            static std::mt19937 engine;
        };

        template <typename T>
        inline T Real(const T THE_MIN, const T THE_MAX)
        {
            if (misc::IsRealClose(THE_MIN, THE_MAX))
            {
                return THE_MIN;
            }
            else
            {
                // Tested uniform_real_distribution on both Windows and MacOS over ranges
                // where min==max and where min>max.  In both cases the distribution didn't
                // crash and behaved as expected.  So no need to check that min<max.

                // uniform_real_distribution is [x,y) so the nextafter() call is needed
                std::uniform_real_distribution<T> uniformDist(
                    THE_MIN, std::nextafter(THE_MAX, std::numeric_limits<T>::max()));

                return uniformDist(MersenneTwister::engine);
            }
        }

        inline double Double(const double THE_MIN, const double THE_MAX)
        {
            return Real<double>(THE_MIN, THE_MAX);
        }

        inline double Double(const double THE_MAX = 1.0) { return Double(0.0, THE_MAX); }

        inline float Float(const float THE_MIN, const float THE_MAX)
        {
            return Real<float>(THE_MIN, THE_MAX);
        }

        inline float Float(const float THE_MAX = 1.0f) { return Float(0.0f, THE_MAX); }

        // The result is undefined if T is not one of: short, int, long, long long, unsigned short,
        // unsigned int, unsigned long, or unsigned long long.
        template <typename T>
        inline T NonReal(const T THE_MIN, const T THE_MAX)
        {
            // Tested uniform_int_distribution<int> on both Windows and MacOS over ranges where
            // min==max and where min>max.  In both cases the distribution didn't crash and behaved
            // as expected, except when min>max when it seemed to return random out-of-range values.
            // So to ensure results within range check that min<=max.

            M_ASSERT_OR_LOGANDTHROW_SS(
                (THE_MIN <= THE_MAX),
                "misc::random::NonReal<" << typeid(T).name() << ">(min=" << THE_MIN << ", max="
                                         << THE_MAX << ")  The min was not <= the max.");
            if (THE_MIN == THE_MAX)
            {
                return THE_MIN;
            }
            else
            {
                // uniform_int_distribution is [x,y] so no nextafter() call is needed
                std::uniform_int_distribution<T> uni_dist(THE_MIN, THE_MAX);
                return uni_dist(MersenneTwister::engine);
            }
        }

        template <typename T>
        inline T NonReal(const T THE_MAX)
        {
            return NonReal<T>(0, THE_MAX);
        }

        inline int Int(const int THE_MIN, const int THE_MAX)
        {
            return NonReal<int>(THE_MIN, THE_MAX);
        }

        inline int Int(const int THE_MAX) { return Int(0, THE_MAX); }

        inline std::size_t SizeT(const std::size_t THE_MIN, const std::size_t THE_MAX)
        {
            return NonReal<std::size_t>(THE_MIN, THE_MAX);
        }

        inline std::size_t SizeT(const std::size_t THE_MAX) { return SizeT(0, THE_MAX); }

        inline bool Bool() { return (Int(1) == 1); }
    } // namespace random
} // namespace misc
} // namespace heroespath

#endif // HEROESPATH_MISC_RANDOM_HPP_INCLUDED
