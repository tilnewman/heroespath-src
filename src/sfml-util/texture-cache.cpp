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

#include "log/log-macros.hpp"
#include "game/game-data-file.hpp"

#include "misc/assertlogandthrow.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <map>
#include <vector>
#include <cctype>
#include <utility>
#include <algorithm>


namespace heroespath
{
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

        //put an empty image at index zero so there is something to return by default
        cacheUVec_.push_back(std::make_unique<sf::Texture>());
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
            instanceUPtr_ = std::make_unique<TextureCache>();
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


    std::size_t TextureCache::AddByKey(
        const std::string & GAMEDATAFILE_KEY_STR,
        const bool          WILL_SMOOTH)
    {
        return AddByPath(
            game::GameDataFile::Instance()->GetMediaPath(GAMEDATAFILE_KEY_STR), WILL_SMOOTH);
    }


    std::size_t TextureCache::AddByPath(
        const std::string & PATH_TO_TEXTURE_STR,
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


    const misc::SizetVec_t TextureCache::AddAllInDirectoryByKey(
        const std::string & DIR_PATH_KEY,
        const bool          WILL_SMOOTH)
    {
        return AddAllInDirectoryByPath(game::GameDataFile::Instance()->GetMediaPath(
            DIR_PATH_KEY), WILL_SMOOTH);
    }


    const misc::SizetVec_t TextureCache::AddAllInDirectoryByPath(
        const std::string & DIR_PATH_PARAM_STR,
        const bool WILL_SMOOTH)
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

        //verify directory exists
        namespace bfs = boost::filesystem;

        auto const DIR_PATH{ bfs::system_complete(bfs::path(DIR_PATH_PARAM_STR)) };
        auto const DIR_PATH_COMPLETE_STR{ DIR_PATH.string() };

        M_ASSERT_OR_LOGANDTHROW_SS((bfs::exists(DIR_PATH)), "sfml_util::TextureCache::"
            << "AddAllInDirectory(\"" << DIR_PATH_COMPLETE_STR
            << "\") failed because that path does not exist!");

        M_ASSERT_OR_LOGANDTHROW_SS((bfs::is_directory(DIR_PATH)), "sfml_util::TextureCache::"
            << "AddAllInDirectory(\"" << DIR_PATH_COMPLETE_STR
            << "\") failed because that is not a directory.");

        //collect all valid file paths in pathVec (not in any order)
        std::vector<std::string> pathVec;

        bfs::directory_iterator endItr;
        for (bfs::directory_iterator dirItr(DIR_PATH); endItr != dirItr; ++dirItr)
        {
            if (bfs::is_regular_file(dirItr->status()) == false)
            {
                continue;
            }

            const std::vector<std::string> INVALID_TEXT_VEC = { ".txt",
                                                                ".DS_Store",
                                                                "sample.gif",
                                                                ".html",
                                                                ".htm"};

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
                pathVec.push_back(NEXT_PATH_STR);
                M_HP_LOG_DBG("AddAllInDirectoryByPath() - " << NEXT_PATH_STR);
            }
        }

        //order the path strings by the last occurring numbers in the filenames
        ReorderByLastNumber(pathVec);

        //add each file to indexVec (now in order)
        misc::SizetVec_t indexVec;
        for (auto const & PATH_STR : pathVec)
        {
            indexVec.push_back(AddByPathInternal(PATH_STR, WILL_SMOOTH));
        }

        //verify that at least one valid file was found
        M_ASSERT_OR_LOGANDTHROW_SS((indexVec.empty() == false), "sfml_util::TextureCache::"
            << "AddAllInDirectory(\"" << DIR_PATH_COMPLETE_STR
            << "\") failed to find any valid images.");

        //keep track of which texture indexes were associated with which directory
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

        //make a copy of the vector because RemoveByIndexVec() may delete FOUND_ITER
        auto const VEC_COPY{ FOUND_ITER->second };

        RemoveByIndexVec(VEC_COPY);
    }


    void TextureCache::RemoveByIndex(const std::size_t INDEX)
    {
        if (INDEX == 0)
        {
            M_HP_LOG_ERR("sfml_util::TextureCache::RemoveByIndex(0) "
                << "failed because zero is invalid.");

            return;
        }

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
        auto const NUM_INDEXES{ INDEX_VEC.size() };
        for (std::size_t i(0); i < NUM_INDEXES; ++i)
        {
            auto const NEXT_INDEX{ INDEX_VEC[i] };
            if (NEXT_INDEX == 0)
            {
                M_HP_LOG_ERR("sfml_util::TextureCache::RemoveByIndexVec(INDEX_VEC.size()="
                    << NUM_INDEXES << ") failed at INDEX_VEC[" << i
                    << "] because it contained zero.");
            }
            else
            {
                RemoveByIndex(NEXT_INDEX);
            }
        }
    }


    void TextureCache::RemoveAll()
    {
        for(auto & u_ptr : cacheUVec_)
        {
            u_ptr.reset();
        }

        strToVecMap_.clear();

        cacheUVec_[0] = std::make_unique<sf::Texture>();
    }


    const sf::Texture & TextureCache::GetByIndex(const std::size_t INDEX) const
    {
        if (INDEX == 0)
        {
            M_HP_LOG_ERR("sfml_util::TextureCache::GetByIndex(0"
                << "\") failed because zero is always an invalid index.");

            return * cacheUVec_[0];
        }

        if (INDEX >= cacheUVec_.size())
        {
            M_HP_LOG_ERR("sfml_util::TextureCache::GetByIndex(" << INDEX
                << "\") failed because that index is out of range."
                << "(size=" << cacheUVec_.size() << ")");

            return * cacheUVec_[0];
        }

        if (cacheUVec_[INDEX].get() == nullptr)
        {
            M_HP_LOG_ERR("sfml_util::TextureCache::GetByIndex(" << INDEX
                << "\") failed because the pointer at that index is null.");

            return * cacheUVec_[0];
        }

        return * cacheUVec_[INDEX];
    }


    std::size_t TextureCache::EstablishNextAvailableIndex()
    {
        auto const NUM_TEXTURES{ cacheUVec_.size() };

        //start at one because zero is always an invalid index
        for (std::size_t i(1); i < NUM_TEXTURES; ++i)
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

        try
        {
            sfml_util::LoadTexture( * cacheUVec_[INDEX], PATH_TO_TEXTURE_STR, WILL_SMOOTH);
        }
        catch (...)
        {
            cacheUVec_[INDEX].reset();
            throw;
        }

        return INDEX;
    }


    void TextureCache::ReorderByLastNumber(std::vector<std::string> & strVec)
    {
        std::multimap<std::size_t, std::string> numberToStrMMap;

        for (auto const & STR : strVec)
        {
            numberToStrMMap.insert(
                std::pair<std::size_t, std::string>(FindLastNumber(STR), STR) );
        }

        strVec.clear();

        for (auto const & NUM_STR_PAIR : numberToStrMMap)
        {
            strVec.push_back(NUM_STR_PAIR.second);
        }
    }


    std::size_t TextureCache::FindLastNumber(const std::string & FILE_NAME)
    {
        if (FILE_NAME.empty())
        {
            return 0;
        }

        std::string numberStr{ "" };
        std::size_t i{ FILE_NAME.size() - 1 };
        do
        {
            const char LETTER{ FILE_NAME[i] };
            const bool IS_DIGIT{ std::isdigit(LETTER) != 0 };
            
            if ((numberStr.empty() == false) && (IS_DIGIT == false))
            {
                break;
            }
            else if (IS_DIGIT)
            {
                numberStr.push_back(LETTER);
            }

            --i;
        } while (i != 0);

        
        std::reverse(numberStr.begin(), numberStr.end());

        try
        {
            return boost::lexical_cast<std::size_t>(numberStr);
        }
        catch (...)
        {
            return 0;
        }
    }

}
}
