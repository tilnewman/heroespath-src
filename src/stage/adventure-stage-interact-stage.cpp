// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// adventure-stage-interact-stage.hpp
//
#include "adventure-stage-interact-stage.hpp"
#include "map/map.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "interact/i-interaction.hpp"


namespace heroespath
{
namespace stage
{

    const sf::Uint8 InteractStage::CONTEXT_IMAGE_ALPHA_{ 64 };
    const float InteractStage::SUBJECT_REGION_WIDTH_RATIO_{ 0.2f };
    const float InteractStage::SUBJECT_REGION_HEIGHT_RATIO_{ 0.5f };
    const float InteractStage::SUBJECT_IMAGE_PAD_RATIO_{ 0.9f };
    const float InteractStage::CONTEXT_IMAGE_PAD_RATIO_{ 0.8f };


    InteractStage::InteractStage(
        map::Map & map,
        const sf::FloatRect & STAGE_REGION,
        interact::InteractionManager & interactionManager)
    :
        Stage("AdventureInteract", STAGE_REGION),
        map_(map),
        interactionManager_(interactionManager),
        subjectSprite_(),
        contextSprite_(),
        textRegionUPtr_(std::make_unique<sfml_util::gui::TextRegion>("AdventureStage'sInteractStage's"))
    {}


    void InteractStage::Setup()
    {}


    void InteractStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        DrawInteraction(target);
        Stage::Draw(target, STATES);
    }


    void InteractStage::UpdateTime(const float)
    {
        if (interactionManager_.HasCurrentChanged())
        {
            SetupInteractionForDrawing(interactionManager_.Current());
        }
    }


    void InteractStage::SetupInteractionForDrawing(const interact::IInteraction * const INTERACTION_PTR)
    {
        if (INTERACTION_PTR != nullptr)
        {
            const sf::FloatRect SUBJECT_REGION{
                StageRegionLeft(),
                StageRegionTop(),
                StageRegionWidth() * SUBJECT_REGION_WIDTH_RATIO_,
                StageRegionHeight() * SUBJECT_REGION_HEIGHT_RATIO_ };

            const sf::FloatRect TEXT_REGION{
                StageRegionLeft() + SUBJECT_REGION.width,
                StageRegionTop(),
                StageRegionWidth() - SUBJECT_REGION.width,
                SUBJECT_REGION.height };

            const sf::FloatRect BUTTON_REGION{
                StageRegionLeft(),
                StageRegionTop() + SUBJECT_REGION.height,
                StageRegionWidth(),
                StageRegionHeight() - SUBJECT_REGION.height };

            const sf::FloatRect CONTEXT_REGION{
                TEXT_REGION.left,
                TEXT_REGION.top,
                TEXT_REGION.width,
                TEXT_REGION.height + BUTTON_REGION.height};

            contextSprite_.setTexture(INTERACTION_PTR->ContextTexture(), true);
            contextSprite_.setColor(sf::Color(255, 255, 255, CONTEXT_IMAGE_ALPHA_));

            sfml_util::CenterAndScaleSpriteToFit(
                contextSprite_,
                CONTEXT_REGION,
                CONTEXT_IMAGE_PAD_RATIO_);

            subjectSprite_.setTexture(INTERACTION_PTR->SubjectTexture(), true);

            sfml_util::CenterAndScaleSpriteToFit(
                subjectSprite_,
                SUBJECT_REGION,
                SUBJECT_IMAGE_PAD_RATIO_);

            textRegionUPtr_->Setup(INTERACTION_PTR->Text(), TEXT_REGION, this);
        }
    }


    void InteractStage::DrawInteraction(sf::RenderTarget & target) const
    {
        if (interactionManager_.Current() != nullptr)
        {
            target.draw(contextSprite_);
            target.draw(subjectSprite_);
            target.draw( * textRegionUPtr_ );
        }
    }

}
}
