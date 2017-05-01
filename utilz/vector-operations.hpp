#ifndef VECTOROPERATIONS_HPP_INCLUDED
#define VECTOROPERATIONS_HPP_INCLUDED
//
// vector-operations.hpp
//  A set of helper functions for working with vectors
//
#include <vector>
#include <algorithm>


namespace utilz
{

    //simple wrapper for vector helper functions
    struct VectorOps
    {
        //appends A into B, stable unless WILL_UNIQUE
        template<typename T>
        static void Append(const std::vector<T> & A_VEC, std::vector<T> & b_Vec, const bool WILL_UNIQUE = false)
        {
            if (A_VEC.empty())
                return;

            std::copy(A_VEC.begin(), A_VEC.end(), back_inserter(b_Vec));

            if (WILL_UNIQUE)
            {
                std::unique(b_Vec.begin(), b_Vec.end());
            }
        }


        template<typename T>
        static std::vector<T> And(const std::vector<T> & A_VEC, const std::vector<T> & B_VEC, const bool WILL_UNIQUE = false)
        {
            std::vector<T> finalVec;

            if (A_VEC.empty() == false)
            {
                std::copy(A_VEC.begin(), A_VEC.end(), back_inserter(finalVec));
            }

            if (B_VEC.empty() == false)
            {
                std::copy(B_VEC.begin(), B_VEC.end(), back_inserter(finalVec));
            }

            if (WILL_MAKE_UNIQUE)
            {
                std::unique(finalVec.begin(), finalVec.end());
            }

            return finalVec;
        }


        template<typename T>
        static const std::vector<T> Exclude(const std::vector<T> & ORIG_VEC, const T toExclude)
        {
            std::vector<T> v{ toExclude };
            return Exclude(ORIG_VEC, v);
        }


        template<typename T>
        static const std::vector<T> Exclude(const std::vector<T> & ORIG_VEC, const std::vector<T> & TO_EXCLUDE_VEC)
        {
            if (ORIG_VEC.empty())
                return std::vector<T>();

            if (TO_EXCLUDE_VEC.empty())
                return ORIG_VEC;

            std::vector<T> finalVec;

            std::copy_if(ORIG_VEC.begin(),
                         ORIG_VEC.end(),
                         back_inserter(finalVec),
                         [& TO_EXCLUDE_VEC] (T NEXT_FROM_ORIG)
                         {
                             for(auto const NEXT_TO_EXCLUDE : TO_EXCLUDE_VEC)
                                 if (NEXT_FROM_ORIG == NEXT_TO_EXCLUDE)
                                     return false;

                             return true;
                         });

            return finalVec;
        }
    };
}

#endif //VECTOROPERATIONS_HPP_INCLUDED
