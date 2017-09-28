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
#ifndef SFMLUTIL_GUI_FOURSTATEBUTTON_INCLUDED
#define SFMLUTIL_GUI_FOURSTATEBUTTON_INCLUDED
//
// four-state-button.hpp
//  Drawing and handling code for a button type that has four visual and four text states.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    //forward declarations
    namespace box
    {
        class Box;
        using BoxUPtr_t = std::unique_ptr<Box>;
    }

    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;


    class FourStateButton;
    namespace callback
    {
        using FourStateButtonCallbackPackage_t = sfml_util::callback::PtrWrapper<FourStateButton>;
        using IFourStateButtonCallbackHandler_t = sfml_util::callback::ICallbackHandler<FourStateButtonCallbackPackage_t, bool>;
    }


    //A simple button class
    class FourStateButton : public GuiEntity
    {
        FourStateButton(const FourStateButton &) =delete;
        FourStateButton & operator=(const FourStateButton &) =delete;

    public:
        //if using this constructor, Setup() must be called before any other functions
        explicit FourStateButton(const std::string & NAME);

        FourStateButton(const std::string &    NAME,
                         const float           POS_LEFT,
                         const float           POS_TOP,
                         const std::string &   IMAGE_PATH_UP,
                         const std::string &   IMAGE_PATH_DOWN     = "",
                         const std::string &   IMAGE_PATH_OVER     = "",
                         const std::string &   IMAGE_PATH_DISABLED = "",
                         const MouseTextInfo & MOUSE_TEXT_INFO     = MouseTextInfo(),
                         const TextInfo &      TEXT_INFO_DISABLED  = TextInfo(),
                         const bool            WILL_BOX            = false,
                         const float           SCALE               = 1.0f,
                         const bool            IS_DISABLED         = false);

        FourStateButton(const std::string &    NAME,
                         const float           POS_LEFT,
                         const float           POS_TOP,
                         const sf::Texture &   IMAGE_UP,
                         const bool            HAS_IMAGE_UP,
                         const sf::Texture &   IMAGE_DOWN         = sf::Texture(),
                         const bool            HAS_IMAGE_DOWN     = false,
                         const sf::Texture &   IMAGE_OVER         = sf::Texture(),
                         const bool            HAS_IMAGE_OVER     = false,
                         const sf::Texture &   IMAGE_DISABLED     = sf::Texture(),
                         const bool            HAS_IMAGE_DISABLED = false,
                         const MouseTextInfo & MOUSE_TEXT_INFO    = MouseTextInfo(),
                         const TextInfo &      TEXT_INFO_DISABLED = TextInfo(),
                         const bool            WILL_BOX           = false,
                         const float           SCALE              = 1.0f,
                         const bool            IS_DISABLED        = false);

        virtual ~FourStateButton();

        virtual void Setup(const float           POS_LEFT,
                           const float           POS_TOP,
                           const std::string &   IMAGE_PATH_UP,
                           const std::string &   IMAGE_PATH_DOWN     = "",
                           const std::string &   IMAGE_PATH_OVER     = "",
                           const std::string &   IMAGE_PATH_DISABLED = "",
                           const MouseTextInfo & MOUSE_TEXT_INFO     = MouseTextInfo(),
                           const TextInfo &      TEXT_INFO_DISABLED  = TextInfo(),
                           const bool            WILL_BOX            = false,
                           const float           SCALE               = 1.0f,
                           const bool            IS_DISABLED         = false);

        virtual void Setup(const float           POS_LEFT,
                           const float           POS_TOP,
                           const sf::Texture &   IMAGE_UP,
                           const bool            HAS_IMAGE_UP,
                           const sf::Texture &   IMAGE_DOWN         = sf::Texture(),
                           const bool            HAS_IMAGE_DOWN     = false,
                           const sf::Texture &   IMAGE_OVER         = sf::Texture(),
                           const bool            HAS_IMAGE_OVER     = false,
                           const sf::Texture &   IMAGE_DISABLED     = sf::Texture(),
                           const bool            HAS_IMAGE_DISABLED = false,
                           const MouseTextInfo & MOUSE_TEXT_INFO    = MouseTextInfo(),
                           const TextInfo &      TEXT_INFODISABLED  = TextInfo(),
                           const bool            WILL_BOX           = false,
                           const float           SCALE              = 1.0f,
                           const bool            IS_DISABLED        = false);

        virtual void SetText(const std::string & TEXT_UP,
                             const std::string & TEXT_DOWN     = "",
                             const std::string & TEXT_OVER     = "",
                             const std::string & TEXT_DISABLED = "");

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V);
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);
        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        inline virtual void SetCallbackHandler(callback::IFourStateButtonCallbackHandler_t * const ptr)
        {
            callbackHandlerPtr_ = ptr;
        }

        inline bool IsDisabled() const { return isDisabled_; }
        void SetIsDisabled(const bool);

        virtual void SetMouseState(const MouseState::Enum);

        virtual void SetScaleToRes();
        virtual void SetVertPositionToBottomOfScreenByRes(const float POS_LEFT);

    protected:
        virtual void Reset();
        virtual void OnClick(const sf::Vector2f &);
        virtual void SetScale(const float NEW_SCALE);

    protected:
        bool             isDisabled_;
        sf::Texture      textureUp_;
        sf::Texture      textureDown_;
        sf::Texture      textureOver_;
        sf::Texture      textureDisabled_;
        bool             hasUp_;
        bool             hasDown_;
        bool             hasOver_;
        bool             hasDisabled_;
        sf::Sprite       buttonSprite_;
        TextRegion *     textRegionCurrPtr_;
        TextRegionUPtr_t textRegionUpUPtr_;
        TextRegionUPtr_t textRegionDownUPtr_;
        TextRegionUPtr_t textRegionOverUPtr_;
        TextRegionUPtr_t textRegionDisabledUPtr_;
        box::BoxUPtr_t   boxUPtr_;
        float            scale_;
        callback::IFourStateButtonCallbackHandler_t * callbackHandlerPtr_;
    };

    using FourStateButtonPtr_t  = FourStateButton *;
    using FourStateButtonPVec_t = std::vector<FourStateButtonPtr_t>;
    //
    using FourStateButtonUPtr_t = std::unique_ptr<FourStateButton>;
    using FourStateButtonUVec_t = std::vector<FourStateButtonUPtr_t>;
    //
    using FourStateButtonUPtr_t = std::unique_ptr<FourStateButton>;

}
}
#endif //SFMLUTIL_GUI_FOURSTATEBUTTON_INCLUDED
