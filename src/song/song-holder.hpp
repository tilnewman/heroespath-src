// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SONG_SONG_HOLDER_HPP_INCLUDED
#define HEROESPATH_SONG_SONG_HOLDER_HPP_INCLUDED
//
// song-holder.hpp
//
#include "misc/not-null.hpp"
#include "song/song-enum.hpp"

#include <memory>
#include <vector>

namespace heroespath
{

namespace stage
{
    struct IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;
} // namespace stage

namespace song
{

    // forward declarations
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
    using SongUPtr_t = std::unique_ptr<Song>;
    using SongUVec_t = std::vector<SongUPtr_t>;

    // Responsible for the lifetimes of all Songs in the game.
    struct Holder
    {
        static void Fill();
        static void Empty();
        static bool Test(stage::IStagePtr_t iStagePtr);
        static const SongPtr_t Get(const Songs::Enum);

    private:
        static SongUVec_t songsUVec_;
    };

} // namespace song
} // namespace heroespath

#endif // HEROESPATH_SONG_SONG_HOLDER_HPP_INCLUDED
