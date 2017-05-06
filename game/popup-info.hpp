#ifndef GAME_POPUPINFO_INCLUDED
#define GAME_POPUPINFO_INCLUDED
//
// popup-info.hpp
//  A class the encapsulates everything about a popup
//  window except for drawing details such as position etc.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/popup-enums.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/box.hpp"

#include "game/which-popup-enum.hpp"

#include <string>


namespace game
{

namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
}

    //encapsulates all the info about a popup window except for screen position
    class PopupInfo
    {
    public:
        //default constructor for most popups
        PopupInfo(const std::string &                     NAME,
                  const sfml_util::gui::TextInfo &        TEXT_INFO,
                  const sfml_util::PopupButtons::Enum     BUTTONS              = sfml_util::PopupButtons::Okay,
                  const sfml_util::PopupImage::Enum       IMAGE                = sfml_util::PopupImage::Banner,
                  const float                             IMAGE_SCALE          = 1.0f,
                  const game::Popup::Enum                 TYPE                 = game::Popup::Generic,
                  const sfml_util::sound_effect::Enum     SOUND_EFFECT         = sfml_util::sound_effect::PromptGeneric,
                  const sfml_util::PopupButtonColor::Enum BUTTON_COLOR         = sfml_util::PopupButtonColor::Dark,
                  const bool                              WILL_ADD_RAND_IMAGE  = true,
                  const std::vector<std::size_t> &        INVALID_NUM_VEC      = std::vector<std::size_t>(),
                  const sfml_util::TextureSVec_t &        IMAGES_VEC           = sfml_util::TextureSVec_t(),
                  const float                             IMAGE_FADE_SPEED     = IMAGE_FADE_SPEED_DEFAULT_,
                  const creature::CreaturePtr_t           CREATURE_CPTR        = nullptr,
                  const std::size_t                       INITIAL_SELECTION    = 0);

        //use this constructor for boxed popups with no background image
        PopupInfo(const std::string &                     NAME,
                  const sfml_util::gui::TextInfo &        TEXT_INFO,
                  const sfml_util::PopupButtons::Enum     BUTTONS,
                  const sfml_util::gui::box::Info &       BOX_INFO,
                  const float                             MAX_SIZE_RATIO_X    = 0.25f,
                  const float                             MAX_SIZE_RATIO_Y    = 0.5f,
                  const game::Popup::Enum                 TYPE                = game::Popup::Generic,
                  const sfml_util::sound_effect::Enum     SOUND_EFFECT        = sfml_util::sound_effect::PromptGeneric,
                  const sfml_util::PopupButtonColor::Enum BUTTON_COLOR        = sfml_util::PopupButtonColor::Light,
                  const bool                              WILL_ADD_RAND_IMAGE = true);

        //use this constructor for image selection popups
        PopupInfo(const std::string &                     NAME,
                  const sfml_util::gui::TextInfo &        TEXT_INFO,
                  const sfml_util::TextureSVec_t &        IMAGES_SVEC,
                  const float                             IMAGE_SCALE,
                  const sfml_util::sound_effect::Enum     SOUND_EFFECT = sfml_util::sound_effect::PromptGeneric,
                  const sfml_util::PopupButtonColor::Enum BUTTON_COLOR = sfml_util::PopupButtonColor::Dark);

        //use this constructor for number selection popups
        PopupInfo(const std::string &              NAME,
                  const sfml_util::gui::TextInfo & TEXT_INFO,
                  const std::size_t                THE_MIN,
                  const std::size_t                THE_MAX,
                  const float                      IMAGE_SCALE);

        virtual ~PopupInfo();

        inline const std::string                 Name() const               { return name_; }
        inline sfml_util::PopupButtons::Enum     Buttons() const            { return buttons_; }
        inline sfml_util::PopupImage::Enum       Image() const              { return image_; }
        inline const sfml_util::gui::TextInfo    TextInfo() const           { return textInfo_; }
        inline game::Popup::Enum                 Type() const               { return type_; }
        inline sfml_util::sound_effect::Enum     SoundEffect() const        { return soundEffect_; }
        inline const sfml_util::gui::box::Info   BoxInfo() const            { return boxInfo_; }
        inline float                             SizeX() const              { return ratioX_; }
        inline float                             SizeY() const              { return ratioY_; }
        inline sfml_util::PopupButtonColor::Enum ButtonColor() const        { return buttonColor_; }
        inline bool                              WillAddRandImage() const   { return willAddRandImage_; }
        inline float                             ImageScale() const         { return imageScale_; }
        inline const sfml_util::TextureSVec_t    Images() const             { return textureSVec_; }
        inline std::size_t                       NumberSelMin() const       { return numberMin_; }
        inline std::size_t                       NumberSelMax() const       { return numberMax_; }
        inline const std::vector<std::size_t>    NumberSelInvVec() const    { return numberInvalidVec_; }
        inline float                             ImageFadeSpeed() const     { return imageFadeSpeed_; }
        inline creature::CreaturePtr_t           CreaturePtr() const        { return creatureCPtr_; }
        inline std::size_t                       InitialSelection() const   { return initialSelection_; }

        inline static std::size_t ContentNum_Item()          { return 0; }
        inline static std::size_t ContentNum_Coins()         { return 1; }
        inline static std::size_t ContentNum_Gems()          { return 2; }
        inline static std::size_t ContentNum_MeteorShards()  { return 3; }

        bool IsNumberValid(const std::size_t) const;

        friend bool operator==(const PopupInfo & L, const PopupInfo & R);

        const std::string ToStringShort(const bool WILL_WRAP = true) const;
        const std::string ToStringFull(const bool WILL_WRAP = true) const;

    public:
        static const float IMAGE_FADE_SPEED_DEFAULT_;

    private:
        const std::string ToStringCommon(const bool WILL_WRAP, const bool WILL_SHORTEN) const;
        //
        std::string                       name_;
        sfml_util::gui::TextInfo          textInfo_;
        sfml_util::PopupButtons::Enum     buttons_;
        sfml_util::PopupImage::Enum       image_;
        game::Popup::Enum                 type_;
        sfml_util::sound_effect::Enum     soundEffect_;
        sfml_util::gui::box::Info         boxInfo_;
        float                             ratioX_;
        float                             ratioY_;
        sfml_util::PopupButtonColor::Enum buttonColor_;
        bool                              willAddRandImage_;
        float                             imageScale_;
        sfml_util::TextureSVec_t          textureSVec_;
        std::size_t                       numberMin_;
        std::size_t                       numberMax_;
        std::vector<std::size_t>          numberInvalidVec_;
        float                             imageFadeSpeed_;
        creature::CreaturePtr_t           creatureCPtr_;
        std::size_t                       initialSelection_;
    };


    bool operator==(const PopupInfo & L, const PopupInfo & R);

    inline bool operator!=(const PopupInfo & L, const PopupInfo & R)
    {
        return ! (L == R);
    }

}
#endif //GAME_POPUPINFO_INCLUDED
