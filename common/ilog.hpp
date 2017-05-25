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
#ifndef APPBASE_ILOG_HPP
#define APPBASE_ILOG_HPP
//
// ilog.hpp
//
#include <string>


namespace appbase
{
    //simple wrapper for logger priority enum
    struct LogPri
    {
        enum Enum
        {
            None = 0,
            Debug,
            Default,
            Warn,
            Warning = Warn,
            Error,
            Fatal,
            Count,
            Disabled = Count,
        };

        static const std::string GetName(const Enum X)
        {
            switch(X)
            {
                case Debug:     { return "Debug";   }
                case Default:   { return "Default"; }
                case Warn:      { return "Warning"; }
                case Error:     { return "Error";   }
                case Fatal:     { return "Fatal";   }
                case None:
                case Count:
                default:        { return "None";    }
            }
        }
    };


    //
    // ILog Class
    //  Usage interface
    class ILog
    {
    public:
        virtual ~ILog() {}

        virtual void Log(const std::string & MSG) = 0;

        virtual void Log(const std::string & FILE,
                         const int LINE, //why signed?
                         const std::string & MSG) = 0;

        virtual void Log(const LogPri::Enum Default,
                         const std::string & MSG) = 0;

        virtual void Log(const LogPri::Enum Default,
                         const std::string & FILE,
                         const int LINE, //why signed?
                         const std::string & MSG) = 0;
    };

}//end of namespace appbase


#endif //APPBASE_ILOG_HPP
