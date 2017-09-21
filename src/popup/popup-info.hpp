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
#ifndef POPUP_POPUPINFO_HPP_INCLUDED
#define POPUP_POPUPINFO_HPP_INCLUDED
//
// popup-info.hpp
//  A class that encapsulates everything about a popup
//  window except for drawing details such as position etc.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/box.hpp"

#include "popup/popup-enums.hpp"

#include "game/combat/combat-over-enum.hpp"

#include <string>
#include <vector>


namespace game
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;

    class Title;
    using TitlePtr_t = Title *;
}
}
namespace popup
{
    //encapsulates all the info about a popup window except for screen position
    class PopupInfo
    {
    public:
        //default constructor for most popups
        PopupInfo(const std::string &                     NAME,
                  const sfml_util::gui::TextInfo &        TEXT_INFO,
                  const PopupButtons::Enum                BUTTONS              = PopupButtons::Okay,
                  const PopupImage::Enum                  IMAGE                = PopupImage::Banner,
                  const float                             IMAGE_SCALE          = 1.0f,
                  const sfml_util::sound_effect::Enum     SOUND_EFFECT         = sfml_util::sound_effect::PromptGeneric,
                  const PopupButtonColor::Enum            BUTTON_COLOR         = PopupButtonColor::Dark,
                  const bool                              WILL_ADD_RAND_IMAGE  = true,
                  const std::vector<std::size_t> &        INVALID_NUM_VEC      = std::vector<std::size_t>(),
                  const sfml_util::TextureVec_t &         TEXTURE_VEC          = sfml_util::TextureVec_t(),
                  const std::vector<std::string> &        TEXT_VEC             = std::vector<std::string>(),
                  const float                             IMAGE_FADE_SPEED     = IMAGE_FADE_SPEED_DEFAULT_,
                  const game::creature::CreaturePtr_t     CREATURE_CPTR        = nullptr,
                  const std::size_t                       INITIAL_SELECTION    = 0,
                  const bool                              ARE_IMAGES_CREATURES = false,
                  const std::string &                     TITLE_TEXT           = "",
                  const std::string &                     DESC_TEXT            = "",
                  const game::creature::TitlePtr_t        FROM_TITLE_PTR       = nullptr,
                  const game::creature::TitlePtr_t        TO_TITLE_PTR         = nullptr);

        //use this constructor for boxed popups with no background image
        PopupInfo(const std::string &                     NAME,
                  const sfml_util::gui::TextInfo &        TEXT_INFO,
                  const PopupButtons::Enum                BUTTONS,
                  const sfml_util::gui::box::Info &       BOX_INFO,
                  const float                             MAX_SIZE_RATIO_X    = 0.25f,
                  const float                             MAX_SIZE_RATIO_Y    = 0.5f,
                  const sfml_util::sound_effect::Enum     SOUND_EFFECT        = sfml_util::sound_effect::PromptGeneric,
                  const PopupButtonColor::Enum            BUTTON_COLOR        = PopupButtonColor::Light,
                  const bool                              WILL_ADD_RAND_IMAGE = true);

        //use this constructor for image selection popups
        PopupInfo(const std::string &                     NAME,
                  const sfml_util::gui::TextInfo &        TEXT_INFO,
                  const sfml_util::TextureVec_t &         TEXTURE_VEC,
                  const bool                              ARE_IMAGES_CREATURES,
                  const float                             IMAGE_SCALE,
                  const sfml_util::sound_effect::Enum     SOUND_EFFECT = sfml_util::sound_effect::PromptGeneric,
                  const PopupButtonColor::Enum BUTTON_COLOR = PopupButtonColor::Dark);

        //use this constructor for number selection popups
        PopupInfo(const std::string &              NAME,
                  const sfml_util::gui::TextInfo & TEXT_INFO,
                  const std::size_t                THE_MIN,
                  const std::size_t                THE_MAX,
                  const float                      IMAGE_SCALE);

        //use this constructor for end-of-combat popups
        PopupInfo(const std::string &                 NAME,
                  const sfml_util::gui::TextInfo &    TEXT_INFO,
                  const float                         IMAGE_SCALE,
                  const PopupButtons::Enum            BUTTONS,
                  const game::combat::CombatEnd::Enum HOW_COMBAT_ENDED);

        virtual ~PopupInfo();

        PopupInfo(const PopupInfo &);
        PopupInfo & operator=(const PopupInfo &);

        inline const std::string                 Name() const               { return name_; }
        inline PopupButtons::Enum                Buttons() const            { return buttons_; }
        inline PopupImage::Enum                  Image() const              { return image_; }
        inline const sfml_util::gui::TextInfo    TextInfo() const           { return textInfo_; }
        inline sfml_util::sound_effect::Enum     SoundEffect() const        { return soundEffect_; }
        inline const sfml_util::gui::box::Info   BoxInfo() const            { return boxInfo_; }
        inline float                             SizeX() const              { return ratioX_; }
        inline float                             SizeY() const              { return ratioY_; }
        inline PopupButtonColor::Enum ButtonColor() const        { return buttonColor_; }
        inline bool                              WillAddRandImage() const   { return willAddRandImage_; }
        inline float                             ImageScale() const         { return imageScale_; }
        inline const sfml_util::TextureVec_t &   Images() const             { return textureVec_; }
        inline std::size_t                       ImagesCount() const        { return textureVec_.size(); }
        inline std::size_t                       NumberSelMin() const       { return numberMin_; }
        inline std::size_t                       NumberSelMax() const       { return numberMax_; }
        inline const std::vector<std::size_t>    NumberSelInvVec() const    { return numberInvalidVec_; }
        inline float                             ImageFadeSpeed() const     { return imageFadeSpeed_; }
        inline game::creature::CreaturePtr_t     CreaturePtr() const        { return creatureCPtr_; }
        inline std::size_t                       InitialSelection() const   { return initialSelection_; }
        inline bool                              AreImagesCreatures() const { return areImgsCreatures_; }
        inline const std::vector<std::string> &  TextVec() const            { return textVec_; }
        inline game::combat::CombatEnd::Enum     HowCombatEnded() const     { return howCombatEnded_; }
        inline game::creature::TitlePtr_t        TitleFromPtr() const       { return titleFromPtr_; }
        inline game::creature::TitlePtr_t        TitleToPtr() const         { return titleToPtr_; }
        inline const std::string                 TitleText() const          { return titleText_; }
        inline const std::string                 DescText() const           { return descText_; }
        inline bool                              WillIncludeItems() const   { return willIncludeItems_; }

        inline const sf::Texture & ImagesAt(const std::size_t I) const
        {
            return textureVec_.at(I);
        }

        inline static std::size_t ContentNum_Item()          { return 0; }
        inline static std::size_t ContentNum_Coins()         { return 1; }
        inline static std::size_t ContentNum_Gems()          { return 2; }
        inline static std::size_t ContentNum_MeteorShards()  { return 3; }

        bool IsNumberValid(const std::size_t) const;

        const std::string ToStringShort(const bool WILL_WRAP = true) const;
        const std::string ToStringFull(const bool WILL_WRAP = true) const;

        inline void SetBoxInfo(const sfml_util::gui::box::Info & NEW_BOX_INFO)
        {
            boxInfo_ = NEW_BOX_INFO;
        }

        inline void SetDoesIncludeItems(const bool WILL_INCLUDE_ITEMS)
        {
            willIncludeItems_ = WILL_INCLUDE_ITEMS;
        }

    public:
        static const float IMAGE_FADE_SPEED_DEFAULT_;

    private:
        const std::string ToStringCommon(const bool WILL_WRAP, const bool WILL_SHORTEN) const;
        //
        std::string                       name_;
        sfml_util::gui::TextInfo          textInfo_;
        PopupButtons::Enum                buttons_;
        PopupImage::Enum                  image_;
        sfml_util::sound_effect::Enum     soundEffect_;
        sfml_util::gui::box::Info         boxInfo_;
        float                             ratioX_;
        float                             ratioY_;
        PopupButtonColor::Enum buttonColor_;
        bool                              willAddRandImage_;
        float                             imageScale_;
        sfml_util::TextureVec_t           textureVec_;
        std::size_t                       numberMin_;
        std::size_t                       numberMax_;
        std::vector<std::size_t>          numberInvalidVec_;
        float                             imageFadeSpeed_;
        game::creature::CreaturePtr_t     creatureCPtr_;
        std::size_t                       initialSelection_;
        bool                              areImgsCreatures_;
        std::vector<std::string>          textVec_;
        game::combat::CombatEnd::Enum     howCombatEnded_;
        game::creature::TitlePtr_t        titleFromPtr_;
        game::creature::TitlePtr_t        titleToPtr_;
        std::string                       titleText_;
        std::string                       descText_;
        bool                              willIncludeItems_;
    };


}
#endif //POPUP_POPUPINFO_HPP_INCLUDED
