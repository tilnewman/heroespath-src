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
#ifndef HEROESPATH_PARTYSTAGE_HPP_INCLUDED
#define HEROESPATH_PARTYSTAGE_HPP_INCLUDED
//
// party-stage.hpp
//  A Stage class that allows grouping of saved characters into a party.
//
#include "avatar/avatar-enum.hpp"
#include "popup/i-popup-callback.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/main-menu-title.hpp"
#include "sfml-util/ouroboros.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/stage.hpp"

#include <memory>
#include <set>
#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    } // namespace gui
} // namespace sfml_util

namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreaturePSet_t = std::set<CreaturePtr_t>;
} // namespace player

namespace stage
{

    // A Stage class that displays saved characters and allows grouping them into a party of six
    class PartyStage
        : public sfml_util::Stage
        , public sfml_util::gui::callback::IListBoxCallbackHandler
        , public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
        , public popup::IPopupHandler_t
    {
    public:
        PartyStage(const PartyStage &) = delete;
        PartyStage(PartyStage &&) = delete;
        PartyStage & operator=(const PartyStage &) = delete;
        PartyStage & operator=(PartyStage &&) = delete;

    public:
        PartyStage();
        virtual ~PartyStage();

        virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);
        virtual bool
            HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);
        virtual bool HandleCallback(const popup::PopupResponse &);
        //
        virtual bool HandleCallback_BackButton();
        virtual bool HandleCallback_StartButton();
        virtual bool HandleCallback_DeleteButton();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual std::size_t NumCharactersInTheParty() const;
        virtual sfml_util::gui::ListBoxItemSPtr_t GetSelectedItemSPtr() const;
        virtual creature::CreaturePtr_t GetSelectedCharacter() const;

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);

        virtual void UpdateMousePos(const sf::Vector2i & MOUSE_POS_V);

        virtual void StartNewGame(const avatar::Avatar::Enum);

        virtual bool KeyRelease(const sf::Event::KeyEvent &);

    private:
        void ResetMouseOverPopupState();

        bool AreAnyCharactersBeasts() const;
        bool AreAnyCharactersBeastmasters() const;

        void MissingBeastmasterPopup();
        void NotEnoughCharactersPopup();
        void PartyAvatarSelectionPopup();

    private:
        static const std::string POPUP_NAME_STR_NOT_ENOUGH_CHARS_;
        static const std::string POPUP_NAME_STR_TOO_MANY_CHARS_;
        static const std::string POPUP_NAME_STR_DELETE_CONFIRM_;
        static const std::string POPUP_NAME_STR_PARTY_IMAGE_SELECT_;
        static const float MOUSE_OVER_POPUP_DELAY_SEC_;
        static const float MOUSE_OVER_SLIDER_SPEED_;
        static const float MOUSE_OVER_POPUP_POS_LEFT_;
        static const float MOUSE_OVER_POPUP_POS_TOP_;
        static const float MOUSE_OVER_IMAGE_PAD_;
        static const float MOUSE_OVER_BOX_ALPHA_;
        //
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        sfml_util::MainMenuTitle mainMenuTitle_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sfml_util::gui::FourStateButtonUPtr_t backButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t startButtonUPtr_;
        sfml_util::gui::FourStateButtonUPtr_t deleteButtonUPtr_;
        sfml_util::gui::ListBoxUPtr_t characterListBoxUPtr_;
        sfml_util::gui::ListBoxUPtr_t partyListBoxUPtr_;
        sfml_util::gui::TextRegionUPtr_t insTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t upTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t partyTextRegionUPtr_;
        sfml_util::gui::TextInfo warningTextInfo_;
        sfml_util::gui::TextRegionUPtr_t warningTextRegionUPtr_;
        sfml_util::sliders::Slider<sf::Uint8, float> warningTextSlider_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::BottomSymbol bottomSymbol_;
        bool willDisplayCharacterCountWarningText_;

        // mouseover popup and animation members
        bool willShowMouseOverPopup_;
        float mouseOverPopupTimerSec_;
        sf::VertexArray mouseOverQuad_;
        float mouseOverBoxWidth_;
        float mouseOverBoxHeight_;
        sf::Vector2f mouseOverPosV_;
        sf::Sprite mouseOverSprite_;
        creature::CreaturePtr_t mouseOverCharPtr_;
        sf::Texture mouseOverTexture_;
        bool isMouseOverTexture_;
        sfml_util::gui::TextRegionUPtr_t mouseOverTextRegionUPtr_;
        sfml_util::sliders::ZeroSliderOnce<float> mouseOverSlider_;

        //
        creature::CreaturePSet_t charactersPSet_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_PARTYSTAGE_HPP_INCLUDED
