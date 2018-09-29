// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-info.cpp
//
#include "popup-info.hpp"

#include "creature/creature.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/vectors.hpp"

#include <sstream>

namespace heroespath
{
namespace popup
{

    const float PopupInfo::IMAGE_FADE_SPEED_DEFAULT_(-1.0f); // any negative value will work here

    PopupInfo::PopupInfo(
        const std::string & NAME,
        const sfml_util::TextInfo & TEXT_INFO,
        const PopupButtons::Enum BUTTONS,
        const PopupImage::Enum IMAGE,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const PopupButtonColor::Enum BUTTON_COLOR,
        const bool WILL_ADD_RAND_IMAGE,
        const std::vector<std::size_t> & INVALID_CHAR_NUM_VEC,
        const sfml_util::CachedTextureVec_t & TEXTURE_VEC,
        const std::vector<std::string> & TEXT_VEC,
        const float IMAGE_FADE_SPEED,
        const creature::CreaturePtrOpt_t & CREATURE_PTR_OPT,
        const std::size_t INITIAL_SELECTION,
        const std::string & TITLE_TEXT,
        const std::string & DESC_TEXT,
        const float KEEP_ALIVE_SECONDS)
        : name_(NAME)
        , textInfo_(TEXT_INFO)
        , buttons_(BUTTONS)
        , image_(IMAGE)
        , soundEffect_(SOUND_EFFECT)
        , ratioX_(1.0f)
        , ratioY_(1.0f)
        , buttonColor_(BUTTON_COLOR)
        , willAddRandImage_(WILL_ADD_RAND_IMAGE)
        , textures_(TEXTURE_VEC)
        , numberMin_(0)
        , numberMax_(0)
        , numberInvalidVec_(INVALID_CHAR_NUM_VEC)
        , imageFadeSpeed_(IMAGE_FADE_SPEED)
        , creaturePtrOpt_(CREATURE_PTR_OPT)
        , initialSelection_(INITIAL_SELECTION)
        , textVec_(TEXT_VEC)
        , howCombatEnded_(combat::CombatEnd::Count)
        , titleText_(TITLE_TEXT)
        , descText_(DESC_TEXT)
        , willIncludeItems_(false)
        , keepAliveSeconds_(KEEP_ALIVE_SECONDS)
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (TEXT_INFO.text.empty() == false),
            "popup::PopupInfo(name=\"" << name_ << "\", buttons=" << PopupButtons::ToString(BUTTONS)
                                       << ", image=" << PopupImage::ToString(IMAGE)
                                       << ", textInfo=\"" << TEXT_INFO.text
                                       << "\") was given TEXT_INFO.text that was empty.");

        M_ASSERT_OR_LOGANDTHROW_SS(
            (PopupImage::IsValid(IMAGE)),
            "popup::PopupInfo(name=\""
                << name_ << "\", buttons=" << PopupButtons::ToString(BUTTONS)
                << ", image=" << PopupImage::ToString(IMAGE) << ", textInfo=\"" << TEXT_INFO.text
                << "\") was given an IMAGE value of " << IMAGE << ", which is invalid.");

        if ((imageFadeSpeed_ > 0.0f) && (textures_.empty()))
        {
            std::ostringstream ss;
            ss << "popup::PopupInfo(name=" << name_
               << "\", buttons=" << PopupButtons::ToString(BUTTONS)
               << ", image=" << PopupImage::ToString(IMAGE) << ", textInfo=\"" << TEXT_INFO.text
               << "\") was given an image fade speed of " << IMAGE_FADE_SPEED
               << " but the TEXTURE_VEC was empty, which makes no sense.  If there is a fade "
               << "speed then there must be images to fade-in.";

            throw std::runtime_error(ss.str());
        }

        if (IMAGE == PopupImage::Banner)
        {
            willAddRandImage_ = false;
        }
    }

    PopupInfo::PopupInfo(
        const std::string & NAME,
        const sfml_util::TextInfo & TEXT_INFO,
        const sfml_util::CachedTextureVec_t & TEXTURE_VEC,
        const std::size_t INITIAL_SELECTION,
        const sfml_util::sound_effect::Enum SOUND_EFFECT,
        const PopupButtonColor::Enum BUTTON_COLOR)
        : name_(NAME)
        , textInfo_(TEXT_INFO)
        , buttons_(PopupButtons::SelectCancel)
        , image_(PopupImage::Large)
        , soundEffect_(SOUND_EFFECT)
        , ratioX_(1.0f)
        , ratioY_(1.0f)
        , buttonColor_(BUTTON_COLOR)
        , willAddRandImage_(true)
        , textures_(TEXTURE_VEC)
        , numberMin_(0)
        , numberMax_(0)
        , numberInvalidVec_()
        , imageFadeSpeed_(IMAGE_FADE_SPEED_DEFAULT_)
        , creaturePtrOpt_()
        , initialSelection_(INITIAL_SELECTION)
        , textVec_()
        , howCombatEnded_(combat::CombatEnd::Count)
        , titleText_("")
        , descText_("")
        , willIncludeItems_(false)
        , keepAliveSeconds_(-1.0f) // any negative will work here
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            ((INITIAL_SELECTION < textures_.size()) || textures_.empty()),
            "popup::PopupInfo::Constructor(image selection) INITIAL_SELECTION="
                << INITIAL_SELECTION << " is NOT less than the textures_vec.size()"
                << textures_.size());
    }

    PopupInfo::PopupInfo(
        const std::string & NAME,
        const sfml_util::TextInfo & TEXT_INFO,
        const std::size_t THE_MIN,
        const std::size_t THE_MAX)
        : name_(NAME)
        , textInfo_(TEXT_INFO)
        , buttons_(PopupButtons::SelectCancel)
        , image_(PopupImage::Large)
        , soundEffect_(sfml_util::sound_effect::PromptQuestion)
        , ratioX_(1.0f)
        , ratioY_(1.0f)
        , buttonColor_(PopupButtonColor::Dark)
        , willAddRandImage_(true)
        , textures_()
        , numberMin_(THE_MIN)
        , numberMax_(THE_MAX)
        , numberInvalidVec_()
        , imageFadeSpeed_(IMAGE_FADE_SPEED_DEFAULT_)
        , creaturePtrOpt_()
        , initialSelection_(0)
        , textVec_()
        , howCombatEnded_(combat::CombatEnd::Count)
        , titleText_("")
        , descText_("")
        , willIncludeItems_(false)
        , keepAliveSeconds_(-1.0f) // any negative will work here
    {}

    PopupInfo::PopupInfo(
        const std::string & NAME,
        const sfml_util::TextInfo & TEXT_INFO,
        const PopupButtons::Enum BUTTONS,
        const combat::CombatEnd::Enum HOW_COMBAT_ENDED)
        : name_(NAME)
        , textInfo_(TEXT_INFO)
        , buttons_(BUTTONS)
        , image_(PopupImage::Large)
        , soundEffect_(sfml_util::sound_effect::None)
        , ratioX_(1.0f)
        , ratioY_(1.0f)
        , buttonColor_(PopupButtonColor::Dark)
        , willAddRandImage_(false)
        , textures_()
        , numberMin_(0)
        , numberMax_(0)
        , numberInvalidVec_()
        , imageFadeSpeed_(IMAGE_FADE_SPEED_DEFAULT_)
        , creaturePtrOpt_()
        , initialSelection_(0)
        , textVec_()
        , howCombatEnded_(HOW_COMBAT_ENDED)
        , titleText_("")
        , descText_("")
        , willIncludeItems_(false)
        , keepAliveSeconds_(-1.0f) // any negative will work here
    {}

    const std::string PopupInfo::ToStringShort(const bool WILL_WRAP) const
    {
        return ToStringCommon(WILL_WRAP, true);
    }

    const std::string PopupInfo::ToStringFull(const bool WILL_WRAP) const
    {
        return ToStringCommon(WILL_WRAP, false);
    }

    const std::string PopupInfo::ToStringCommon(const bool WILL_WRAP, const bool WILL_SHORTEN) const
    {
        std::ostringstream ss;

        if (WILL_WRAP)
        {
            ss << "(";
        }

        ss << "\"" << name_ << "\", " << PopupButtons::ToString(buttons_) << ", ";

        if (WILL_SHORTEN)
        {
            ss << PopupImage::ToString(image_);
        }
        else
        {
            ss << ", image=" << PopupImage::ToString(image_);
            ss << ", button_color=" << PopupButtonColor::ToString(buttonColor_);
        }

        ss << ", accent_image=" << std::boolalpha << willAddRandImage_;

        ss << ", sound_effect=" << sfml_util::sound_effect::ToString(soundEffect_);

        if (numberMin_ != numberMax_)
        {
            ss << ", number_select=[" << numberMin_ << "," << numberMax_ << "]";
        }

        if (textures_.size() > 0)
        {
            ss << ", images_count=" << textures_.size();
        }

        if (imageFadeSpeed_ > 0.0f)
        {
            ss << ", fade_speed=" << imageFadeSpeed_;
        }

        if (creaturePtrOpt_)
        {
            ss << ", creature=" << creaturePtrOpt_.value()->Name();
        }

        if (initialSelection_ != 0)
        {
            ss << ", initial_selection=" << initialSelection_;
        }

        if ((textInfo_.text.size() > 20) && (WILL_SHORTEN))
        {
            ss << ", \"" << std::string(textInfo_.text.substr(0, 19).append("...")) << "\"";
        }
        else
        {
            ss << ", \"" << textInfo_.text << "\"";
        }

        if (numberInvalidVec_.empty() == false)
        {
            ss << ", invalid_selections=" << misc::Vector::Join(numberInvalidVec_);
        }

        if (creaturePtrOpt_)
        {
            ss << ", creature=" << creaturePtrOpt_.value()->NameAndRaceAndRole();
        }

        if (howCombatEnded_ != combat::CombatEnd::Count)
        {
            ss << ", combat_ended=" << combat::CombatEnd::ToString(howCombatEnded_);
        }

        if (titleText_.empty() == false)
        {
            ss << ", title_text=\"" << titleText_ << "\"";
        }

        if (descText_.empty() == false)
        {
            ss << ", desc_text=\"" << descText_ << "\"";
        }

        if (WILL_WRAP)
        {
            ss << ")";
        }

        return ss.str();
    }

    bool PopupInfo::IsNumberValid(const std::size_t NUM) const
    {
        if (numberInvalidVec_.empty())
        {
            return true;
        }
        else
        {
            return (
                std::find(numberInvalidVec_.begin(), numberInvalidVec_.end(), NUM)
                == numberInvalidVec_.end());
        }
    }

} // namespace popup
} // namespace heroespath
