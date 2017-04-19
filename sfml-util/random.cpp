//
//  random.cpp
//
#include "random.hpp"


namespace sfml_util
{
namespace rand
{

    std::mt19937 MersenneTwister::engine;


    int Int(const int THE_MIN, const int THE_MAX)
    {
        if (THE_MIN == THE_MAX)
        {
            return THE_MIN;
        }
        else
        {
            M_ASSERT_OR_LOGANDTHROW_SS((THE_MIN < THE_MAX), "sfml_util::rand::Int(min=" << THE_MIN << ", max=" << THE_MAX << ")  The min was not less than the max.");

            //uniform_int_distribution is [x,y] (inclusive to the max value) so no increment is needed
            std::uniform_int_distribution<int> uni_int_dist(THE_MIN, THE_MAX);

            return uni_int_dist(MersenneTwister::engine);
        }
    }

}
}
