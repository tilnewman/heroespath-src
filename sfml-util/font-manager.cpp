//
// font-manager.cpp
//
#include "font-manager.hpp"

#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include "game/log-macros.hpp"

#include "common/assertorthrow.hpp"

#include <boost/filesystem.hpp>

#include <memory>


namespace sfml_util
{
    const unsigned int FontManager::SIZE_LARGER_MIN_(36);
    const unsigned int FontManager::SIZE_LARGER_MAX_(144);
    //
    const unsigned int FontManager::SIZE_LARGE_MIN_(31);
    const unsigned int FontManager::SIZE_LARGE_MAX_(124);
    //
    const unsigned int FontManager::SIZE_LARGEISH_MIN_(28);
    const unsigned int FontManager::SIZE_LARGEISH_MAX_(112);
    //
    const unsigned int FontManager::SIZE_NORMAL_MIN_(25);
    const unsigned int FontManager::SIZE_NORMAL_MAX_(100);
    //
    const unsigned int FontManager::SIZE_SMALLISH_MIN_(20);
    const unsigned int FontManager::SIZE_SMALLISH_MAX_(80);
    //
    const unsigned int FontManager::SIZE_SMALL_MIN_(17);
    const unsigned int FontManager::SIZE_SMALL_MAX_(68);
    //
    const unsigned int FontManager::SIZE_TINY_MIN_(14);
    const unsigned int FontManager::SIZE_TINY_MAX_(56);
    //
    std::string FontManager::fontsDirectoryPath_;
    std::unique_ptr<FontManager> FontManager::instanceUPtr_{ nullptr };


    FontManager::FontManager()
    :
        euler_fontSPtr_          (),
        gentiumPlus_fontSPtr_    (),
        goudyBookletter_fontSPtr_(),
        modernAntiqua_fontSPtr_  (),
        queenCountry_fontSPtr_   (),
        quillSword_fontSPtr_     (),
        valleyForge_fontSPtr_    ()
    {}


    FontManager * FontManager::Instance()
    {
        if (instanceUPtr_.get() == nullptr)
        {
            instanceUPtr_.reset(new FontManager);
        }

        return instanceUPtr_.get();
    }


    void FontManager::InstanceRelease()
    {
        instanceUPtr_.reset();
    }


    void FontManager::SetFontsDirectory(const std::string & PATH)
    {
        fontsDirectoryPath_ = PATH;
    }


    unsigned int FontManager::Size_Larger() const
    {
        return sfml_util::MapByRes(SIZE_LARGER_MIN_, SIZE_LARGER_MAX_);
    }


    unsigned int FontManager::Size_Large() const
    {
        return sfml_util::MapByRes(SIZE_LARGE_MIN_, SIZE_LARGE_MAX_);
    }


    unsigned int FontManager::Size_Largeish() const
    {
        return sfml_util::MapByRes(SIZE_LARGEISH_MIN_, SIZE_LARGEISH_MAX_);
    }


    unsigned int FontManager::Size_Normal() const
    {
        return sfml_util::MapByRes(SIZE_NORMAL_MIN_, SIZE_NORMAL_MAX_);
    }


    unsigned int FontManager::Size_Smallish() const
    {
        return sfml_util::MapByRes(SIZE_SMALLISH_MIN_, SIZE_SMALLISH_MAX_);
    }


    unsigned int FontManager::Size_Small() const
    {
        return sfml_util::MapByRes(SIZE_SMALL_MIN_, SIZE_SMALL_MAX_);
    }


    unsigned int FontManager::Size_Tiny() const
    {
        return sfml_util::MapByRes(SIZE_TINY_MIN_, SIZE_TINY_MAX_);
    }


    void FontManager::LoadFont(const std::string & FONT_FILE_NAME, FontSPtr_t & fontSPtr) const
    {
        namespace bfs = boost::filesystem;
        const bfs::path PATH_OBJ(bfs::system_complete(bfs::path(fontsDirectoryPath_) / bfs::path(FONT_FILE_NAME)));
        sfml_util::LoadFontSPtr(fontSPtr, PATH_OBJ.string());
    }

}
