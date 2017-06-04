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
#ifndef SFMLUTIL_FONTMANAGER_INCLUDED
#define SFMLUTIL_FONTMANAGER_INCLUDED
//
// font-manager.hpp
//  Font specific gui code.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/popup-enums.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{

    //A class that loads, stores, and distributes fonts by style.
    class FontManager
    {
        //prevent copy construction
        FontManager(const FontManager &) =delete;

        //prevent copy assignment
        FontManager & operator=(const FontManager &) =delete;

        //prevent non-singleton construction
        FontManager();

        //Note:  Keep order in sync with FontManager constructor
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
        ~FontManager();

        static FontManager * Instance();
        static void Acquire();
        static void Release();
        static void Fill();
        static void Empty();
        static void SetFontsDirectory(const std::string & PATH);

        //colors
        static inline const sf::Color Color_GrayLight()     { return sf::Color(200, 200, 200, 255); }
        static inline const sf::Color Color_GrayLighter()   { return sf::Color(232, 232, 232, 255); }
        static inline const sf::Color Color_GrayDark()      { return sf::Color(100, 100, 100, 255); }
        static inline const sf::Color Color_GrayDarker()    { return sf::Color(64,  64,  64,  255); }
        static inline const sf::Color Color_Orange()        { return sf::Color(255, 223, 181, 255); }
        static inline const sf::Color Color_Light()         { return sf::Color(220, 220, 220, 255); }
        static inline const sf::Color Color_LightGold()     { return sf::Color(255, 248, 220, 255); }
        //
        static inline const sf::Color Color_PopupButtonUp(const PopupButtonColor::Enum C)   { if (C == PopupButtonColor::Dark) return sf::Color(0, 0, 0, 70);  else return sf::Color(255, 255, 255, 127); }
        static inline const sf::Color Color_PopupButtonDown(const PopupButtonColor::Enum C) { if (C == PopupButtonColor::Dark) return sf::Color(0, 0, 0, 255); else return sf::Color::White; }
        static inline const sf::Color Color_PopupButtonOver(const PopupButtonColor::Enum C) { if (C == PopupButtonColor::Dark) return sf::Color(0, 0, 0, 150); else return sf::Color(255, 255, 255, 200); }
        //
        static inline sf::Uint8 ColorValue_Highlight()      { return 55; }

        //sizes
        unsigned int Size_Larger() const;
        unsigned int Size_Large() const;
        unsigned int Size_Largeish() const;
        unsigned int Size_Normal() const;
        unsigned int Size_Smallish() const;
        unsigned int Size_Small() const;
        unsigned int Size_Tiny() const;

        inline const std::string NumbersDefault1FamilyName() { return Font_NumbersDefault1()->getInfo().family; }

        inline FontPtr_t Font_NumbersDefault1() { return fontUVec_[static_cast<std::size_t>(Fonts::QuillSword)].get(); }
        inline FontPtr_t Font_NumbersTypical1() { return fontUVec_[static_cast<std::size_t>(Fonts::GentiumPlus)].get(); }
        inline FontPtr_t Font_NumbersTypical2() { return fontUVec_[static_cast<std::size_t>(Fonts::ModernAntiqua)].get(); }
        inline FontPtr_t Font_NumbersSmall()    { return fontUVec_[static_cast<std::size_t>(Fonts::ModernAntiqua)].get(); }
        //
        inline FontPtr_t Font_Default1()        { return fontUVec_[static_cast<std::size_t>(Fonts::Euler)].get(); }
        inline FontPtr_t Font_Default2()        { return fontUVec_[static_cast<std::size_t>(Fonts::GentiumPlus)].get(); }
        inline FontPtr_t Font_Typical()         { return fontUVec_[static_cast<std::size_t>(Fonts::GoudyBookletter)].get(); }
        //
        inline FontPtr_t Font_Dialog1()         { return fontUVec_[static_cast<std::size_t>(Fonts::Euler)].get(); }
        inline FontPtr_t Font_Dialog2()         { return fontUVec_[static_cast<std::size_t>(Fonts::GentiumPlus)].get(); }
        inline FontPtr_t Font_Dialog3()         { return fontUVec_[static_cast<std::size_t>(Fonts::ModernAntiqua)].get(); }
        //
        inline FontPtr_t Font_BigFlavor1()      { return fontUVec_[static_cast<std::size_t>(Fonts::ModernAntiqua)].get(); }
        inline FontPtr_t Font_BigFlavor2()      { return fontUVec_[static_cast<std::size_t>(Fonts::ValleyForge)].get(); }
        inline FontPtr_t Font_BigFlavor3()      { return fontUVec_[static_cast<std::size_t>(Fonts::QueenCountry)].get(); }
        //
        inline FontPtr_t Font_PopupButton()     { return Font_BigFlavor2(); }

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
        static FontUVec_t fontUVec_;
    };

}
#endif //SFMLUTIL_FONTMANAGER_INCLUDED
