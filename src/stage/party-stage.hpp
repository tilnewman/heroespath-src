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
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/box-entity.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/color-shaker-rect.hpp"
#include "sfml-util/colored-rect.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/image-text-entity.hpp"
#include "sfml-util/list-box.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/ouroboros.hpp"
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
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;

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
        , public sfml_util::ListBox<PartyStage, creature::CreaturePtr_t>::Callback_t::IHandler_t
        , public sfml_util::ImageTextEntity::Callback_t::IHandler_t
        , public sfml_util::PopupCallback_t::IHandler_t
    {
        using PartyListBox_t = sfml_util::ListBox<PartyStage, creature::CreaturePtr_t>;
        using PartyListBoxUPtr_t = std::unique_ptr<PartyListBox_t>;

    public:
        PartyStage(const PartyStage &) = delete;
        PartyStage(PartyStage &&) = delete;
        PartyStage & operator=(const PartyStage &) = delete;
        PartyStage & operator=(PartyStage &&) = delete;

        PartyStage();
        virtual ~PartyStage();

        bool HandleCallback(const PartyListBox_t::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const sfml_util::ImageTextEntity::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t &) override;
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

        const sfml_util::MouseImageInfo MakeMouseImageInfoForMenuButton(
            const std::string & IMAGE_PATH_KEY_UP, const std::string & IMAGE_PATH_KEY_OVER);

    private:
        static const std::string POPUP_NAME_STR_NOT_ENOUGH_CHARS_;
        static const std::string POPUP_NAME_STR_TOO_MANY_CHARS_;
        static const std::string POPUP_NAME_STR_DELETE_CONFIRM_;
        static const std::string POPUP_NAME_STR_PARTY_IMAGE_SELECT_;
        static const float MOUSEOVER_POPUP_DELAY_SEC_;
        static const float MOUSEOVER_SLIDER_SPEED_;
        static const float MOUSEOVER_BACKGROUND_FINAL_ALPHA_;
        //
        const sfml_util::GuiFont::Enum LISTBOX_FONT_ENUM_;
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
        sfml_util::BottomSymbol bottomSymbol_;

        sfml_util::BoxEntityInfo listBoxInfo_;
        sfml_util::StageTitle stageTitle_;
        sfml_util::BoxEntity backgroundBox_;
        sfml_util::MainMenuButtonUPtr_t backButtonUPtr_;
        sfml_util::ImageTextEntityUPtr_t startButtonUPtr_;
        sfml_util::ImageTextEntityUPtr_t deleteButtonUPtr_;
        PartyListBoxUPtr_t characterListBoxUPtr_;
        PartyListBoxUPtr_t partyListBoxUPtr_;
        sfml_util::TextRegionUPtr_t insTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t upTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t partyTextRegionUPtr_;
        sfml_util::TextInfo warningTextInfo_;
        sfml_util::TextRegionUPtr_t warningTextRegionUPtr_;
        sfml_util::sliders::Slider<sf::Uint8, float> warningTextSlider_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
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
        sfml_util::CachedTextureOpt_t mouseOverCreatureTextureOpt_;
        sfml_util::TextRegionUPtr_t mouseOverTextRegionUPtr_;
        sfml_util::sliders::ZeroSliderOnce<float> mouseOverSlider_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_PARTYSTAGE_HPP_INCLUDED
