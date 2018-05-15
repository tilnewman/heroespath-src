// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// font-manager.cpp
//
#include "font-manager.hpp"

#include "game/game-data-file.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"

#include <boost/filesystem.hpp>

namespace heroespath
{
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
    std::unique_ptr<FontManager> FontManager::instanceUPtr_;

    FontManager::FontManager()
        : fontUVec_()
    {
        M_HP_LOG_DBG("Subsystem Construction: FontManager");

        // Note:  Keep order in sync with enum FontManager::Fonts
        fontUVec_.emplace_back(std::make_unique<sf::Font>(LoadFont("/euler/euler.otf")));

        fontUVec_.emplace_back(
            std::make_unique<sf::Font>(LoadFont("/gentium-plus/gentium-plus.ttf")));

        fontUVec_.emplace_back(
            std::make_unique<sf::Font>(LoadFont("/goudy-bookletter/goudy-bookletter.otf")));

        fontUVec_.emplace_back(
            std::make_unique<sf::Font>(LoadFont("/modern-antiqua/modern-antiqua.ttf")));

        fontUVec_.emplace_back(
            std::make_unique<sf::Font>(LoadFont("/queen-country/queen-country.ttf")));

        fontUVec_.emplace_back(std::make_unique<sf::Font>(LoadFont("/quill-sword/quillsword.ttf")));

        fontUVec_.emplace_back(
            std::make_unique<sf::Font>(LoadFont("/valley-forge/valleyforge.ttf")));
    }

    FontManager::~FontManager() { M_HP_LOG_DBG("Subsystem Destruction: FontManager"); }

    misc::NotNull<FontManager *> FontManager::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() before Acquire(): FontManager");
            Acquire();
        }

        return instanceUPtr_.get();
    }

    void FontManager::Acquire()
    {
        if (!instanceUPtr_)
        {
            instanceUPtr_ = std::make_unique<FontManager>();
        }
        else
        {
            M_HP_LOG_ERR("Subsystem Acquire() after Construction: FontManager");
        }
    }

    void FontManager::Release()
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (instanceUPtr_), "sfml_util::FontManager::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    const sf::Color FontManager::Color_PopupButtonUp(const popup::PopupButtonColor::Enum C)
    {
        if (C == popup::PopupButtonColor::Dark)
        {
            return sf::Color(0, 0, 0, 70);
        }
        else
        {
            return sf::Color(255, 255, 255, 127);
        }
    }

    const sf::Color FontManager::Color_PopupButtonDown(const popup::PopupButtonColor::Enum C)
    {
        if (C == popup::PopupButtonColor::Dark)
        {
            return sf::Color(0, 0, 0, 255);
        }
        else
        {
            return sf::Color::White;
        }
    }

    const sf::Color FontManager::Color_PopupButtonOver(const popup::PopupButtonColor::Enum C)
    {
        if (C == popup::PopupButtonColor::Dark)
        {
            return sf::Color(0, 0, 0, 150);
        }
        else
        {
            return sf::Color(255, 255, 255, 200);
        }
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

    const sf::Font FontManager::LoadFont(const std::string & FONT_FILE_NAME)
    {
        namespace bfs = boost::filesystem;

        auto const FONTS_DIR{ game::GameDataFile::Instance()->GetMediaPath("media-fonts-dir") };

        const bfs::path PATH_OBJ(
            bfs::system_complete(bfs::path(FONTS_DIR) / bfs::path(FONT_FILE_NAME)));

        sf::Font font;
        sfml_util::LoadFont(font, PATH_OBJ.string());
        return font;
    }

} // namespace sfml_util
} // namespace heroespath
