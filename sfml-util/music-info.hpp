#ifndef SFMLUTIL_MUSICINFO_INCLUDED
#define SFMLUTIL_MUSICINFO_INCLUDED
//
// music-info.hpp
//  A class that completely defines a piece of music.
//
#include "sfml-util/music-enum.hpp"
#include "sfml-util/date-time.hpp"

#include <string>
#include <vector>


namespace sfml_util
{

    //simple wrapper class for information known about a piece of music
    class MusicInfo
    {
    public:
        explicit MusicInfo(const music::Enum WHICH_MUSIC);

        explicit MusicInfo(const music::Enum       WHICH_MUSIC,
                           const bool              IS_LOOPED,
                           const std::string &     ARTIST_NAME,
                           const std::string &     SONG_NAME,
                           const std::string &     LICENSE_TITLE,
                           const std::string &     FILE_NAME,
                           const std::string &     DIR_NAME,
                           const sfml_util::Time & DURATION = sfml_util::Time());

        inline music::Enum Which() const                { return which_; }
        inline bool IsLooped() const                    { return isLooped_; }
        inline const std::string WhichName() const      { return music::ToString(which_); }
        inline const std::string ArtistName() const     { return artistName_; }
        inline const std::string SongName() const       { return songName_; }
        inline const std::string LicenseTitle() const   { return licenseTitle_; }
        inline const std::string Filename() const       { return filename_; }
        inline const std::string DirName() const        { return dirName_; }

        inline const sfml_util::Time Duration() const { return duration_; }
        inline void Duration(const sfml_util::Time & DURATION) { duration_ = DURATION; }

        friend bool operator==(const MusicInfo & L, const MusicInfo & R);

    private:
        music::Enum which_;
        bool        isLooped_;
        std::string artistName_;
        std::string songName_;
        std::string licenseTitle_;
        std::string filename_;
        std::string dirName_;
        sfml_util::Time duration_;
    };

    using MusicInfoSVec_t = std::vector<MusicInfo>;

    bool operator==(const MusicInfo & L, const MusicInfo & R);
    inline bool operator!=(const MusicInfo & L, const MusicInfo & R) { return !(L == R); }

}

#endif //SFMLUTIL_MUSICINFO_INCLUDED
