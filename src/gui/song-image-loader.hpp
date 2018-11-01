// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SONG_IMAGE_LOADER_HPP_INCLUDED
#define HEROESPATH_GUI_SONG_IMAGE_LOADER_HPP_INCLUDED
//
// song-image-loader.hpp
//
#include "gui/enum-image-loader.hpp"
#include "misc/config-file.hpp"
#include "song/song-enum.hpp"

namespace heroespath
{
namespace gui
{

    // Responsible for loading Song images.
    class SongImageLoader : public EnumImageLoader<song::Songs>
    {
    public:
        SongImageLoader(const SongImageLoader &) = delete;
        SongImageLoader(SongImageLoader &&) = delete;
        SongImageLoader & operator=(const SongImageLoader &) = delete;
        SongImageLoader & operator=(SongImageLoader &&) = delete;

        SongImageLoader()
            : EnumImageLoader<song::Songs>(
                misc::ConfigFile::Instance()->GetMediaPath("media-images-songs-dir"))
        {}
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SONGIMAGEMANAGER_HPP_INCLUDED
