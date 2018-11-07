// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// music-info.cpp
//
#include "music-info.hpp"

namespace heroespath
{
namespace gui
{

    MusicInfo::MusicInfo(
        const music::Enum WHICH,
        const bool IS_LOOPED,
        const std::string & ARTIST_NAME,
        const std::string & SONG_NAME,
        const std::string & LICENSE_TITLE,
        const std::string & FILE_NAME,
        const std::string & DIR_NAME,
        const gui::Time & DURATION)
        : which_(WHICH)
        , isLooped_(IS_LOOPED)
        , artistName_(ARTIST_NAME)
        , songName_(SONG_NAME)
        , licenseTitle_(LICENSE_TITLE)
        , filename_(FILE_NAME)
        , dirName_(DIR_NAME)
        , duration_(DURATION)
    {}

    void MusicInfo::AutoSetup()
    {
        if (music::IsValid(which_))
        {
            isLooped_ = music::IsLooped(which_);
            artistName_ = music::ArtistName(which_);
            songName_ = music::SongName(which_);
            licenseTitle_ = music::LicenseTitle(which_);
            filename_ = music::Filename(which_);
            dirName_ = music::Directory(which_);
            // duration_ not set intentionally, see setter in header,
            // and see its only use in MusicOperator's constructor.
        }
    }

    bool operator==(const MusicInfo & L, const MusicInfo & R)
    {
        return std::tie(
                   L.which_,
                   L.isLooped_,
                   L.artistName_,
                   L.songName_,
                   L.licenseTitle_,
                   L.filename_,
                   L.dirName_,
                   L.duration_)
            == std::tie(
                   R.which_,
                   R.isLooped_,
                   R.artistName_,
                   R.songName_,
                   R.licenseTitle_,
                   R.filename_,
                   R.dirName_,
                   R.duration_);
    }
} // namespace gui
} // namespace heroespath
