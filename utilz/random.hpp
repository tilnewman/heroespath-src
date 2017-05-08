#ifndef UTILZ_RANDOM_HPP_INCLUDED
#define UTILZ_RANDOM_HPP_INCLUDED
//
// random.hpp
//
#include "utilz/real.hpp"
#include "utilz/assertlogandthrow.hpp"

#include <algorithm>
#include <random>
#include <vector>
#include <limits> //for std::uniform_..._distribution() and numeric_limits<>
#include <cmath> //for std::nextafter()
#include <array>


namespace utilz
{
namespace random
{

    //This single engine and seed is used for all randomness in the game engine
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


    template<typename T>
    T Real(const T THE_MIN, const T THE_MAX)
    {
        if (utilz::IsRealClose(THE_MIN, THE_MAX))
        {
            return THE_MIN;
        }
        else
        {
            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX), "utilz::random::Real(min=" << THE_MIN << ", max=" << THE_MAX << ")  The min was not less than the max.");

            //uniform_real_distribution is [x,y) so the nextafter() call is needed
            std::uniform_real_distribution<T> uniform_dist(THE_MIN, std::nextafter(THE_MAX, std::numeric_limits<T>::max()));

            return uniform_dist(MersenneTwister::engine);
        }
    }


    inline double Double(const double THE_MIN, const double THE_MAX)    { return Real<double>(THE_MIN, THE_MAX); }

    inline double Double(const double THE_MAX = 1.0)                    { return Double(0.0, THE_MAX); }


    inline float Float(const float THE_MIN, const float THE_MAX)        { return Real<float>(THE_MIN, THE_MAX); }

    inline float Float(const float THE_MAX = 1.0f)                      { return Float(0.0f, THE_MAX); }


    int Int(const int THE_MIN, const int THE_MAX);

    inline int Int(const int THE_MAX)                                   { return Int(0, THE_MAX); }


    inline bool Bool()                                                  { return (Int(1) == 1); }

}
}

#endif //UTILZ_RANDOM_HPP_INCLUDED
