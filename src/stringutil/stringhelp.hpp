///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_STRINGHELP_HPP_INCLUDED
#define HEROESPATH_STRINGHELP_HPP_INCLUDED
//
// stringhelp.hpp
//
//  Commonly required, higher-level, easy-to-use string functions based on the stl and boost.
//  The goal of these functions is to standardize common operations and reduce repeated code
//  at the cost of performance on specialized or larger scale strings and containers.
//  All functions are compatible with sequential stl containers (vector, list, and sometimes set),
//  or any others that implement compatible iterators.
//
//  For example, since ease-of-use means supporting multiple stl container types, and this is
//  a higher priority than performance, all functions below will use:  x.insert(t.end(), ...).
//  This means these functions perform poorly with containers/strings tens of thousands in size,
//  but are quick and reliable in all other cases.  The intended uses of these functions is for
//  validating user data, GUI input, cfg file reads, etc, in order to standardize and reduce
//  repeated code.
//
//  This also means that Unique(), Intersection(), and RemoveAFromB() accept different containers types
//  in the same call.  So you can write code such as:   StringHelp::Intersection(myVec, myList, mySet)
//
//  The STL and Boost's string algorithm lib are supposed to be used in conjunction with these functions.
//  So for example there is no join() defined here, because boost's join() simple and avoids repeating code.
//  However, boost's split() is often followed by the same set of operations or customizations, so
//  SplitByChar() is defined here to avoid repeating calls to remove empties or to trim strings.
//
//  Simply including this file gives access to boost/algorithm/string.hpp, which includes all other
//  stl and boost string algorithms that you are encouraged to be aware of and use. For example:
//      to_upper, to_lower, trim, trim_left, trim_right
//      starts_with, contains
//      find_first, find_last, find_regex
//      replace, replace_regex
//      is_digit/graph/space/alpha/printable/upper/lower/punct/etc.
//      join
//      (avoid using boost's split -use the Tokenizer instead)
//
#include <set>
#include <string>
#include <algorithm>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4512)
#pragma warning(disable: 4701)
#pragma warning(disable: 4127)
#endif

//suppress warnings that are safe to ignore in boost
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__WINDOWS__)
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__WINDOWS__)
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Wswitch-enum"
#endif


#ifdef _MSC_VER
#pragma warning(pop)
#endif


namespace appbase
{
    //useful types to cleanup Tokenizer code
    using TokSepChar = boost::char_separator<char>;
    using TokChar = boost::tokenizer<TokSepChar>;
    using TokCharCIter = TokChar::const_iterator;
    //There is a non-const iter, but it is the same as the const, becuase you can't modify the contents of a tokenizer.
    //using TokCharIter = TokChar::iterator;


namespace stringhelp
{
    //Since boost::algorithm::join() only works on containers of strings,
    //this function works with std::ostringstream << allowing vector<int> etc.
    template<typename T>
    const std::string Join(const T & CONTAINER, const std::string & SEP_STR = ", ");


    //numbers to std::string
    template<typename T>
    bool NumToStdStr(   const T             NUM,
                        std::string &       outStr,
                        const std::string & DEFAULT);

    template<typename T>
    const std::string NumToStdStr(  const T             NUM,
                                    const std::string & DEFAULT);


    //The INPUT_STR is split by SEP_CHAR and appended to outContainer.
    //Returns the number of strings that were appended to the outContainer -NOT it's final size.
    //Empty string parameters will not crash/throw, as longs as they are null terminated.
    //An empty separator string will result in the entire unmodified input string being appended to outContainer.
    //Separator chars are removed, and not added to the outContainer in any way.
    //Trim chars will depend on boost's locale, but typically include: space,tab,carr-ret,newline,v-tab,form-feed
    //If trim reduces a string to length of zero, then it will be excluded from the output if WILL_SKIP_EMPTY is true.
    template <typename T>
    std::size_t SplitByChar( const std::string & INPUT_STR,
                        T &                 outContainer,
                        const char          SEP_CHAR        = ',',
                        const bool          WILL_SKIP_EMPTY = false,
                        const bool          WILL_TRIM       = false);

    //NOTE: This version splits on any chars found in SEP_CHARS, NOT on the entire string as a whole.
    template <typename T>
    std::size_t SplitByChars(const std::string & INPUT_STR,
                        T &                 outContainer,
                        const std::string & SEP_CHARS,
                        const bool          WILL_SKIP_EMPTY = false,
                        const bool          WILL_TRIM       = false);


    //Use this function to cleanup a string list input by hand, etc.
    //
    //It is also very handy for making another copy of this with an extra space
    //after each token that is easier to read in output for the user on a gui, etc.
    //
    //For example, if strToClean is: "first, second,, compound entry, last   ,,"
    //  CleanStringList(strToClean)                              "first,second,compound entry,last"
    //  CleanStringList(strToClean, false)                       "first,second,,compound entry,last,,"
    //  CleanStringList(strToClean,",",true,true," ","{","}")    "{first} {second} {compound entry} {last}"
    //  CleanStringList(strToClean,",",true,false," ","","|")    "first| second| compound entry| last   |"
    //
    //If NEW_SEP string is empty, then the separator will not change.
    //Returns true if the input string was changed in any way.
    bool CleanStringList(   std::string &       strToClean,
                            const std::string & PARSE_SEPS      = ",",
                            const bool          WILL_SKIP_EMPTY = true,
                            const bool          WILL_TRIM       = true,
                            const std::string & NEW_SEP         = "",
                            const std::string & WRAP_LEFT       = "",
                            const std::string & WRAP_RIGHT      = "",
                            const bool          WILL_SORT       = false,
                            const bool          WILL_UNIQUE     = false);

    const std::string CleanStringListCopy(  const std::string & STR_TO_CLEAN,
                                            const std::string & PARSE_SEPS      = ",",
                                            const bool          WILL_SKIP_EMPTY = true,
                                            const bool          WILL_TRIM       = true,
                                            const std::string & NEW_SEP         = "",
                                            const std::string & WRAP_LEFT       = "",
                                            const std::string & WRAP_RIGHT      = "",
                                            const bool          WILL_SORT       = false,
                                            const bool          WILL_UNIQUE     = false);


    //An alternative to std::unique() that will also remove non-adjacent duplicates and is also stable.
    //Returns true if any duplicates were removed.
    template <typename T>
    bool Unique(T &);

    template <typename IN_t, typename OUT_t>
    bool Unique(const IN_t & A, OUT_t & out);


    //Sorts and removes duplicates.
    //Returns true if any duplicates were removed.
    template <typename T>
    bool SortAndUnique(T &);


    //Appends the intersection of A and B to out, in the order of (stable for) A.
    //Returns true if any intersections were found.
    //Intended for sequential containers, but supports any that implement standard iterators.
    //NOTE:  If WILL_UNIQUE is true then it will operate on ALL of out, not only those appended.
    template <typename A_t, typename B_t, typename OUT_t>
    bool Intersection(  const A_t & A,
                        const B_t & B,
                        OUT_t &     out,
                        const bool  WILL_UNIQUE = true);

    //This version returns the result instead
    template <typename A_t, typename B_t>
    const A_t Intersection( const A_t & A,
                            const B_t & B,
                            const bool  WILL_UNIQUE = true);


    //Removes all in A from b, including duplicates. (stable)
    //Returns true if any were removed.
    //Intended for sequential containers, but supports any that implement standard iterators.
    template <typename A_t, typename B_t>
    bool RemoveAFromB(const A_t & A, B_t & b);

    //This version returns the result without changing B.
    template <typename A_t, typename B_t>
    const B_t RemoveAFromB_Copy(const A_t & A, const B_t & B);

}//end of namespace stringhelp
}//end of namespace appbase




//definitions
namespace appbase
{
namespace stringhelp
{

    template<typename T>
    const std::string Join(const T & CONTAINER, const std::string & SEP_STR)
    {
        std::ostringstream ss;
        for(typename T::const_iterator i(CONTAINER.begin()); i != (CONTAINER.end()); ++i)
        {
            if (i != CONTAINER.begin())
            {
                ss << SEP_STR;
            }

            ss << * i;
        }

        return ss.str();
    }


    template<typename T>
    bool NumToStdStr(   const T             NUM,
                        std::string &       outStr,
                        const std::string & DEFAULT)
    {
        try
        {
            outStr = boost::lexical_cast<std::string>(NUM);
            return true;
        }
        catch(...)
        {
            outStr = DEFAULT;
        }

        return false;
    }



    template<typename T>
    const std::string NumToStdStr(  const T             NUM,
                                    const std::string & DEFAULT)
    {
        std::string s;
        NumToStdStr(NUM, s, DEFAULT);
        return s;
    }



    template <typename T>
    std::size_t SplitByChar( const std::string & INPUT_STR,
                        T &                 outContainer,
                        const char          SEP_CHAR,
                        const bool          WILL_SKIP_EMPTY,
                        const bool          WILL_TRIM)
    {
        return SplitByChars(INPUT_STR,
                            outContainer,
                            std::string(1, SEP_CHAR),
                            WILL_SKIP_EMPTY,
                            WILL_TRIM);
    }



    template <typename T>
    std::size_t SplitByChars(const std::string & INPUT_STR,
                        T &                 outContainer,
                        const std::string & SEP_CHARS,
                        const bool          WILL_SKIP_EMPTY,
                        const bool          WILL_TRIM)
    {
        if (INPUT_STR.empty()) return 0;

        using namespace boost;
        const empty_token_policy ETP(((WILL_SKIP_EMPTY) ? drop_empty_tokens : keep_empty_tokens));
        TokSepChar tokenSep(SEP_CHARS.c_str(), "", ETP);

        TokChar tokenator(INPUT_STR, tokenSep);
        std::size_t appendCount(0);
        for(TokCharCIter i(tokenator.begin()); i != tokenator.end(); ++i)
        {
            std::string nextToken(* i);

            if (WILL_TRIM)
                boost::algorithm::trim_if(nextToken, boost::algorithm::is_space());

            if ((WILL_SKIP_EMPTY == false) || (nextToken.empty() == false))
            {
                ++appendCount;
                outContainer.insert(outContainer.end(), nextToken);
            }
        }

        return appendCount;
    }



    template <typename T>
    bool Unique(T & a)
    {
        T temp;
        const bool RESULT( Unique(a, temp) );
        a = temp;
        return RESULT;
    }



    template <typename IN_t, typename OUT_t>
    bool Unique(const IN_t & A, OUT_t & out)
    {
        //Loop over A, using helperSet to eliminate duplicates.
        std::set<typename IN_t::value_type> helperSet;
        for(typename IN_t::const_iterator i(A.begin()); A.end() != i; ++i)
        {
            if (helperSet.end() == helperSet.find(* i))
            {
                out.insert(out.end(), * i);
                helperSet.insert(* i);
            }
        }

        return (A.size() > out.size());
    }



    template <typename T>
    bool SortAndUnique(T & a)
    {
        const std::size_t ORIG_SIZE(a.size());
        if (ORIG_SIZE > 1)
        {
            std::sort(a.begin(), a.end());
            a.erase( std::unique(a.begin(), a.end()), a.end() );
            //sort makes all duplicates adjacent so std::unique() works here
        }

        return (ORIG_SIZE != a.size());
    }



    template <typename A_t, typename B_t>
    const A_t Intersection( const A_t & A,
                            const B_t & B,
                            const bool  WILL_UNIQUE)
    {
        A_t temp;
        Intersection(A, B, temp, WILL_UNIQUE);
        return temp;
    }



    template <typename A_t, typename B_t, typename OUT_t>
    bool Intersection(  const A_t & A,
                        const B_t & B,
                        OUT_t &     out,
                        const bool  WILL_UNIQUE)
    {
        bool foundAny(false);
        for(typename A_t::const_iterator i(A.begin()); A.end() != i; ++i)
        {
            if (std::find(B.begin(), B.end(), * i) != B.end())
            {
                foundAny = true;
                out.insert(out.end(), * i);
            }
        }

        if (WILL_UNIQUE) Unique(out);
        return foundAny;
    }



    template <typename A_t, typename B_t>
    const B_t RemoveAFromB_Copy(const A_t & A, const B_t & B)
    {
        B_t temp(B);
        RemoveAFromB(A, temp);
        return temp;
    }



    template <typename A_t, typename B_t>
    bool RemoveAFromB(const A_t & A, B_t & b)
    {
        const std::size_t ORIG_SIZE(b.size());

        for(typename A_t::const_iterator i(A.begin()); A.end() != i; ++i)
            b.erase(std::remove(b.begin(), b.end(), * i), b.end());

        return (ORIG_SIZE != b.size());
    }

}//end of namespace stringhelp
}//end of namespace appbase

#endif //HEROESPATH_STRINGHELP_HPP_INCLUDED
