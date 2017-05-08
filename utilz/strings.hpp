#ifndef UTILZ_STRINGS_HPP_INCLUDED
#define UTILZ_STRINGS_HPP_INCLUDED
//
// strings.hpp
//
#include <string>
#include <sstream>


namespace utilz
{

    struct String
    {

        template<typename T>
        static const std::string DecorateNumber(const T NUMBER)
        {
            std::ostringstream ss;
            ss << NUMBER;

            if (NUMBER == 1)
            {
                ss << "st";
            }
            else if (NUMBER == 2)
            {
                ss << "nd";
            }
            else if (NUMBER == 3)
            {
                ss << "rd";
            }
            else
            {
                ss << "th";
            }

            return ss.str();
        }

    };

}

#endif //UTILZ_STRINGS_HPP_INCLUDED
