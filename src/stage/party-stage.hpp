// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_PARTYSTAGE_HPP_INCLUDED
#define HEROESPATH_PARTYSTAGE_HPP_INCLUDED
//
// party-stage.hpp
//
#include "avatar/avatar-enum.hpp"
#include "gui/box-entity.hpp"
#include "gui/cached-texture.hpp"
#include "gui/colored-rect.hpp"
#include "gui/horiz-symbol.hpp"
#include "gui/image-text-entity.hpp"
#include "gui/list-box.hpp"
#include "gui/main-menu-background.hpp"
#include "gui/main-menu-buttons.hpp"
#include "gui/ouroboros.hpp"
#include "gui/slider-colored-rect.hpp"
#include "gui/sliders.hpp"
#include "gui/stage-title.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/callback.hpp"
#include "misc/not-null.hpp"
#include "stage/stage-base.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

} // namespace gui
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace stage
{

    // A Stage class that displays saved characters and allows grouping them into a party of six
    class PartyStage
        : public stage::StageBase
        , public gui::ListBox<PartyStage, creature::CreaturePtr_t>::Callback_t::IHandler_t
        , public gui::ImageTextEntity::Callback_t::IHandler_t
        , public misc::PopupCallback_t::IHandler_t
    {
        using PartyListBox_t = gui::ListBox<PartyStage, creature::CreaturePtr_t>;
        using PartyListBoxUPtr_t = std::unique_ptr<PartyListBox_t>;

    public:
        PartyStage();
        virtual ~PartyStage();

        PartyStage(const PartyStage &) = delete;
        PartyStage(PartyStage &&) = delete;
        PartyStage & operator=(const PartyStage &) = delete;
        PartyStage & operator=(PartyStage &&) = delete;

        const std::string HandleCallback(
            const PartyListBox_t::Callback_t::Packet_t & PACKET,
            const std::string & PACKET_DESCRIPTION) override;

        const std::string HandleCallback(
            const gui::ImageTextEntity::Callback_t::Packet_t & PACKET,
            const std::string & PACKET_DESCRIPTION) override;

        const std::string HandleCallback(
            const misc::PopupCallback_t::Packet_t & PACKET,
            const std::string & PACKET_DESCRIPTION) override;

        const std::string HandleCallback_StartButton();
        const std::string HandleCallback_DeleteButton();

        void Setup() override;
        void Setup_Ouroboros();
        void Setup_Buttons();
        void Setup_TopInstructionText();
        void Setup_TopWarningText();
        void Setup_CharactersListBoxLabel();
        void Setup_PartyListBoxLabel();
        void Setup_CharactersListBox();
        void Setup_PartyListBox();

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        const creature::CreaturePtrOpt_t GetSelectedCharacterPtrOpt() const;

        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        void UpdateTime_WarningTextColorCycling(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_MouseOver_Detection(const float ELAPSED_TIME_SECONDS);
        void UpdateTime_MouseOver_Animation(const float ELAPSED_TIME_SECONDS);

        void UpdateMousePos(const sf::Vector2i & MOUSE_POS_V) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        void StartNewGame(const avatar::Avatar::Enum);
        void ResetMouseOverPopupState();
        bool AreAnyInPartyBeasts() const;
        bool AreAnyInPartyBeastmasters() const;
        void MissingBeastmasterPopup();
        void NotEnoughCharactersPopup();
        void PartyAvatarSelectionPopup();
        void UpdateWillDisplayCharacterCountWarning();
        void SetupMouseOverPositionsAndDimmensions(const creature::CreaturePtr_t);
        bool DeleteCharacterIfSelected(PartyListBox_t &);

    private:
        static const std::string POPUP_NAME_STR_NOT_ENOUGH_CHARS_;
        static const std::string POPUP_NAME_STR_TOO_MANY_CHARS_;
        static const std::string POPUP_NAME_STR_DELETE_CONFIRM_;
        static const std::string POPUP_NAME_STR_PARTY_IMAGE_SELECT_;
        static const float MOUSEOVER_POPUP_DELAY_SEC_;
        static const float MOUSEOVER_SLIDER_SPEED_;
        static const float MOUSEOVER_BACKGROUND_FINAL_ALPHA_;
        //
        const gui::GuiFont::Enum LISTBOX_FONT_ENUM_;
        const unsigned LISTBOX_FONT_SIZE_;
        const float LISTBOX_WIDTH_;
        const float LISTBOX_HEIGHT_;
        const float BETWEEN_LISTBOXES_SPACER_;
        const float CHAR_LISTBOX_POS_LEFT_;
        const float PARTY_LISTBOX_POS_LEFT_;
        const float MOUSEOVER_FINAL_INNER_EDGE_PAD_;
        const float MOUSEOVER_CREATURE_IMAGE_WIDTH_MAX_;
        const sf::Color MOUSEOVER_COLORCYCLE_START_;
        const sf::Color HEROESPATH_ORANGE_;
        const sf::Color MOUSEOVER_COLORCYCLE_ALT_;
        const float MOUSEOVER_COLORCYCLE_SPEED_;
        const std::size_t MOUSEOVER_COLORCYCLE_COUNT_;
        gui::BottomSymbol bottomSymbol_;

        gui::BoxEntityInfo listBoxInfo_;
        gui::StageTitle stageTitle_;
        gui::MainMenuBackground background_;
        gui::MainMenuButtonUPtr_t backButtonUPtr_;
        gui::MainMenuButtonUPtr_t startButtonUPtr_;
        gui::MainMenuButtonUPtr_t deleteButtonUPtr_;
        PartyListBoxUPtr_t characterListBoxUPtr_;
        PartyListBoxUPtr_t partyListBoxUPtr_;
        gui::TextRegionUPtr_t insTextRegionUPtr_;
        gui::TextRegionUPtr_t upTextRegionUPtr_;
        gui::TextRegionUPtr_t partyTextRegionUPtr_;
        gui::TextInfo warningTextInfo_;
        gui::TextRegionUPtr_t warningTextRegionUPtr_;
        gui::SliderOscillator<sf::Uint8, float> warningTextSlider_;
        gui::OuroborosUPtr_t ouroborosUPtr_;
        bool willDisplayCharacterCountWarningText_;
        creature::CreaturePVec_t unplayedCharactersPVec_;
        gui::ColoredRectSlider listboxSelectedItemColoredRectSlider_;

        // mouseover popup animation members
        bool willShowMouseOverPopup_;
        float mouseOverPopupTimerSec_;
        gui::ColoredRect mouseOverBackground_;
        sf::Vector2f mousePosV_;
        sf::FloatRect mouseOverBackgroundRectFinal_;
        sf::Sprite mouseOverCreatureSprite_;
        gui::CachedTextureOpt_t mouseOverCreatureTextureOpt_;
        gui::TextRegionUPtr_t mouseOverTextRegionUPtr_;
        gui::SliderZeroToOne mouseOverSlider_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_PARTYSTAGE_HPP_INCLUDED
