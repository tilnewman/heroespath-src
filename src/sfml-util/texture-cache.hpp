///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef SFMLUTIL_TEXTURECACHE_HPP_INCLUDED
#define SFMLUTIL_TEXTURECACHE_HPP_INCLUDED
//
// texture-cache.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include "misc/handy-types.hpp"

#include <map>
#include <string>
#include <memory>


namespace sfml_util
{

    using StrToSizetVecMap_t = std::map<std::string, misc::SizetVec_t>;
    

    //stores textures in a single location and makes them available by index
    class TextureCache
    {
        //prevent copy construction
        TextureCache(const TextureCache &);

        //prevent copy assignment
        TextureCache & operator=(const TextureCache &);

    public:
        TextureCache();
        ~TextureCache();

        static TextureCache * Instance();
        static void Acquire();
        static void Release();

        std::size_t AddByKey(const std::string & GAMEDATAFILE_KEY_STR,
                             const bool          WILL_SMOOTH = true);

        std::size_t AddByPath(const std::string & PATH_TO_TEXTURE_STR,
                              const bool          WILL_SMOOTH = true);

        const misc::SizetVec_t AddAllInDirectoryByKey(const std::string & DIR_PATH_KEY,
                                                      const bool          WILL_SMOOTH = true);

        const misc::SizetVec_t AddAllInDirectoryByPath(const std::string & DIR_PATH_PARAM_STR,
                                                       const bool          WILL_SMOOTH = true);

        void RemoveByKey(const std::string & GAMEDATAFILE_KEY_STR);

        void RemoveByPath(const std::string & PATH_TO_TEXTURE_STR);

        void RemoveByIndex(const std::size_t INDEX);

        void RemoveByIndexVec(const misc::SizetVec_t & INDEX_VEC);

        void RemoveAll();

        TexturePtr_t GetPtrByIndex(const std::size_t INDEX) const;

        const sf::Texture & GetByIndex(const std::size_t INDEX) const;

    private:
        std::size_t EstablishNextAvailableIndex();
        std::size_t AddByPathInternal(const std::string & PATH_TO_TEXTURE_STR,
                                      const bool          WILL_SMOOTH);

    private:
        static std::unique_ptr<TextureCache> instanceUPtr_;
        TextureUVec_t cacheUVec_;

        //Keys on the result of GameDataFile::GetMediaPath(),
        //NOT the complete system path.
        StrToSizetVecMap_t strToVecMap_;
    };
}

#endif //SFMLUTIL_TEXTURECACHE_HPP_INCLUDED
