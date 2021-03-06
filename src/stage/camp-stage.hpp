// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CAMP_STAGE_HPP_INCLUDED
#define HEROESPATH_CAMP_STAGE_HPP_INCLUDED
//
// camp-stage.hpp
//
#include "gui/box-entity.hpp"
#include "gui/cached-texture.hpp"
#include "gui/horiz-symbol.hpp"
#include "gui/stage-title.hpp"
#include "misc/callback.hpp"
#include "stage/stage-base.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>

namespace heroespath
{
namespace gui
{
    class Animation;
    using AnimationUPtr_t = std::unique_ptr<Animation>;

    class Ouroboros;
    using OuroborosUPtr_t = std::unique_ptr<Ouroboros>;
} // namespace gui

namespace stage
{

    // A Stage class that allows the party to camp for resting and healing
    class CampStage
        : public stage::StageBase
        , public misc::PopupCallback_t::IHandler_t
    {
    public:
        CampStage();
        virtual ~CampStage();

        CampStage(const CampStage &) = delete;
        CampStage(CampStage &&) = delete;
        CampStage & operator=(const CampStage &) = delete;
        CampStage & operator=(CampStage &&) = delete;

        const std::string HandleCallback(
            const misc::PopupCallback_t::Packet_t &,
            const std::string & PACKET_DESCRIPTION) override;

        void Setup() override;
        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

    private:
        const std::string ComposeNewGamePopupText1();
        const std::string ComposeNewGamePopupText2();
        const std::string ComposeNewGamePopupText3();
        const std::string ComposeNewGamePopupText4();

    public:
        static const std::string NEWHEROESPATH_POPUP_NAME1_;
        static const std::string NEWHEROESPATH_POPUP_NAME2_;
        static const std::string NEWHEROESPATH_POPUP_NAME3_;
        static const std::string NEWHEROESPATH_POPUP_NAME4_;

    private:
        gui::StageTitle stageTitle_;
        gui::CachedTexture campfireCachedTexture_;
        sf::Sprite campfireSprite_;
        gui::BoxEntity backgroundBox_;
        gui::AnimationUPtr_t fireAnimUPtr_;
        bool showNewGamePopup1_;
        bool showNewGamePopup2_;
        bool showNewGamePopup3_;
        bool showNewGamePopup4_;
        gui::OuroborosUPtr_t ouroborosUPtr_;
        gui::BottomSymbol bottomSymbol_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CAMP_STAGE_HPP_INCLUDED
