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
#ifndef SFMLUTIL_RADIOBUTTON_INCLUDED
#define SFMLUTIL_RADIOBUTTON_INCLUDED
//
// radio-button.hpp
//  Radio button drawing and handling code.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/brightness-enum.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/two-state-entity.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"

#include "misc/handy-types.hpp"

#include <memory>
#include <string>
#include <vector>
#include <set>


namespace sfml_util
{
    namespace gui
    {
        class RadioButtonSet;
    }

    namespace callback
    {
        using RadioButtonCallbackPackage_t = sfml_util::callback::PtrWrapper<sfml_util::gui::RadioButtonSet>;
        using IRadioButtonSetCallbackHandler_t = ICallbackHandler<RadioButtonCallbackPackage_t, bool>;
        using RadioButtonCallbackHandlerPtrSet_t = std::set<IRadioButtonSetCallbackHandler_t *>;
    }

namespace gui
{

    //Represents a clickable radio button or check box, etc.
    class RadioButton : public TwoStateEntity
    {
    public:
        RadioButton(const std::string &    NAME,
                    const Brightness::Enum BRIGHTNESS,
                    const float            POS_LEFT,
                    const float            POS_TOP,
                    const sf::Sprite &     SPRITE_UP,
                    const sf::Sprite &     SPRITE_DOWN,
                    const MouseTextInfo &  MOUSE_TEXT_INFO,
                    const bool             IS_SELECTED = false,
                    const bool             IS_INVALID  = false);

        RadioButton(const std::string &    NAME,
                    const Brightness::Enum BRIGHTNESS,
                    const float            POS_LEFT,
                    const float            POS_TOP,
                    const MouseTextInfo &  MOUSE_TEXT_INFO,
                    const bool             IS_SELECTED = false,
                    const bool             IS_INVALID  = false);

        RadioButton(const std::string &    NAME,
                    const Brightness::Enum BRIGHTNESS,
                    const float            POS_LEFT,
                    const float            POS_TOP,
                    const TextInfo &       TEXT_INFO_UP,
                    const bool             IS_SELECTED = false,
                    const bool             IS_INVALID  = false);

        virtual ~RadioButton();

        void Setup(const float POS_LEFT, const float POS_TOP, const bool IS_INVALID);

        void SetValidity(const bool IS_VALID);

        static const sf::Color COLOR_VALID_;
        static const sf::Color COLOR_INVALID_;

    private:
        virtual void SetupSprites(const Brightness::Enum BRIGHTNESS);
    };


    using RadioButtonSPtr_t = std::shared_ptr<RadioButton>;
    using RadioButtonSVec_t = std::vector<RadioButtonSPtr_t>;


    //Represents a set of radio buttons
    class RadioButtonSet : public GuiEntity
    {
        //prevent copy construction
        RadioButtonSet(const RadioButtonSet &) =delete;

        //prevent copy assignment
        RadioButtonSet & operator=(const RadioButtonSet &) =delete;

    public:
        //If Using this constructor, then one of the Setup() functions must be called before any other member
        explicit RadioButtonSet(const std::string & NAME);

        //Constructor for cases where each radio button's label text has a different style, size, etc.
        RadioButtonSet( const std::string &      NAME,
                        const float              POS_LEFT,
                        const float              POS_TOP,
                        const TextInfoVec_t &    TEXT_INFO_VEC,
                        const std::size_t        INITIAL_SELECTION,
                        const Brightness::Enum   BRIGHTNESS,
                        const misc::SizetVec_t & INVALID_SEL_VEC = misc::SizetVec_t(),
                        const box::Info &        BOX_INFO        = box::Info(),
                        const float              OUTER_PAD       = OUTER_PAD_DEFAULT_,
                        const float              BETWEEN_PAD     = BETWEEN_PAD_DEFAULT_);

        RadioButtonSet( const std::string &        NAME,
                        const float                POS_LEFT,
                        const float                POS_TOP,
                        const MouseTextInfoVec_t & TEXT_INFO_VEC,
                        const std::size_t          INITIAL_SELECTION,
                        const Brightness::Enum     BRIGHTNESS,
                        const misc::SizetVec_t &   INVALID_SEL_VEC = misc::SizetVec_t(),
                        const box::Info &          BOX_INFO        = box::Info(),
                        const float                OUTER_PAD       = OUTER_PAD_DEFAULT_,
                        const float                BETWEEN_PAD     = BETWEEN_PAD_DEFAULT_);

        //Constructor for cases where each radio button's label text has the same style, size, etc.
        RadioButtonSet( const std::string &              NAME,
                        const float                      POS_LEFT,
                        const float                      POS_TOP,
                        const TextInfo &                 TEXT_INFO,
                        const std::vector<std::string> & LABEL_VEC,
                        const std::size_t                INITIAL_SELECTION,
                        const Brightness::Enum           BRIGHTNESS,
                        const misc::SizetVec_t &         INVALID_SEL_VEC = misc::SizetVec_t(),
                        const box::Info &                BOX_INFO        = box::Info(),
                        const float                      OUTER_PAD       = OUTER_PAD_DEFAULT_,
                        const float                      BETWEEN_PAD     = BETWEEN_PAD_DEFAULT_);

        virtual ~RadioButtonSet();

        void Setup(const float              POS_LEFT,
                   const float              POS_TOP,
                   const TextInfoVec_t &    TEXT_INFO_VEC,
                   const std::size_t        INITIAL_SELECTION,
                   const Brightness::Enum   BRIGHTNESS,
                   const misc::SizetVec_t & INVALID_SEL_VEC = misc::SizetVec_t(),
                   const box::Info &        BOX_INFO        = box::Info(),
                   const float              OUTER_PAD       = OUTER_PAD_DEFAULT_,
                   const float              BETWEEN_PAD     = BETWEEN_PAD_DEFAULT_);

        void Setup(const float                POS_LEFT,
                   const float                POS_TOP,
                   const MouseTextInfoVec_t & TEXT_INFO_VEC,
                   const std::size_t          INITIAL_SELECTION,
                   const Brightness::Enum     BRIGHTNESS,
                   const misc::SizetVec_t &   INVALID_SEL_VEC = misc::SizetVec_t(),
                   const box::Info &          BOX_INFO        = box::Info(),
                   const float                OUTER_PAD       = OUTER_PAD_DEFAULT_,
                   const float                BETWEEN_PAD     = BETWEEN_PAD_DEFAULT_);

        void Setup( const float                      POS_LEFT,
                    const float                      POS_TOP,
                    const TextInfo &                 TEXT_INFO,
                    const std::vector<std::string> & LABEL_VEC,
                    const std::size_t                INITIAL_SELECTION,
                    const Brightness::Enum           BRIGHTNESS,
                    const misc::SizetVec_t &         INVALID_SEL_VEC = misc::SizetVec_t(),
                    const box::Info &                BOX_INFO        = box::Info(),
                    const float                      OUTER_PAD       = OUTER_PAD_DEFAULT_,
                    const float                      BETWEEN_PAD     = BETWEEN_PAD_DEFAULT_ );

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline std::size_t GetSelectedNumber() const            { return currentSelection_; }
        inline const RadioButtonSPtr_t GetSelectedButton()      { return buttonSVec_[currentSelection_]; }

        //throws on out of bounds, but will allow setting a grayed-out number
        void SetSelectNumber(const std::size_t);

        void SetInvalidSelections(const misc::SizetVec_t & INVALID_SELECTIONS_VEC);
        inline const misc::SizetVec_t GetInvalidSelections() const    { return invalidSelectionVec_; }

        //returns true if the currentSelection_ changed
        virtual bool MouseUp(const sf::Vector2f & MOUSE_POS_V);

        //cannot change any states, so always returns false
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);

        //returns true if any MouseStates were changed
        virtual bool UpdateMousePos(const sf::Vector2f & MOUSE_POS_V);

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        void CallbackHandlerAdd(sfml_util::callback::IRadioButtonSetCallbackHandler_t  * const handlerPtr);
        bool CallbackHandlerRemove(sfml_util::callback::IRadioButtonSetCallbackHandler_t * const handlerPtr);

        virtual bool SetHasFocus(const bool);
    protected:
        bool IsInvalid(const std::size_t INDEX) const;


        void PostSetupTasks(const float       POS_LEFT,
                            const float       POS_TOP,
                            const box::Info & BOX_INFO);

        void PositionRadioButtons(const float POS_LEFT, const float POS_TOP);

        void RevertToPreviousSelection();

        void ChangeCurrentSelection(const std::size_t NEW_SELECTION);

        virtual void OnClick(const sf::Vector2f &);

        virtual void OnColorChange();

        virtual void TriggerCallback();

    public:
        static const float OUTER_PAD_DEFAULT_;
        static const float BETWEEN_PAD_DEFAULT_;
    protected:
        //
        float              outerPad_;
        float              betweenPad_;
        std::size_t        currentSelection_;
        RadioButtonSVec_t  buttonSVec_;
        box::Box           box_;
        std::size_t        downInWhichRegion_;
        misc::SizetVec_t   invalidSelectionVec_;
        std::size_t        prevSelection_;
        //
        sfml_util::callback::RadioButtonCallbackHandlerPtrSet_t callbackHandlerPtrSet_;
    };

    using RadioButtonSetSPtr_t = std::shared_ptr<RadioButtonSet>;
    using RadioButtonSVec_t = std::vector<RadioButtonSPtr_t>;
}
}
#endif //SFMLUTIL_RADIOBUTTON_INCLUDED
