//
// popup-info.cpp
//
#include "popup-info.hpp"

#include "sfml-util/sfml-util.hpp"

#include "utilz/assertlogandthrow.hpp"
#include "game/i-popup-callback.hpp"

#include <sstream>


namespace game
{

    const float PopupInfo::IMAGE_FADE_SPEED_DEFAULT_(-1.0f);//any negative value will work here


    PopupInfo::PopupInfo(const std::string &                     NAME,
                         const sfml_util::gui::TextInfo &        TEXT_INFO,
                         const sfml_util::PopupButtons::Enum     BUTTONS,
                         const sfml_util::PopupImage::Enum       IMAGE,
                         const float                             IMAGE_SCALE,
                         const game::Popup::Enum           TYPE,
                         const sfml_util::sound_effect::Enum     SOUND_EFFECT,
                         const sfml_util::PopupButtonColor::Enum BUTTON_COLOR,
                         const bool                              WILL_ADD_RAND_IMAGE,
                         const std::vector<std::size_t> &             INVALID_CHAR_NUM_VEC,
                         const sfml_util::TextureSVec_t &        IMAGES_VEC,
                         const float                             IMAGE_FADE_SPEED)
    :
        name_            (NAME),
        textInfo_        (TEXT_INFO),
        buttons_         (BUTTONS),
        image_           (IMAGE),
        type_            (TYPE),
        soundEffect_     (SOUND_EFFECT),
        boxInfo_         (),
        ratioX_          (1.0f),
        ratioY_          (1.0f),
        buttonColor_     (BUTTON_COLOR),
        willAddRandImage_(WILL_ADD_RAND_IMAGE),
        imageScale_      (IMAGE_SCALE),
        textureSVec_     (IMAGES_VEC),
        numberMin_       (0),
        numberMax_       (0),
        numberInvalidVec_(INVALID_CHAR_NUM_VEC),
        imageFadeSpeed_  (IMAGE_FADE_SPEED)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((sfml_util::PopupButtons::IsValid(BUTTONS)), "game::PopupInfo(type=" << game::Popup::ToString(TYPE) << ", buttons=" << sfml_util::PopupButtons::ToString(BUTTONS) << ", image=" << sfml_util::PopupImage::ToString(IMAGE) << ", textInfo=\"" << TEXT_INFO.text << "\") was given a BUTTONS value of " << BUTTONS << ", which is invalid.");
        M_ASSERT_OR_LOGANDTHROW_SS((false == TEXT_INFO.text.empty()),           "game::PopupInfo(type=" << game::Popup::ToString(TYPE) << ", buttons=" << sfml_util::PopupButtons::ToString(BUTTONS) << ", image=" << sfml_util::PopupImage::ToString(IMAGE) << ", textInfo=\"" << TEXT_INFO.text << "\") was given TEXT_INFO.text that was empty.");
        M_ASSERT_OR_LOGANDTHROW_SS((sfml_util::PopupImage::IsValid(IMAGE)),     "game::PopupInfo(type=" << game::Popup::ToString(TYPE) << ", buttons=" << sfml_util::PopupButtons::ToString(BUTTONS) << ", image=" << sfml_util::PopupImage::ToString(IMAGE) << ", textInfo=\"" << TEXT_INFO.text << "\") was given an IMAGE value of " << IMAGE << ", which is invalid.");

        if ((imageFadeSpeed_ > 0.0f) && (textureSVec_.empty()))
        {
            std::ostringstream ss;
            ss << "game::PopupInfo(type=" << game::Popup::ToString(TYPE) << ", buttons=" << sfml_util::PopupButtons::ToString(BUTTONS) << ", image=" << sfml_util::PopupImage::ToString(IMAGE) << ", textInfo=\"" << TEXT_INFO.text << "\") was given an image fade speed of " << IMAGE_FADE_SPEED << " but there were no images in IMAGES_VEC, which makes no sense.  If there is a fade speed then there must be images to fade-in.";
            throw std::runtime_error(ss.str());
        }

        if (IMAGE == sfml_util::PopupImage::Banner)
            willAddRandImage_ = false;
    }


    PopupInfo::PopupInfo(const std::string &                     NAME,
                         const sfml_util::gui::TextInfo &        TEXT_INFO,
                         const sfml_util::PopupButtons::Enum     BUTTONS,
                         const sfml_util::gui::box::Info &       BOX_INFO,
                         const float                             MAX_SIZE_RATIO_X,
                         const float                             MAX_SIZE_RATIO_Y,
                         const game::Popup::Enum           TYPE,
                         const sfml_util::sound_effect::Enum     SOUND_EFFECT,
                         const sfml_util::PopupButtonColor::Enum BUTTON_COLOR,
                         const bool                              WILL_ADD_RAND_IMAGE)
    :
        name_            (NAME),
        textInfo_        (TEXT_INFO),
        buttons_         (BUTTONS),
        image_           (sfml_util::PopupImage::Custom),
        type_            (TYPE),
        soundEffect_     (SOUND_EFFECT),
        boxInfo_         (BOX_INFO),
        ratioX_          (MAX_SIZE_RATIO_X),
        ratioY_          (MAX_SIZE_RATIO_Y),
        buttonColor_     (BUTTON_COLOR),
        willAddRandImage_(WILL_ADD_RAND_IMAGE),
        imageScale_      (1.0f),
        textureSVec_     (),
        numberMin_       (0),
        numberMax_       (0),
        numberInvalidVec_(),
        imageFadeSpeed_  (IMAGE_FADE_SPEED_DEFAULT_)
    {}


    PopupInfo::PopupInfo(const std::string &                     NAME,
                         const sfml_util::gui::TextInfo &        TEXT_INFO,
                         const sfml_util::TextureSVec_t &        IMAGES_SVEC,
                         const float                             IMAGE_SCALE,
                         const sfml_util::sound_effect::Enum     SOUND_EFFECT,
                         const sfml_util::PopupButtonColor::Enum BUTTON_COLOR)
    :
        name_            (NAME),
        textInfo_        (TEXT_INFO),
        buttons_         (sfml_util::PopupButtons::SelectCancel),
        image_           (sfml_util::PopupImage::Large),
        type_            (game::Popup::ImageSelection),
        soundEffect_     (SOUND_EFFECT),
        boxInfo_         (),
        ratioX_          (1.0f),
        ratioY_          (1.0f),
        buttonColor_     (BUTTON_COLOR),
        willAddRandImage_(false),
        imageScale_      (IMAGE_SCALE),
        textureSVec_     (IMAGES_SVEC),
        numberMin_       (0),
        numberMax_       (0),
        numberInvalidVec_(),
        imageFadeSpeed_  (IMAGE_FADE_SPEED_DEFAULT_)
    {}


    PopupInfo::PopupInfo(const std::string &              NAME,
                         const sfml_util::gui::TextInfo & TEXT_INFO,
                         const std::size_t                THE_MIN,
                         const std::size_t                THE_MAX,
                         const float                      IMAGE_SCALE)
    :
        name_            (NAME),
        textInfo_        (TEXT_INFO),
        buttons_         (sfml_util::PopupButtons::SelectCancel),
        image_           (sfml_util::PopupImage::Large),
        type_            (game::Popup::NumberSelection),
        soundEffect_     (sfml_util::sound_effect::PromptQuestion),
        boxInfo_         (),
        ratioX_          (1.0f),
        ratioY_          (1.0f),
        buttonColor_     (sfml_util::PopupButtonColor::Dark),
        willAddRandImage_(true),
        imageScale_      (IMAGE_SCALE),
        textureSVec_     (),
        numberMin_       (THE_MIN),
        numberMax_       (THE_MAX),
        numberInvalidVec_(),
        imageFadeSpeed_  (IMAGE_FADE_SPEED_DEFAULT_)
    {}


    PopupInfo::~PopupInfo()
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
            ss << "(";

        ss << "\"" << name_ << "\", "
            << Popup::ToString(type_) << ", "
            << sfml_util::PopupButtons::ToString(buttons_) << ", ";

        if (WILL_SHORTEN)
        {
            ss << sfml_util::PopupImage::ToString(image_);
        }
        else
        {
            ss << ", image=" << sfml_util::PopupImage::ToString(image_);
            ss << ", image_scale=" << imageScale_;
            ss << ", button_color=" << sfml_util::PopupButtonColor::ToString(buttonColor_);

            if (image_ == sfml_util::PopupImage::Custom)
            {
                if (false == boxInfo_.bg_info.path.empty())
                    ss << ", bg_image=\"" << boxInfo_.bg_info.path << "\"";
                else
                    if (boxInfo_.bg_info.textureSPtr.get() == nullptr)
                        ss << ", color=" << sfml_util::ColorToString(boxInfo_.bg_info.color);
            }
        }

        ss << ", accent_image=" << std::boolalpha << willAddRandImage_;

        ss << ", sound_effect=" << sfml_util::sound_effect::ToString(soundEffect_);

        if (numberMin_ != numberMax_)
            ss << ", number_select=[" << numberMin_ << "," << numberMax_ << "]";

        if (textureSVec_.size() > 0)
            ss << ", images_count=" << textureSVec_.size();

        if (imageFadeSpeed_ > 0.0f)
            ss << ", fade_speed=" << imageFadeSpeed_;

        if ((textInfo_.text.size() > 20) && (WILL_SHORTEN))
            ss << ", \"" << std::string(textInfo_.text.substr(0, 19).append("...")) << "\"";
        else
            ss << ", \"" << textInfo_.text << "\"";

        if (WILL_WRAP)
            ss << ")";

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
            return (std::find(numberInvalidVec_.begin(), numberInvalidVec_.end(), NUM) == numberInvalidVec_.end());
        }
    }


    bool operator==(const PopupInfo & L, const PopupInfo & R)
    {
        return ((L.name_ == R.name_) &&
                (L.image_ == R.image_) &&
                (L.buttons_ == R.buttons_) &&
                (L.textInfo_ == R.textInfo_) &&
                (L.type_ == R.type_) &&
                (L.soundEffect_ == R.soundEffect_) &&
                (L.boxInfo_ == R.boxInfo_) &&
                (sfml_util::IsRealClose(L.ratioX_, R.ratioX_)) &&
                (sfml_util::IsRealClose(L.ratioY_, R.ratioY_)) &&
                (L.willAddRandImage_ == R.willAddRandImage_) &&
                (sfml_util::IsRealClose(L.imageScale_, R.imageScale_)) &&
                (L.textureSVec_ == R.textureSVec_) &&
                (L.numberMin_ == R.numberMin_) &&
                (L.numberMax_ == R.numberMax_) &&
                (L.numberInvalidVec_ == R.numberInvalidVec_) &&
                (sfml_util::IsRealClose(L.imageFadeSpeed_, R.imageFadeSpeed_)));
    }

}
