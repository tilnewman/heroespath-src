#ifndef APPBASE_SFMLUTIL_FONTMANAGER_INCLUDED
#define APPBASE_SFMLUTIL_FONTMANAGER_INCLUDED
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

    //types required by the singleton implementation
    class FontManager;
    using FontManagerSPtr_t = std::shared_ptr<FontManager>;


    //A class that loads, stores, and distributes fonts by style.
    class FontManager
    {
        //prevent copy construction
        FontManager(const FontManager &);

        //prevent copy assignment
        FontManager & operator=(const FontManager &);

        //prevent non-singleton construction
        FontManager();

    public:
        virtual ~FontManager();

        static FontManagerSPtr_t Instance();
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

        inline FontSPtr_t Font_NumbersDefault1()    { if (quillSword_fontSPtr_.get() == nullptr)        LoadFont("/quill-sword/quillsword.ttf", quillSword_fontSPtr_);            return quillSword_fontSPtr_; }
        inline FontSPtr_t Font_NumbersTypical1()    { if (gentiumPlus_fontSPtr_.get() == nullptr)       LoadFont("/gentium-plus/gentium-plus.ttf", gentiumPlus_fontSPtr_);        return gentiumPlus_fontSPtr_; }
        inline FontSPtr_t Font_NumbersTypical2()    { if (modernAntiqua_fontSPtr_.get() == nullptr)     LoadFont("/modern-antiqua/modern-antiqua.ttf", modernAntiqua_fontSPtr_);  return modernAntiqua_fontSPtr_; }
        inline FontSPtr_t Font_NumbersSmall()       { if (modernAntiqua_fontSPtr_.get() == nullptr)     LoadFont("/modern-antiqua/modern-antiqua.ttf", modernAntiqua_fontSPtr_);  return modernAntiqua_fontSPtr_; }
        //
        inline FontSPtr_t Font_Default1()           { if (euler_fontSPtr_.get() == nullptr)             LoadFont("/euler/euler.otf", euler_fontSPtr_);                                   return euler_fontSPtr_; }
        inline FontSPtr_t Font_Default2()           { if (gentiumPlus_fontSPtr_.get() == nullptr)       LoadFont("/gentium-plus/gentium-plus.ttf", gentiumPlus_fontSPtr_);               return gentiumPlus_fontSPtr_; }
        inline FontSPtr_t Font_Typical()            { if (goudyBookletter_fontSPtr_.get() == nullptr)   LoadFont("/goudy-bookletter/goudy-bookletter.otf", goudyBookletter_fontSPtr_);   return goudyBookletter_fontSPtr_; }
        //
        inline FontSPtr_t Font_Dialog1()            { if (euler_fontSPtr_.get() == nullptr)             LoadFont("/euler/euler.otf", euler_fontSPtr_);                           return euler_fontSPtr_; }
        inline FontSPtr_t Font_Dialog2()            { if (gentiumPlus_fontSPtr_.get() == nullptr)       LoadFont("/gentium-plus/gentium-plus.ttf", gentiumPlus_fontSPtr_);       return gentiumPlus_fontSPtr_; }
        inline FontSPtr_t Font_Dialog3()            { if (modernAntiqua_fontSPtr_.get() == nullptr)     LoadFont("/modern-antiqua/modern-antiqua.ttf", modernAntiqua_fontSPtr_); return modernAntiqua_fontSPtr_; }
        //
        inline FontSPtr_t Font_BigFlavor1()         { if (modernAntiqua_fontSPtr_.get() == nullptr)     LoadFont("/modern-antiqua/modern-antiqua.ttf", modernAntiqua_fontSPtr_); return modernAntiqua_fontSPtr_; }
        inline FontSPtr_t Font_BigFlavor2()         { if (valleyForge_fontSPtr_.get() == nullptr)       LoadFont("/valley-forge/valleyforge.ttf", valleyForge_fontSPtr_);        return valleyForge_fontSPtr_; }
        inline FontSPtr_t Font_BigFlavor3()         { if (queenCountry_fontSPtr_.get() == nullptr)      LoadFont("/queen-country/queen-country.ttf", queenCountry_fontSPtr_);    return queenCountry_fontSPtr_; }
        //
        inline FontSPtr_t Font_PopupButton()        { return Font_BigFlavor2(); }

    private:
        void LoadFont(const std::string & FONT_FILE_NAME, FontSPtr_t & fontSPtr) const;

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
        static FontManagerSPtr_t instanceSPtr_;
        //
        FontSPtr_t euler_fontSPtr_;
        FontSPtr_t gentiumPlus_fontSPtr_;
        FontSPtr_t goudyBookletter_fontSPtr_;
        FontSPtr_t modernAntiqua_fontSPtr_;
        FontSPtr_t queenCountry_fontSPtr_;
        FontSPtr_t quillSword_fontSPtr_;
        FontSPtr_t valleyForge_fontSPtr_;
    };

}
#endif //APPBASE_SFMLUTIL_FONTMANAGER_INCLUDED
