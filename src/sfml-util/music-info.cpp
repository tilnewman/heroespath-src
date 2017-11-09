// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// music-info.cpp
//
#include "music-info.hpp"


namespace heroespath
{
namespace sfml_util
{

    MusicInfo::MusicInfo(const music::Enum   WHICH,
                         const bool          IS_LOOPED,
                         const std::string & ARTIST_NAME,
                         const std::string & SONG_NAME,
                         const std::string & LICENSE_TITLE,
                         const std::string & FILE_NAME,
                         const std::string & DIR_NAME,
                         const sfml_util::Time & DURATION)
    :
        which_       (WHICH),
        isLooped_    (IS_LOOPED),
        artistName_  (ARTIST_NAME),
        songName_    (SONG_NAME),
        licenseTitle_(LICENSE_TITLE),
        filename_    (FILE_NAME),
        dirName_     (DIR_NAME),
        duration_    (DURATION)
    {}


    void MusicInfo::AutoSetup()
    {
        if ((which_ != music::Count) &&
            (which_ != music::All) &&
            (which_ != music::None))
        {
            isLooped_ = music::IsLooped(which_);
            artistName_ = music::ArtistName(which_);
            songName_ = music::SongName(which_);
            licenseTitle_ = music::LicenseTitle(which_);
            filename_ = music::Filename(which_);
            dirName_ = music::Directory(which_);
            //duration_ not set intentionally, see setter in header,
            //and see it's only use in MusicOperator's constructor.
        }
    }


    bool operator==(const MusicInfo & L, const MusicInfo & R)
    {
        return std::tie(L.which_,
                        L.isLooped_,
                        L.artistName_,
                        L.songName_,
                        L.licenseTitle_,
                        L.filename_,
                        L.dirName_,
                        L.duration_)
            ==
            std::tie(R.which_,
                     R.isLooped_,
                     R.artistName_,
                     R.songName_,
                     R.licenseTitle_,
                     R.filename_,
                     R.dirName_,
                     R.duration_);
    }

}
}
