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
#ifndef HEROESPATH_SFMLUTIL_FONTMANAGER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_FONTMANAGER_HPP_INCLUDED
//
// font-manager.hpp
//  Font specific gui code.
//
#include "misc/not-null.hpp"
#include "popup/popup-enums.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // A class that loads, stores, and distributes fonts by style.
    class FontManager
    {
    public:
        FontManager(const FontManager &) = delete;
        FontManager(FontManager &&) = delete;
        FontManager & operator=(const FontManager &) = delete;
        FontManager & operator=(FontManager &&) = delete;

        // Note:  Keep order in sync with FontManager constructor
        enum class Fonts
        {
            Euler = 0,
            GentiumPlus,
            GoudyBookletter,
            ModernAntiqua,
            QueenCountry,
            QuillSword,
            ValleyForge,
            Count
        };

    public:
        FontManager();
        ~FontManager();

        static misc::NotNull<FontManager *> Instance();
        static void Acquire();
        static void Release();
        static void SetFontsDirectory(const std::string & PATH);

        // colors
        static const sf::Color Color_GrayLight() { return sf::Color(200, 200, 200); }
        static const sf::Color Color_GrayLighter() { return sf::Color(232, 232, 232); }
        static const sf::Color Color_GrayDark() { return sf::Color(100, 100, 100); }
        static const sf::Color Color_GrayDarker() { return sf::Color(64, 64, 64); }
        static const sf::Color Color_Orange() { return sf::Color(255, 223, 181); }
        static const sf::Color Color_Light() { return sf::Color(220, 220, 220); }
        static const sf::Color Color_LightGold() { return sf::Color(255, 248, 220); }

        static const sf::Color Color_PopupButtonUp(const popup::PopupButtonColor::Enum);
        static const sf::Color Color_PopupButtonDown(const popup::PopupButtonColor::Enum);
        static const sf::Color Color_PopupButtonOver(const popup::PopupButtonColor::Enum);

        static sf::Uint8 ColorValue_Highlight() { return 55; }

        // sizes
        unsigned int Size_Larger() const;
        unsigned int Size_Large() const;
        unsigned int Size_Largeish() const;
        unsigned int Size_Normal() const;
        unsigned int Size_Smallish() const;
        unsigned int Size_Small() const;
        unsigned int Size_Tiny() const;
        unsigned int Size_CombatCreatureLabels() const { return Size_Smallish(); }

        const std::string NumbersDefault1FamilyName()
        {
            return Font_NumbersDefault1()->getInfo().family;
        }

        const FontPtr_t Font_NumbersDefault1()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::QuillSword)].get();
        }

        const FontPtr_t Font_NumbersTypical1()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::GentiumPlus)].get();
        }

        const FontPtr_t Font_NumbersTypical2()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::ModernAntiqua)].get();
        }

        const FontPtr_t Font_NumbersSmall()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::ModernAntiqua)].get();
        }

        const FontPtr_t Font_Default1()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::Euler)].get();
        }

        const FontPtr_t Font_Default2()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::GentiumPlus)].get();
        }

        const FontPtr_t Font_Typical()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::GoudyBookletter)].get();
        }

        const FontPtr_t Font_Dialog1()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::Euler)].get();
        }

        const FontPtr_t Font_Dialog2()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::GentiumPlus)].get();
        }

        const FontPtr_t Font_Dialog3()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::ModernAntiqua)].get();
        }

        const FontPtr_t Font_BigFlavor1()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::ModernAntiqua)].get();
        }

        const FontPtr_t Font_BigFlavor2()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::ValleyForge)].get();
        }

        const FontPtr_t Font_BigFlavor3()
        {
            return fontUVec_[static_cast<std::size_t>(Fonts::QueenCountry)].get();
        }

        const FontPtr_t Font_PopupButton() { return Font_BigFlavor2(); }

    private:
        static const sf::Font LoadFont(const std::string & FONT_FILE_NAME);

    public:
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

    private:
        static std::string fontsDirectoryPath_;
        static std::unique_ptr<FontManager> instanceUPtr_;
        FontUVec_t fontUVec_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_FONTMANAGER_HPP_INCLUDED
