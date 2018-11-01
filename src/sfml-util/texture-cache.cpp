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
#include "game/game-controller.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "sfml-util/loaders.hpp"
#include "texture-cache.hpp"

#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>

namespace heroespath
{
namespace gui
{

    std::unique_ptr<TextureCache> TextureCache::instanceUPtr_;

    TextureCache::TextureCache()
        : textureUPtrs_()
        , pathOptToIndexesMap_()
        , stageToCountDataMap_()
        , cachedCount_(0)
        , cachedDataBytes_(0)
        , totalLoadedDataBytes_(0)
        , stageName_("")
    {
        M_HP_LOG_DBG("Subsystem Construction: TextureCache");
        Reset();
    }

    TextureCache::~TextureCache()
    {
        M_HP_LOG_DBG(
            "Subsystem Desstruction: TextureCache (total_loaded_bytes="
            << BytesToString(totalLoadedDataBytes_) << ")");

        DumpCacheToLog(LogDumpContext::Destruct);
    }

    misc::NotNull<TextureCache *> TextureCache::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: TextureCache");
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
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "gui::TextureCache::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    std::size_t TextureCache::AddByKey(
        const std::string & GAMEDATAFILE_KEY_STR, const ImageOptions & OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (GAMEDATAFILE_KEY_STR.empty() == false),
            "gui::TextureCache::AddByKey(key=\"" << GAMEDATAFILE_KEY_STR << "\", " << OPTIONS
                                                 << ") failed because key was empty.");

        return AddByPath(misc::ConfigFile::Instance()->GetMediaPath(GAMEDATAFILE_KEY_STR), OPTIONS);
    }

    std::size_t TextureCache::AddByPath(
        const std::string & PATH_TO_TEXTURE_STR_ORIG, const ImageOptions & OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (PATH_TO_TEXTURE_STR_ORIG.empty() == false),
            "gui::TextureCache::AddByPath(path=\"" << PATH_TO_TEXTURE_STR_ORIG << "\", " << OPTIONS
                                                   << ") failed because path was empty.");

        const auto PATH_TO_TEXTURE_STR_FINAL { misc::filesystem::CleanPath(
            PATH_TO_TEXTURE_STR_ORIG) };

        const auto PATH_OPTIONS_PAIR { std::make_pair(PATH_TO_TEXTURE_STR_FINAL, OPTIONS) };

        auto foundIter { pathOptToIndexesMap_.Find(PATH_OPTIONS_PAIR) };

        if (foundIter != std::end(pathOptToIndexesMap_))
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (foundIter->second.ref_count > 0),
                "gui::TextureCache::AddByPath(orig_path=\""
                    << PATH_TO_TEXTURE_STR_ORIG << "\", final_path=\"" << PATH_TO_TEXTURE_STR_FINAL
                    << "\", " << OPTIONS << ") failed because pathOptToIndexesMap_.ref_count=0.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (foundIter->second.indexes.empty() == false),
                "gui::TextureCache::AddByPath(orig_path=\""
                    << PATH_TO_TEXTURE_STR_ORIG << "\", final_path=\"" << PATH_TO_TEXTURE_STR_FINAL
                    << "\", " << OPTIONS
                    << ") failed because pathOptToIndexesMap_.indexes vec was empty.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (foundIter->second.indexes.size() == 1),
                "gui::TextureCache::AddByPath(orig_path=\""
                    << PATH_TO_TEXTURE_STR_ORIG << "\", final_path=\"" << PATH_TO_TEXTURE_STR_FINAL
                    << "\", " << OPTIONS
                    << ") failed because that path+option was previously added by directory and "
                       "contains "
                    << foundIter->second.indexes.size() << " images.");

            const auto INDEX { foundIter->second.indexes[0] };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (INDEX < textureUPtrs_.size()),
                "gui::TextureCache::AddByPath(orig_path=\""
                    << PATH_TO_TEXTURE_STR_ORIG << "\", final_path=\"" << PATH_TO_TEXTURE_STR_FINAL
                    << "\", " << OPTIONS << ") failed because pathOptToIndexesMap_.indexes[0]="
                    << INDEX << " is not < textureUPtrs_.size()=" << textureUPtrs_.size() << ".");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (textureUPtrs_[INDEX]),
                "gui::TextureCache::AddByPath(path=\""
                    << PATH_TO_TEXTURE_STR_ORIG << "\", final_path=\"" << PATH_TO_TEXTURE_STR_FINAL
                    << "\", " << OPTIONS
                    << ") failed because "
                       "textureUPtrs_[pathOptToIndexesMap_.indexes[0]].get()==null.");

            foundIter->second.ref_count += 1;
            return INDEX;
        }

        const auto INDEX { AddByPathInternal(PATH_TO_TEXTURE_STR_FINAL, OPTIONS) };
        pathOptToIndexesMap_.Append(PATH_OPTIONS_PAIR, TextureIndexes(1, { INDEX }));
        return INDEX;
    }

    std::size_t TextureCache::AddByPathFake(
        const std::string & FAKE_PATH_TO_TEXTURE_STR,
        const sf::Texture & TEXTURE,
        const ImageOptions & OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (FAKE_PATH_TO_TEXTURE_STR.empty() == false),
            "gui::TextureCache::AddByPathFake(fake_path=\"" << FAKE_PATH_TO_TEXTURE_STR << "\", "
                                                            << OPTIONS
                                                            << ") failed because path was empty.");

        const auto PATH_OPTIONS_PAIR { std::make_pair(FAKE_PATH_TO_TEXTURE_STR, OPTIONS) };

        auto foundIter { pathOptToIndexesMap_.Find(PATH_OPTIONS_PAIR) };

        if (foundIter != std::end(pathOptToIndexesMap_))
        {
            // note:  if a fake path matches another non-fake path it is not an error
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (foundIter->second.ref_count > 0),
                "gui::TextureCache::AddByPathFake(fake_path=\""
                    << FAKE_PATH_TO_TEXTURE_STR << "\", " << OPTIONS
                    << ") failed because pathOptToIndexesMap_.ref_count=0.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (foundIter->second.indexes.empty() == false),
                "gui::TextureCache::AddByPathFake(fake_path=\""
                    << FAKE_PATH_TO_TEXTURE_STR << "\", " << OPTIONS
                    << ") failed because pathOptToIndexesMap_.indexes vec was empty.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (foundIter->second.indexes.size() == 1),
                "gui::TextureCache::AddByPathFake(fake_path=\""
                    << FAKE_PATH_TO_TEXTURE_STR << "\", " << OPTIONS
                    << ") failed because that path+option was previously added by directory and "
                       "contains "
                    << foundIter->second.indexes.size() << " images.");

            const auto INDEX { foundIter->second.indexes[0] };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (INDEX < textureUPtrs_.size()),
                "gui::TextureCache::AddByPathFake(fake_path=\""
                    << FAKE_PATH_TO_TEXTURE_STR << "\", " << OPTIONS
                    << ") failed because pathOptToIndexesMap_.indexes[0]=" << INDEX
                    << " is not < textureUPtrs_.size()=" << textureUPtrs_.size() << ".");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (textureUPtrs_[INDEX]),
                "gui::TextureCache::AddByPathFake(fake_path=\""
                    << FAKE_PATH_TO_TEXTURE_STR << "\", " << OPTIONS
                    << ") failed because "
                       "textureUPtrs_[pathOptToIndexesMap_.indexes[0]].get()==null.");

            foundIter->second.ref_count += 1;
            return INDEX;
        }

        const auto INDEX { AddByPathInternalFake(TEXTURE, OPTIONS) };
        pathOptToIndexesMap_.Append(PATH_OPTIONS_PAIR, TextureIndexes(1, { INDEX }));
        return INDEX;
    }

    const std::vector<std::size_t> TextureCache::AddDirectoryByKey(
        const std::string & DIR_PATH_KEY, const ImageOptions & OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (DIR_PATH_KEY.empty() == false),
            "gui::TextureCache::AddDirectoryByKey(key=\"" << DIR_PATH_KEY << "\", " << OPTIONS
                                                          << ") failed because path was empty.");

        return AddDirectoryByPath(
            misc::ConfigFile::Instance()->GetMediaPath(DIR_PATH_KEY), OPTIONS);
    }

    const std::vector<std::size_t> TextureCache::AddDirectoryByPath(
        const std::string & DIR_PATH_PARAM_STR_ORIG, const ImageOptions & OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (DIR_PATH_PARAM_STR_ORIG.empty() == false),
            "gui::TextureCache::AddDirectoryByPath(path=\"" << DIR_PATH_PARAM_STR_ORIG << "\", "
                                                            << OPTIONS
                                                            << ") failed because path was empty.");

        const auto DIR_PATH_PARAM_STR_FINAL { misc::filesystem::CleanPath(
            DIR_PATH_PARAM_STR_ORIG) };

        const auto PATH_OPTIONS_PAIR { std::make_pair(DIR_PATH_PARAM_STR_FINAL, OPTIONS) };

        auto foundIter { pathOptToIndexesMap_.Find(PATH_OPTIONS_PAIR) };

        if (foundIter != std::end(pathOptToIndexesMap_))
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (foundIter->second.ref_count > 0),
                "gui::TextureCache::AddDirectoryByPath(orig_dir_path=\""
                    << DIR_PATH_PARAM_STR_ORIG << "\", final_dir_path=\""
                    << DIR_PATH_PARAM_STR_FINAL << "\", " << OPTIONS
                    << ") failed because pathOptToIndexesMap_.ref_count=0.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (foundIter->second.indexes.empty() == false),
                "gui::TextureCache::AddDirectoryByPath(orig_dir_path=\""
                    << DIR_PATH_PARAM_STR_ORIG << "\", final_dir_path=\""
                    << DIR_PATH_PARAM_STR_FINAL << "\", " << OPTIONS
                    << ") failed because pathOptToIndexesMap_.indexes vec was empty.");

            ++foundIter->second.ref_count;
            return foundIter->second.indexes;
        }
        else
        {
            const auto IMAGE_FILE_PATHS_ORIG { misc::filesystem::FindFiles(
                false,
                DIR_PATH_PARAM_STR_FINAL,
                "",
                "",
                misc::filesystem::COMMON_FILE_NAME_PARTS_TO_EXCLUDE_VEC_) };

            const auto IMAGE_FILE_PATHS_SORTED { misc::filesystem::SortByLastNumberInFilename(
                IMAGE_FILE_PATHS_ORIG) };

            TextureIndexes textureIndexes;

            for (const auto & PATH_STR : IMAGE_FILE_PATHS_SORTED)
            {
                textureIndexes.indexes.emplace_back(AddByPathInternal(PATH_STR, OPTIONS));
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (textureIndexes.indexes.empty() == false),
                "gui::TextureCache::AddDirectoryByPath(orig_path=\""
                    << DIR_PATH_PARAM_STR_ORIG << "\", final_path=\"" << DIR_PATH_PARAM_STR_FINAL
                    << "\", " << OPTIONS << ") failed to find any valid images at that path.");

            ++textureIndexes.ref_count;

            // keep track of which texture indexes were associated with which directory
            pathOptToIndexesMap_.Append(PATH_OPTIONS_PAIR, textureIndexes);

            return textureIndexes.indexes;
        }
    }

    void TextureCache::RemoveByKey(
        const std::string & GAMEDATAFILE_KEY_STR, const ImageOptions & OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (GAMEDATAFILE_KEY_STR.empty() == false),
            "gui::TextureCache::RemoveByKey(key=\"" << GAMEDATAFILE_KEY_STR << "\", " << OPTIONS
                                                    << ") failed because path was empty.");

        RemoveByPath(misc::ConfigFile::Instance()->GetMediaPath(GAMEDATAFILE_KEY_STR), OPTIONS);
    }

    void TextureCache::RemoveByPath(
        const std::string & PATH_TO_TEXTURE_STR_ORIG, const ImageOptions & OPTIONS)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (PATH_TO_TEXTURE_STR_ORIG.empty() == false),
            "gui::TextureCache::RemoveByPath(path=\"" << PATH_TO_TEXTURE_STR_ORIG << "\", "
                                                      << OPTIONS
                                                      << ") failed because path was empty.");

        const auto PATH_TO_TEXTURE_STR_FINAL { misc::filesystem::CleanPath(
            PATH_TO_TEXTURE_STR_ORIG) };

        auto pathOptionsPair { std::make_pair(PATH_TO_TEXTURE_STR_FINAL, OPTIONS) };

        auto foundIter { pathOptToIndexesMap_.Find(pathOptionsPair) };

        if (foundIter == std::end(pathOptToIndexesMap_))
        {
            pathOptionsPair.first = PATH_TO_TEXTURE_STR_ORIG;
            foundIter = pathOptToIndexesMap_.Find(pathOptionsPair);
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (foundIter != std::end(pathOptToIndexesMap_)),
            "gui::TextureCache::RemoveByPath(orig_path=\""
                << PATH_TO_TEXTURE_STR_ORIG << "\", final_path=" << PATH_TO_TEXTURE_STR_FINAL
                << "\", " << OPTIONS << ") failed because that path/option was not found.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (foundIter->second.ref_count > 0),
            "gui::TextureCache::RemoveByPath(orig_path=\""
                << PATH_TO_TEXTURE_STR_ORIG << "\", final_path=" << PATH_TO_TEXTURE_STR_FINAL
                << "\", " << OPTIONS << ") failed because pathOptToIndexesMap_.ref_count=0.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (foundIter->second.indexes.empty() == false),
            "gui::TextureCache::RemoveByPath(orig_path=\""
                << PATH_TO_TEXTURE_STR_ORIG << "\", final_path=" << PATH_TO_TEXTURE_STR_FINAL
                << "\", " << OPTIONS
                << ") failed because pathOptToIndexesMap_.indexes vec was empty.");

        if (--foundIter->second.ref_count == 0)
        {
            for (const auto INDEX : foundIter->second.indexes)
            {
                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (INDEX < textureUPtrs_.size()),
                    "gui::TextureCache::RemoveByPath(orig_path=\""
                        << PATH_TO_TEXTURE_STR_ORIG
                        << "\", final_path=" << PATH_TO_TEXTURE_STR_FINAL << "\", " << OPTIONS
                        << ") found a TextureIndexes object with that path but one of the "
                           "indexes in the vector ("
                        << INDEX << ") was out of bounds with textureUPtrs_.size()="
                        << textureUPtrs_.size() << ".");

                auto & textureUPtr { textureUPtrs_[INDEX] };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (textureUPtr),
                    "gui::TextureCache::RemoveByPath(orig_path=\""
                        << PATH_TO_TEXTURE_STR_ORIG
                        << "\", final_path=" << PATH_TO_TEXTURE_STR_FINAL << "\", " << OPTIONS
                        << ") found a TextureIndexes object with that path but one of the "
                           "indexes in the vector ("
                        << INDEX << ") referenced a TextureUPtr that was null.");

                RemoveInternal(INDEX);
            }

            pathOptToIndexesMap_.Erase(pathOptionsPair);
        }
    }

    void TextureCache::RemoveAll() { Reset(); }

    const sf::Texture & TextureCache::GetByIndex(const std::size_t INDEX) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INDEX < textureUPtrs_.size()),
            "gui::TextureCache::GetByIndex(" << INDEX
                                             << ") failed because that index is out of range."
                                             << "(size=" << textureUPtrs_.size() << ")");

        auto & textureUPtr { textureUPtrs_[INDEX] };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (textureUPtr),
            "gui::TextureCache::GetByIndex("
                << INDEX << ") failed because the pointer at that index is null.");

        return *textureUPtr;
    }

    const ImageOptions TextureCache::GetOptionsByIndex(const std::size_t INDEX) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INDEX < textureUPtrs_.size()),
            "gui::TextureCache::GetOptionsByIndex(index="
                << INDEX << ") but that index is out of bounds with textureUPtrs_.size()="
                << textureUPtrs_.size() << ".");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (textureUPtrs_[INDEX]),
            "gui::TextureCache::GetOptionsByIndex(index=" << INDEX << ") but textureUPtrs_["
                                                          << INDEX << "] is null.");

        const auto INFO_PAIR { FindInfoByIndex(INDEX) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INFO_PAIR.first.first.empty() == false),
            "gui::TextureCache::GetOptionsByIndex(index="
                << INDEX << ") but that index could not be found.");

        return INFO_PAIR.first.second;
    }

    const std::string TextureCache::GetPathByIndex(const std::size_t INDEX) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INDEX < textureUPtrs_.size()),
            "gui::TextureCache::GetPathByIndex(index="
                << INDEX << ") but that index is out of bounds with textureUPtrs_.size()="
                << textureUPtrs_.size() << ".");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (textureUPtrs_[INDEX]),
            "gui::TextureCache::GetPathByIndex(index=" << INDEX << ") but textureUPtrs_[" << INDEX
                                                       << "] is null.");

        const auto INFO_PAIR { FindInfoByIndex(INDEX) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INFO_PAIR.first.first.empty() == false),
            "gui::TextureCache::GetPathByIndex(index=" << INDEX
                                                       << ") but that index could not be found.");

        return INFO_PAIR.first.first;
    }

    std::size_t TextureCache::GetRefCountByIndex(const std::size_t INDEX) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INDEX < textureUPtrs_.size()),
            "gui::TextureCache::GetRefCountByIndex(index="
                << INDEX << ") but that index is out of bounds with textureUPtrs_.size()="
                << textureUPtrs_.size() << ".");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (textureUPtrs_[INDEX]),
            "gui::TextureCache::GetRefCountByIndex(index=" << INDEX << ") but textureUPtrs_["
                                                           << INDEX << "] is null.");

        const auto INFO_PAIR { FindInfoByIndex(INDEX) };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INFO_PAIR.first.first.empty() == false),
            "gui::TextureCache::GetRefCountByIndex(index="
                << INDEX << ") but that index could not be found.");

        return INFO_PAIR.second.ref_count;
    }

    const PathOptToIndexesPair_t TextureCache::FindInfoByIndex(const std::size_t INDEX) const
    {
        for (const auto & PATHOPT_TO_TEXTINDEXES_PAIR : pathOptToIndexesMap_)
        {
            const std::vector<std::size_t> & INDEXES { PATHOPT_TO_TEXTINDEXES_PAIR.second.indexes };
            if (std::find(std::begin(INDEXES), std::end(INDEXES), INDEX) != std::end(INDEXES))
            {
                return PATHOPT_TO_TEXTINDEXES_PAIR;
            }
        }

        return { { "", ImageOptions::NoOptions() }, TextureIndexes() };
    }

    void TextureCache::Reset()
    {
        DumpCacheToLog(LogDumpContext::Reset);

        textureUPtrs_.clear();
        pathOptToIndexesMap_.Clear();

        // Any value greater than (about) a thousand will work here.
        // Even an aggressize stage (like Credits) should not exceed 500.
        textureUPtrs_.reserve(1024);
        pathOptToIndexesMap_.Reserve(1024);

        cachedCount_ = 0;
        cachedDataBytes_ = 0;
        stageName_.clear();
    }

    std::size_t TextureCache::FindNextAvailableIndex()
    {
        const auto NUM_TEXTURES { textureUPtrs_.size() };

        for (std::size_t i(0); i < NUM_TEXTURES; ++i)
        {
            if (!textureUPtrs_[i])
            {
                return i;
            }
        }

        textureUPtrs_.emplace_back(std::make_unique<sf::Texture>());
        return NUM_TEXTURES;
    }

    std::size_t TextureCache::AddByPathInternal(
        const std::string & PATH_TO_TEXTURE_STR, const ImageOptions & OPTIONS)
    {
        const auto INDEX { FindNextAvailableIndex() };

        auto & textureUPtr { textureUPtrs_[INDEX] };

        if (!textureUPtr)
        {
            textureUPtr = std::make_unique<sf::Texture>();
        }

        try
        {
            gui::Loaders::Texture(*textureUPtr, PATH_TO_TEXTURE_STR, false);
        }
        catch (...)
        {
            textureUPtr.reset();
            throw;
        }

        OPTIONS.Apply(*textureUPtr);
        UpdateCountAndSizeTracker(*textureUPtr);
        return INDEX;
    }

    std::size_t TextureCache::AddByPathInternalFake(
        const sf::Texture & TEXTURE, const ImageOptions & OPTIONS)
    {
        const auto INDEX { FindNextAvailableIndex() };
        auto & textureUPtr { textureUPtrs_[INDEX] };

        if (!textureUPtr)
        {
            textureUPtr = std::make_unique<sf::Texture>(TEXTURE);
        }
        else
        {
            *textureUPtr = TEXTURE;
        }

        OPTIONS.Apply(*textureUPtr);
        UpdateCountAndSizeTracker(TEXTURE);
        return INDEX;
    }

    void TextureCache::RemoveInternal(const std::size_t INDEX)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INDEX < textureUPtrs_.size()),
            "gui::TextureCache::"
                << "RemoveInternal(" << INDEX
                << ") but that index is out of range.  (textureUPtrs_.size()="
                << textureUPtrs_.size() << ")");

        auto & textureUPtr { textureUPtrs_[INDEX] };

        if (textureUPtr)
        {
            cachedDataBytes_ -= TextureSizeInBytes(*textureUPtr);
            --cachedCount_;
            textureUPtr.reset();
        }
    }

    long long TextureCache::TextureSizeInBytes(const sf::Texture & TEXTURE) const
    {
        return static_cast<long long>(TEXTURE.getSize().x * TEXTURE.getSize().y * 4);
    }

    void TextureCache::UpdateCountAndSizeTracker(const sf::Texture & TEXTURE)
    {
        const auto TEXTURE_SIZE_BYTES { TextureSizeInBytes(TEXTURE) };
        cachedDataBytes_ += TEXTURE_SIZE_BYTES;
        totalLoadedDataBytes_ += TEXTURE_SIZE_BYTES;

        ++cachedCount_;

        if (stageName_.empty())
        {
            stageName_ = game::GameController::Instance()->GetStageName();
        }

        auto & countSizePair { stageToCountDataMap_[stageName_] };

        if (countSizePair.first < cachedCount_)
        {
            countSizePair.first = cachedCount_;
        }

        if (countSizePair.second < cachedDataBytes_)
        {
            countSizePair.second = cachedDataBytes_;
        }
    }

    void TextureCache::DumpCacheToLog(const LogDumpContext CONTEXT) const
    {
        if ((cachedCount_ > 0) || (cachedDataBytes_ > 0) || (pathOptToIndexesMap_.Empty() == false))
        {
            std::ostringstream ss;

            ss << "TextureCache, just before "
               << ((CONTEXT == LogDumpContext::Reset) ? "Resetting" : "Destructing") << " in the \""
               << stageName_ << "\" stage, found:  count=" << cachedCount_
               << ", data=" << BytesToString(cachedDataBytes_) << ", and the following paths:";

            for (const auto & PATHOPTION_TO_INDEXES_PAIR : pathOptToIndexesMap_)
            {
                std::ostringstream lineSS;
                lineSS << "\n\t..."
                       << misc::filesystem::LimitPathDept(PATHOPTION_TO_INDEXES_PAIR.first.first, 4)
                       << "\t" << PATHOPTION_TO_INDEXES_PAIR.first.second << "\t"
                       << PATHOPTION_TO_INDEXES_PAIR.second.ref_count << "\t[";

                for (const auto INDEX : PATHOPTION_TO_INDEXES_PAIR.second.indexes)
                {
                    lineSS << INDEX << ",";
                }

                lineSS << "]";

                ss << lineSS.str();
            }

            M_HP_LOG_WRN(ss.str());
        }

        if ((CONTEXT == LogDumpContext::Destruct) && (stageToCountDataMap_.Empty() == false))
        {
            std::ostringstream ss;
            ss << "TextureCache collected the following info about stage texture usage:";

            for (const auto & STAGENAME_MAX_PAIR : stageToCountDataMap_)
            {
                ss << "\n\tThe " << STAGENAME_MAX_PAIR.first << " stage had at most "
                   << STAGENAME_MAX_PAIR.second.first
                   << " textures loaded at once, and a max memory usage of "
                   << BytesToString(STAGENAME_MAX_PAIR.second.second);
            }

            M_HP_LOG_DBG(ss.str());
        }
    }

    const std::string TextureCache::BytesToString(const long long BYTES) const
    {
        std::ostringstream ss;
        const long long MB { 1 << 20 };
        const long long KB { 1 << 10 };

        if (BYTES >= MB)
        {
            ss << static_cast<long double>(BYTES) / static_cast<long double>(MB) << "MB";
        }
        else if (BYTES >= KB)
        {
            ss << static_cast<long double>(BYTES) / static_cast<long double>(KB) << "KB";
        }
        else
        {
            ss << BYTES << "B";
        }

        return ss.str();
    }

} // namespace gui
} // namespace heroespath
