// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_LOADGAMEMENU_HPP_INCLUDED
#define HEROESPATH_STAGE_LOADGAMEMENU_HPP_INCLUDED
//
// load-game-menu-stage.hpp
//  A Stage class that allows players to load previous games
//
#include "gui/box-entity.hpp"
#include "gui/horiz-symbol.hpp"
#include "gui/list-box.hpp"
#include "gui/main-menu-buttons.hpp"
#include "gui/sliders.hpp"
#include "gui/stage-title.hpp"
#include "misc/not-null.hpp"
#include "stage/stage-base.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{
    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;

    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    using TextRegionUVec_t = std::vector<TextRegionUPtr_t>;

} // namespace gui

namespace game
{
    class GameState;
    using GameStatePtr_t = misc::NotNull<GameState *>;
    using GameStatePVec_t = std::vector<GameStatePtr_t>;
} // namespace game

namespace stage
{

    // A Stage class that allows players to load saved games
    class LoadGameStage
        : public stage::StageBase
        , public gui::ImageTextEntity::Callback_t::IHandler_t
        , public gui::ListBox<LoadGameStage, game::GameStatePtr_t>::Callback_t::IHandler_t
    {
    public:
        using GameListBox_t = gui::ListBox<LoadGameStage, game::GameStatePtr_t>;
        using GameListBoxUPtr_t = std::unique_ptr<GameListBox_t>;

        LoadGameStage(const LoadGameStage &) = delete;
        LoadGameStage(LoadGameStage &&) = delete;
        LoadGameStage & operator=(const LoadGameStage &) = delete;
        LoadGameStage & operator=(LoadGameStage &&) = delete;

        LoadGameStage();
        virtual ~LoadGameStage();

        bool HandleCallback(const GameListBox_t::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const gui::ImageTextEntity::Callback_t::PacketPtr_t &) override
        {
            return false;
        }

        void Setup() override;
        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        void SetupGameInfoDisplay();

    private:
        gui::StageTitle stageTitle_;
        gui::BoxEntity backgroundBox_;
        gui::MainMenuButtonUPtr_t backButtonUPtr_;
        sf::Vector2f screenSizeV_;
        sf::FloatRect gsListBoxRect_;
        sf::Color gsListBoxBGColor_;
        gui::FocusColors gsListBoxFocusColors_;
        GameListBoxUPtr_t gsListBoxUPtr_;
        gui::TextRegionUPtr_t locTextRegionUPtr_;
        gui::TextRegionUVec_t charTextRegionUVec_;
        gui::TextRegionUPtr_t charLabelTextRegionUPtr_;
        gui::OuroborosUPtr_t ouroborosUPtr_;
        game::GameStatePVec_t gamestatePVec_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_LOADGAMEMENU_HPP_INCLUDED
