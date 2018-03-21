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
#ifndef HEROESPATH_SFMLUTIL_MUSICINFO_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MUSICINFO_HPP_INCLUDED
//
// music-info.hpp
//  A class that completely defines a piece of music.
//
#include "sfml-util/date-time.hpp"
#include "sfml-util/music-enum.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // simple wrapper class for information known about a piece of music
    class MusicInfo
    {
    public:
        explicit MusicInfo(
            const music::Enum WHICH_MUSIC = music::None,
            const bool IS_LOOPED = false,
            const std::string & ARTIST_NAME = "",
            const std::string & SONG_NAME = "",
            const std::string & LICENSE_TITLE = "",
            const std::string & FILE_NAME = "",
            const std::string & DIR_NAME = "",
            const sfml_util::Time & DURATION = sfml_util::Time());

        music::Enum Which() const { return which_; }
        bool IsLooped() const { return isLooped_; }
        const std::string WhichName() const { return music::ToString(which_); }
        const std::string ArtistName() const { return artistName_; }
        const std::string SongName() const { return songName_; }
        const std::string LicenseTitle() const { return licenseTitle_; }
        const std::string Filename() const { return filename_; }
        const std::string DirName() const { return dirName_; }

        const sfml_util::Time Duration() const { return duration_; }
        void Duration(const sfml_util::Time & DURATION) { duration_ = DURATION; }

        void AutoSetup();

        friend bool operator==(const MusicInfo & L, const MusicInfo & R);

    private:
        music::Enum which_;
        bool isLooped_;
        std::string artistName_;
        std::string songName_;
        std::string licenseTitle_;
        std::string filename_;
        std::string dirName_;
        sfml_util::Time duration_;
    };

    using MusicInfoVec_t = std::vector<MusicInfo>;

    bool operator==(const MusicInfo & L, const MusicInfo & R);

    inline bool operator!=(const MusicInfo & L, const MusicInfo & R) { return !(L == R); }
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MUSICINFO_HPP_INCLUDED
