#ifndef SFMLUTIL_RESPONSE_ENUM_HPP_INCLUDED
#define SFMLUTIL_RESPONSE_ENUM_HPP_INCLUDED
//
// response-enum.hpp
//
#include <string>


namespace sfml_util
{

    struct Response
    {
        enum Enum : unsigned int
        {
            Error    = 0,
            None     = Error,
            Okay     = 1 << 0,
            Continue = 1 << 1,
            Yes      = 1 << 2,
            No       = 1 << 3,
            Cancel   = 1 << 4,
            Select   = 1 << 5
        };

        static const std::string ToString(const Response::Enum E);
        static bool IsValid(const Response::Enum E);
        static bool IsAffirmative(const Response::Enum E);
    };

}

#endif //SFMLUTIL_RESPONSE_ENUM_HPP_INCLUDED
