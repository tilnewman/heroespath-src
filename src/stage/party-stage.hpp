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
//  A Stage class that allows grouping of saved characters into a party.
//
#include "avatar/avatar-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/i-popup-callback.hpp"
#include "sfml-util/color-shaker-rect.hpp"
#include "sfml-util/colored-rect.hpp"
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
#include <string>
#include <vector>

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
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePtrOpt_t = boost::optional<CreaturePtr_t>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
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

        PartyStage();
        virtual ~PartyStage();

        const std::string HandlerName() const override { return GetStageName(); }
        bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &) override;

        bool HandleCallback(
            const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &) override;

        bool HandleCallback(const popup::PopupResponse &) override;
        bool HandleCallback_BackButton();
        bool HandleCallback_StartButton();
        bool HandleCallback_DeleteButton();

        void Setup() override;
        void Setup_Ouroboros();
        void Setup_Buttons();
        void Setup_TopInstructionText();
        void Setup_TopWarningText();
        void Setup_CharactersListBoxLabel();
        void Setup_PartyListBoxLabel();
        void Setup_CharactersListBox();
        void Setup_PartyListBox();

        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;

        sfml_util::gui::ListBoxItemPtrOpt_t GetSelectedItemPtrOpt() const;
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
        bool AreAnyCharactersBeasts() const;
        bool AreAnyCharactersBeastmasters() const;
        void MissingBeastmasterPopup();
        void NotEnoughCharactersPopup();
        void PartyAvatarSelectionPopup();
        void UpdateWillDisplayCharacterCountWarning();
        void SetupMouseOverPositionsAndDimmensions(const creature::CreaturePtr_t);

    private:
        static const std::string POPUP_NAME_STR_NOT_ENOUGH_CHARS_;
        static const std::string POPUP_NAME_STR_TOO_MANY_CHARS_;
        static const std::string POPUP_NAME_STR_DELETE_CONFIRM_;
        static const std::string POPUP_NAME_STR_PARTY_IMAGE_SELECT_;
        static const float MOUSEOVER_POPUP_DELAY_SEC_; 
        static const float MOUSEOVER_SLIDER_SPEED_;
        static const float MOUSEOVER_BACKGROUND_FINAL_ALPHA_;
        //
        const sfml_util::Font::Enum LISTBOX_FONT_ENUM_;
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
        sfml_util::gui::box::Info listBoxInfo_;
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
        creature::CreaturePVec_t unplayedCharactersPVec_;
        sfml_util::ColorShakerRect colorShakerRect_;

        // mouseover popup animation members
        bool willShowMouseOverPopup_;
        float mouseOverPopupTimerSec_;
        sfml_util::ColoredRect mouseOverBackground_;
        sf::Vector2f mousePosV_;
        sf::FloatRect mouseOverBackgroundRectFinal_;
        sf::Sprite mouseOverCreatureSprite_;
        sf::Texture mouseOverCreatureTexture_;
        sfml_util::gui::TextRegionUPtr_t mouseOverTextRegionUPtr_;
        sfml_util::sliders::ZeroSliderOnce<float> mouseOverSlider_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_PARTYSTAGE_HPP_INCLUDED
