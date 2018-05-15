// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-response-enum.cpp
//
#include "popup-response-enum.hpp"

namespace heroespath
{
namespace popup
{
    void ResponseTypes::ToStringPopulate(
        std::ostringstream & ss,
        const misc::EnumUnderlying_t ENUM_VALUE,
        const std::string & SEPARATOR)
    {
        AppendNameIfBitIsSet(ss, ENUM_VALUE, ResponseTypes::Okay, "Okay", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, ResponseTypes::Continue, "Continue", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, ResponseTypes::Yes, "Yes", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, ResponseTypes::No, "No", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, ResponseTypes::Cancel, "Cancel", SEPARATOR);
        AppendNameIfBitIsSet(ss, ENUM_VALUE, ResponseTypes::Select, "Select", SEPARATOR);
    }

    bool ResponseTypes::IsAffirmative(const ResponseTypes::Enum RESPONSE_TYPE)
    {
        switch (RESPONSE_TYPE)
        {
            case Okay:
            case Continue:
            case Yes:
            case Select:
            {
                return true;
            }
            case No:
            case Cancel:
            {
                return false;
            }
            case Error:
            default:
            {
                ThrowInvalidValueForFunction(RESPONSE_TYPE, "IsAffirmative");
            }
        }
    }

} // namespace popup
} // namespace heroespath
