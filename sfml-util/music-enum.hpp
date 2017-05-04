#ifndef SFMLUTIL_MUSICENUM_INCLUDED
#define SFMLUTIL_MUSICENUM_INCLUDED
//
// music-enum.hpp
//  An enum defining the various background music files
//
#include <string>
#include <vector>
#include <set>


namespace sfml_util
{

    struct music
    {
        enum Enum
        {
            Theme = 0,
            Wind,
            Fire1,
            Fire2,
            CombatIntro,
            PartyCreation,
            Credits1,
            Credits2,
            Count,
            None,
            All
        };

        static const std::string ToString(const music::Enum);
        static const std::string FileExt(const music::Enum);
        static const std::string Filename(const music::Enum);
        static const std::string Directory(const music::Enum);
        static const std::string ArtistName(const music::Enum);
        static const std::string LicenseTitle(const music::Enum);
        static const std::string SongName(const music::Enum);
        static bool IsLooped(const music::Enum);
    };

    using MusicEnumVec_t = std::vector<music::Enum>;
    using MusicEnumSet_t = std::set<music::Enum>;

}
#endif //SFMLUTIL_MUSICENUM_INCLUDED
