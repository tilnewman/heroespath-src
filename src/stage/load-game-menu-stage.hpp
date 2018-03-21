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
#ifndef HEROESPATH_STAGE_LOADGAMEMENU_HPP_INCLUDED
#define HEROESPATH_STAGE_LOADGAMEMENU_HPP_INCLUDED
//
// load-game-menu-stage.hpp
//  A Stage class that allows players to load previous games
//
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/stage.hpp"

#include "popup/i-popup-callback.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/main-menu-title.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;

    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
        using TextRegionUVec_t = std::vector<TextRegionUPtr_t>;
    } // namespace gui
} // namespace sfml_util

namespace state
{
    class GameState;
    using GameStatePtr_t = GameState *;
    using GameStatePSet_t = std::set<GameStatePtr_t>;
} // namespace state

namespace stage
{

    // A Stage class that allows players to load saved games
    class LoadGameStage
        : public sfml_util::Stage
        , public sfml_util::gui::callback::IListBoxCallbackHandler
        , public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
    public:
        LoadGameStage(const LoadGameStage &) = delete;
        LoadGameStage(LoadGameStage &&) = delete;
        LoadGameStage & operator=(const LoadGameStage &) = delete;
        LoadGameStage & operator=(LoadGameStage &&) = delete;

    public:
        LoadGameStage();
        virtual ~LoadGameStage();

        virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool
            HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);
        virtual bool KeyRelease(const sf::Event::KeyEvent &);

    private:
        void SetupGameInfoDisplay();

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        //
        sfml_util::MainMenuTitle mainMenuTitle_;
        sfml_util::gui::BackgroundImage backgroundImage_;
        sfml_util::gui::FourStateButtonUPtr_t backButtonUPtr_;
        sfml_util::gui::ListBoxUPtr_t gsListBoxUPtr_;
        //
        sfml_util::gui::TextRegionUPtr_t locTextRegionUPtr_;
        sfml_util::gui::TextRegionUVec_t charTextRegionUVec_;
        sfml_util::gui::TextRegionUPtr_t charLabelTextRegionUPtr_;
        //
        float gsListBoxPosLeft_;
        float gsListBoxPosTop_;
        float gsListBoxPosWidth_;
        float gsListBoxPosHeight_;
        //
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::BottomSymbol bottomSymbol_;
        //
        state::GameStatePSet_t gamestatePSet_;
    };
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_LOADGAMEMENU_HPP_INCLUDED
