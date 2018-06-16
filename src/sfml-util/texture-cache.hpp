// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_TEXTURECACHE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_TEXTURECACHE_HPP_INCLUDED
//
// texture-cache.hpp
//
#include "misc/handy-types.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    using StrToTextureInfoMap_t = misc::VectorMap<std::string, std::pair<int, long long>>;

    struct TextureIndexes
    {
        TextureIndexes(const std::size_t REF_COUNT = 0, const misc::SizetVec_t & INDEXES = {})
            : ref_count(REF_COUNT)
            , indexes(INDEXES)
        {}

        std::size_t ref_count;
        misc::SizetVec_t indexes;
    };

    using StrToIndexesMap_t = misc::VectorMap<std::string, TextureIndexes>;

    // stores textures in a single location and makes them available by index.
    // Index zero is always invalid.  Attempting to use index zero in these
    // functions will result in an error log, not an exception.
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
            const bool WILL_SMOOTH = true,
            const bool WILL_FLIP_HORIZ = false);

        std::size_t AddByPath(
            const std::string & PATH_TO_TEXTURE_STR,
            const bool WILL_SMOOTH = true,
            const bool WILL_FLIP_HORIZ = false);

        std::size_t
            AddByPathFake(const std::string & FAKE_PATH_TO_TEXTURE_STR, const sf::Texture &);

        // not recursive
        const misc::SizetVec_t AddDirectoryByKey(
            const std::string & DIR_PATH_KEY,
            const bool WILL_SMOOTH = true,
            const bool WILL_FLIP_HORIZ = false);

        // not recursive
        const misc::SizetVec_t AddDirectoryByPath(
            const std::string & DIR_PATH_PARAM_STR,
            const bool WILL_SMOOTH = true,
            const bool WILL_FLIP_HORIZ = false);

        // all Remove...() functions decrement the ref_count
        void RemoveByKey(const std::string & GAMEDATAFILE_KEY_STR);
        void RemoveByPath(const std::string & PATH_TO_TEXTURE_STR);
        void RemoveByIndex(const std::size_t INDEX);

        // reduces all ref_counts to zero and destroys all textures and saved paths
        void RemoveAll();

        // this function does not change the ref_count
        const sf::Texture & GetByIndex(const std::size_t INDEX) const;

    private:
        void Reset();

        std::size_t FindNextAvailableIndex();

        std::size_t AddByPathInternal(
            const std::string & PATH_TO_TEXTURE_STR,
            const bool WILL_SMOOTH,
            const bool WILL_FLIP_HORIZ);

        std::size_t AddByPathInternalFake(const sf::Texture &);

        void RemoveInternal(const std::size_t INDEX);

        long long TextureSizeInBytes(const sf::Texture &) const;

        void UpdateCoundAndSizeTracker(const sf::Texture &);

        void LogStageTextureCounts() const;

    private:
        static std::unique_ptr<TextureCache> instanceUPtr_;
        TextureUVec_t textureUPtrs_;
        StrToIndexesMap_t strToIndexesMap_;
        StrToTextureInfoMap_t stageNameToTextureMaxMap_;
        int textureCount_;
        long long dataSizeBytes_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXTURECACHE_HPP_INCLUDED
