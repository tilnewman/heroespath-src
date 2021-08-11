// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_MUSICINFO_HPP_INCLUDED
#define HEROESPATH_GUI_MUSICINFO_HPP_INCLUDED
//
// music-info.hpp
//  A class that completely defines a piece of music.
//
#include "gui/date-time.hpp"
#include "gui/music-enum.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace gui
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
            const gui::Time & DURATION = gui::Time());

        music::Enum Which() const { return which_; }
        bool IsLooped() const { return isLooped_; }
        const std::string WhichName() const { return music::ToString(which_); }
        const std::string ArtistName() const { return artistName_; }
        const std::string SongName() const { return songName_; }
        const std::string LicenseTitle() const { return licenseTitle_; }
        const std::string Filename() const { return filename_; }
        const std::string DirName() const { return dirName_; }

        const gui::Time Duration() const { return duration_; }
        void Duration(const gui::Time & DURATION) { duration_ = DURATION; }

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
        gui::Time duration_;
    };

    using MusicInfoVec_t = std::vector<MusicInfo>;

    bool operator==(const MusicInfo & L, const MusicInfo & R);

    inline bool operator!=(const MusicInfo & L, const MusicInfo & R) { return !(L == R); }
} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_MUSICINFO_HPP_INCLUDED
