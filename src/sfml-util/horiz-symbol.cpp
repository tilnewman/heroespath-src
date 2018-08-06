// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// horiz-symbol.cpp
//
#include "horiz-symbol.hpp"

#include "sfml-util/display.hpp"
#include "sfml-util/sfml-util-fitting.hpp"
#include "sfml-util/sfml-util-vertex.hpp"

namespace heroespath
{
namespace sfml_util
{

    const sf::Color BottomSymbol::DEFAULT_COLOR_ { sf::Color(255, 255, 255, 127) };

    BottomSymbol::BottomSymbol(
        const float VERT_SCALE, const bool WILL_INVERT_COLOR, const sf::Color & COLOR)
        : cachedTextureOpt_()
        , vertexArray_(sf::PrimitiveType::Quads)
        , region_(0.0f, 0.0f, 0.0f, 0.0f)
    {
        Setup(VERT_SCALE, WILL_INVERT_COLOR, COLOR);
    }

    void BottomSymbol::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(vertexArray_, states);
    }

    void BottomSymbol::Setup(
        const float VERT_SCALE, const bool WILL_INVERT_COLOR, const sf::Color & COLOR)
    {
        cachedTextureOpt_ = CachedTexture(
            "media-images-gui-accents-symbol1",
            ImageOpt::Default | ((WILL_INVERT_COLOR) ? ImageOpt::Invert : ImageOpt::None));

        const sf::FloatRect TEXTURE_REGION(
            sf::Vector2f(0.0f, 0.0f), sf::Vector2f(cachedTextureOpt_->Get().getSize()));

        const auto IMAGE_HEIGHT { ScreenRatioToPixelsVert(0.137f) * VERT_SCALE };
        const auto IMAGE_SIZE_V { FitCopy(Size(TEXTURE_REGION), sf::Vector2f(0.0f, IMAGE_HEIGHT)) };
        auto const POS_TOP { (sfml_util::Display::Instance()->GetWinHeight() - IMAGE_SIZE_V.y) };

        auto const PAD { 8.0f };
        auto const THREE_PADS { PAD * 3.0f };
        auto const HALF_SCREEN_WIDTH { sfml_util::Display::Instance()->GetWinWidth() * 0.5f };

        const sf::Vector2f POS_1_V(((HALF_SCREEN_WIDTH - IMAGE_SIZE_V.x) + PAD), POS_TOP);
        const sf::Vector2f POS_2_V((HALF_SCREEN_WIDTH - PAD), POS_TOP);

        const sf::Vector2f POS_3_V(
            ((HALF_SCREEN_WIDTH - (IMAGE_SIZE_V.x * 2.0f)) + THREE_PADS), POS_TOP);

        const sf::Vector2f POS_4_V(((HALF_SCREEN_WIDTH + IMAGE_SIZE_V.x) - THREE_PADS), POS_TOP);

        AppendVertexesForQuad(vertexArray_, POS_1_V, TEXTURE_REGION, COLOR);
        AppendVertexesForQuad(vertexArray_, POS_2_V, TEXTURE_REGION, COLOR);
        AppendVertexesForQuad(vertexArray_, POS_3_V, TEXTURE_REGION, COLOR);
        AppendVertexesForQuad(vertexArray_, POS_4_V, TEXTURE_REGION, COLOR);

        region_ = vertexArray_.getBounds();
    }

    const sf::Color BottomSymbol::Color() const
    {
        if (vertexArray_.getVertexCount() > 0)
        {
            return vertexArray_[0].color;
        }
        else
        {
            return defaults::None;
        }
    }

    void BottomSymbol::Color(const sf::Color & NEW_COLOR)
    {
        const auto VERTEX_COUNT { vertexArray_.getVertexCount() };
        for (std::size_t i(0); i < VERTEX_COUNT; ++i)
        {
            vertexArray_[i].color = NEW_COLOR;
        }
    }

    void BottomSymbol::SetPos(const float POS_LEFT, const float POS_TOP)
    {
        if (vertexArray_.getVertexCount() > 0)
        {
            MovePos(POS_LEFT - vertexArray_[0].position.x, POS_TOP - vertexArray_[0].position.y);
        }

        region_.left = POS_LEFT;
        region_.top = POS_TOP;
    }

    void BottomSymbol::MovePos(const float HORIZ, const float VERT)
    {
        const sf::Vector2f MOVE_V(HORIZ, VERT);

        const auto VERTEX_COUNT { vertexArray_.getVertexCount() };
        for (std::size_t i(0); i < VERTEX_COUNT; ++i)
        {
            vertexArray_[i].position += MOVE_V;
        }

        region_.left += HORIZ;
        region_.top += VERT;
    }

} // namespace sfml_util
} // namespace heroespath
