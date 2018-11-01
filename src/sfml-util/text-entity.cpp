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

#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "sfml-util/mouse-text-info.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/text-renderer.hpp"
#include "sfutil/size-and-scale.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/RenderTexture.hpp>

namespace heroespath
{
namespace gui
{

    TextEntity::TextEntity(
        const std::string & NAME, const bool WILL_CACHE, const bool WILL_PLAY_MOUSEOVER_TICK_SFX)
        : Entity(std::string(NAME).append("_TextEntity1"), 0.0f, 0.0f)
        , mouseTextInfo_()
        , cacheMap_()
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
        , cacheMap_()
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
        cacheMap_.Clear();

        willCache_ = WILL_CACHE;
        willPlayMouseOverTickSfx_ = WILL_PLAY_MOUSEOVER_TICK_SFX;
        entityRegion_.left = POS_LEFT;
        entityRegion_.top = POS_TOP;
        mouseTextInfo_ = MOUSE_TEXT_INFO;
        textWidthLimit_ = TEXT_WIDTH_LIMIT;

        TextEntity::Sync();
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

        // TODO this is probably wrong, if the region size grows/shrinks then the cached text
        // image should have whitespace added or be clipped, I don't think scaling the text
        // image size is the right thing to do here...
        sfutil::SetSizeAndPos(sprite_, R);
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

        Sync();
    }

    void TextEntity::Sync()
    {
        const auto CURRENT_MOUSE_STATE { GetMouseState() };

        willDraw_ = false;

        // make sure all text is rendered and cached
        for (misc::EnumUnderlying_t mouseStateIndex(0); mouseStateIndex < MouseState::Count;
             ++mouseStateIndex)
        {
            const auto LOOP_MOUSE_STATE { static_cast<MouseState::Enum>(mouseStateIndex) };
            const TextInfo & TEXT_INFO { mouseTextInfo_.FromMouseState(LOOP_MOUSE_STATE) };

            if (TEXT_INFO.WillDraw() == false)
            {
                continue;
            }

            auto finalRegion { entityRegion_ };

            auto & renderTextureUPtr { cacheMap_[TEXT_INFO] };

            if (!renderTextureUPtr)
            {
                // make sure the region has size zero if rendering otherwise the size will limit the
                // rendered text
                finalRegion.width = 0.0f;
                finalRegion.height = 0.0f;

                const auto RENDER_RESULT { TextRenderer::ToTexture(
                    TEXT_INFO, finalRegion, renderTextureUPtr, finalRegion) };

                if ((RENDER_RESULT == false) || !renderTextureUPtr)
                {
                    cacheMap_.Erase(TEXT_INFO);
                    continue;
                }
            }

            if (LOOP_MOUSE_STATE == CURRENT_MOUSE_STATE)
            {
                sprite_.setTexture(renderTextureUPtr->getTexture());

                const sf::Vector2u RENDERED_TEXT_SIZE_V_UINT { sfutil::Size(finalRegion) };

                const auto WIDTH_LIMIT_UINT { (
                    (textWidthLimit_ < 1.0f) ? 0 : static_cast<unsigned>(textWidthLimit_)) };

                const auto FINAL_WIDTH_UINT { (
                    (WIDTH_LIMIT_UINT == 0)
                        ? RENDERED_TEXT_SIZE_V_UINT.x
                        : std::min(WIDTH_LIMIT_UINT, RENDERED_TEXT_SIZE_V_UINT.x)) };

                const auto FINAL_HEIGHT_UINT { RENDERED_TEXT_SIZE_V_UINT.y };

                const sf::Vector2i FINAL_TEXTURE_SIZE_V_INT { sf::Vector2u(
                    FINAL_WIDTH_UINT, FINAL_HEIGHT_UINT) };

                const sf::IntRect FINAL_TEXTURE_IRECT(sf::Vector2i(), FINAL_TEXTURE_SIZE_V_INT);

                sprite_.setTextureRect(FINAL_TEXTURE_IRECT);
                sprite_.setPosition(sfutil::Position(finalRegion));
                sprite_.setColor(TEXT_INFO.color);

                entityRegion_ = finalRegion;
                willDraw_ = true;
            }
        }
    }

} // namespace gui
} // namespace heroespath
