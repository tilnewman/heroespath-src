// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_FONTMANAGER_HPP_INCLUDED
#define HEROESPATH_GUI_FONTMANAGER_HPP_INCLUDED
//
// font-manager.hpp
//  Font specific gui code.
//
#include "gui/font-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-enums.hpp"

#include <SFML/Graphics/Color.hpp>

#include <memory>
#include <string>
#include <vector>

namespace sf
{
class Font;
}

namespace heroespath
{

using FontPtr_t = misc::NotNull<sf::Font *>;
using FontUPtr_t = std::unique_ptr<sf::Font>;
using FontUVec_t = std::vector<FontUPtr_t>;

namespace gui
{

    // A class that loads, stores, and distributes fonts.
    class FontManager
    {
    public:
        FontManager();
        ~FontManager();

        FontManager(const FontManager &) = delete;
        FontManager(FontManager &&) = delete;
        FontManager & operator=(const FontManager &) = delete;
        FontManager & operator=(FontManager &&) = delete;

        static misc::NotNull<FontManager *> Instance();
        static void Acquire();
        static void Release();

        const FontPtr_t GetFont(const GuiFont::Enum);
        void Load(const GuiFont::Enum);
        void Load(const FontEnumVec_t &);
        void Unload(const GuiFont::Enum);
        void Unload(const FontEnumVec_t &);
        void UnloadAll();
        bool IsLoaded(const GuiFont::Enum) const;

        static const sf::Color Color_PopupButtonUp(const popup::PopupButtonColor::Enum);
        static const sf::Color Color_PopupButtonDown(const popup::PopupButtonColor::Enum);
        static const sf::Color Color_PopupButtonOver(const popup::PopupButtonColor::Enum);
        static const sf::Color Color_PopupButtonDisabled(const popup::PopupButtonColor::Enum);

        // sizes
        unsigned int Size_Larger() const;
        unsigned int Size_Large() const;
        unsigned int Size_Largeish() const;
        unsigned int Size_Normal() const;
        unsigned int Size_Smallish() const;
        unsigned int Size_Small() const;
        unsigned int Size_Tiny() const;
        unsigned int Size_CombatCreatureLabels() const;

        const std::string NumberFontFamilyName() { return numberFontFamilyName_; }

    private:
        static const unsigned int SIZE_LARGER_MAX_;
        static const unsigned int SIZE_LARGER_MIN_;
        static const unsigned int SIZE_LARGE_MAX_;
        static const unsigned int SIZE_LARGE_MIN_;
        static const unsigned int SIZE_LARGEISH_MAX_;
        static const unsigned int SIZE_LARGEISH_MIN_;
        static const unsigned int SIZE_NORMAL_MIN_;
        static const unsigned int SIZE_NORMAL_MAX_;
        static const unsigned int SIZE_SMALLISH_MIN_;
        static const unsigned int SIZE_SMALLISH_MAX_;
        static const unsigned int SIZE_SMALL_MIN_;
        static const unsigned int SIZE_SMALL_MAX_;
        static const unsigned int SIZE_TINY_MIN_;
        static const unsigned int SIZE_TINY_MAX_;

        static std::string fontsDirectoryPath_;
        static std::unique_ptr<FontManager> instanceUPtr_;
        FontUVec_t fontUVec_;
        std::string numberFontFamilyName_;
        std::string fontsDirPathStr_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_FONTMANAGER_HPP_INCLUDED
