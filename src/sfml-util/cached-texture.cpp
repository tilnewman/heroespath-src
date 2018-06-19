// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// cached-texture.cpp
//
#include "cached-texture.hpp"

#include "game/game-data-file.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/texture-cache.hpp"

#include <boost/filesystem/path.hpp>

namespace heroespath
{
namespace sfml_util
{

    CachedTexture::CachedTexture(const CachedTexture & CT)
        : path_(CT.path_)
        , index_(TextureCache::Instance()->AddByPath(
              path_)) // this call required to increment the reference count
    {}

    CachedTexture::CachedTexture(CachedTexture && ct)
        : path_(std::move(ct.path_))
        , index_(std::move(ct.index_))
    {
        // zero the moved ct so it's destructor won't try to TextureCache::Remove...()
        ct.path_.clear();
        ct.index_ = 0;
    }

    CachedTexture & CachedTexture::operator=(const CachedTexture & CT)
    {
        if (this != &CT)
        {
            path_ = CT.path_;

            // this call required to increment the reference count
            index_ = TextureCache::Instance()->AddByPath(path_);
        }

        return *this;
    }

    CachedTexture & CachedTexture::operator=(CachedTexture && ct)
    {
        if (this != &ct)
        {
            path_ = std::move(ct.path_);
            index_ = std::move(ct.index_);

            // zero the moved ct so it's destructor won't try to TextureCache::Remove...()
            ct.path_.clear();
            ct.index_ = 0;
        }

        return *this;
    }

    CachedTexture::CachedTexture(
        const char * const GAME_DATAFILE_KEY, const bool WILL_SMOOTH, const bool WILL_FLIP_HORIZ)
        : path_(game::GameDataFile::Instance()->GetMediaPath(GAME_DATAFILE_KEY))
        , index_(TextureCache::Instance()->AddByPath(path_, WILL_SMOOTH, WILL_FLIP_HORIZ))
    {}

    CachedTexture::CachedTexture(
        const std::string & GAME_DATAFILE_KEY, const bool WILL_SMOOTH, const bool WILL_FLIP_HORIZ)
        : path_(game::GameDataFile::Instance()->GetMediaPath(GAME_DATAFILE_KEY))
        , index_(TextureCache::Instance()->AddByPath(path_, WILL_SMOOTH, WILL_FLIP_HORIZ))
    {}

    CachedTexture::CachedTexture(
        const boost::filesystem::path & PATH, const bool WILL_SMOOTH, const bool WILL_FLIP_HORIZ)
        : path_(PATH.string())
        , index_(TextureCache::Instance()->AddByPath(path_, WILL_SMOOTH, WILL_FLIP_HORIZ))
    {}

    CachedTexture::CachedTexture(const std::string & FAKE_PATH, const sf::Texture & TEXTURE)
        : path_(FAKE_PATH)
        , index_(TextureCache::Instance()->AddByPathFake(path_, TEXTURE))
    {}

    CachedTexture::~CachedTexture()
    {
        if ((path_.empty() == false) && (0 != index_))
        {
            TextureCache::Instance()->RemoveByPath(path_);
        }
    }

    const sf::Texture & CachedTexture::Get() const
    {
        return TextureCache::Instance()->GetByIndex(index_);
    }

    CachedTextures::CachedTextures(const CachedTextures & CT)
        : path_(CT.path_)
        , indexes_(TextureCache::Instance()->AddDirectoryByPath(
              path_)) // this call required to increment the reference count
    {}

    CachedTextures::CachedTextures(CachedTextures && ct)
        : path_(std::move(ct.path_))
        , indexes_(std::move(ct.indexes_))
    {
        // zero the moved ct so it's destructor won't try to TextureCache::Remove...()
        ct.path_.clear();
        ct.indexes_.clear();
    }

    CachedTextures & CachedTextures::operator=(const CachedTextures & CT)
    {
        if (this != &CT)
        {
            path_ = CT.path_;

            // this call required to increment the reference count
            indexes_ = TextureCache::Instance()->AddDirectoryByPath(path_);
        }

        return *this;
    }

    CachedTextures & CachedTextures::operator=(CachedTextures && ct)
    {
        if (this != &ct)
        {
            path_ = std::move(ct.path_);
            indexes_ = std::move(ct.indexes_);

            // zero the moved ct so it's destructor won't try to TextureCache::Remove...()
            ct.path_.clear();
            ct.indexes_.clear();
        }

        return *this;
    }

    CachedTextures::CachedTextures(const std::string & GAME_DATAFILE_KEY, const bool WILL_SMOOTH)
        : path_(game::GameDataFile::Instance()->GetMediaPath(GAME_DATAFILE_KEY))
        , indexes_(TextureCache::Instance()->AddDirectoryByPath(path_, WILL_SMOOTH))
    {}

    CachedTextures::CachedTextures(const boost::filesystem::path & DIR_PATH, const bool WILL_SMOOTH)
        : path_(DIR_PATH.string())
        , indexes_(TextureCache::Instance()->AddDirectoryByPath(path_, WILL_SMOOTH))
    {}

    CachedTextures::~CachedTextures()
    {
        if ((path_.empty() == false) && (indexes_.empty() == false))
        {
            TextureCache::Instance()->RemoveByPath(path_);
        }
    }

    const sf::Texture & CachedTextures::operator[](const std::size_t INDEX) const
    {
        return TextureCache::Instance()->GetByIndex(indexes_[INDEX]);
    }

    const sf::Texture & CachedTextures::At(const std::size_t INDEX) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (INDEX < indexes_.size()),
            "sfml_util::CachedTextures::At("
                << INDEX << ") but that index was out of bounds.  (indexes_.size()="
                << indexes_.size() << ")");

        return operator[](INDEX);
    }

    const sf::Texture & CachedTextures::Front() const { return operator[](0); }

    const sf::Texture & CachedTextures::Back() const { return operator[](indexes_.size() - 1); }

} // namespace sfml_util
} // namespace heroespath
