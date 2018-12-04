// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_CACHED_TEXTURE_HPP_INCLUDED
#define HEROESPATH_GUI_CACHED_TEXTURE_HPP_INCLUDED
//
// cached-texture.hpp
//
#include "gui/image-options.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <SFML/Graphics/Texture.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{

// i just made this because I needed two constructors of CachedTexture to differ on the first
// type but they were both std::strings, so this is just a hack wrapper to change the type
struct PathWrapper
{
    PathWrapper() = delete;

    explicit PathWrapper(const std::string & PATH_STR)
        : path_str(PATH_STR)
    {}

    PathWrapper(const PathWrapper &) = default;
    PathWrapper(PathWrapper &&) = default;
    PathWrapper & operator=(const PathWrapper &) = default;
    PathWrapper & operator=(PathWrapper &&) = default;

    std::string path_str;
};

namespace gui
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
            const char * const GAME_DATAFILE_KEY, const ImageOptions & OPTIONS = ImageOptions());

        explicit CachedTexture(
            const std::string & GAME_DATAFILE_KEY, const ImageOptions & OPTIONS = ImageOptions());

        explicit CachedTexture(
            const PathWrapper & PATH_WRAPPER, const ImageOptions & OPTIONS = ImageOptions());

        CachedTexture(
            const std::string & FAKE_PATH,
            const sf::Texture & TEXTURE,
            const ImageOptions & OPTIONS = ImageOptions());

        ~CachedTexture();

        const sf::Texture & Get() const { return *texturePtr_; }
        std::size_t Index() const { return index_; }
        const std::string Path() const { return path_; }
        const ImageOptions Options() const { return options_; }

        // this value is not stored in this object so this call is expensive and only really
        // provided for testing
        std::size_t RefCount() const;

        friend void swap(CachedTexture & l, CachedTexture & r);

    private:
        void Release();

        std::string path_;
        std::size_t index_;
        ImageOptions options_;
        misc::NotNull<const sf::Texture *> texturePtr_;

        static const sf::Texture alwaysEmptyTexture_;
    };

    inline void swap(CachedTexture & l, CachedTexture & r)
    {
        // enable ADL (not always necessary, but good practice)
        using std::swap;

        swap(l.path_, r.path_);
        swap(l.index_, r.index_);
        swap(l.options_, r.options_);
        swap(l.texturePtr_, r.texturePtr_);
    }

    inline bool operator==(const CachedTexture & L, const CachedTexture & R)
    {
        return (L.Index() == R.Index());
    }

    inline bool operator!=(const CachedTexture & L, const CachedTexture & R) { return !(L == R); }

    inline bool operator<(const CachedTexture & L, const CachedTexture & R)
    {
        return (L.Index() < R.Index());
    }

    inline bool operator>(const CachedTexture & L, const CachedTexture & R) { return (R < L); }

    inline bool operator<=(const CachedTexture & L, const CachedTexture & R) { return !(L > R); }

    inline bool operator>=(const CachedTexture & L, const CachedTexture & R) { return !(L < R); }

    using CachedTextureUPtr_t = std::unique_ptr<CachedTexture>;
    using CachedTextureOpt_t = boost::optional<CachedTexture>;
    using CachedTextureVec_t = std::vector<CachedTexture>;

    // Responsible for the cached lifetime of, and fast access to, all the images in a directory.
    class CachedTextures
    {
    public:
        CachedTextures(const CachedTextures &);
        CachedTextures(CachedTextures &&);

        CachedTextures & operator=(const CachedTextures &);
        CachedTextures & operator=(CachedTextures &&);

        explicit CachedTextures(
            const std::string & GAME_DATAFILE_KEY, const ImageOptions & OPTIONS = ImageOptions());

        explicit CachedTextures(
            const PathWrapper & DIR_PATH_WRAPPER, const ImageOptions & OPTIONS = ImageOptions());

        ~CachedTextures();

        const std::vector<std::size_t> Indexes() const { return indexes_; }
        bool Empty() const { return indexes_.empty(); }
        std::size_t Size() const { return indexes_.size(); }
        const std::string Path() const { return path_; }
        const ImageOptions Options() const { return options_; }

        // this value is not stored in this object so this call is expensive and only really
        // provided for testing
        std::size_t RefCount() const;

        const sf::Texture & operator[](const std::size_t INDEX) const;
        const sf::Texture & At(const std::size_t INDEX) const;
        const sf::Texture & Front() const;
        const sf::Texture & Back() const;

        friend void swap(CachedTextures & l, CachedTextures & r);

        friend bool operator==(const CachedTextures & L, const CachedTextures & R);
        friend bool operator!=(const CachedTextures & L, const CachedTextures & R);
        friend bool operator<(const CachedTextures & L, const CachedTextures & R);

    private:
        void Release();

        std::string path_;
        std::vector<std::size_t> indexes_;
        ImageOptions options_;
    };

    inline void swap(CachedTextures & l, CachedTextures & r)
    {
        // enable ADL (not always necessary, but good practice)
        using std::swap;

        swap(l.path_, r.path_);
        swap(l.indexes_, r.indexes_);
        swap(l.options_, r.options_);
    }

    inline bool operator==(const CachedTextures & L, const CachedTextures & R)
    {
        return (L.indexes_ == R.indexes_);
    }

    inline bool operator!=(const CachedTextures & L, const CachedTextures & R) { return !(L == R); }

    inline bool operator<(const CachedTextures & L, const CachedTextures & R)
    {
        return (L.indexes_ < R.indexes_);
    }

    inline bool operator>(const CachedTextures & L, const CachedTextures & R) { return (R < L); }

    inline bool operator<=(const CachedTextures & L, const CachedTextures & R) { return !(L > R); }

    inline bool operator>=(const CachedTextures & L, const CachedTextures & R) { return !(L < R); }

    using CachedTexturesUPtr_t = std::unique_ptr<CachedTextures>;
    using CachedTexturesOpt_t = boost::optional<CachedTextures>;

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_CACHED_TEXTURE_HPP_INCLUDED
