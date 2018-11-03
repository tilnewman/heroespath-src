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

#include "gui/loaders.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/config-file.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "sfutil/display.hpp"

namespace heroespath
{
namespace gui
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
        : fontUVec_(GuiFont::Count)
        , numberFontFamilyName_("")
        , fontsDirPathStr_(misc::ConfigFile::Instance()->GetMediaPath("media-fonts-dir"))
    {
        M_HP_LOG_DBG("Subsystem Construction: FontManager");

        Load(GuiFont::Number);
        numberFontFamilyName_ = GetFont(GuiFont::Number)->getInfo().family;
        Unload(GuiFont::Number);
    }

    FontManager::~FontManager() { M_HP_LOG_DBG("Subsystem Destruction: FontManager"); }

    misc::NotNull<FontManager *> FontManager::Instance()
    {
        if (!instanceUPtr_)
        {
            M_HP_LOG_ERR("Subsystem Instance() called but instanceUPtr_ was null: FontManager");
            Acquire();
        }

        return instanceUPtr_;
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
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (instanceUPtr_), "gui::FontManager::Release() found instanceUPtr that was null.");

        instanceUPtr_.reset();
    }

    const FontPtr_t FontManager::GetFont(const GuiFont::Enum FONT)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (GuiFont::IsValid(FONT)),
            "gui::FontManager::GetFont(font_enum=" << static_cast<misc::EnumUnderlying_t>(FONT)
                                                   << ") given an invalid font enum.");

        if (IsLoaded(FONT) == false)
        {
            M_HP_LOG_WRN(
                "gui::FontManager::GetFont("
                << GuiFont::ToString(FONT) << "/" << GuiFont::Name(FONT)
                << ") asked to return a font that was not already loaded.  Loading now...");

            Load(FONT);
        }

        return GetFontRef(FONT);
    }

    void FontManager::Load(const GuiFont::Enum FONT)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (GuiFont::IsValid(FONT)),
            "gui::FontManager::Load(font_enum=" << static_cast<misc::EnumUnderlying_t>(FONT)
                                                << ") given an invalid font enum.");

        if (IsLoaded(FONT) == false)
        {
            auto & fontUPtr { GetFontRef(FONT) };
            fontUPtr = std::make_unique<sf::Font>();
            gui::Loaders::Font(
                *fontUPtr,
                misc::filesystem::CombinePathsThenClean(fontsDirPathStr_, GuiFont::Path(FONT)));
        }
    }

    void FontManager::Load(const FontEnumVec_t & FONT_ENUM_VEC)
    {
        for (const auto FONT_ENUM : FONT_ENUM_VEC)
        {
            Load(FONT_ENUM);
        }
    }

    void FontManager::Unload(const GuiFont::Enum FONT)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (GuiFont::IsValid(FONT)),
            "gui::FontManager::Unload(font_enum=" << static_cast<misc::EnumUnderlying_t>(FONT)
                                                  << ") given an invalid font enum.");

        GetFontRef(FONT).reset();
    }

    void FontManager::Unload(const FontEnumVec_t & FONT_ENUM_VEC)
    {
        for (const auto FONT_ENUM : FONT_ENUM_VEC)
        {
            Unload(FONT_ENUM);
        }
    }

    void FontManager::UnloadAll()
    {
        for (auto & fontUPtr : fontUVec_)
        {
            fontUPtr.reset();
        }
    }

    bool FontManager::IsLoaded(const GuiFont::Enum FONT) const
    {
        if (GuiFont::IsValid(FONT))
        {
            return !!fontUVec_.at(static_cast<std::size_t>(FONT));
        }

        return false;
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

    const sf::Color FontManager::Color_PopupButtonDisabled(const popup::PopupButtonColor::Enum C)
    {
        if (C == popup::PopupButtonColor::Dark)
        {
            return sf::Color(0, 0, 0, 40);
        }
        else
        {
            return sf::Color(255, 255, 255, 70);
        }
    }

    unsigned int FontManager::Size_Larger() const
    {
        return sfutil::MapByRes(SIZE_LARGER_MIN_, SIZE_LARGER_MAX_);
    }

    unsigned int FontManager::Size_Large() const
    {
        return sfutil::MapByRes(SIZE_LARGE_MIN_, SIZE_LARGE_MAX_);
    }

    unsigned int FontManager::Size_Largeish() const
    {
        return sfutil::MapByRes(SIZE_LARGEISH_MIN_, SIZE_LARGEISH_MAX_);
    }

    unsigned int FontManager::Size_Normal() const
    {
        return sfutil::MapByRes(SIZE_NORMAL_MIN_, SIZE_NORMAL_MAX_);
    }

    unsigned int FontManager::Size_Smallish() const
    {
        return sfutil::MapByRes(SIZE_SMALLISH_MIN_, SIZE_SMALLISH_MAX_);
    }

    unsigned int FontManager::Size_Small() const
    {
        return sfutil::MapByRes(SIZE_SMALL_MIN_, SIZE_SMALL_MAX_);
    }

    unsigned int FontManager::Size_Tiny() const
    {
        return sfutil::MapByRes(SIZE_TINY_MIN_, SIZE_TINY_MAX_);
    }

    unsigned int FontManager::Size_CombatCreatureLabels() const { return Size_Smallish(); }

    FontUPtr_t & FontManager::GetFontRef(const GuiFont::Enum FONT)
    {
        return fontUVec_.at(static_cast<std::size_t>(FONT));
    }

} // namespace gui
} // namespace heroespath
