// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_TEXTURECACHE_HPP_INCLUDED
#define HEROESPATH_GUI_TEXTURECACHE_HPP_INCLUDED
//
// texture-cache.hpp
//
#include "gui/image-options.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"

#include <SFML/Graphics/Texture.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace gui
{

    using PathOptPair_t = std::pair<std::string, ImageOptions>;

    using CountDataPair_t = std::pair<int, long long>;

    using NameToTextureInfoMap_t = misc::VectorMap<std::string, CountDataPair_t>;

    struct TextureIndexes
    {
        TextureIndexes(
            const std::size_t REF_COUNT = 0, const std::vector<std::size_t> & INDEXES = {})
            : ref_count(REF_COUNT)
            , indexes(INDEXES)
        {}

        std::size_t ref_count;
        std::vector<std::size_t> indexes;
    };

    using PathOptToIndexesMap_t = misc::VectorMap<PathOptPair_t, TextureIndexes>;

    using PathOptToIndexesPair_t = PathOptToIndexesMap_t::value_type;

    // Responsible for storing references counted textures paired with image options (such as
    // flip/invert/etc.) that can added and removed by the filesystem path but can be accessed by
    // simple/fast index lookup.
    class TextureCache
    {
        TextureCache(const TextureCache &);
        TextureCache & operator=(const TextureCache &);

    public:
        TextureCache();
        ~TextureCache();

        static misc::NotNull<TextureCache *> Instance();
        static void Acquire();
        static void Release();

        // all Add...() functions increment the ref_count
        std::size_t AddByKey(
            const std::string & GAMEDATAFILE_KEY_STR,
            const ImageOptions & OPTIONS = ImageOptions());

        std::size_t AddByPath(
            const std::string & PATH_TO_TEXTURE_STR, const ImageOptions & OPTIONS = ImageOptions());

        const std::tuple<std::size_t, misc::NotNull<const sf::Texture *>>
            AddByPathAndReturnIndexAndRef(
                const std::string & PATH_TO_TEXTURE_STR,
                const ImageOptions & OPTIONS = ImageOptions());

        // if the given fake path and options ARE already in the cache then:
        //  - the texture given will be assumed identical with what is already in the cache
        //  - the options specified will NOT be applied
        //  - the texture in the cache will NOT be updated
        //  - the ref_count WILL be incremented
        //  - the pre-existing cached index will be returned
        //
        // if the given fake path and options are NOT already in the cache then:
        //  - the options specified WILL be applied to the given texture
        //  - the given altered texture will be cached under the fake path and options given
        std::size_t AddByPathFake(
            const std::string & FAKE_PATH_TO_TEXTURE_STR,
            const sf::Texture &,
            const ImageOptions & OPTIONS = ImageOptions());

        const std::tuple<std::size_t, misc::NotNull<const sf::Texture *>>
            AddByPathFakeAndReturnIndexAndRef(
                const std::string & FAKE_PATH_TO_TEXTURE_STR,
                const sf::Texture &,
                const ImageOptions & OPTIONS = ImageOptions());

        // not recursive
        const std::vector<std::size_t> AddDirectoryByKey(
            const std::string & DIR_PATH_KEY, const ImageOptions & OPTIONS = ImageOptions());

        // not recursive
        const std::vector<std::size_t> AddDirectoryByPath(
            const std::string & DIR_PATH_PARAM_STR, const ImageOptions & OPTIONS = ImageOptions());

        // all Remove...() functions decrement the ref_count
        void RemoveByKey(
            const std::string & GAMEDATAFILE_KEY_STR,
            const ImageOptions & OPTIONS = ImageOptions());

        void RemoveByPath(
            const std::string & PATH_TO_TEXTURE_STR, const ImageOptions & OPTIONS = ImageOptions());

        // reduces all ref_counts to zero and destroys all textures and saved paths
        // once CachedTextures are in place this becomes not only useless but dangerous
        void RemoveAll();

        // the following functions (all functions that accept an INDEX) throw if INDEX is out of
        // range or not the location of a cached texture

        // this function does not increment the ref_count, only functions that start with "Add"
        // increment the reference count
        const sf::Texture & GetByIndex(const std::size_t INDEX) const;

        const ImageOptions GetOptionsByIndex(const std::size_t INDEX) const;
        const std::string GetPathByIndex(const std::size_t INDEX) const;
        std::size_t GetRefCountByIndex(const std::size_t INDEX) const;

    private:
        const PathOptToIndexesPair_t FindInfoByIndex(const std::size_t INDEX) const;

        void Reset();

        std::size_t FindNextAvailableIndex();

        std::size_t AddByPathInternal(
            const std::string & PATH_TO_TEXTURE_STR, const ImageOptions & OPTIONS);

        std::size_t AddByPathInternalFake(const sf::Texture &, const ImageOptions & OPTIONS);

        void RemoveInternal(const std::size_t INDEX);

        long long TextureSizeInBytes(const sf::Texture &) const;

        void UpdateCountAndSizeTracker(const sf::Texture &);

        enum class LogDumpContext
        {
            Destruct,
            Reset
        };

        void DumpCacheToLog(const LogDumpContext CONTEXT) const;

        const std::string BytesToString(const long long BYTES) const;

    private:
        static std::unique_ptr<TextureCache> instanceUPtr_;
        std::vector<std::unique_ptr<sf::Texture>> textureUPtrs_;
        PathOptToIndexesMap_t pathOptToIndexesMap_;
        NameToTextureInfoMap_t stageToCountDataMap_;
        int cachedCount_;
        long long cachedDataBytes_;
        long long totalLoadedDataBytes_;
        std::string stageName_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_TEXTURECACHE_HPP_INCLUDED
