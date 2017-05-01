#ifndef SFMLUTIL_REALUTILS_HPP_INCLUDED
#define SFMLUTIL_REALUTILS_HPP_INCLUDED
//
// real-utils.hpp
//
#include <cmath> //for std::fabs
#include <limits> //for epsilon


namespace sfml_util
{

    template<typename T>
    inline bool IsRealClose(const T A,
                            const T B,
                            const T TOLERANCE = std::numeric_limits<T>::epsilon())
    {
        return std::fabs(A - B) < TOLERANCE;
    }


    template<typename T>
    inline bool IsRealOne(const T X) { return IsRealClose(X, 1.0f); }


    template<typename T>
    inline bool IsRealZero(const T X) { return IsRealClose(X, 0.0f); }

}

#endif //SFMLUTIL_REALUTILS_HPP_INCLUDED
