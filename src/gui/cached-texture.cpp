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

#include "gui/texture-cache.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace gui
{

    CachedTexture::CachedTexture(const CachedTexture & CT)
        : path_(CT.path_)
        , index_(0) // set below
        , options_(CT.options_)
    {
        if (CT.path_.empty() == false)
        {
            // this call is required because copying must always increment the ref_count
            index_ = TextureCache::Instance()->AddByPath(CT.path_, CT.options_);
        }
    }

    CachedTexture::CachedTexture(CachedTexture && ct)
        : path_(std::move(ct.path_))
        , index_(std::move(ct.index_))
        , options_(std::move(ct.options_))
    {
        // empty ct's path_ so its destructor won't try to TextureCache::Remove()
        ct.path_ = "";
    }

    CachedTexture & CachedTexture::operator=(const CachedTexture & CT)
    {
        if ((this != &CT) && (CT.index_ != index_))
        {
            Release();

            if (CT.path_.empty() == false)
            {
                // this call is required because copying must always increment the ref_count
                index_ = TextureCache::Instance()->AddByPath(CT.path_, CT.options_);
            }

            path_ = CT.path_;
            options_ = CT.options_;
        }

        return *this;
    }

    CachedTexture & CachedTexture::operator=(CachedTexture && ct)
    {
        if (this != &ct)
        {
            if (index_ != ct.index_)
            {
                Release();

                path_ = std::move(ct.path_);
                index_ = std::move(ct.index_);
                options_ = std::move(ct.options_);
            }

            // zero the moved ct's index_ so it's destructor won't try to TextureCache::Remove...()
            ct.path_ = "";
        }

        return *this;
    }

    CachedTexture::CachedTexture(const char * const GAME_DATAFILE_KEY, const ImageOptions & OPTIONS)
        : path_(misc::filesystem::CleanPath(
            misc::ConfigFile::Instance()->GetMediaPath(GAME_DATAFILE_KEY)))
        , index_(TextureCache::Instance()->AddByPath(path_, OPTIONS))
        , options_(OPTIONS)
    {}

    CachedTexture::CachedTexture(
        const std::string & GAME_DATAFILE_KEY, const ImageOptions & OPTIONS)
        : path_(misc::filesystem::CleanPath(
            misc::ConfigFile::Instance()->GetMediaPath(GAME_DATAFILE_KEY)))
        , index_(TextureCache::Instance()->AddByPath(path_, OPTIONS))
        , options_(OPTIONS)
    {}

    CachedTexture::CachedTexture(const PathWrapper & PATH_WRAPPER, const ImageOptions & OPTIONS)
        : path_(misc::filesystem::CleanPath(PATH_WRAPPER.path_str))
        , index_(TextureCache::Instance()->AddByPath(path_, OPTIONS))
        , options_(OPTIONS)
    {}

    CachedTexture::CachedTexture(
        const std::string & FAKE_PATH, const sf::Texture & TEXTURE, const ImageOptions & OPTIONS)
        : path_(FAKE_PATH)
        , index_(TextureCache::Instance()->AddByPathFake(path_, TEXTURE, OPTIONS))
        , options_(OPTIONS)
    {}

    CachedTexture::~CachedTexture() { Release(); }

    const sf::Texture & CachedTexture::Get() const
    {
        return TextureCache::Instance()->GetByIndex(index_);
    }

    std::size_t CachedTexture::RefCount() const
    {
        return TextureCache::Instance()->GetRefCountByIndex(index_);
    }

    void CachedTexture::Release()
    {
        if (path_.empty() == false)
        {
            TextureCache::Instance()->RemoveByPath(path_, options_);

            // clear path_ so that repeated calls to Release() are safe
            path_.clear();
        }
    }

    CachedTextures::CachedTextures(const CachedTextures & CT)
        : path_(CT.path_)
        , indexes_()
        , options_(CT.options_)
    {
        if (CT.path_.empty() == false)
        {
            // this call is required because copying must always increment the ref_count
            indexes_ = TextureCache::Instance()->AddDirectoryByPath(CT.path_, CT.options_);
        }
    }

    CachedTextures::CachedTextures(CachedTextures && ct)
        : path_(std::move(ct.path_))
        , indexes_(std::move(ct.indexes_))
        , options_(ct.options_)
    {
        // zero the moved ct so its destructor won't try to TextureCache::Remove...()
        ct.path_ = "";
    }

    CachedTextures & CachedTextures::operator=(const CachedTextures & CT)
    {
        if ((this != &CT) && (CT.indexes_ != indexes_))
        {
            Release();

            if (CT.path_.empty() == false)
            {
                // this call is required because copying must always increment the ref_count
                indexes_ = TextureCache::Instance()->AddDirectoryByPath(CT.path_, CT.options_);
            }

            path_ = CT.path_;
            options_ = CT.options_;
        }

        return *this;
    }

    CachedTextures & CachedTextures::operator=(CachedTextures && ct)
    {
        if (this != &ct)
        {
            if (ct.indexes_ != indexes_)
            {
                Release();

                path_ = std::move(ct.path_);
                indexes_ = std::move(ct.indexes_);
                options_ = std::move(ct.options_);
            }

            // zero the moved ct so its destructor won't try to TextureCache::Remove...()
            ct.path_ = "";
        }

        return *this;
    }

    CachedTextures::CachedTextures(
        const std::string & GAME_DATAFILE_KEY, const ImageOptions & OPTIONS)
        : path_(misc::filesystem::CleanPath(
            misc::ConfigFile::Instance()->GetMediaPath(GAME_DATAFILE_KEY)))
        , indexes_(TextureCache::Instance()->AddDirectoryByPath(path_, OPTIONS))
        , options_(OPTIONS)
    {}

    CachedTextures::CachedTextures(
        const PathWrapper & DIR_PATH_WRAPER, const ImageOptions & OPTIONS)
        : path_(misc::filesystem::CleanPath(DIR_PATH_WRAPER.path_str))
        , indexes_(TextureCache::Instance()->AddDirectoryByPath(path_, OPTIONS))
        , options_(OPTIONS)
    {}

    CachedTextures::~CachedTextures() { Release(); }

    std::size_t CachedTextures::RefCount() const
    {
        return TextureCache::Instance()->GetRefCountByIndex(indexes_.at(0));
    }

    const sf::Texture & CachedTextures::operator[](const std::size_t INDEX) const
    {
        return TextureCache::Instance()->GetByIndex(indexes_[INDEX]);
    }

    const sf::Texture & CachedTextures::At(const std::size_t INDEX) const
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (INDEX < indexes_.size()),
            "gui::CachedTextures::At("
                << INDEX << ") but that index was out of bounds.  (indexes_.size()="
                << indexes_.size() << ", path=\"" << path_ << "\", " << options_ << ")");

        return operator[](INDEX);
    }

    const sf::Texture & CachedTextures::Front() const { return operator[](0); }

    const sf::Texture & CachedTextures::Back() const { return operator[](indexes_.size() - 1); }

    void CachedTextures::Release()
    {
        if (path_.empty() == false)
        {
            TextureCache::Instance()->RemoveByPath(path_, options_);

            // empty indexes_ so that repeated calls to Release() are safe
            path_ = "";
        }
    }

} // namespace gui
} // namespace heroespath
