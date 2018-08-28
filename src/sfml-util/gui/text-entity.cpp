// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// text-entity.cpp
//
#include "text-entity.hpp"

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/text-rendering.hpp"

#include <SFML/Graphics/RenderTexture.hpp>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        TextEntity::TextEntity(
            const std::string & NAME,
            const bool WILL_CACHE,
            const bool WILL_PLAY_MOUSEOVER_TICK_SFX)
            : Entity(std::string(NAME).append("_TextEntity1"), 0.0f, 0.0f)
            , mouseTextInfo_()
            , profileToTextureMap_()
            , renderTexturesUVec_()
            , sprite_()
            , textWidthLimit_(0.0f)
            , willCache_(WILL_CACHE)
            , willPlayMouseOverTickSfx_(WILL_PLAY_MOUSEOVER_TICK_SFX)
            , willDraw_(false)
        {}

        TextEntity::TextEntity(
            const std::string & NAME,
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const float TEXT_WIDTH_LIMIT,
            const bool WILL_CACHE,
            const bool WILL_PLAY_MOUSEOVER_TICK_SFX)
            : Entity(std::string(NAME).append("_TextEntity2"), POS_LEFT, POS_TOP)
            , mouseTextInfo_(MOUSE_TEXT_INFO)
            , profileToTextureMap_()
            , renderTexturesUVec_()
            , sprite_()
            , textWidthLimit_(TEXT_WIDTH_LIMIT)
            , willCache_(WILL_CACHE)
            , willPlayMouseOverTickSfx_(WILL_PLAY_MOUSEOVER_TICK_SFX)
            , willDraw_(false)
        {
            Setup(
                POS_LEFT,
                POS_TOP,
                MOUSE_TEXT_INFO,
                TEXT_WIDTH_LIMIT,
                WILL_CACHE,
                WILL_PLAY_MOUSEOVER_TICK_SFX);
        }

        TextEntity::~TextEntity() = default;

        void TextEntity::Setup(
            const float POS_LEFT,
            const float POS_TOP,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const float TEXT_WIDTH_LIMIT,
            const bool WILL_CACHE,
            const bool WILL_PLAY_MOUSEOVER_TICK_SFX)
        {
            willDraw_ = false;
            sprite_ = sf::Sprite();
            profileToTextureMap_.Clear();
            renderTexturesUVec_.clear();

            willCache_ = WILL_CACHE;
            willPlayMouseOverTickSfx_ = WILL_PLAY_MOUSEOVER_TICK_SFX;
            entityRegion_.left = POS_LEFT;
            entityRegion_.top = POS_TOP;
            mouseTextInfo_ = MOUSE_TEXT_INFO;
            textWidthLimit_ = TEXT_WIDTH_LIMIT;

            Sync();
        }

        void TextEntity::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            if (willDraw_)
            {
                target.draw(sprite_, states);
            }
        }

        bool TextEntity::SetMouseState(const MouseState::Enum MOUSE_STATE_NEW)
        {
            const auto MOUSE_STATE_ORIG { GetMouseState() };

            if (Entity::SetMouseState(MOUSE_STATE_NEW))
            {
                if (willPlayMouseOverTickSfx_)
                {
                    if (MOUSE_STATE_NEW == MouseState::Over)
                    {
                        SoundManager::Instance()->PlaySfx_TickOn();
                    }
                    else if (
                        (MOUSE_STATE_NEW != MouseState::Disabled)
                        && (MOUSE_STATE_ORIG == MouseState::Over))
                    {
                        SoundManager::Instance()->PlaySfx_TickOff();
                    }
                }

                Sync();
                return true;
            }

            return false;
        }

        void TextEntity::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            MoveEntityPos(POS_LEFT - entityRegion_.left, POS_TOP - entityRegion_.top);
        }

        void TextEntity::MoveEntityPos(const float HORIZ, const float VERT)
        {
            Entity::MoveEntityPos(HORIZ, VERT);
            sprite_.move(HORIZ, VERT);
        }

        void TextEntity::SetEntityRegion(const sf::FloatRect & R)
        {
            Entity::SetEntityRegion(R);
            SetSizeAndPos(sprite_, R);
        }

        void TextEntity::SetText(const std::string & TEXT)
        {
            if (mouseTextInfo_.up.text.empty() == false)
            {
                mouseTextInfo_.up.text = TEXT;
            }

            if (mouseTextInfo_.down.text.empty() == false)
            {
                mouseTextInfo_.down.text = TEXT;
            }

            if (mouseTextInfo_.over.text.empty() == false)
            {
                mouseTextInfo_.over.text = TEXT;
            }

            if (mouseTextInfo_.disabled.text.empty() == false)
            {
                mouseTextInfo_.disabled.text = TEXT;
            }
        }

        void TextEntity::Sync()
        {
            const TextInfo & TEXT_INFO { FromMouseState() };

            if (TEXT_INFO.IsValid())
            {
                const RenderProfile RENDER_PROFILE(
                    TEXT_INFO, mouseTextInfo_.numbers_font_ptr, textWidthLimit_);

                const auto FOUND_ITER { profileToTextureMap_.Find(RENDER_PROFILE) };

                if (FOUND_ITER == std::end(profileToTextureMap_))
                {
                    renderTexturesUVec_.emplace_back(std::make_unique<sf::RenderTexture>());

                    const auto RENDERED_TEXT { TextRenderer::Render(
                        TEXT_INFO,
                        sf::Vector2f(0.0f, 0.0f),
                        textWidthLimit_,
                        mouseTextInfo_.numbers_font_ptr) };

                    RENDERED_TEXT.CreateTextureAndRenderOffscreen(*renderTexturesUVec_.back());

                    const TextureCPtr_t TEXTURE_CPTR { &renderTexturesUVec_.back()->getTexture() };

                    if (willCache_)
                    {
                        profileToTextureMap_.Append(RENDER_PROFILE, TEXTURE_CPTR);
                    }

                    sprite_.setTexture(*TEXTURE_CPTR, true);
                }
                else
                {
                    sprite_.setTexture(*FOUND_ITER->second, true);
                }

                SetEntityRegion(sprite_.getGlobalBounds());
                willDraw_ = true;
            }
            else
            {
                willDraw_ = false;
            }
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
