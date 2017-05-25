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
#ifndef SFMLUTIL_POPUPMANAGER_INCLUDED
#define SFMLUTIL_POPUPMANAGER_INCLUDED
//
// popup-manager.hpp
//  Code to load and store popup window textures.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/popup-enums.hpp"
#include "sfml-util/justified-enum.hpp"
#include "sfml-util/sound-effects-enum.hpp"

#include "game/i-popup-callback.hpp"
#include "game/popup-info.hpp"

#include <boost/filesystem.hpp>

#include <map>
#include <string>
#include <vector>
#include <memory>


namespace game
{
namespace creature
{
    class Creature;
    using CreaturePtr = Creature *;
}
}

namespace sfml_util
{
    class PopupStage;
    using PopupStageSPtr_t = std::shared_ptr<PopupStage>;

namespace gui
{

    namespace box
    {
        class Info;
    }


    using PathVec_t = std::vector<boost::filesystem::path>;


    //A class that loads, stores, and distributes fonts by style.
    class PopupManager
    {
        //prevent copy construction
        PopupManager(const PopupManager &) =delete;

        //prevent copy assignment
        PopupManager & operator=(const PopupManager &) =delete;

        //prevent non-singleton construction
        PopupManager();

    public:
        static PopupManager * Instance();
        static void Acquire();
        static void Release();
        static void SetTexturesDirectoryPaths(const std::string & WINDOWS_PATH,
                                              const std::string & ACCENTS_PATH);

        bool Test();

        void LoadAssets();

        //colors
        static inline sf::Color Color_Fade() { return sf::Color(0, 0, 0, 127); }
        static inline sf::Color Color_Font() { return fontColor_; }

        //speed multipliers
        static inline float SpeedMult_Fade() { return 500.0f; }

        //inner rects that content can appear within
        static inline sf::IntRect Rect_Banner()                 { return sf::IntRect( 78,  32, 338, 184); }
        static inline sf::IntRect Rect_Regular()                { return sf::IntRect( 32,  32, 431, 268); }
        static inline sf::IntRect Rect_RegularSidebar()         { return sf::IntRect(115,  32, 347, 268); }
        static inline sf::IntRect Rect_Large()                  { return sf::IntRect( 47,  47, 407, 403); }
        static inline sf::IntRect Rect_LargeSidebar()           { return sf::IntRect( 85,  47, 360, 403); }
        static inline sf::IntRect Rect_Spellbook_PageLeft()     { return sf::IntRect(165, 155, 652, 990); }
        static inline sf::IntRect Rect_Spellbook_PageRight()    { return sf::IntRect(932, 155, 652, 990); }

        //throws range_error on an unknown enum value
        const sf::IntRect Rect(const PopupImage::Enum, const float SCALE = 1.0f) const;

        //get popup window textures based on the style name
        inline TextureSPtr_t Texture_Banner() const         { return popupBannerTextureSPtr_; }
        inline TextureSPtr_t Texture_Regular() const        { return popupRegularTextureSPtr_; }
        inline TextureSPtr_t Texture_RegularSidebar() const { return popupRegularSidebarTextureSPtr_; }
        inline TextureSPtr_t Texture_Large() const          { return popupLargeTextureSPtr_; }
        inline TextureSPtr_t Texture_LargeSidebar() const   { return popupLargeSidebarTextueSPtr_; }
        inline TextureSPtr_t Texture_Spellbook() const      { return popupSpellbookTextureSPtr_; }

        //TextInfo creation helper functions
        const sfml_util::gui::TextInfo TextInfoDefault(const std::string &              TEXT,
                                                       const sfml_util::Justified::Enum JUSTIFIED = Justified::Center,
                                                       const unsigned int               FONT_SIZE = FontManager::Instance()->Size_Large()) const;

        //create popup window info objects
        //use this function to make popup windows with the typical paper image backgrounds
        const game::PopupInfo CreatePopupInfo(const std::string &           POPUP_NAME,
                                              const std::string &           PROMPT_TEXT,
                                              const PopupButtons::Enum      BUTTONS      = PopupButtons::Okay,
                                              const PopupImage::Enum        IMAGE        = PopupImage::Banner,
                                              const Justified::Enum         JUSTIFIED    = Justified::Center,
                                              const sound_effect::Enum      SOUND_EFFECT = sound_effect::PromptGeneric,
                                              const game::Popup::Enum WHICH_POPUP  = game::Popup::Generic,
                                              const unsigned int            FONT_SIZE    = FontManager::Instance()->Size_Normal());

        //use this function to create popup windows with a simple box and custom background color
        const game::PopupInfo CreatePopupInfo(const std::string &      POPUP_NAME,
                                              const std::string &      PROMPT_TEXT,
                                              const sf::Color &        TEXT_COLOR,
                                              const gui::box::Info &   BOX_INFO,
                                              const PopupButtons::Enum BUTTONS      = sfml_util::PopupButtons::Okay,
                                              const Justified::Enum    JUSTIFIED    = sfml_util::Justified::Center,
                                              const sound_effect::Enum SOUND_EFFECT = sound_effect::PromptGeneric,
                                              const unsigned int       FONT_SIZE    = FontManager::Instance()->Size_Smallish());

        //use this function to create image select popup windows
        const game::PopupInfo CreatePopupInfo(const std::string &              POPUP_NAME,
                                              const std::string &              PROMPT_TEXT,
                                              const sfml_util::TextureSVec_t & IMAGES_SVEC,
                                              const sound_effect::Enum         SOUND_EFFECT = sound_effect::PromptGeneric,
                                              const unsigned int               FONT_SIZE    = FontManager::Instance()->Size_Normal());

        //use this function to create the number selection popup window
        const game::PopupInfo CreatePopupInfo(const std::string & POPUP_NAME,
                                              const std::string & PROMPT_TEXT,
                                              const std::size_t   THE_MIN,
                                              const std::size_t   THE_MAX,
                                              const unsigned int  FONT_SIZE = FontManager::Instance()->Size_Normal());

        //use this function to make character selection popup windows
        const game::PopupInfo CreatePopupInfo(const std::string &              POPUP_NAME,
                                              const std::string &              PROMPT_TEXT,
                                              const std::vector<std::size_t> & INVALID_PLAYER_NUM_VEC,
                                              const unsigned int               FONT_SIZE = FontManager::Instance()->Size_Large());

        //use this function to make image fade/transition popup windows
        const game::PopupInfo CreatePopupInfo(const std::string &              POPUP_NAME,
                                              const std::string &              PROMPT_TEXT,
                                              const sfml_util::TextureSPtr_t & FROM_IMAGE,
                                              const sfml_util::TextureSPtr_t & TO_IMAGE,
                                              const PopupButtons::Enum         BUTTONS      = PopupButtons::Okay,
                                              const unsigned int               FONT_SIZE    = FontManager::Instance()->Size_Normal(),
                                              const sound_effect::Enum         SOUND_EFFECT = sound_effect::PromptGeneric);

        //use this function to make the spellbook popup window
        const game::PopupInfo CreateSpellbookPopupInfo(const std::string &                 POPUP_NAME,
                                                       const game::creature::CreaturePtr_t CREATURE_CPTR,
                                                       const std::size_t                   INITIAL_SELECTION);

        //throws range error upon unknown enum value
        TextureSPtr_t Texture(const PopupImage::Enum) const;

        sfml_util::PopupStageSPtr_t CreatePopupStage(const game::PopupInfo &);

        TextureSPtr_t LoadRandomAccentImage() const;

    private:
        void LoadPopup(const std::string & POPUP_FILE_NAME,
                       TextureSPtr_t &     textureSPtr) const;

        float GetScaleForImage(const PopupImage::Enum E) const;

        void LoadAccentImagePaths();

    private:
        static std::string windowTextureDirectoryPath_;
        static std::string accentTextureDirectoryPath_;
        static sf::Color fontColor_;
        static std::unique_ptr<PopupManager> instanceUPtr_;
        //
        const float BACKGROUND_IMAGE_SCALE_DEFAULT_;
        //
        TextureSPtr_t popupBannerTextureSPtr_;
        TextureSPtr_t popupRegularTextureSPtr_;
        TextureSPtr_t popupRegularSidebarTextureSPtr_;
        TextureSPtr_t popupLargeTextureSPtr_;
        TextureSPtr_t popupLargeSidebarTextueSPtr_;
        TextureSPtr_t popupSpellbookTextureSPtr_;
        //
        PathVec_t accentPathsVec_;
    };

}
}
#endif //SFMLUTIL_POPUPMANAGER_INCLUDED
