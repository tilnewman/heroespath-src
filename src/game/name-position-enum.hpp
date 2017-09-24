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
#ifndef GAME_NAMEPOSITIONENUM_HPP_INCLUDED
#define GAME_NAMEPOSITIONENUM_HPP_INCLUDED
//
// name-position-enum.hpp
//
#include <string>
#include <tuple>


namespace game
{

    struct NamePosition
    {
        enum Enum
        {
            NoName = 0,
            SourceBefore,
            SourceAfter,
            SourceThenTarget,
            TargetBefore,
            TargetAfter,
            TargetThenSource,
            Count
        };

        static const std::string ToString(const Enum);
    };


    class ContentAndNamePos
    {
    public:
        explicit ContentAndNamePos(const std::string &      PRE_STR       = "",
                                   const std::string &      CONTENT_STR   = "",
                                   const std::string &      POST_STR      = "",
                                   const NamePosition::Enum NAME_POS_ENUM = NamePosition::NoName);

        ContentAndNamePos(const std::string &      CONTENT_STR,
                          const NamePosition::Enum NAME_POS_ENUM);

        inline const std::string Pre() const        { return pre_; }
        inline const std::string Content() const    { return content_; }
        inline const std::string Post() const       { return post_; }
        inline NamePosition::Enum NamePos() const   { return posEnum_; }

        const std::string Compose(const std::string & SOURCE_NAME,
                                  const std::string & TARGET_NAME) const;

        friend bool operator==(const ContentAndNamePos & L, const ContentAndNamePos & R);
        friend bool operator<(const ContentAndNamePos & L, const ContentAndNamePos & R);

    private:
        std::string pre_;
        std::string content_;
        std::string post_;
        NamePosition::Enum posEnum_;
    };


    inline bool operator==(const ContentAndNamePos & L, const ContentAndNamePos & R)
    {
        return std::tie(L.pre_, L.content_, L.post_, L.posEnum_)
               ==
               std::tie(R.pre_, R.content_, R.post_, R.posEnum_);
    }

    inline bool operator!=(const ContentAndNamePos & L, const ContentAndNamePos & R)
    {
        return ! (L == R);
    }

    inline bool operator<(const ContentAndNamePos & L, const ContentAndNamePos & R)
    {
        return std::tie(L.pre_, L.content_, L.post_, L.posEnum_)
               <
               std::tie(R.pre_, R.content_, R.post_, R.posEnum_);
    }

}

#endif //GAME_NAMEPOSITIONENUM_HPP_INCLUDED
