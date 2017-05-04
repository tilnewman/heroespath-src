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

#include <string>
#include <vector>
#include <memory>


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


    //types required by the singleton implementation
    class PopupManager;
    using PopupManagerSPtr_t = std::shared_ptr<PopupManager>;


    //A class that loads, stores, and distributes fonts by style.
    class PopupManager
    {
        //prevent copy construction
        PopupManager(const PopupManager &);

        //prevent copy assignment
        PopupManager & operator=(const PopupManager &);

        //prevent non-singleton construction
        PopupManager();

    public:
        virtual ~PopupManager();
        static PopupManagerSPtr_t Instance();

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
        static inline sf::IntRect Rect_Banner()         { return sf::IntRect(78,  32, 338, 184); }
        static inline sf::IntRect Rect_Regular()        { return sf::IntRect(32,  32, 431, 268); }
        static inline sf::IntRect Rect_RegularSidebar() { return sf::IntRect(115, 32, 347, 268); }
        static inline sf::IntRect Rect_Large()          { return sf::IntRect(47,  47, 407, 403); }
        static inline sf::IntRect Rect_LargeSidebar()   { return sf::IntRect(85,  47, 360, 403); }

        //Returns a sprite for a randomly selected popup window accent that is
        //positioned, scaled, and colored for drawing to the paper popup window.
        //throws std::range_error on an unknown enum value
        const sf::Sprite AccentSprite(const game::PopupInfo &, TextureSPtr_t &) const;

        //throws range_error on an unknown enum value
        const sf::IntRect Rect(const PopupImage::Enum, const float SCALE = 1.0f) const;

        //get popup window textures based on the style name
        inline TextureSPtr_t Texture_Banner() const         { return popupBannerTextureSPtr_; }
        inline TextureSPtr_t Texture_Regular() const        { return popupRegularTextureSPtr_; }
        inline TextureSPtr_t Texture_RegularSidebar() const { return popupRegularSidebarTextureSPtr_; }
        inline TextureSPtr_t Texture_Large() const          { return popupLargeTextureSPtr_; }
        inline TextureSPtr_t Texture_LargeSidebar() const   { return popupLargeSidebarTextueSPtr_; }

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

        const game::PopupInfo CreatePopupInfo(const std::string &              POPUP_NAME,
                                                    const std::string &              PROMPT_TEXT,
                                                    const sfml_util::TextureSPtr_t & FROM_IMAGE,
                                                    const sfml_util::TextureSPtr_t & TO_IMAGE,
                                                    const PopupButtons::Enum         BUTTONS      = PopupButtons::Okay,
                                                    const unsigned int               FONT_SIZE    = FontManager::Instance()->Size_Normal(),
                                                    const sound_effect::Enum         SOUND_EFFECT = sound_effect::PromptGeneric);

        //throws range error upon unknown enum value
        TextureSPtr_t Texture(const PopupImage::Enum) const;

        sfml_util::PopupStageSPtr_t CreatePopupStage(const game::PopupInfo &);

    private:
        void LoadPopup(const std::string & POPUP_FILE_NAME,
                       TextureSPtr_t &     textureSPtr) const;

        void LoadAccent(const std::string & ACCENT_FILE_NAME,
                        TextureSPtr_t &     textureSPtr) const;

        float GetScaleForImage(const PopupImage::Enum E) const;

    private:
        static std::string windowTextureDirectoryPath_;
        static std::string accentTextureDirectoryPath_;
        static const sf::Uint8 ACCENT_IMAGE_ALPHA_;
        static PopupManagerSPtr_t instanceSPtr_;
        static sf::Color fontColor_;
        //
        const float BACKGROUND_IMAGE_SCALE_DEFAULT_;
        //
        TextureSPtr_t popupBannerTextureSPtr_;
        TextureSPtr_t popupRegularTextureSPtr_;
        TextureSPtr_t popupRegularSidebarTextureSPtr_;
        TextureSPtr_t popupLargeTextureSPtr_;
        TextureSPtr_t popupLargeSidebarTextueSPtr_;
    };

}
}
#endif //SFMLUTIL_POPUPMANAGER_INCLUDED
