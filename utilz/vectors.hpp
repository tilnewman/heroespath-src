#ifndef UTILZ_VECTOROPERATIONS_HPP_INCLUDED
#define UTILZ_VECTOROPERATIONS_HPP_INCLUDED
//
// vectors.hpp
//  A set of helper functions for working with vectors
//
#include "utilz/random.hpp"
#include "utilz/assertlogandthrow.hpp"

#include <vector>
#include <sstream>
#include <algorithm>


namespace utilz
{

    //simple wrapper for vector helper functions
    struct Vector
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


        template<typename T>
        static bool ShuffleVec(std::vector<T> & v)
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
        static T SelectRandom(const std::vector<T> & V)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((V.empty() == false), "utilz::Vector::SelectRandom() was given an empty vector.");
            if (V.size() == 1)
            {
                return V[0];
            }
            else
            {
                return V[utilz::random::Int(V.size() - 1)];
            }
        }


        template<typename T>
        static const std::string Join(const std::vector<T> VEC,
                                      const bool           WILL_WRAP     = false,
                                      const bool           WILL_AND      = false,
                                      const std::size_t    MAX_COUNT     = 0,
                                      const bool           WILL_ELLIPSIS = false,
                                      const std::string(*TO_STRING_FUNC)(const T) = [](const T x) -> const std::string
                                        {
                                            std::ostringstream ss;
                                            ss << x;
                                            return ss.str();
                                        },
                                      bool(*ONLY_IF_FUNC)(const T) = [](const T) -> bool
                                        {
                                            return true;
                                        })
        {
            const std::size_t VEC_ELEMENT_COUNT{ VEC.size() };
            if (VEC_ELEMENT_COUNT == 0)
            {
                return "";//skip wrapping on empty case
            }

            std::size_t toJoinCount{ 0 };
            std::vector<T> toJoinVec;
            for (auto const & NEXT_ELEMENT : VEC)
            {
                if (ONLY_IF_FUNC(NEXT_ELEMENT))
                {
                    toJoinVec.push_back(NEXT_ELEMENT);
                    if ((MAX_COUNT != 0) && (++toJoinCount == MAX_COUNT))
                    {
                        break;
                    }
                }
            }

            const std::size_t TO_JOIN_ELEMENT_COUNT{ toJoinVec.size() };
            if (TO_JOIN_ELEMENT_COUNT == 0)
            {
                return "";//skip wrapping on empty case
            }

            std::ostringstream ss;
            for (std::size_t i(0); i < TO_JOIN_ELEMENT_COUNT; ++i)
            {
                if (i != 0)
                {
                    ss << ", ";
                }
                
                if (WILL_AND &&
                    (TO_JOIN_ELEMENT_COUNT > 2) &&
                    (i >= 2) &&
                    ((TO_JOIN_ELEMENT_COUNT - 1) == i))
                {
                    ss << "and ";
                }

                ss << TO_STRING_FUNC(toJoinVec[i]);

                if (WILL_ELLIPSIS &&
                    (TO_JOIN_ELEMENT_COUNT < VEC_ELEMENT_COUNT) &&
                    ((TO_JOIN_ELEMENT_COUNT - 1) == i))
                {
                    ss << "...";
                }
            }

            if (WILL_WRAP && (ss.str().empty() == false))
            {
                return "(" + ss.str() + ")";
            }
            else
            {
                return ss.str();
            }
        }
    };

}

#endif //UTILZ_VECTOROPERATIONS_HPP_INCLUDED
