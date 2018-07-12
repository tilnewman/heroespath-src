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
#include "sfml-util/texture-options-enum.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>
#include <vector>

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
            const TextureOpt::Enum OPTIONS = TextureOpt::Default);

        explicit CachedTexture(
            const std::string & GAME_DATAFILE_KEY,
            const TextureOpt::Enum OPTIONS = TextureOpt::Default);

        explicit CachedTexture(
            const boost::filesystem::path & PATH,
            const TextureOpt::Enum OPTIONS = TextureOpt::Default);

        CachedTexture(
            const std::string & FAKE_PATH,
            const sf::Texture & TEXTURE,
            const TextureOpt::Enum OPTIONS = TextureOpt::Default);

        ~CachedTexture();

        const sf::Texture & Get() const;

        std::size_t Index() const { return index_; }
        const std::string Path() const { return path_; }
        TextureOpt::Enum Options() const { return options_; }

        std::size_t RefCount() const;

        friend bool operator==(const CachedTexture & L, const CachedTexture & R);
        friend bool operator!=(const CachedTexture & L, const CachedTexture & R);

    private:
        void Release();

        std::string path_;
        std::size_t index_;
        TextureOpt::Enum options_;
    };

    inline bool operator==(const CachedTexture & L, const CachedTexture & R)
    {
        return ((L.path_ == R.path_) && (L.index_ == R.index_) && (R.options_ == L.options_));
    }

    inline bool operator!=(const CachedTexture & L, const CachedTexture & R) { return !(L == R); }

    using CachedTextureOpt_t = boost::optional<CachedTexture>;

    // Responsible for the cached lifetime of, and fast access to, all the images in a directory.
    class CachedTextures
    {
    public:
        CachedTextures(const CachedTextures &);
        CachedTextures(CachedTextures &&);

        CachedTextures & operator=(const CachedTextures &);
        CachedTextures & operator=(CachedTextures &&);

        CachedTextures(
            const std::string & GAME_DATAFILE_KEY,
            const TextureOpt::Enum OPTIONS = TextureOpt::Default);

        CachedTextures(
            const boost::filesystem::path & DIR_PATH,
            const TextureOpt::Enum OPTIONS = TextureOpt::Default);

        ~CachedTextures();

        const std::vector<std::size_t> Indexes() const { return indexes_; }
        std::size_t Size() const { return indexes_.size(); }
        const std::string Path() const { return path_; }
        TextureOpt::Enum Options() const { return options_; }

        std::size_t RefCount() const;

        const sf::Texture & operator[](const std::size_t INDEX) const;
        const sf::Texture & At(const std::size_t INDEX) const;
        const sf::Texture & Front() const;
        const sf::Texture & Back() const;

        friend bool operator==(const CachedTextures & L, const CachedTextures & R);
        friend bool operator!=(const CachedTextures & L, const CachedTextures & R);

    private:
        void Release();

        std::string path_;
        std::vector<std::size_t> indexes_;
        TextureOpt::Enum options_;
    };

    inline bool operator==(const CachedTextures & L, const CachedTextures & R)
    {
        return ((L.path_ == R.path_) && (L.options_ == R.options_) && (L.indexes_ == R.indexes_));
    }

    inline bool operator!=(const CachedTextures & L, const CachedTextures & R) { return !(L == R); }

    using CachedTexturesOpt_t = boost::optional<CachedTextures>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_CACHED_TEXTURE_HPP_INCLUDED
