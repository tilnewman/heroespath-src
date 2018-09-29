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
#include "misc/not-null.hpp"
#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/gui/main-menu-buttons.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/stage-title.hpp"
#include "sfml-util/stage.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;

    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    using TextRegionUVec_t = std::vector<TextRegionUPtr_t>;

} // namespace sfml_util

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
        : public sfml_util::Stage
        , public sfml_util::ImageTextEntity::Callback_t::IHandler_t
        , public sfml_util::ListBox<LoadGameStage, game::GameStatePtr_t>::Callback_t::IHandler_t
    {
    public:
        using GameListBox_t = sfml_util::ListBox<LoadGameStage, game::GameStatePtr_t>;
        using GameListBoxUPtr_t = std::unique_ptr<GameListBox_t>;

        LoadGameStage(const LoadGameStage &) = delete;
        LoadGameStage(LoadGameStage &&) = delete;
        LoadGameStage & operator=(const LoadGameStage &) = delete;
        LoadGameStage & operator=(LoadGameStage &&) = delete;

    public:
        LoadGameStage();
        virtual ~LoadGameStage();

        bool HandleCallback(const GameListBox_t::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const sfml_util::ImageTextEntity::Callback_t::PacketPtr_t &) override
        {
            return false;
        }

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        void SetupGameInfoDisplay();

    private:
        sfml_util::StageTitle stageTitle_;
        sfml_util::BoxEntity backgroundBox_;
        sfml_util::MainMenuButtonUPtr_t backButtonUPtr_;
        sf::Vector2f screenSizeV_;
        sf::FloatRect gsListBoxRect_;
        sf::Color gsListBoxBGColor_;
        sfml_util::FocusColors gsListBoxFocusColors_;
        GameListBoxUPtr_t gsListBoxUPtr_;
        sfml_util::TextRegionUPtr_t locTextRegionUPtr_;
        sfml_util::TextRegionUVec_t charTextRegionUVec_;
        sfml_util::TextRegionUPtr_t charLabelTextRegionUPtr_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        game::GameStatePVec_t gamestatePVec_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_LOADGAMEMENU_HPP_INCLUDED
