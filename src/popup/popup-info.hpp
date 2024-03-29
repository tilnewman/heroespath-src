// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPINFO_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPINFO_HPP_INCLUDED
//
// popup-info.hpp
//
#include "combat/combat-over-enum.hpp"
#include "creature/title.hpp"
#include "gui/cached-texture.hpp"
#include "gui/sound-effects-enum.hpp"
#include "gui/text-info.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-enums.hpp"

#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
} // namespace creature

namespace popup
{
    // encapsulates all the info about a popup window except for screen position
    class PopupInfo
    {
    public:
        // default constructor for most popups
        PopupInfo(
            const PopupStage::Enum STAGE,
            const std::string & NAME,
            const gui::TextInfo & TEXT_INFO,
            const PopupButtons::Enum BUTTONS = PopupButtons::Okay,
            const PopupImage::Enum IMAGE = PopupImage::Banner,
            const gui::sound_effect::Enum SOUND_EFFECT = gui::sound_effect::PromptGeneric,
            const PopupButtonColor::Enum BUTTON_COLOR = PopupButtonColor::Dark,
            const bool WILL_ADD_RAND_IMAGE = true,
            const std::vector<std::size_t> & INVALID_NUM_VEC = std::vector<std::size_t>(),
            const gui::CachedTextureVec_t & TEXTURE_VEC = gui::CachedTextureVec_t(),
            const std::vector<std::string> & TEXT_VEC = std::vector<std::string>(),
            const float IMAGE_FADE_SPEED = IMAGE_FADE_SPEED_DEFAULT_,
            const creature::CreaturePtrOpt_t & CREATURE_PTR_OPT = boost::none,
            const std::size_t INITIAL_SELECTION = 0,
            const std::string & TITLE_TEXT = "",
            const std::string & DESC_TEXT = "",
            const float KEEP_ALIVE_SECONDS = -1.0f); // any negative works

        // use this constructor for image selection popups
        PopupInfo(
            const std::string & NAME,
            const gui::TextInfo & TEXT_INFO,
            const gui::CachedTextureVec_t & TEXTURE_VEC,
            const std::size_t INITIAL_SELECTION,
            const gui::sound_effect::Enum SOUND_EFFECT = gui::sound_effect::PromptGeneric,
            const PopupButtonColor::Enum BUTTON_COLOR = PopupButtonColor::Dark);

        // use this constructor for number selection popups
        PopupInfo(
            const std::string & NAME,
            const gui::TextInfo & TEXT_INFO,
            const std::size_t THE_MIN,
            const std::size_t THE_MAX);

        // use this constructor for end-of-combat popups
        PopupInfo(
            const std::string & NAME,
            const gui::TextInfo & TEXT_INFO,
            const PopupButtons::Enum BUTTONS,
            const combat::CombatEnd::Enum HOW_COMBAT_ENDED);

        PopupInfo(const PopupInfo &) = default;
        PopupInfo(PopupInfo &&) = default;
        PopupInfo & operator=(const PopupInfo &) = default;
        PopupInfo & operator=(PopupInfo &&) = default;

        PopupStage::Enum Stage() const { return stage_; }
        const std::string Name() const { return name_; }
        PopupButtons::Enum Buttons() const { return buttons_; }
        PopupImage::Enum Image() const { return image_; }
        const gui::TextInfo TextInfo() const { return textInfo_; }
        gui::sound_effect::Enum SoundEffect() const { return soundEffect_; }
        float SizeX() const { return ratioX_; }
        float SizeY() const { return ratioY_; }
        PopupButtonColor::Enum ButtonColor() const { return buttonColor_; }
        bool WillAddRandImage() const { return willAddRandImage_; }
        std::size_t ImagesCount() const { return textures_.size(); }
        std::size_t NumberSelMin() const { return numberMin_; }
        std::size_t NumberSelMax() const { return numberMax_; }
        const std::vector<std::size_t> NumberSelInvVec() const { return numberInvalidVec_; }
        float ImageFadeSpeed() const { return imageFadeSpeed_; }
        const creature::CreaturePtrOpt_t CreaturePtrOpt() const { return creaturePtrOpt_; }
        std::size_t InitialSelection() const { return initialSelection_; }
        const std::vector<std::string> & TextVec() const { return textVec_; }
        combat::CombatEnd::Enum HowCombatEnded() const { return howCombatEnded_; }
        const std::string TitleText() const { return titleText_; }
        const std::string DescText() const { return descText_; }
        bool WillIncludeItems() const { return willIncludeItems_; }
        float KeepAliveSec() const { return keepAliveSeconds_; }

        const gui::CachedTexture & ImagesAt(const std::size_t INDEX) const
        {
            return textures_.at(INDEX);
        }

        static std::size_t ContentNum_Item() { return 0; }
        static std::size_t ContentNum_Coins() { return 1; }
        static std::size_t ContentNum_Gems() { return 2; }
        static std::size_t ContentNum_MeteorShards() { return 3; }

        bool IsNumberValid(const std::size_t) const;

        const std::string ToStringShort(const bool WILL_WRAP = true) const;
        const std::string ToStringFull(const bool WILL_WRAP = true) const;

        void SetDoesIncludeItems(const bool WILL_INCLUDE_ITEMS)
        {
            willIncludeItems_ = WILL_INCLUDE_ITEMS;
        }

        void TextVec(const std::vector<std::string> & NEW_TEXT_VEC) { textVec_ = NEW_TEXT_VEC; }

        bool IsAsCloseAsIsReasonableToCheck(const PopupInfo & OTHER) const
        {
            return (
                std::tie(
                    stage_,
                    name_,
                    textInfo_,
                    titleText_,
                    descText_,
                    buttons_,
                    image_,
                    buttonColor_,
                    numberMin_,
                    numberMax_,
                    howCombatEnded_,
                    creaturePtrOpt_)
                == std::tie(
                    OTHER.stage_,
                    OTHER.name_,
                    OTHER.textInfo_,
                    OTHER.titleText_,
                    OTHER.descText_,
                    OTHER.buttons_,
                    OTHER.image_,
                    OTHER.buttonColor_,
                    OTHER.numberMin_,
                    OTHER.numberMax_,
                    OTHER.howCombatEnded_,
                    OTHER.creaturePtrOpt_));
        }

    public:
        static const float IMAGE_FADE_SPEED_DEFAULT_;

    private:
        const std::string ToStringCommon(const bool WILL_WRAP, const bool WILL_SHORTEN) const;
        //
        PopupStage::Enum stage_;
        std::string name_;
        gui::TextInfo textInfo_;
        PopupButtons::Enum buttons_;
        PopupImage::Enum image_;
        gui::sound_effect::Enum soundEffect_;
        float ratioX_;
        float ratioY_;
        PopupButtonColor::Enum buttonColor_;
        bool willAddRandImage_;
        gui::CachedTextureVec_t textures_;
        std::size_t numberMin_;
        std::size_t numberMax_;
        std::vector<std::size_t> numberInvalidVec_;
        float imageFadeSpeed_;
        creature::CreaturePtrOpt_t creaturePtrOpt_;
        std::size_t initialSelection_;
        std::vector<std::string> textVec_;
        combat::CombatEnd::Enum howCombatEnded_;
        std::string titleText_;
        std::string descText_;
        bool willIncludeItems_;
        float keepAliveSeconds_;
    };

    using PopupInfoOpt_t = boost::optional<PopupInfo>;

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPINFO_HPP_INCLUDED
