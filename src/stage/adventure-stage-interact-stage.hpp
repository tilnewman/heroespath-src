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
#ifndef HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
#define HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
//
// adventure-stage-interact-stage.hpp
//
#include "interact/interaction-manager.hpp"
#include "interact/lock-interactions.hpp"
#include "map/transition.hpp"
#include "popup/i-popup-callback.hpp"
#include "sfml-util/gui/text-button.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"
#include <memory>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
} // namespace creature
namespace sfml_util
{
    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    } // namespace gui
} // namespace sfml_util
namespace map
{
    class Map;
}
namespace stage
{

    // Responsible for the interaction region of the Adventure Stage.
    class InteractStage
        : public sfml_util::Stage
        , public sfml_util::gui::callback::ITextButtonCallbackHandler_t
        , public popup::IPopupHandler_t
    {
        InteractStage(const InteractStage &) = delete;
        InteractStage(const InteractStage &&) = delete;
        InteractStage & operator=(const InteractStage &) = delete;
        InteractStage & operator=(const InteractStage &&) = delete;

    public:
        InteractStage(
            map::Map &, const sf::FloatRect & STAGE_REGION, interact::InteractionManager &);

        virtual ~InteractStage();

        virtual const std::string HandlerName() const final { return GetStageName(); }

        virtual bool
            HandleCallback(const sfml_util::gui::callback::TextButtonCallbackPackage_t &) final;

        virtual bool HandleCallback(const popup::PopupResponse &) override;

        virtual void Setup() override;
        virtual void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        virtual bool KeyRelease(const sf::Event::KeyEvent &) override;

        interact::InteractionManager & InteractionManager() { return interactionManager_; }

        interact::LockPicking & LockPick() { return lockPicking_; }

        void MapTransition(const map::Transition &);

    private:
        void SetupInteractionForDrawing(interact::IInteraction * const);
        void DrawInteraction(sf::RenderTarget &) const;

    private:
        static const sf::Uint8 CONTEXT_IMAGE_ALPHA_;
        static const float SUBJECT_REGION_WIDTH_RATIO_;
        static const float SUBJECT_REGION_HEIGHT_RATIO_;
        static const float SUBJECT_IMAGE_PAD_RATIO_;
        static const float CONTEXT_IMAGE_PAD_RATIO_;

        map::Map & map_;
        interact::InteractionManager & interactionManager_;
        sf::Sprite subjectSprite_;
        sf::Sprite contextSprite_;
        sfml_util::gui::TextRegionUPtr_t textRegionUPtr_;
        std::vector<sfml_util::gui::TextButtonUPtr_t> buttons_;
        interact::LockPicking lockPicking_;
    };
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
