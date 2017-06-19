// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// texture-cache.cpp
//
#include "texture-cache.hpp"

#include "sfml-util/loaders.hpp"

#include "game/log-macros.hpp"
#include "game/game-data-file.hpp"

#include "misc/assertlogandthrow.hpp"

#include <boost/filesystem.hpp>


namespace sfml_util
{

    std::unique_ptr<TextureCache> TextureCache::instanceUPtr_{ nullptr };


    TextureCache::TextureCache()
    :
        cacheUVec_  (),
        strToVecMap_()
    {
        M_HP_LOG_DBG("Singleton Construction: TextureCache");

        //Any value greater than (about) a thousand will work here.
        //Even an aggressize stage should not exceed 500.
        cacheUVec_.reserve(1024);
    }


    TextureCache::~TextureCache()
    {
        M_HP_LOG_DBG("Singleton Construction: TextureCache");
    }


    TextureCache * TextureCache::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            M_HP_LOG_WRN("Singleton Instance() before Acquire(): TextureCache");
            Acquire();
        }

        return instanceUPtr_.get();
    }


    void TextureCache::Acquire()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new TextureCache);
        }
        else
        {
            M_HP_LOG_WRN("Singleton Acquire() after Construction: TextureCache");
        }
    }


    void TextureCache::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((instanceUPtr_.get() != nullptr),
            "sfml_util::TextureCache::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }


    std::size_t TextureCache::AddByKey(const std::string & GAMEDATAFILE_KEY_STR,
                                       const bool          WILL_SMOOTH)
    {
        return AddByPath(game::GameDataFile::Instance()->GetMediaPath(GAMEDATAFILE_KEY_STR),
            WILL_SMOOTH);
    }


    std::size_t TextureCache::AddByPath(const std::string & PATH_TO_TEXTURE_STR,
                                        const bool          WILL_SMOOTH)
    {
        auto const FOUND_ITER{ strToVecMap_.find(PATH_TO_TEXTURE_STR) };
        if (FOUND_ITER != strToVecMap_.end())
        {
            M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER->second.empty() == false),
                "sfml_util::TextureCache::AddByPath(path=\"" << PATH_TO_TEXTURE_STR
                << "\", will_smooth=" << std::boolalpha << WILL_SMOOTH
                << ") failed because strToVecMap_ entry was an empty vec.");

            M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER->second[0] < cacheUVec_.size()),
                "sfml_util::TextureCache::AddByPath(path=\"" << PATH_TO_TEXTURE_STR
                << "\", will_smooth=" << std::boolalpha << WILL_SMOOTH
                << ") failed because strToVecMap_ entry=" << FOUND_ITER->second[0]
                << " was out of bounds with cacheUVec_.size()=" << cacheUVec_.size() << ".");

            M_ASSERT_OR_LOGANDTHROW_SS((cacheUVec_[FOUND_ITER->second[0]].get() != nullptr),
                "sfml_util::TextureCache::AddByPath(path=\"" << PATH_TO_TEXTURE_STR
                << "\", will_smooth=" << std::boolalpha << WILL_SMOOTH
                << ") failed because strToVecMap_ entry pointed to a null pointer.");

            return FOUND_ITER->second[0];
        }

        auto const INDEX{ AddByPathInternal(PATH_TO_TEXTURE_STR, WILL_SMOOTH) };
        strToVecMap_.insert( std::make_pair(PATH_TO_TEXTURE_STR, misc::SizetVec_t(1, INDEX)) );
        return INDEX;
    }


    const misc::SizetVec_t TextureCache::AddAllInDirectoryByKey(const std::string & DIR_PATH_KEY,
                                                                const bool          WILL_SMOOTH)
    {
        return AddAllInDirectoryByPath(game::GameDataFile::Instance()->GetMediaPath(
            DIR_PATH_KEY), WILL_SMOOTH);
    }


    const misc::SizetVec_t TextureCache::AddAllInDirectoryByPath(
        const std::string & DIR_PATH_PARAM_STR,
        const bool          WILL_SMOOTH)
    {
        auto const FOUND_ITER{ strToVecMap_.find(DIR_PATH_PARAM_STR) };
        if (FOUND_ITER != strToVecMap_.end())
        {
            M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER->second.empty() == false),
                "sfml_util::TextureCache::AddAllInDirectoryByPath(path=\"" << DIR_PATH_PARAM_STR
                << "\", will_smooth=" << std::boolalpha << WILL_SMOOTH
                << ") failed because strToVecMap_ entry was an empty vec.");

            return FOUND_ITER->second;
        }

        namespace bfs = boost::filesystem;

        auto const DIR_PATH{ bfs::system_complete(bfs::path(DIR_PATH_PARAM_STR)) };
        auto const DIR_PATH_COMPLETE_STR{ DIR_PATH.string() };

        M_ASSERT_OR_LOGANDTHROW_SS((bfs::exists(DIR_PATH)), "sfml_util::TextureCache::"
            << "AddAllInDirectory(\"" << DIR_PATH_COMPLETE_STR
            << "\") failed because that path does not exist!");

        M_ASSERT_OR_LOGANDTHROW_SS((bfs::is_directory(DIR_PATH)), "sfml_util::TextureCache::"
            << "AddAllInDirectory(\"" << DIR_PATH_COMPLETE_STR
            << "\") failed because that is not a directory.");

        misc::SizetVec_t indexVec;

        bfs::directory_iterator endItr;
        for (bfs::directory_iterator dirItr(DIR_PATH); endItr != dirItr; ++dirItr)
        {
            if (bfs::is_regular_file(dirItr->status()) == false)
            {
                continue;
            }

            const std::vector<std::string> INVALID_TEXT_VEC = { ".txt",
                                                                ".DS_Store",
                                                                "sample.gif" };

            auto const NEXT_PATH_STR{ dirItr->path().string() };

            auto didFindInvalid{ false };
            for (auto const & NEXT_INVALID_STRING : INVALID_TEXT_VEC)
            {
                if (boost::algorithm::icontains(NEXT_PATH_STR, NEXT_INVALID_STRING))
                {
                    didFindInvalid = true;
                    break;
                }
            }

            if (didFindInvalid)
            {
                continue;
            }
            else
            {
                indexVec.push_back( AddByPathInternal(NEXT_PATH_STR, WILL_SMOOTH) );
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS((indexVec.empty() == false), "sfml_util::TextureCache::"
            << "AddAllInDirectory(\"" << DIR_PATH_COMPLETE_STR
            << "\") failed to find any valid images.");

        strToVecMap_.insert(std::make_pair(DIR_PATH_PARAM_STR, indexVec));
        return indexVec;
    }


    void TextureCache::RemoveByKey(const std::string & GAMEDATAFILE_KEY_STR)
    {
        RemoveByPath(game::GameDataFile::Instance()->GetMediaPath(GAMEDATAFILE_KEY_STR));
    }
    
    
    void TextureCache::RemoveByPath(const std::string & PATH_TO_TEXTURE_STR)
    {
        auto const FOUND_ITER{ strToVecMap_.find(PATH_TO_TEXTURE_STR) };

        M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER != strToVecMap_.end()),
            "sfml_util::TextureCache::RemoveByPath(\"" << PATH_TO_TEXTURE_STR
            << ") failed because that path was not found.");

        M_ASSERT_OR_LOGANDTHROW_SS((FOUND_ITER->second.empty() == false),
            "sfml_util::TextureCache::RemoveByPath(\"" << PATH_TO_TEXTURE_STR
            << ") failed because strToVecMap_ entry was an empty vec.");
        
        RemoveByIndexVec(FOUND_ITER->second);
    }


    void TextureCache::RemoveByIndex(const std::size_t INDEX)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((INDEX < cacheUVec_.size()), "sfml_util::TextureCache::"
            << "RemoveByIndex(" << INDEX << "\") failed because that index is out of range."
            << "(size=" << cacheUVec_.size() << ")");

        M_ASSERT_OR_LOGANDTHROW_SS((cacheUVec_[INDEX].get() != nullptr),"sfml_util::TextureCache::"
            << "RemoveByIndex(" << INDEX << "\") failed because that pointer was already null.");

        cacheUVec_[INDEX].reset();

        misc::StrVec_t keysToBeRemovedVec;
        for (auto const & NEXT_PAIR : strToVecMap_)
        {
            if ((NEXT_PAIR.second.empty() == false) && (NEXT_PAIR.second[0] == INDEX))
            {
                keysToBeRemovedVec.push_back(NEXT_PAIR.first);
            }
        }

        for (auto const & NEXT_KEY : keysToBeRemovedVec)
        {
            strToVecMap_.erase(NEXT_KEY);
        }
    }


    void TextureCache::RemoveByIndexVec(const misc::SizetVec_t & INDEX_VEC)
    {
        for (auto const NEXT_INDEX : INDEX_VEC)
        {
            RemoveByIndex(NEXT_INDEX);
        }
    }


    void TextureCache::RemoveAll()
    {
        auto const NUM_TEXTURES{ cacheUVec_.size() };
        for (std::size_t i(0); i < NUM_TEXTURES; ++i)
        {
            cacheUVec_[i].reset();
        }

        strToVecMap_.clear();
    }


    TexturePtr_t TextureCache::GetPtrByIndex(const std::size_t INDEX) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS((INDEX < cacheUVec_.size()), "sfml_util::TextureCache::"
            << "GetPtr(" << INDEX << "\") failed because that index is out of range."
            << "(size=" << cacheUVec_.size() << ")");

        M_ASSERT_OR_LOGANDTHROW_SS((cacheUVec_[INDEX].get() != nullptr),"sfml_util::TextureCache::"
            << "GetPtr(" << INDEX << "\") failed because that pointer was null.");

        return cacheUVec_[INDEX].get();
    }


    const sf::Texture & TextureCache::GetByIndex(const std::size_t INDEX) const
    {
        return * GetPtrByIndex(INDEX);
    }


    std::size_t TextureCache::EstablishNextAvailableIndex()
    {
        auto const NUM_TEXTURES{ cacheUVec_.size() };
        for (std::size_t i(0); i < NUM_TEXTURES; ++i)
        {
            if (cacheUVec_[i].get() == nullptr)
            {
                return i;
            }
        }

        cacheUVec_.push_back( std::make_unique<sf::Texture>() );
        return NUM_TEXTURES;
    }


    std::size_t TextureCache::AddByPathInternal(const std::string & PATH_TO_TEXTURE_STR,
                                                const bool          WILL_SMOOTH)
    {
        auto const INDEX{ EstablishNextAvailableIndex() };

        if (cacheUVec_[INDEX].get() == nullptr)
        {
            cacheUVec_[INDEX] = std::make_unique<sf::Texture>();
        }

        sfml_util::LoadImageOrTexture( * cacheUVec_[INDEX], PATH_TO_TEXTURE_STR);
        cacheUVec_[INDEX]->setSmooth(WILL_SMOOTH);
        return INDEX;
    }

}