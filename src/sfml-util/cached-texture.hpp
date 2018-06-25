// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_CACHED_TEXTURE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_CACHED_TEXTURE_HPP_INCLUDED
//
// caced-texture.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/handy-types.hpp"
#include "misc/not-null.hpp"

#include <string>
#include <vector>

namespace sf
{
class Texture;
}

namespace boost
{
namespace filesystem
{
    class path;
}
} // namespace boost

namespace heroespath
{
namespace sfml_util
{

    // Responsible for the cached lifetime of, and fast access to, a single sf::Texture.
    class CachedTexture
    {
    public:
        CachedTexture(const CachedTexture &);
        CachedTexture(CachedTexture &&);

        CachedTexture & operator=(const CachedTexture &);
        CachedTexture & operator=(CachedTexture &&);

        explicit CachedTexture(
            const char * const GAME_DATAFILE_KEY,
            const bool WILL_SMOOTH = true,
            const bool WILL_FLIP_HORIZ = false);

        explicit CachedTexture(
            const std::string & GAME_DATAFILE_KEY,
            const bool WILL_SMOOTH = true,
            const bool WILL_FLIP_HORIZ = false);

        explicit CachedTexture(
            const boost::filesystem::path & PATH,
            const bool WILL_SMOOTH = true,
            const bool WILL_FLIP_HORIZ = false);

        CachedTexture(const std::string & FAKE_PATH, const sf::Texture & TEXTURE);

        ~CachedTexture();

        const sf::Texture & Get() const;

        const std::string Path() const { return path_; }

        bool IsPathFake() const { return isPathFake_; }

    private:
        std::string path_;
        std::size_t index_;
        bool isPathFake_;
    };

    using CachedTextureOpt_t = boost::optional<CachedTexture>;

    // Responsible for the cached lifetime of, and fast access to, all the images in a directory.
    class CachedTextures
    {
    public:
        CachedTextures(const CachedTextures &);
        CachedTextures(CachedTextures &&);

        CachedTextures & operator=(const CachedTextures &);
        CachedTextures & operator=(CachedTextures &&);

        CachedTextures(const std::string & GAME_DATAFILE_KEY, const bool WILL_SMOOTH = true);
        CachedTextures(const boost::filesystem::path & DIR_PATH, const bool WILL_SMOOTH = true);

        ~CachedTextures();

        const misc::SizetVec_t & Indexes() const { return indexes_; }

        std::size_t Size() const { return indexes_.size(); }

        const sf::Texture & operator[](const std::size_t INDEX) const;
        const sf::Texture & At(const std::size_t INDEX) const;
        const sf::Texture & Front() const;
        const sf::Texture & Back() const;

        const std::string Path() const { return path_; }

    private:
        std::string path_;
        misc::SizetVec_t indexes_;
    };

    using CachedTexturesOpt_t = boost::optional<CachedTextures>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_CACHED_TEXTURE_HPP_INCLUDED
