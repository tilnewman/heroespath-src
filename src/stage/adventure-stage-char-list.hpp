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
#ifndef HEROESPATH_STAGE_ADVENTURESTAGECHARLIST_HPP_INCLUDED
#define HEROESPATH_STAGE_ADVENTURESTAGECHARLIST_HPP_INCLUDED
//
// adventure-stage-char-list.hpp
//
#include "sfml-util/gui/four-state-button.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/line.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include "misc/vector-map.hpp"
#include "player/character.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace stage
{

    // Responsible for displaying and managing player interactions with a
    // list of characters and associated information about them.
    class AdventureCharacterList
        : public sfml_util::gui::GuiEntity
        , public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
        AdventureCharacterList(const AdventureCharacterList &);
        AdventureCharacterList & operator=(const AdventureCharacterList &);

    public:
        AdventureCharacterList(sfml_util::IStage *);
        virtual ~AdventureCharacterList();

        virtual inline const std::string HandlerName() const override { return GetEntityName(); }

        virtual bool HandleCallback(
            const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &) override;

        virtual void draw(sf::RenderTarget &, sf::RenderStates) const override;

        virtual void SetEntityPos(const float LEFT, const float TOP) override;

        void Setup();

    protected:
        virtual void OnClick(const sf::Vector2f &) override {}

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

        const std::string NameButtonMouseoverText(const player::CharacterPtr_t);

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
        sfml_util::IStage * stagePtr_;
        sfml_util::gui::FourStateButtonUVec_t namesButtonUVec_;
        sfml_util::gui::TextRegionUVec_t condsTextRegionsUVec_;
        sfml_util::gui::TextRegionUVec_t healthTextRegionsUVec_;
        sfml_util::gui::TextRegionUVec_t manaTextRegionsUVec_;
        sfml_util::FloatRectVec_t imageColumnRects_;
        sfml_util::FloatRectVec_t nameColumnRects_;
        sfml_util::FloatRectVec_t healthColumnRects_;
        sfml_util::FloatRectVec_t manaColumnRects_;
        sfml_util::FloatRectVec_t conditionColumnRects_;
        std::vector<sf::Vertex> lineVerts_;
        std::vector<sf::Vertex> quadVerts_;

        using ImagePair_t = std::pair<sf::Texture, sf::Sprite>;
        using CharImageMap_t = misc::VectorMap<player::CharacterPtr_t, ImagePair_t>;
        CharImageMap_t charImages_;

        sfml_util::gui::Line charListSepLine_;
    };

    using AdventureCharacterListUPtr_t = std::unique_ptr<AdventureCharacterList>;
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ADVENTURESTAGECHARLIST_STAGE_HPP_INCLUDED
