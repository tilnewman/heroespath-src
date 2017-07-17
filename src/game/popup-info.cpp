// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// popup-info.cpp
//
#include "popup-info.hpp"

#include "sfml-util/sfml-util.hpp"

#include "misc/assertlogandthrow.hpp"

#include "game/i-popup-callback.hpp"
#include "game/creature/creature.hpp"

#include <sstream>
#include <vector>


namespace game
{

    const float PopupInfo::IMAGE_FADE_SPEED_DEFAULT_(-1.0f);//any negative value will work here


    PopupInfo::PopupInfo(const std::string &                     NAME,
                         const sfml_util::gui::TextInfo &        TEXT_INFO,
                         const sfml_util::PopupButtons::Enum     BUTTONS,
                         const sfml_util::PopupImage::Enum       IMAGE,
                         const float                             IMAGE_SCALE,
                         const game::Popup::Enum                 TYPE,
                         const sfml_util::sound_effect::Enum     SOUND_EFFECT,
                         const sfml_util::PopupButtonColor::Enum BUTTON_COLOR,
                         const bool                              WILL_ADD_RAND_IMAGE,
                         const std::vector<std::size_t> &        INVALID_CHAR_NUM_VEC,
                         const sfml_util::TextureVec_t &         TEXTURE_VEC,
                         const std::vector<std::string> &        TEXT_VEC,
                         const float                             IMAGE_FADE_SPEED,
                         const creature::CreaturePtr_t           CREATURE_CPTR,
                         const std::size_t                       INITIAL_SELECTION,
                         const bool                              ARE_IMAGES_CREATURES,
                         const std::string &                     TITLE_TEXT,
                         const std::string &                     DESC_TEXT,
                         const creature::TitlePtr_t              FROM_TITLE_PTR,
                         const creature::TitlePtr_t              TO_TITLE_PTR)
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
        textureVec_      (TEXTURE_VEC),
        numberMin_       (0),
        numberMax_       (0),
        numberInvalidVec_(INVALID_CHAR_NUM_VEC),
        imageFadeSpeed_  (IMAGE_FADE_SPEED),
        creatureCPtr_    (CREATURE_CPTR),
        initialSelection_(INITIAL_SELECTION),
        areImgsCreatures_(ARE_IMAGES_CREATURES),
        textVec_         (TEXT_VEC),
        howCombatEnded_  (combat::CombatEnd::Count),
        titleFromPtr_    (FROM_TITLE_PTR),
        titleToPtr_      (TO_TITLE_PTR),
        titleText_       (TITLE_TEXT),
        descText_        (DESC_TEXT)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((TEXT_INFO.text.empty() == false),
            "game::PopupInfo(type=" << game::Popup::ToString(TYPE) << ", buttons="
            << sfml_util::PopupButtons::ToString(BUTTONS) << ", image="
            << sfml_util::PopupImage::ToString(IMAGE) << ", textInfo=\""
            << TEXT_INFO.text << "\") was given TEXT_INFO.text that was empty.");

        M_ASSERT_OR_LOGANDTHROW_SS((sfml_util::PopupImage::IsValid(IMAGE)),
            "game::PopupInfo(type=" << game::Popup::ToString(TYPE) << ", buttons="
            << sfml_util::PopupButtons::ToString(BUTTONS) << ", image="
            << sfml_util::PopupImage::ToString(IMAGE) << ", textInfo=\"" << TEXT_INFO.text <<
            "\") was given an IMAGE value of " << IMAGE << ", which is invalid.");

        if ((game::Popup::Spellbook == type_) && (creatureCPtr_ == nullptr))
        {
            throw std::runtime_error(
                std::string("game::PopupInfo(type=Spellbook) constructor found spellbook").append(
                    " popup with a creaturePtr that was null.") );
        }

        if ((imageFadeSpeed_ > 0.0f) && (textureVec_.empty()))
        {
            std::ostringstream ss;
            ss << "game::PopupInfo(type=" << game::Popup::ToString(TYPE) << ", buttons="
                << sfml_util::PopupButtons::ToString(BUTTONS) << ", image="
                << sfml_util::PopupImage::ToString(IMAGE) << ", textInfo=\"" << TEXT_INFO.text
                << "\") was given an image fade speed of " << IMAGE_FADE_SPEED
                << " but the TEXTURE_VEC was empty, which makes no sense.  If there is a fade "
                << "speed then there must be images to fade-in.";

            throw std::runtime_error(ss.str());
        }

        if (IMAGE == sfml_util::PopupImage::Banner)
        {
            willAddRandImage_ = false;
        }
    }


    PopupInfo::PopupInfo(const std::string &                     NAME,
                         const sfml_util::gui::TextInfo &        TEXT_INFO,
                         const sfml_util::PopupButtons::Enum     BUTTONS,
                         const sfml_util::gui::box::Info &       BOX_INFO,
                         const float                             MAX_SIZE_RATIO_X,
                         const float                             MAX_SIZE_RATIO_Y,
                         const game::Popup::Enum                 TYPE,
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
        textureVec_      (),
        numberMin_       (0),
        numberMax_       (0),
        numberInvalidVec_(),
        imageFadeSpeed_  (IMAGE_FADE_SPEED_DEFAULT_),
        creatureCPtr_    (nullptr),
        initialSelection_(0),
        areImgsCreatures_(false),
        textVec_         (),
        howCombatEnded_  (combat::CombatEnd::Count),
        titleFromPtr_    (nullptr),
        titleToPtr_      (nullptr),
        titleText_       (""),
        descText_        ("")
    {}


    PopupInfo::PopupInfo(const std::string &                     NAME,
                         const sfml_util::gui::TextInfo &        TEXT_INFO,
                         const sfml_util::TextureVec_t &         TEXTURE_VEC,
                         const bool                              ARE_IMAGES_CREATURES,
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
        textureVec_      (TEXTURE_VEC),
        numberMin_       (0),
        numberMax_       (0),
        numberInvalidVec_(),
        imageFadeSpeed_  (IMAGE_FADE_SPEED_DEFAULT_),
        creatureCPtr_    (nullptr),
        initialSelection_(0),
        areImgsCreatures_(ARE_IMAGES_CREATURES),
        textVec_         (),
        howCombatEnded_  (combat::CombatEnd::Count),
        titleFromPtr_    (nullptr),
        titleToPtr_      (nullptr),
        titleText_       (""),
        descText_        ("")
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
        textureVec_      (),
        numberMin_       (THE_MIN),
        numberMax_       (THE_MAX),
        numberInvalidVec_(),
        imageFadeSpeed_  (IMAGE_FADE_SPEED_DEFAULT_),
        creatureCPtr_    (nullptr),
        initialSelection_(0),
        areImgsCreatures_(false),
        textVec_         (),
        howCombatEnded_  (combat::CombatEnd::Count),
        titleFromPtr_    (nullptr),
        titleToPtr_      (nullptr),
        titleText_       (""),
        descText_        ("")
    {}


    PopupInfo::PopupInfo(const std::string &                 NAME,
                         const sfml_util::gui::TextInfo &    TEXT_INFO,
                         const float                         IMAGE_SCALE,
                         const sfml_util::PopupButtons::Enum BUTTONS,
                         const combat::CombatEnd::Enum       HOW_COMBAT_ENDED)
    :
        name_            (NAME),
        textInfo_        (TEXT_INFO),
        buttons_         (BUTTONS),
        image_           (sfml_util::PopupImage::Large),
        type_            (game::Popup::CombatOver),
        soundEffect_     (sfml_util::sound_effect::None),
        boxInfo_         (),
        ratioX_          (1.0f),
        ratioY_          (1.0f),
        buttonColor_     (sfml_util::PopupButtonColor::Dark),
        willAddRandImage_(false),
        imageScale_      (IMAGE_SCALE),
        textureVec_      (),
        numberMin_       (0),
        numberMax_       (0),
        numberInvalidVec_(),
        imageFadeSpeed_  (IMAGE_FADE_SPEED_DEFAULT_),
        creatureCPtr_    (nullptr),
        initialSelection_(0),
        areImgsCreatures_(false),
        textVec_         (),
        howCombatEnded_  (HOW_COMBAT_ENDED),
        titleFromPtr_    (nullptr),
        titleToPtr_      (nullptr),
        titleText_       (""),
        descText_        ("")
    {}


    PopupInfo::~PopupInfo()
    {}


    PopupInfo::PopupInfo(const PopupInfo & PI)
    :
        name_            (PI.name_),
        textInfo_        (PI.textInfo_),
        buttons_         (PI.buttons_),
        image_           (PI.image_),
        type_            (PI.type_),
        soundEffect_     (PI.soundEffect_),
        boxInfo_         (PI.boxInfo_),
        ratioX_          (PI.ratioX_),
        ratioY_          (PI.ratioY_),
        buttonColor_     (PI.buttonColor_),
        willAddRandImage_(PI.willAddRandImage_),
        imageScale_      (PI.imageScale_),
        textureVec_      (PI.textureVec_),
        numberMin_       (PI.numberMin_),
        numberMax_       (PI.numberMax_),
        numberInvalidVec_(PI.numberInvalidVec_),
        imageFadeSpeed_  (PI.imageFadeSpeed_),
        
        //The creature object is not managed by this class,
        //so it is safe to copy here.
        creatureCPtr_    (PI.creatureCPtr_),

        initialSelection_(PI.initialSelection_),
        areImgsCreatures_(PI.areImgsCreatures_),
        textVec_         (PI.textVec_),
        howCombatEnded_  (PI.howCombatEnded_),
        titleFromPtr_    (PI.titleFromPtr_),
        titleToPtr_      (PI.titleToPtr_),
        titleText_       (PI.titleText_),
        descText_        (PI.descText_)
    {}


    PopupInfo & PopupInfo::operator=(const PopupInfo & PI)
    {
        if (& PI != this)
        {
            name_             = PI.name_;
            textInfo_         = PI.textInfo_;
            buttons_          = PI.buttons_;
            image_            = PI.image_;
            type_             = PI.type_;
            soundEffect_      = PI.soundEffect_;
            boxInfo_          = PI.boxInfo_;
            ratioX_           = PI.ratioX_;
            ratioY_           = PI.ratioY_;
            buttonColor_      = PI.buttonColor_;
            willAddRandImage_ = PI.willAddRandImage_;
            imageScale_       = PI.imageScale_;
            textureVec_       = PI.textureVec_;
            numberMin_        = PI.numberMin_;
            numberMax_        = PI.numberMax_;
            numberInvalidVec_ = PI.numberInvalidVec_;
            imageFadeSpeed_   = PI.imageFadeSpeed_;
            areImgsCreatures_ = PI.areImgsCreatures_;

            //The lifetime of these objects is not managed by this class.
            //Usage is short-term observation only, so copying is safe.
            creatureCPtr_     = PI.creatureCPtr_;

            initialSelection_ = PI.initialSelection_;
            textVec_          = PI.textVec_;
            howCombatEnded_   = PI.howCombatEnded_;
            titleFromPtr_     = PI.titleFromPtr_;
            titleToPtr_       = PI.titleToPtr_;
            titleText_        = PI.titleText_;
            descText_         = PI.descText_;
        }

        return * this;
    }


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
                {
                    ss << ", bg_image=\"" << boxInfo_.bg_info.path << "\"";
                }
                else
                {
                    if (false == boxInfo_.bg_info.hasTexture)
                    {
                        ss << ", color=" << sfml_util::color::ColorToString(boxInfo_.bg_info.color);
                    }
                }
            }
        }

        ss << ", accent_image=" << std::boolalpha << willAddRandImage_;

        ss << ", sound_effect=" << sfml_util::sound_effect::ToString(soundEffect_);

        if (numberMin_ != numberMax_)
        {
            ss << ", number_select=[" << numberMin_ << "," << numberMax_ << "]";
        }

        if (textureVec_.size() > 0)
        {
            ss << ", images_count=" << textureVec_.size();
        }

        if (imageFadeSpeed_ > 0.0f)
        {
            ss << ", fade_speed=" << imageFadeSpeed_;
        }

        if (creatureCPtr_ != nullptr)
        {
            ss << ", creature=" << creatureCPtr_->Name();
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
            return (std::find(numberInvalidVec_.begin(), numberInvalidVec_.end(), NUM) == numberInvalidVec_.end());
        }
    }

}
