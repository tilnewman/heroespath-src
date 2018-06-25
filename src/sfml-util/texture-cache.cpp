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
#include "game/loop-manager.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/filesystem-helpers.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <utility>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    std::unique_ptr<TextureCache> TextureCache::instanceUPtr_;

    TextureCache::TextureCache()
        : textureUPtrs_()
        , strToIndexesMap_()
        , stageNameToTextureMaxMap_()
        , textureCount_(0)
        , dataSizeBytes_(0)
    {
        M_HP_LOG_DBG("Subsystem Construction: TextureCache");
        Reset();
    }

    TextureCache::~TextureCache() { M_HP_LOG_DBG("Subsystem Desstruction: TextureCache"); }

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
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_),
            "sfml_util::TextureCache::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    std::size_t TextureCache::AddByKey(
        const std::string & GAMEDATAFILE_KEY_STR,
        const bool WILL_SMOOTH,
        const bool WILL_FLIP_HORIZ)
    {
        return AddByPath(
            game::GameDataFile::Instance()->GetMediaPath(GAMEDATAFILE_KEY_STR),
            WILL_SMOOTH,
            WILL_FLIP_HORIZ);
    }

    std::size_t TextureCache::AddByPath(
        const std::string & PATH_TO_TEXTURE_STR_ORIG,
        const bool WILL_SMOOTH,
        const bool WILL_FLIP_HORIZ)
    {
        auto const PATH_TO_TEXTURE_STR_FINAL{ misc::filesystem::MakePathPretty(
            PATH_TO_TEXTURE_STR_ORIG) };

        auto foundIter{ strToIndexesMap_.Find(PATH_TO_TEXTURE_STR_FINAL) };
        if (foundIter != std::end(strToIndexesMap_))
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (foundIter->second.ref_count != 0),
                "sfml_util::TextureCache::AddByPath(path=\""
                    << PATH_TO_TEXTURE_STR_ORIG << "\" (\"" << PATH_TO_TEXTURE_STR_FINAL
                    << "\"), will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because textureIndexes.ref_count==0.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (foundIter->second.indexes.empty() == false),
                "sfml_util::TextureCache::AddByPath(path=\""
                    << PATH_TO_TEXTURE_STR_ORIG << "\" (\"" << PATH_TO_TEXTURE_STR_FINAL
                    << "\"), will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because textureIndexes.indexes vec was empty.");

            auto const INDEX{ foundIter->second.indexes[0] };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (INDEX < textureUPtrs_.size()),
                "sfml_util::TextureCache::AddByPath(path=\""
                    << PATH_TO_TEXTURE_STR_ORIG << "\" (\"" << PATH_TO_TEXTURE_STR_FINAL
                    << "\"), will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because textureIndexes.indexes[0]=" << INDEX
                    << " is not < textureUPtrs_.size()=" << textureUPtrs_.size() << ".");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (textureUPtrs_[INDEX]),
                "sfml_util::TextureCache::AddByPath(path=\""
                    << PATH_TO_TEXTURE_STR_ORIG << "\" (\"" << PATH_TO_TEXTURE_STR_FINAL
                    << "\"), will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because textureUPtrs_[textureIndexes.indexes[0]].get()==null.");

            foundIter->second.ref_count += 1;
            return INDEX;
        }

        auto const INDEX{ AddByPathInternal(
            PATH_TO_TEXTURE_STR_FINAL, WILL_SMOOTH, WILL_FLIP_HORIZ) };

        strToIndexesMap_[PATH_TO_TEXTURE_STR_FINAL] = TextureIndexes(1, { INDEX });
        return INDEX;
    }

    std::size_t TextureCache::AddByPathFake(
        const std::string & FAKE_PATH_TO_TEXTURE_STR, const sf::Texture & TEXTURE)
    {
        auto foundIter{ strToIndexesMap_.Find(FAKE_PATH_TO_TEXTURE_STR) };

        if (foundIter != std::end(strToIndexesMap_))
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (foundIter->second.ref_count != 0),
                "sfml_util::TextureCache::AddByPathFake(fake_path=\""
                    << FAKE_PATH_TO_TEXTURE_STR
                    << "\") failed because textureIndexes.ref_count==0.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (foundIter->second.indexes.empty() == false),
                "sfml_util::TextureCache::AddByPathake(fake_path=\""
                    << FAKE_PATH_TO_TEXTURE_STR
                    << "\") failed because textureIndexes.indexes vec was empty.");

            auto const INDEX{ foundIter->second.indexes[0] };

            M_ASSERT_OR_LOGANDTHROW_SS(
                (INDEX < textureUPtrs_.size()),
                "sfml_util::TextureCache::AddByPathake(fake_path=\""
                    << FAKE_PATH_TO_TEXTURE_STR << "\") failed because textureIndexes.indexes[0]="
                    << INDEX << " is not < textureUPtrs_.size()=" << textureUPtrs_.size() << ".");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (textureUPtrs_[INDEX]),
                "sfml_util::TextureCache::AddByPathake(fake_path=\""
                    << FAKE_PATH_TO_TEXTURE_STR
                    << "\" failed because "
                       "textureUPtrs_[textureIndexes.indexes[0]].get()==null.");

            foundIter->second.ref_count += 1;
            return INDEX;
        }

        auto const INDEX{ AddByPathInternalFake(TEXTURE) };
        strToIndexesMap_[FAKE_PATH_TO_TEXTURE_STR] = TextureIndexes(1, { INDEX });
        return INDEX;
    }

    const misc::SizetVec_t TextureCache::AddDirectoryByKey(
        const std::string & DIR_PATH_KEY, const bool WILL_SMOOTH, const bool WILL_FLIP_HORIZ)
    {
        return AddDirectoryByPath(
            game::GameDataFile::Instance()->GetMediaPath(DIR_PATH_KEY),
            WILL_SMOOTH,
            WILL_FLIP_HORIZ);
    }

    const misc::SizetVec_t TextureCache::AddDirectoryByPath(
        const std::string & DIR_PATH_PARAM_STR_ORIG,
        const bool WILL_SMOOTH,
        const bool WILL_FLIP_HORIZ)
    {
        auto const DIR_PATH_PARAM_STR_FINAL{ misc::filesystem::MakePathPretty(
            DIR_PATH_PARAM_STR_ORIG) };

        auto foundIter{ strToIndexesMap_.Find(DIR_PATH_PARAM_STR_FINAL) };

        if (foundIter != std::end(strToIndexesMap_))
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (foundIter->second.ref_count != 0),
                "sfml_util::TextureCache::AddDirectoryByPath(path=\""
                    << DIR_PATH_PARAM_STR_ORIG << "\" (\"" << DIR_PATH_PARAM_STR_FINAL
                    << "\"), will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because textureIndexes.ref_count=0.");

            M_ASSERT_OR_LOGANDTHROW_SS(
                (foundIter->second.indexes.empty() == false),
                "sfml_util::TextureCache::AddDirectoryByPath(path=\""
                    << DIR_PATH_PARAM_STR_ORIG << "\" (\"" << DIR_PATH_PARAM_STR_FINAL
                    << "\"), will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed because textureIndexes.indexes vec was empty.");

            ++foundIter->second.ref_count;
            return foundIter->second.indexes;
        }
        else
        {
            namespace fs = misc::filesystem;

            auto filePaths{ fs::FindFilesInDirectory(
                boost::filesystem::path(DIR_PATH_PARAM_STR_FINAL),
                "",
                "",
                fs::FilenameText::TO_EXCLUDE_VEC_) };

            fs::SortByNumberInFilename(filePaths);

            TextureIndexes textureIndexes;

            for (auto const & PATH : filePaths)
            {
                textureIndexes.indexes.emplace_back(
                    AddByPathInternal(PATH.string(), WILL_SMOOTH, WILL_FLIP_HORIZ));
            }

            M_ASSERT_OR_LOGANDTHROW_SS(
                (textureIndexes.indexes.empty() == false),
                "sfml_util::TextureCache::AddDirectoryByPath(path=\""
                    << DIR_PATH_PARAM_STR_ORIG << "\" (\"" << DIR_PATH_PARAM_STR_FINAL
                    << "\"), will_smooth=" << std::boolalpha << WILL_SMOOTH
                    << ") failed to find any valid images.");

            ++textureIndexes.ref_count;

            // keep track of which texture indexes were associated with which directory
            strToIndexesMap_[DIR_PATH_PARAM_STR_FINAL] = textureIndexes;

            return textureIndexes.indexes;
        }
    }

    void TextureCache::RemoveByKey(const std::string & GAMEDATAFILE_KEY_STR)
    {
        RemoveByPath(game::GameDataFile::Instance()->GetMediaPath(GAMEDATAFILE_KEY_STR));
    }

    void TextureCache::RemoveByPath(const std::string & PATH_TO_TEXTURE_STR_ORIG)
    {
        auto const PATH_TO_TEXTURE_STR_FINAL{ misc::filesystem::MakePathPretty(
            PATH_TO_TEXTURE_STR_ORIG) };

        auto foundIter{ strToIndexesMap_.Find(PATH_TO_TEXTURE_STR_FINAL) };

        if (foundIter == std::end(strToIndexesMap_))
        {
            foundIter = strToIndexesMap_.Find(PATH_TO_TEXTURE_STR_ORIG);
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (foundIter != std::end(strToIndexesMap_)),
            "sfml_util::TextureCache::RemoveByPath(path=\""
                << PATH_TO_TEXTURE_STR_ORIG << "\" (\"" << PATH_TO_TEXTURE_STR_FINAL
                << "\")) failed because that path was not found.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (foundIter->second.ref_count != 0),
            "sfml_util::TextureCache::RemoveByPath(path=\""
                << PATH_TO_TEXTURE_STR_ORIG << "\" (\"" << PATH_TO_TEXTURE_STR_FINAL
                << "\")) failed because textureIndexes.ref_count=0.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (foundIter->second.indexes.empty() == false),
            "sfml_util::TextureCache::RemoveByPath(path=\""
                << PATH_TO_TEXTURE_STR_ORIG << "\" (\"" << PATH_TO_TEXTURE_STR_FINAL
                << "\")) failed because textureIndexes.indexes vec was empty.");

        if (--foundIter->second.ref_count == 0)
        {
            for (auto const INDEX : foundIter->second.indexes)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (INDEX < textureUPtrs_.size()),
                    "sfml_util::TextureCache::RemoveByPath(path=\""
                        << PATH_TO_TEXTURE_STR_ORIG << "\" (\"" << PATH_TO_TEXTURE_STR_FINAL
                        << "\")) found a TextureIndexes object with that path but one of the "
                           "indexes in the vector ("
                        << INDEX << ") was out of bounds with textureUPtrs_.size()="
                        << textureUPtrs_.size() << ".");

                auto & textureUPtr{ textureUPtrs_[INDEX] };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (textureUPtr),
                    "sfml_util::TextureCache::RemoveByPath(path=\""
                        << PATH_TO_TEXTURE_STR_ORIG << "\" (\"" << PATH_TO_TEXTURE_STR_FINAL
                        << "\")) found a TextureIndexes object with that path but one of the "
                           "indexes in the vector ("
                        << INDEX << ") referenced a TextureUPtr that was null.");

                RemoveInternal(INDEX);
            }

            strToIndexesMap_.Erase(PATH_TO_TEXTURE_STR_FINAL);
        }
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
            (INDEX < textureUPtrs_.size()),
            "sfml_util::TextureCache::"
                << "RemoveByIndex(" << INDEX << ") failed because that index is out of range."
                << "(size=" << textureUPtrs_.size() << ")");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (textureUPtrs_[INDEX]),
            "sfml_util::TextureCache::"
                << "RemoveByIndex(" << INDEX
                << ") failed because that textureUPtrs_[INDEX].get()==null.");

        std::vector<StrToIndexesMap_t::Pair_t> singlePathIndexesFound;
        std::vector<StrToIndexesMap_t::Pair_t> multiPathIndexesFound;
        for (auto & pathIndexesPair : strToIndexesMap_)
        {
            auto const & INDEXES{ pathIndexesPair.second.indexes };
            if ((INDEXES.size() == 1) && (INDEXES[0] == INDEX))
            {
                singlePathIndexesFound.emplace_back(pathIndexesPair);
            }
            else
            {
                if (std::find(std::begin(INDEXES), std::end(INDEXES), INDEX) != std::end(INDEXES))
                {
                    multiPathIndexesFound.emplace_back(pathIndexesPair);
                }
            }
        }

        auto makeFoundIndexesStr{ [](const std::vector<StrToIndexesMap_t::Pair_t> & PAIR_VEC) {
            std::ostringstream ss;
            ss << "{\n";

            for (auto const & PATH_INDEXES_PAIR : PAIR_VEC)
            {
                ss << "(path=\"" << PATH_INDEXES_PAIR.first
                   << "\", indexes_count=" << PATH_INDEXES_PAIR.second.indexes.size()
                   << ", indexes[0]=" << PATH_INDEXES_PAIR.second.indexes.front()
                   << ", ref_count=" << PATH_INDEXES_PAIR.second.ref_count << ")\n";
            }

            ss << "}\n";
            return ss.str();
        } };

        if (multiPathIndexesFound.empty() == false)
        {
            M_HP_LOG_WRN(
                "sfml_util::TextureCache::"
                << "RemoveByIndex(" << INDEX
                << ") will do nothing because it was found to be a part of the following "
                   "multi-index sets:"
                << makeFoundIndexesStr(multiPathIndexesFound)
                << "and these single-index sets: " << makeFoundIndexesStr(singlePathIndexesFound));
        }
        else
        {
            if (singlePathIndexesFound.size() > 1)
            {
                M_HP_LOG_WRN(
                    "sfml_util::TextureCache::"
                    << "RemoveByIndex(" << INDEX
                    << ") will do nothing because it was found to be a part of the following "
                       "single-index sets:"
                    << makeFoundIndexesStr(singlePathIndexesFound));
            }
            else
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (singlePathIndexesFound.empty() == false),
                    "sfml_util::TextureCache::"
                        << "RemoveByIndex(" << INDEX
                        << ") failed because no paths could be found for that index.");

                // at this point we know singlePathIndexesFound.size()==1
                auto foundIter{ strToIndexesMap_.Find(singlePathIndexesFound.front().first) };
                foundIter->second.ref_count -= 1;
                if (foundIter->second.ref_count == 0)
                {
                    RemoveInternal(INDEX);
                }
            }
        }
    }

    void TextureCache::RemoveAll() { Reset(); }

    const sf::Texture & TextureCache::GetByIndex(const std::size_t INDEX) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (INDEX != 0),
            "sfml_util::TextureCache::GetByIndex(0"
                << ") failed because zero is always an invalid index.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (INDEX < textureUPtrs_.size()),
            "sfml_util::TextureCache::GetByIndex(" << INDEX
                                                   << ") failed because that index is out of range."
                                                   << "(size=" << textureUPtrs_.size() << ")");

        auto & textureUPtr{ textureUPtrs_[INDEX] };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (textureUPtr),
            "sfml_util::TextureCache::GetByIndex("
                << INDEX << ") failed because the pointer at that index is null.");

        return *textureUPtr;
    }

    void TextureCache::Reset()
    {
        LogStageTextureCounts();

        textureUPtrs_.clear();
        strToIndexesMap_.Clear();
        stageNameToTextureMaxMap_.Clear();

        // Any value greater than (about) a thousand will work here.
        // Even an aggressize stage (like Credits) should not exceed 500.
        textureUPtrs_.reserve(1024);
        strToIndexesMap_.Reserve(1024);

        // put an empty image at index zero so there is something to return by default
        textureUPtrs_.emplace_back(std::make_unique<sf::Texture>());

        textureCount_ = 0;
        dataSizeBytes_ = 0;
    }

    std::size_t TextureCache::FindNextAvailableIndex()
    {
        auto const NUM_TEXTURES{ textureUPtrs_.size() };

        // start at one because zero is always an invalid index
        for (std::size_t i(1); i < NUM_TEXTURES; ++i)
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
        const std::string & PATH_TO_TEXTURE_STR, const bool WILL_SMOOTH, const bool WILL_FLIP_HORIZ)
    {
        auto const INDEX{ FindNextAvailableIndex() };

        auto & textureUPtr{ textureUPtrs_[INDEX] };

        if (!textureUPtr)
        {
            textureUPtr = std::make_unique<sf::Texture>();
        }

        try
        {
            sfml_util::Loaders::Texture(*textureUPtr, PATH_TO_TEXTURE_STR, WILL_SMOOTH);

            if (WILL_FLIP_HORIZ)
            {
                sfml_util::FlipHoriz(*textureUPtr);
            }
        }
        catch (...)
        {
            textureUPtr.reset();
            throw;
        }

        UpdateCoundAndSizeTracker(*textureUPtr);

        return INDEX;
    }

    std::size_t TextureCache::AddByPathInternalFake(const sf::Texture & TEXTURE)
    {
        auto const INDEX{ FindNextAvailableIndex() };
        auto & textureUPtr{ textureUPtrs_[INDEX] };

        if (!textureUPtr)
        {
            textureUPtr = std::make_unique<sf::Texture>(TEXTURE);
        }
        else
        {
            *textureUPtr = TEXTURE;
        }

        UpdateCoundAndSizeTracker(TEXTURE);
        return INDEX;
    }

    void TextureCache::RemoveInternal(const std::size_t INDEX)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (INDEX < textureUPtrs_.size()),
            "sfml_util::TextureCache::"
                << "RemoveInternal(" << INDEX
                << ") but that index is out of range.  (textureUPtrs_.size()="
                << textureUPtrs_.size() << ")");

        auto & textureUPtr{ textureUPtrs_[INDEX] };

        if (textureUPtr)
        {
            dataSizeBytes_ -= TextureSizeInBytes(*textureUPtr);
            --textureCount_;
            textureUPtr.reset();
        }
    }

    long long TextureCache::TextureSizeInBytes(const sf::Texture & TEXTURE) const
    {
        return static_cast<long long>(TEXTURE.getSize().x * TEXTURE.getSize().y * 4);
    }

    void TextureCache::UpdateCoundAndSizeTracker(const sf::Texture & TEXTURE)
    {
        dataSizeBytes_ += TextureSizeInBytes(TEXTURE);
        ++textureCount_;

        auto const STAGE_NAME_STR{ sfml_util::LoopState::ToString(
            game::LoopManager::Instance()->GetState()) };

        auto & countSizePair{ stageNameToTextureMaxMap_[STAGE_NAME_STR] };

        if (countSizePair.first < textureCount_)
        {
            countSizePair.first = textureCount_;
        }

        if (countSizePair.second < dataSizeBytes_)
        {
            countSizePair.second = dataSizeBytes_;
        }
    }

    void TextureCache::LogStageTextureCounts() const
    {
        for (auto const & STAGENAME_MAX_PAIR : stageNameToTextureMaxMap_)
        {
            M_HP_LOG_DBG(
                "TextureCache for " << STAGENAME_MAX_PAIR.first << " stage had a max of "
                                    << STAGENAME_MAX_PAIR.second.first
                                    << " textures loaded at once totalling "
                                    << STAGENAME_MAX_PAIR.second.second << " bytes.");
        }
    }

} // namespace sfml_util
} // namespace heroespath
