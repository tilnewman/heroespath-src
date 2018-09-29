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
#include "misc/vector-map.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/colored-rect.hpp"
#include "sfml-util/gold-bar.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/image-text-entity.hpp"
#include "sfml-util/text-region.hpp"

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
        : public sfml_util::Entity
        , public sfml_util::ImageTextEntity::Callback_t::IHandler_t
    {
        AdventureCharacterList(const AdventureCharacterList &);
        AdventureCharacterList & operator=(const AdventureCharacterList &);

    public:
        explicit AdventureCharacterList(const sfml_util::IStagePtr_t);
        virtual ~AdventureCharacterList();

        bool HandleCallback(const sfml_util::ImageTextEntity::Callback_t::PacketPtr_t &) override
        {
            return false;
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
        sfml_util::IStagePtr_t stagePtr_;
        sfml_util::ImageTextEntityUVec_t namesButtonUVec_;
        sfml_util::TextRegionUVec_t condsTextRegionsUVec_;
        sfml_util::TextRegionUVec_t healthTextRegionsUVec_;
        sfml_util::TextRegionUVec_t manaTextRegionsUVec_;
        FloatRectVec_t imageColumnRects_;
        FloatRectVec_t nameColumnRects_;
        FloatRectVec_t healthColumnRects_;
        FloatRectVec_t manaColumnRects_;
        FloatRectVec_t conditionColumnRects_;
        std::vector<sf::Vertex> lineVerts_;
        std::vector<sfml_util::ColoredRect> colorGradientBars_;

        using ImagePair_t = std::pair<sfml_util::CachedTexture, sf::Sprite>;
        using CharImageMap_t = misc::VectorMap<creature::CreaturePtr_t, ImagePair_t>;
        CharImageMap_t charImages_;

        sfml_util::GoldBar charListSepLine_;
    };

    using AdventureCharacterListUPtr_t = std::unique_ptr<AdventureCharacterList>;

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ADVENTURESTAGECHARLIST_HPP_INCLUDED
