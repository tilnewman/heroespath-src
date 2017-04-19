//
// music-info.cpp
//
#include "music-info.hpp"


namespace sfml_util
{

    MusicInfo::MusicInfo(const music::Enum WHICH)
    :
        which_       (WHICH),
        isLooped_    (music::IsLooped(WHICH)),
        artistName_  (music::ArtistName(WHICH)),
        songName_    (music::SongName(WHICH)),
        licenseTitle_(music::LicenseTitle(WHICH)),
        filename_    (music::Filename(WHICH)),
        dirName_     (music::Directory(WHICH)),
        duration_    ()
    {}


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
