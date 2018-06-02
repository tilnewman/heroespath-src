// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// texture-cache.cpp
//
#include "texture-cache.hpp"
#include "game/game-data-file.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/filesystem-helpers.hpp"
#include "sfml-util/loaders.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <cctype>
#include <utility>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    std::unique_ptr<TextureCache> TextureCache::instanceUPtr_;

    TextureCache::TextureCache()
        : cacheUVec_()
        , strToVecMap_()
    {
        M_HP_LOG_DBG("Subsystem Construction: TextureCache");

        // Any value greater than (about) a thousand will work here.
        // Even an aggressize stage should not exceed 500.
        cacheUVec_.reserve(1024);
        strToVecMap_.Reserve(1024);

        // put an empty image at index zero so there is something to return by default
        cacheUVec_.emplace_back(std::make_unique<sf::Texture>());
    }

    TextureCache::~TextureCache() { M_HP_LOG_DBG("Subsystem Construction: TextureCache"); }

    misc::NotNull<TextureCache *> TextureCache::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() before Acquire(): TextureCache");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void TextureCache::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<TextureCache>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: TextureCache");
        }
    }

    void TextureCache::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_),
            "sfml_util::TextureCache::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    std::size_t
        TextureCache::AddByKey(const std::string & GAMEDATAFILE_KEY_STR, const bool WILL_SMOOTH)
    {
        return AddByPath(
            game::GameDataFile::Instance()->GetMediaPath(GAMEDATAFILE_KEY_STR), WILL_SMOOTH);
    }

    std::size_t
        TextureCache::AddByPath(const std::string & PATH_TO_TEXTURE_STR, const bool WILL_SMOOTH)
    {
        misc::SizetVec_t indexVec;
        if (strToVecMap_.Find(PATH_TO_TEXTURE_STR, indexVec))
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (indexVec.empty() == false),
                "sfml_util::TextureCache::AddByPath(path=\""
                    << PATH_TO_TEXTURE_STR << "\", will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because strToVecMap_ entry was an empty vec.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (indexVec[0] < cacheUVec_.size()),
                "sfml_util::TextureCache::AddByPath(path=\""
                    << PATH_TO_TEXTURE_STR << "\", will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because strToVecMap_ entry=" << indexVec[0]
                    << " was out of bounds with cacheUVec_.size()=" << cacheUVec_.size() << ".");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (cacheUVec_[indexVec[0]].get() != nullptr),
                "sfml_util::TextureCache::AddByPath(path=\""
                    << PATH_TO_TEXTURE_STR << "\", will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because strToVecMap_ entry pointed to a null pointer.");

            return indexVec[0];
        }

        auto const INDEX{ AddByPathInternal(PATH_TO_TEXTURE_STR, WILL_SMOOTH) };
        strToVecMap_[PATH_TO_TEXTURE_STR] = misc::SizetVec_t(1, INDEX);
        return INDEX;
    }

    const misc::SizetVec_t TextureCache::AddAllInDirectoryByKey(
        const std::string & DIR_PATH_KEY, const bool WILL_SMOOTH)
    {
        return AddAllInDirectoryByPath(
            game::GameDataFile::Instance()->GetMediaPath(DIR_PATH_KEY), WILL_SMOOTH);
    }

    const misc::SizetVec_t TextureCache::AddAllInDirectoryByPath(
        const std::string & DIR_PATH_PARAM_STR, const bool WILL_SMOOTH)
    {
        misc::SizetVec_t indexVec;

        if (strToVecMap_.Find(DIR_PATH_PARAM_STR, indexVec))
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (indexVec.empty() == false),
                "sfml_util::TextureCache::AddAllInDirectoryByPath(path=\""
                    << DIR_PATH_PARAM_STR << "\", will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because strToVecMap_ entry was an empty vec.");
        }
        else
        {
            namespace fs = misc::filesystem;

            auto filePaths{ fs::FindFilesInDirectory(
                fs::MakePathPretty(boost::filesystem::path(DIR_PATH_PARAM_STR)),
                "",
                "",
                fs::FilenameText::TO_EXCLUDE_VEC_) };

            fs::SortByNumberInFilename(filePaths);

            for (auto const & PATH : filePaths)
            {
                indexVec.emplace_back(AddByPathInternal(PATH.string(), WILL_SMOOTH));
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                (indexVec.empty() == false),
                "sfml_util::TextureCache::AddAllInDirectoryByPath(path=\""
                    << DIR_PATH_PARAM_STR << "\", will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed to find any valid images.");

            // keep track of which texture indexes were associated with which directory
            strToVecMap_[DIR_PATH_PARAM_STR] = indexVec;
        }

        return indexVec;
    }

    void TextureCache::RemoveByKey(const std::string & GAMEDATAFILE_KEY_STR)
    {
        RemoveByPath(game::GameDataFile::Instance()->GetMediaPath(GAMEDATAFILE_KEY_STR));
    }

    void TextureCache::RemoveByPath(const std::string & PATH_TO_TEXTURE_STR)
    {
        misc::SizetVec_t indexVec;
        auto const WAS_FOUND{ strToVecMap_.Find(PATH_TO_TEXTURE_STR, indexVec) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (WAS_FOUND),
            "sfml_util::TextureCache::RemoveByPath(\""
                << PATH_TO_TEXTURE_STR << ") failed because that path was not found.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (indexVec.empty() == false),
            "sfml_util::TextureCache::RemoveByPath(\""
                << PATH_TO_TEXTURE_STR << ") failed because strToVecMap_ entry was an empty vec.");

        // RemoveByIndexVec() may delete the found entry in strToVecMap_
        RemoveByIndexVec(indexVec);
    }

    void TextureCache::RemoveByIndex(const std::size_t INDEX)
    {
        if (INDEX == 0)
        {
            M_HP_LOG_ERR(
                "sfml_util::TextureCache::RemoveByIndex(0) "
                << "failed because zero is invalid.");

            return;
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (INDEX < cacheUVec_.size()),
            "sfml_util::TextureCache::"
                << "RemoveByIndex(" << INDEX << "\") failed because that index is out of range."
                << "(size=" << cacheUVec_.size() << ")");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (cacheUVec_[INDEX].get() != nullptr),
            "sfml_util::TextureCache::"
                << "RemoveByIndex(" << INDEX
                << "\") failed because that pointer was already null.");

        cacheUVec_[INDEX].reset();

        misc::StrVec_t keysToBeRemovedVec;
        for (auto const & NEXT_PAIR : strToVecMap_)
        {
            if ((NEXT_PAIR.second.empty() == false) && (NEXT_PAIR.second[0] == INDEX))
            {
                keysToBeRemovedVec.emplace_back(NEXT_PAIR.first);
            }
        }

        for (auto const & KEY : keysToBeRemovedVec)
        {
            strToVecMap_.Erase(KEY);
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
                M_HP_LOG_ERR(
                    "sfml_util::TextureCache::RemoveByIndexVec(INDEX_VEC.size()="
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
        for (auto & u_ptr : cacheUVec_)
        {
            u_ptr.reset();
        }

        strToVecMap_.Clear();

        cacheUVec_[0] = std::make_unique<sf::Texture>();
    }

    const sf::Texture & TextureCache::GetByIndex(const std::size_t INDEX) const
    {
        if (INDEX == 0)
        {
            M_HP_LOG_ERR(
                "sfml_util::TextureCache::GetByIndex(0"
                << "\") failed because zero is always an invalid index.");

            return *cacheUVec_[0];
        }

        if (INDEX >= cacheUVec_.size())
        {
            M_HP_LOG_ERR(
                "sfml_util::TextureCache::GetByIndex("
                << INDEX << "\") failed because that index is out of range."
                << "(size=" << cacheUVec_.size() << ")");

            return *cacheUVec_[0];
        }

        if (cacheUVec_[INDEX].get() == nullptr)
        {
            M_HP_LOG_ERR(
                "sfml_util::TextureCache::GetByIndex("
                << INDEX << "\") failed because the pointer at that index is null.");

            return *cacheUVec_[0];
        }

        return *cacheUVec_[INDEX];
    }

    std::size_t TextureCache::EstablishNextAvailableIndex()
    {
        auto const NUM_TEXTURES{ cacheUVec_.size() };

        // start at one because zero is always an invalid index
        for (std::size_t i(1); i < NUM_TEXTURES; ++i)
        {
            if (cacheUVec_[i].get() == nullptr)
            {
                return i;
            }
        }

        cacheUVec_.emplace_back(std::make_unique<sf::Texture>());
        return NUM_TEXTURES;
    }

    std::size_t TextureCache::AddByPathInternal(
        const std::string & PATH_TO_TEXTURE_STR, const bool WILL_SMOOTH)
    {
        auto const INDEX{ EstablishNextAvailableIndex() };

        if (cacheUVec_[INDEX].get() == nullptr)
        {
            cacheUVec_[INDEX] = std::make_unique<sf::Texture>();
        }

        try
        {
            sfml_util::Loaders::Texture(*cacheUVec_[INDEX], PATH_TO_TEXTURE_STR, WILL_SMOOTH);
        }
        catch (...)
        {
            cacheUVec_[INDEX].reset();
            throw;
        }

        return INDEX;
    }

} // namespace sfml_util
} // namespace heroespath
