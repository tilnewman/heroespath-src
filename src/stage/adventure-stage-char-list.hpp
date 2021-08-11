// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_ADVENTURESTAGECHARLIST_HPP_INCLUDED
#define HEROESPATH_STAGE_ADVENTURESTAGECHARLIST_HPP_INCLUDED
//
// adventure-stage-char-list.hpp
//
#include "creature/creature.hpp"
#include "gui/cached-texture.hpp"
#include "gui/colored-rect.hpp"
#include "gui/gold-bar.hpp"
#include "gui/image-text-entity.hpp"
#include "gui/text-region.hpp"
#include "misc/vector-map.hpp"
#include "stage/i-stage.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace stage
{

    // Responsible for displaying and managing player interactions with a
    // list of characters and associated information about them.
    class AdventureCharacterList
        : public gui::Entity
        , public gui::ImageTextEntity::Callback_t::IHandler_t
    {
        AdventureCharacterList(const AdventureCharacterList &);
        AdventureCharacterList & operator=(const AdventureCharacterList &);

    public:
        explicit AdventureCharacterList(const stage::IStagePtr_t);
        virtual ~AdventureCharacterList();

        const std::string HandleCallback(
            const gui::ImageTextEntity::Callback_t::Packet_t &, const std::string &) override
        {
            return "";
        }

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        void SetEntityPos(const float LEFT, const float TOP) override;

        void Setup();

    private:
        void SetupNameButtons();
        void SetupHealthNumbersText();
        void SetupManaNumbersText();
        void SetupConditionsText();
        void SetupColumnRects_Image();
        void SetupColumnRects_Name();
        void SetupColumnRects_Health();
        void SetupColumnRects_Mana();
        void SetupColumnRects_Condition();
        void SetupPositions_NameButtons();
        void SetupPositions_HealthBars();
        void SetupPositions_ManaBars();
        void SetupPositions_Conditions();
        void SetupPositions_OverallRegion();
        void SetupCellDividerLines();
        void SetupMouseoverText();
        void SetupCharacterImages();

        const std::string NameButtonMouseoverText(const creature::CreaturePtr_t);

        enum class WhichBar
        {
            Health,
            Mana
        };

        void SetupPositions_Bars(const WhichBar);

        const sf::Color FadedDarkColor_Line() const;
        const sf::Color FadedDarkColor_Text() const;
        const sf::Color FadedDarkColor_CharacterImages() const;

    private:
        const sf::Uint8 ALPHA_FOR_LINES_;
        const sf::Uint8 ALPHA_FOR_TEXT_;
        const sf::Uint8 ALPHA_FOR_CHAR_IMAGES_;
        const sf::Uint8 ALPHA_FOR_COLORED_BARS_;
        const float CELL_LINE_THICKNESS_;
        const float OUTER_SPACER_;
        const float HEALTH_COLUMN_WIDTH_;
        const float MANA_COLUMN_WIDTH_;
        const float CELL_TEXT_LEFT_SPACER_;
        const float CHARLIST_SEP_SPACER_;
        stage::IStagePtr_t stagePtr_;
        gui::ImageTextEntityUVec_t namesButtonUVec_;
        gui::TextRegionUVec_t condsTextRegionsUVec_;
        gui::TextRegionUVec_t healthTextRegionsUVec_;
        gui::TextRegionUVec_t manaTextRegionsUVec_;
        FloatRectVec_t imageColumnRects_;
        FloatRectVec_t nameColumnRects_;
        FloatRectVec_t healthColumnRects_;
        FloatRectVec_t manaColumnRects_;
        FloatRectVec_t conditionColumnRects_;
        std::vector<sf::Vertex> lineVerts_;
        std::vector<gui::ColoredRect> colorGradientBars_;

        using ImagePair_t = std::pair<gui::CachedTexture, sf::Sprite>;
        using CharImageMap_t = misc::VectorMap<creature::CreaturePtr_t, ImagePair_t>;
        CharImageMap_t charImages_;

        gui::GoldBar charListSepLine_;
    };

    using AdventureCharacterListUPtr_t = std::unique_ptr<AdventureCharacterList>;

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ADVENTURESTAGECHARLIST_HPP_INCLUDED
