#ifndef APPBASE_SFMLUTIL_RANDOM_INCLUDED
#define APPBASE_SFMLUTIL_RANDOM_INCLUDED
//
// random.hpp
//
#include "sfml-util/real-utils.hpp"

#include "heroespath/assertlogandthrow.hpp"

#include <algorithm>
#include <random>
#include <vector>
#include <limits> //for std::uniform_..._distribution() and numeric_limits<>
#include <cmath> //for std::nextafter()
#include <array>


namespace sfml_util
{
namespace rand
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
        if (sfml_util::IsRealClose(THE_MIN, THE_MAX))
        {
            return THE_MIN;
        }
        else
        {
            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX), "sfml_util::rand::Real(min=" << THE_MIN << ", max=" << THE_MAX << ")  The min was not less than the max.");

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


    template<typename T>
    bool ShuffleVec(std::vector<T> & v)
    {
        if (v.size() > 1)
        {
            std::shuffle(v.begin(), v.end(), MersenneTwister::engine);
            return true;
        }
        else
        {
            return false;
        }
    }


    template<typename T>
    T SelectRandom(const std::vector<T> & V)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((V.empty() == false), "sfml_util::rand::SelectRandom() was given an empty vector.");
        if (V.size() == 1)
        {
            return V[0];
        }
        else
        {
            return V[ Int(V.size() - 1) ];
        }
    }


    //Helper class for getting linear low weighted random values
    template<typename T> //use either float or double
    class Weighted
    {
    public:
        Weighted(   const bool      IS_LOW_WEIGHTED,
                    const std::size_t    SCALE = 100,
                    const std::size_t    WEIGHT = 1)
        :
            SCALE_  ( static_cast<T>(SCALE) ),
            valVec_ ()
        {
            for (std::size_t x(1); x <= SCALE; ++x)
            {
                for (std::size_t y(1); y <= SCALE - (SCALE - x); ++y)
                {
                    for (std::size_t w(0); w<WEIGHT; ++w)
                        if (IS_LOW_WEIGHTED)
                            valVec_.push_back( static_cast<T>((SCALE - x) + 1) );
                        else
                            valVec_.push_back( static_cast<T>(x) );
                }
            }
        }

        T GetRand(const T THE_MIN = 0.0f, const T THE_MAX = 1.0f) const
        {
            const T RAND_NORM(valVec_[rand::Int(0, valVec_.size() - 1)] / SCALE_);
            const T DIFF(THE_MAX - THE_MIN);
            return THE_MIN + (RAND_NORM * DIFF);
        }

        inline std::size_t GetSize() const { return valVec_.size(); }

        inline T GetValueAtPosition(const std::size_t _POS) const { return valVec_[_POS]; }

    private:
        const T         SCALE_;
        std::vector<T>  valVec_;
    };

}
}

#endif //APPBASE_SFMLUTIL_RANDOM_INCLUDED
