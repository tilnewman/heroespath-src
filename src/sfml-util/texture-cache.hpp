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

    using StrToSizetVecMap_t = misc::VectorMap<std::string, misc::SizetVec_t>;

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

        std::size_t
            AddByKey(const std::string & GAMEDATAFILE_KEY_STR, const bool WILL_SMOOTH = true);

        std::size_t
            AddByPath(const std::string & PATH_TO_TEXTURE_STR, const bool WILL_SMOOTH = true);

        // not recursive
        const misc::SizetVec_t
            AddAllInDirectoryByKey(const std::string & DIR_PATH_KEY, const bool WILL_SMOOTH = true);

        // not recursive
        const misc::SizetVec_t AddAllInDirectoryByPath(
            const std::string & DIR_PATH_PARAM_STR, const bool WILL_SMOOTH = true);

        void RemoveByKey(const std::string & GAMEDATAFILE_KEY_STR);

        void RemoveByPath(const std::string & PATH_TO_TEXTURE_STR);

        void RemoveByIndex(const std::size_t INDEX);

        void RemoveByIndexVec(const misc::SizetVec_t & INDEX_VEC);

        void RemoveAll();

        const sf::Texture & GetByIndex(const std::size_t INDEX) const;

    private:
        std::size_t EstablishNextAvailableIndex();

        std::size_t
            AddByPathInternal(const std::string & PATH_TO_TEXTURE_STR, const bool WILL_SMOOTH);

        static void ReorderByLastNumber(std::vector<std::string> &);

        static std::size_t FindLastNumber(const std::string &);

    private:
        static std::unique_ptr<TextureCache> instanceUPtr_;
        TextureUVec_t cacheUVec_;

        // Keys on the result of GameDataFile::GetMediaPath(),
        // NOT the complete system path.
        StrToSizetVecMap_t strToVecMap_;
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_TEXTURECACHE_HPP_INCLUDED
