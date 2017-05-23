//
// stringhelp.cpp
//
#include "stringhelp.hpp"

#include <vector>
#include <string>
#include <algorithm>


namespace appbase
{
namespace stringhelp
{
    const std::string CleanStringListCopy(  const std::string & STR_TO_CLEAN,
                                            const std::string & PARSE_SEPS,
                                            const bool          WILL_SKIP_EMPTY,
                                            const bool          WILL_TRIM,
                                            const std::string & NEW_SEP,
                                            const std::string & WRAP_LEFT,
                                            const std::string & WRAP_RIGHT,
                                            const bool          WILL_SORT,
                                            const bool          WILL_UNIQUE)
    {
        //split the string into a vector, skip and trim along the way
        std::vector< std::string> v;
        SplitByChars(STR_TO_CLEAN, v, PARSE_SEPS, WILL_SKIP_EMPTY, WILL_TRIM);

        //add wrapping characters if needed
        if ((WRAP_LEFT.empty() == false) || (WRAP_RIGHT.empty() == false))
            for(std::vector< std::string>::iterator i(v.begin()); i != v.end(); ++i)
                (* i) = WRAP_LEFT + (* i) + WRAP_RIGHT;

        //sort if needed, check for more efficient sort algorithm combinations
        if (WILL_SORT && WILL_UNIQUE)
        {
            SortAndUnique(v);
        }
        else
        {
            if (WILL_SORT) std::sort(v.begin(), v.end());
            if (WILL_UNIQUE) Unique(v);
        }

        //join the vector into a single string and return
        return boost::algorithm::join(v, ((NEW_SEP.empty()) ? PARSE_SEPS : NEW_SEP ));
    }



    bool CleanStringList(   std::string &       strToClean,
                            const std::string & PARSE_SEPS,
                            const bool          WILL_SKIP_EMPTY,
                            const bool          WILL_TRIM,
                            const std::string & NEW_SEP,
                            const std::string & WRAP_LEFT,
                            const std::string & WRAP_RIGHT,
                            const bool          WILL_SORT,
                            const bool          WILL_UNIQUE)
    {
        const std::string RESULT( CleanStringListCopy(  strToClean,
                                                        PARSE_SEPS,
                                                        WILL_SKIP_EMPTY,
                                                        WILL_TRIM,
                                                        NEW_SEP,
                                                        WRAP_LEFT,
                                                        WRAP_RIGHT,
                                                        WILL_SORT,
                                                        WILL_UNIQUE) );
        const bool DID_CHANGE(RESULT != strToClean);
        strToClean = RESULT;
        return DID_CHANGE;
    }


}//end of namespace stringhelp
}//end of namespace appbase
