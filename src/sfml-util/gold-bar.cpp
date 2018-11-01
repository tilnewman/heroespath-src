// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// gold-bar.cpp
//
#include "gold-bar.hpp"

#include "misc/log-macros.hpp"
#include "sfml-util/gui-images.hpp"
#include "sfutil/display.hpp"
#include "sfutil/position.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>

namespace heroespath
{
namespace gui
{

    const std::string GoldBar::IMAGE_PATH_KEY_ { "media-images-gui-elements" };

    GoldBar::GoldBar()
        : length_(0.0f)
        , innerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
        , outerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
        , vertexArray_(sf::PrimitiveType::Quads)
        , guiElementsCachedTexture_(IMAGE_PATH_KEY_)
        , willGrowBorderToContainRegion_(false)
        , texturePtr_(&guiElementsCachedTexture_.Get())
    {}

    GoldBar::GoldBar(
        const float POS_LEFT,
        const float POS_TOP,
        const float LENGTH,
        Orientation::Enum ORIENTATION,
        const Side::Enum SIDE,
        const bool WILL_CAP_ENDS)
        : length_(LENGTH)
        , innerRegion_(POS_LEFT, POS_TOP, LENGTH, LENGTH)
        , outerRegion_(POS_LEFT, POS_TOP, LENGTH, LENGTH)
        , vertexArray_(sf::PrimitiveType::Quads)
        , guiElementsCachedTexture_(IMAGE_PATH_KEY_)
        , willGrowBorderToContainRegion_(false)
        , texturePtr_(&guiElementsCachedTexture_.Get())
    {
        Setup(POS_LEFT, POS_TOP, LENGTH, ORIENTATION, SIDE, WILL_CAP_ENDS);
    }

    GoldBar::GoldBar(const std::vector<GoldBar> & GOLD_BARS, const Orientation::Enum ORIENTATION)
        : length_(0.0f)
        , innerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
        , outerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
        , vertexArray_(sf::PrimitiveType::Quads)
        , guiElementsCachedTexture_(IMAGE_PATH_KEY_)
        , willGrowBorderToContainRegion_(false)
        , texturePtr_(&guiElementsCachedTexture_.Get())
    {
        std::vector<sf::FloatRect> outerRegions;
        std::vector<sf::FloatRect> innerRegions;

        for (const auto & GOLD_BAR : GOLD_BARS)
        {
            const auto VERTEX_COUNT { GOLD_BAR.vertexArray_.getVertexCount() };
            for (std::size_t i(0); i < VERTEX_COUNT; ++i)
            {
                outerRegions.emplace_back(GOLD_BAR.OuterRegion());
                innerRegions.emplace_back(GOLD_BAR.InnerRegion());
                vertexArray_.append(GOLD_BAR.vertexArray_[i]);
            }
        }

        outerRegion_ = sfutil::MinimallyEnclosing(outerRegions);
        innerRegion_ = sfutil::MinimallyEnclosing(innerRegions);
        SetupLengthToDefault(ORIENTATION);
    }

    GoldBar::GoldBar(const sf::VertexArray & VERTEX_ARRAY, const Orientation::Enum ORIENTATION)
        : length_(0.0f)
        , innerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
        , outerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
        , vertexArray_(sf::PrimitiveType::Quads)
        , guiElementsCachedTexture_(IMAGE_PATH_KEY_)
        , willGrowBorderToContainRegion_(false)
        , texturePtr_(&guiElementsCachedTexture_.Get())
    {
        const auto VERTEX_COUNT { VERTEX_ARRAY.getVertexCount() };
        for (std::size_t i(0); i < VERTEX_COUNT; ++i)
        {
            vertexArray_.append(VERTEX_ARRAY[i]);
        }

        outerRegion_ = innerRegion_ = vertexArray_.getBounds();
        SetupLengthToDefault(ORIENTATION);
    }

    GoldBar::GoldBar(
        const sf::FloatRect & REGION,
        const Orientation::Enum ORIENTATION,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
        : length_(0.0f)
        , innerRegion_(REGION)
        , outerRegion_(REGION)
        , vertexArray_(sf::PrimitiveType::Quads)
        , guiElementsCachedTexture_(IMAGE_PATH_KEY_)
        , willGrowBorderToContainRegion_(WILL_GROW_BORDER_TO_CONTAIN_REGION)
        , texturePtr_(&guiElementsCachedTexture_.Get())
    {
        Setup(REGION, ORIENTATION, WILL_GROW_BORDER_TO_CONTAIN_REGION);
    }

    void GoldBar::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        states.texture = texturePtr_.Ptr();
        target.draw(vertexArray_, states);
    }

    void GoldBar::Setup(
        const float POS_LEFT,
        const float POS_TOP,
        const float LENGTH,
        const Orientation::Enum ORIENTATION,
        const Side::Enum SIDE,
        const bool WILL_CAP_ENDS)
    {
        length_ = LENGTH;
        innerRegion_ = sf::FloatRect(POS_LEFT, POS_TOP, LENGTH, LENGTH);
        outerRegion_ = sf::FloatRect(POS_LEFT, POS_TOP, LENGTH, LENGTH);
        vertexArray_.clear();

        if (LENGTH > 0.0f)
        {
            SetupVertexesAndRegionsForSingleBar(
                POS_LEFT, POS_TOP, LENGTH, ORIENTATION, SIDE, WILL_CAP_ENDS);

            SetupLengthToDefault(ORIENTATION);
        }
    }

    void GoldBar::Setup(
        const sf::FloatRect & REGION,
        const Orientation::Enum ORIENTATION,
        const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
    {
        length_ = 0.0f;
        innerRegion_ = REGION;
        outerRegion_ = REGION;
        vertexArray_.clear();
        willGrowBorderToContainRegion_ = WILL_GROW_BORDER_TO_CONTAIN_REGION;

        SetupVertexesAndRegionsForSquare(REGION, WILL_GROW_BORDER_TO_CONTAIN_REGION);
        SetupLengthToDefault(ORIENTATION);
    }

    const sf::Color GoldBar::Color() const
    {
        if (WillDraw())
        {
            return vertexArray_[0].color;
        }
        else
        {
            return sf::Color::Transparent;
        }
    }

    void GoldBar::Color(const sf::Color & NEW_COLOR)
    {
        const auto VERTEX_COUNT { vertexArray_.getVertexCount() };
        for (std::size_t i(0); i < VERTEX_COUNT; ++i)
        {
            vertexArray_[i].color = NEW_COLOR;
        }
    }

    void GoldBar::SetPos(const float POS_LEFT, const float POS_TOP)
    {
        const auto DIFF_V { sf::Vector2f(POS_LEFT, POS_TOP) - sfutil::Position(outerRegion_) };

        MovePos(DIFF_V.x, DIFF_V.y);
    }

    void GoldBar::MovePos(const float HORIZ, const float VERT)
    {
        const auto DIFF_V { sf::Vector2f(HORIZ, VERT) };

        outerRegion_
            = sf::FloatRect(sfutil::Position(outerRegion_) + DIFF_V, sfutil::Size(outerRegion_));
        innerRegion_
            = sf::FloatRect(sfutil::Position(innerRegion_) + DIFF_V, sfutil::Size(innerRegion_));

        const auto VERTEX_COUNT { vertexArray_.getVertexCount() };
        for (std::size_t i(0); i < VERTEX_COUNT; ++i)
        {
            vertexArray_[i].position += DIFF_V;
        }
    }

    const std::tuple<sf::FloatRect, sf::FloatRect, sf::FloatRect> GoldBar::GetTextureRectsBar(
        const Orientation::Enum ORIENTATION_PARAM, const Side::Enum SIDE) const
    {
        const auto ORIENTATION { (
            (ORIENTATION_PARAM == Orientation::Count) ? Orientation::Horiz : ORIENTATION_PARAM) };

        sf::FloatRect topOrLeftRect;
        sf::FloatRect botOrRightRect;
        sf::FloatRect middleRect;

        // which spirtes to use, and the length calculations
        if (ORIENTATION == Orientation::Horiz)
        {
            if (SIDE == Side::Top)
            {
                middleRect = GuiImages::LineSmallHorizontalTopF();
            }
            else
            {
                middleRect = GuiImages::LineSmallHorizontalBotF();
            }

            topOrLeftRect = GuiImages::LineSmallEndLeftF();
            botOrRightRect = GuiImages::LineSmallEndRightF();
        }
        else
        {
            if (SIDE == Side::Left)
            {
                middleRect = GuiImages::LineSmallVerticalLeftF();
            }
            else
            {
                middleRect = GuiImages::LineSmallVerticalRightF();
            }

            topOrLeftRect = GuiImages::LineSmallEndTopF();
            botOrRightRect = GuiImages::LineSmallEndBotF();
        }

        return std::make_tuple(topOrLeftRect, botOrRightRect, middleRect);
    }

    void GoldBar::SetupVertexesAndRegionsForSingleBar(
        const float LEFT,
        const float TOP,
        const float LENGTH_ORIG,
        Orientation::Enum ORIENTATION_PARAM,
        const Side::Enum SIDE,
        const bool WILL_CAP_ENDS)
    {
        const auto ORIENTATION { (
            (ORIENTATION_PARAM == Orientation::Count) ? Orientation::Horiz : ORIENTATION_PARAM) };

        const auto LENGTH = [&]() {
            if (LENGTH_ORIG < 0.0f)
            {
                return 0.0f;
            }
            else
            {
                if (ORIENTATION == Orientation::Horiz)
                {
                    return std::floor(LEFT + LENGTH_ORIG) - std::floor(LEFT);
                }
                else
                {
                    return std::floor(TOP + LENGTH_ORIG) - std::floor(TOP);
                }
            }
        }();

        length_ = LENGTH_ORIG;

        vertexArray_.clear();

        if (LENGTH < 1.0f)
        {
            return;
        }

        const auto POS_V { sf::Vector2f(std::floor(LEFT), std::floor(TOP)) };

        const auto [TEXTURE_RECT_TOP_OR_LEFT, TEXTURE_RECT_BOT_OR_RIGHT, TEXTURE_RECT_MIDDLE]
            = GetTextureRectsBar(ORIENTATION, SIDE);

        const auto [END_CAP_LENGTH_TOP_OR_LEFT, END_CAP_LENGTH_BOT_OR_RIGHT] =
            [&,
             TEXTURE_RECT_TOP_OR_LEFT = TEXTURE_RECT_TOP_OR_LEFT,
             TEXTURE_RECT_BOT_OR_RIGHT = TEXTURE_RECT_BOT_OR_RIGHT]() {
                float lenTopOrLeft { 0.0f };
                float lenBotOrRight { 0.0f };

                if (WILL_CAP_ENDS)
                {
                    lenTopOrLeft
                        = ((ORIENTATION == Orientation::Horiz) ? TEXTURE_RECT_TOP_OR_LEFT.width
                                                               : TEXTURE_RECT_TOP_OR_LEFT.height);

                    lenBotOrRight
                        = ((ORIENTATION == Orientation::Horiz) ? TEXTURE_RECT_BOT_OR_RIGHT.width
                                                               : TEXTURE_RECT_BOT_OR_RIGHT.height);

                    if (LENGTH < (lenTopOrLeft + lenBotOrRight))
                    {
                        const auto END_CAP_LENGTH_PARTIAL { LENGTH * 0.5f };

                        if (END_CAP_LENGTH_PARTIAL < 1.0f)
                        {
                            lenTopOrLeft = 0.0f;
                            lenBotOrRight = 0.0f;
                        }
                        else
                        {
                            lenTopOrLeft = END_CAP_LENGTH_PARTIAL;
                            lenBotOrRight = END_CAP_LENGTH_PARTIAL;
                        }
                    }
                }

                return std::make_tuple(lenTopOrLeft, lenBotOrRight);
            }();

        if ((END_CAP_LENGTH_TOP_OR_LEFT < 1.0f) == false)
        {
            sfutil::AppendVertexesForQuad(
                vertexArray_,
                POS_V,
                TEXTURE_RECT_TOP_OR_LEFT,
                sf::Color::White,
                sfutil::CreateVector(ORIENTATION, END_CAP_LENGTH_TOP_OR_LEFT, 0.0f));
        }

        const auto MIDDLE_LENGTH = [&,
                                    END_CAP_LENGTH_TOP_OR_LEFT = END_CAP_LENGTH_TOP_OR_LEFT,
                                    END_CAP_LENGTH_BOT_OR_RIGHT = END_CAP_LENGTH_BOT_OR_RIGHT]() {
            if (WILL_CAP_ENDS)
            {
                if (LENGTH > ((END_CAP_LENGTH_TOP_OR_LEFT + END_CAP_LENGTH_BOT_OR_RIGHT) + 1.0f))
                {
                    return (LENGTH - (END_CAP_LENGTH_TOP_OR_LEFT + END_CAP_LENGTH_BOT_OR_RIGHT));
                }
                else
                {
                    return 0.0f;
                }
            }
            else
            {
                return LENGTH;
            }
        }();

        const auto MIDDLE_POS_V = [&,
                                   END_CAP_LENGTH_TOP_OR_LEFT = END_CAP_LENGTH_TOP_OR_LEFT,
                                   END_CAP_LENGTH_BOT_OR_RIGHT = END_CAP_LENGTH_BOT_OR_RIGHT]() {
            sf::Vector2f v { POS_V };

            if (ORIENTATION == Orientation::Horiz)
            {
                v.x += END_CAP_LENGTH_TOP_OR_LEFT;
            }
            else
            {
                v.y += END_CAP_LENGTH_BOT_OR_RIGHT;
            }

            return v;
        }();

        if ((MIDDLE_LENGTH < 1.0f) == false)
        {
            sfutil::AppendVertexesForQuadRepeatedOverLength(
                vertexArray_, MIDDLE_POS_V, TEXTURE_RECT_MIDDLE, ORIENTATION, MIDDLE_LENGTH);
        }

        const auto BOT_OR_RIGHT_POS_V = [&]() {
            sf::Vector2f v { MIDDLE_POS_V };

            if (ORIENTATION == Orientation::Horiz)
            {
                v.x += MIDDLE_LENGTH;
            }
            else
            {
                v.y += MIDDLE_LENGTH;
            }

            return v;
        }();

        if ((END_CAP_LENGTH_BOT_OR_RIGHT < 1.0f) == false)
        {
            sfutil::AppendVertexesForQuad(
                vertexArray_,
                BOT_OR_RIGHT_POS_V,
                TEXTURE_RECT_BOT_OR_RIGHT,
                sf::Color::White,
                sfutil::CreateVector(ORIENTATION, END_CAP_LENGTH_BOT_OR_RIGHT, 0.0f),
                Orientation::Both);
        }

        outerRegion_ = innerRegion_ = vertexArray_.getBounds();
    }

    void GoldBar::SetupVertexesAndRegionsForSquare(
        const sf::FloatRect & REGION_ORIG, const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
    {
        const auto REGION = [&]() {
            sf::FloatRect r;

            r.width
                = std::floor(REGION_ORIG.left + REGION_ORIG.width) - std::floor(REGION_ORIG.left);

            r.height
                = std::floor(REGION_ORIG.top + REGION_ORIG.height) - std::floor(REGION_ORIG.top);

            r.left = std::floor(REGION_ORIG.left);
            r.top = std::floor(REGION_ORIG.top);
            return r;
        }();

        const sf::Vector2f POS_V { sfutil::Position(REGION) };

        outerRegion_ = sf::FloatRect(POS_V, sf::Vector2f(0.0f, 0.0f));
        innerRegion_ = outerRegion_;

        if ((REGION.width < 1.0f) || (REGION.height < 1.0f))
        {
            return;
        }

        const auto TEXTURE_RECT_SIDE_TOP { GuiImages::LineSmallHorizontalTopF() };
        const auto TEXTURE_RECT_SIDE_RIGHT { GuiImages::LineSmallVerticalRightF() };
        const auto TEXTURE_RECT_SIDE_BOT { GuiImages::LineSmallHorizontalBotF() };
        const auto TEXTURE_RECT_SIDE_LEFT { GuiImages::LineSmallVerticalLeftF() };

        const auto TEXTURE_RECT_CORNER_TOP_LEFT { GuiImages::LineSmallCornerTopLeftF() };
        const auto TEXTURE_RECT_CORNER_TOP_RIGHT { GuiImages::LineSmallCornerTopRightF() };
        const auto TEXTURE_RECT_CORNER_BOT_RIGHT { GuiImages::LineSmallCornerBotRightF() };
        const auto TEXTURE_RECT_CORNER_BOT_LEFT { GuiImages::LineSmallCornerBotLeftF() };

        const auto
            [OUTER_MOVE_TOP_LEFT_V,
             OUTER_MOVE_TOP_RIGHT_V,
             OUTER_MOVE_BOT_RIGHT_V,
             OUTER_MOVE_BOT_LEFT_V]
            = [&]() {
                  sf::Vector2f topLeftV { 0.0f, 0.0f };
                  sf::Vector2f topRightV { 0.0f, 0.0f };
                  sf::Vector2f botRightV { 0.0f, 0.0f };
                  sf::Vector2f botLeftV { 0.0f, 0.0f };

                  if (WILL_GROW_BORDER_TO_CONTAIN_REGION)
                  {
                      topLeftV = { -1.0f * GuiImages::GetLineSmallBoxPadLeft(),
                                   -1.0f * GuiImages::GetLineSmallBoxPadTop() };

                      topRightV = { GuiImages::GetLineSmallBoxPadRight(),
                                    -1.0f * GuiImages::GetLineSmallBoxPadTop() };

                      botRightV = { GuiImages::GetLineSmallBoxPadRight(),
                                    GuiImages::GetLineSmallBoxPadBot() };

                      botLeftV = { -1.0f * GuiImages::GetLineSmallBoxPadLeft(),
                                   GuiImages::GetLineSmallBoxPadBot() };
                  }

                  return std::make_tuple(topLeftV, topRightV, botRightV, botLeftV);
              }();

        const auto CORNER_SIZE_TOP_LEFT_V = [&,
                                             OUTER_MOVE_TOP_LEFT_V = OUTER_MOVE_TOP_LEFT_V,
                                             OUTER_MOVE_TOP_RIGHT_V = OUTER_MOVE_TOP_RIGHT_V,
                                             OUTER_MOVE_BOT_LEFT_V = OUTER_MOVE_BOT_LEFT_V]() {
            sf::Vector2f sizeV(0.0f, 0.0f);

            if (WILL_GROW_BORDER_TO_CONTAIN_REGION)
            {
                const auto MIN_WIDTH { (
                    std::abs(OUTER_MOVE_TOP_LEFT_V.x) + std::abs(OUTER_MOVE_TOP_RIGHT_V.x)) };

                const auto MIN_HEIGHT { (
                    std::abs(OUTER_MOVE_TOP_LEFT_V.y) + std::abs(OUTER_MOVE_BOT_LEFT_V.y)) };

                const auto FULL_WIDTH { (
                    TEXTURE_RECT_CORNER_TOP_LEFT.width + TEXTURE_RECT_CORNER_TOP_RIGHT.width) };

                const auto FULL_HEIGHT { (
                    TEXTURE_RECT_CORNER_TOP_LEFT.height + TEXTURE_RECT_CORNER_BOT_LEFT.height) };

                if (REGION.width < FULL_WIDTH)
                {
                    sizeV.x = std::floor(std::max(REGION.width * 0.5f, MIN_WIDTH * 0.5f));
                }
                else
                {
                    sizeV.x = TEXTURE_RECT_CORNER_TOP_LEFT.width;
                }

                if (REGION.height < FULL_HEIGHT)
                {
                    sizeV.y = std::floor(std::max(REGION.height * 0.5f, MIN_HEIGHT * 0.5f));
                }
                else
                {
                    sizeV.y = TEXTURE_RECT_CORNER_TOP_LEFT.height;
                }
            }
            else
            {
                if (REGION.width
                    < (TEXTURE_RECT_CORNER_TOP_LEFT.width + TEXTURE_RECT_CORNER_TOP_RIGHT.width))
                {
                    sizeV.x = std::floor(REGION.width * 0.5f);
                }
                else
                {
                    sizeV.x = TEXTURE_RECT_CORNER_TOP_LEFT.width;
                }

                if (REGION.height
                    < (TEXTURE_RECT_CORNER_TOP_LEFT.height + TEXTURE_RECT_CORNER_BOT_LEFT.height))
                {
                    sizeV.y = std::floor(REGION.height * 0.5f);
                }
                else
                {
                    sizeV.y = TEXTURE_RECT_CORNER_TOP_LEFT.height;
                }
            }

            return sizeV;
        }();

        // assume that all corner texture rects are square and the same...cause they are.
        const sf::Vector2f CORNER_SIZE_TOP_RIGHT_V { CORNER_SIZE_TOP_LEFT_V };
        const sf::Vector2f CORNER_SIZE_BOT_RIGHT_V { CORNER_SIZE_TOP_LEFT_V };
        const sf::Vector2f CORNER_SIZE_BOT_LEFT_V { CORNER_SIZE_TOP_LEFT_V };

        const auto [SIDE_LEN_TOP, SIDE_LEN_RIGHT, SIDE_LEN_BOT, SIDE_LEN_LEFT] =
            [&,
             OUTER_MOVE_TOP_LEFT_V = OUTER_MOVE_TOP_LEFT_V,
             OUTER_MOVE_TOP_RIGHT_V = OUTER_MOVE_TOP_RIGHT_V,
             OUTER_MOVE_BOT_RIGHT_V = OUTER_MOVE_BOT_RIGHT_V,
             OUTER_MOVE_BOT_LEFT_V = OUTER_MOVE_BOT_LEFT_V]() {
                float top { 0.0f };
                float right { 0.0f };
                float bot { 0.0f };
                float left { 0.0f };

                top = std::max(
                    0.0f,
                    ((REGION.width - CORNER_SIZE_TOP_LEFT_V.x)
                     - (CORNER_SIZE_TOP_RIGHT_V.x + 1.0f)));

                bot = std::max(
                    0.0f,
                    ((REGION.width - CORNER_SIZE_BOT_LEFT_V.x)
                     - (CORNER_SIZE_BOT_RIGHT_V.x + 1.0f)));

                left = std::max(
                    0.0f,
                    ((REGION.height - CORNER_SIZE_TOP_LEFT_V.y)
                     - (CORNER_SIZE_BOT_LEFT_V.y + 1.0f)));

                right = std::max(
                    0.0f,
                    ((REGION.height - CORNER_SIZE_TOP_RIGHT_V.y)
                     - (CORNER_SIZE_BOT_RIGHT_V.y + 1.0f)));

                top += (std::abs(OUTER_MOVE_TOP_LEFT_V.x) + std::abs(OUTER_MOVE_TOP_RIGHT_V.x));
                bot += (std::abs(OUTER_MOVE_BOT_LEFT_V.x) + std::abs(OUTER_MOVE_BOT_RIGHT_V.x));
                left += (std::abs(OUTER_MOVE_TOP_LEFT_V.y) + std::abs(OUTER_MOVE_BOT_LEFT_V.y));

                right += (std::abs(OUTER_MOVE_TOP_RIGHT_V.y) + std::abs(OUTER_MOVE_BOT_RIGHT_V.y));

                return std::make_tuple(top, right, bot, left);
            }();

        const auto
            [CORNER_POS_TOP_LEFT, CORNER_POS_TOP_RIGHT, CORNER_POS_BOT_RIGHT, CORNER_POS_BOT_LEFT]
            = [&,
               SIDE_LEN_TOP = SIDE_LEN_TOP,
               SIDE_LEN_RIGHT = SIDE_LEN_RIGHT,
               SIDE_LEN_BOT = SIDE_LEN_BOT,
               SIDE_LEN_LEFT = SIDE_LEN_LEFT,
               OUTER_MOVE_TOP_LEFT_V = OUTER_MOVE_TOP_LEFT_V,
               OUTER_MOVE_TOP_RIGHT_V = OUTER_MOVE_TOP_RIGHT_V,
               OUTER_MOVE_BOT_LEFT_V = OUTER_MOVE_BOT_LEFT_V]() {
                  sf::Vector2f topLeft { 0.0f, 0.0f };
                  sf::Vector2f topRight { 0.0f, 0.0f };
                  sf::Vector2f botRight { 0.0f, 0.0f };
                  sf::Vector2f botLeft { 0.0f, 0.0f };

                  topLeft = POS_V;
                  topLeft += OUTER_MOVE_TOP_LEFT_V;

                  topRight = sf::Vector2f(
                      (topLeft.x + CORNER_SIZE_TOP_LEFT_V.x + SIDE_LEN_TOP),
                      POS_V.y + OUTER_MOVE_TOP_RIGHT_V.y);

                  botLeft = sf::Vector2f(
                      POS_V.x + OUTER_MOVE_BOT_LEFT_V.x,
                      (topLeft.y + CORNER_SIZE_TOP_LEFT_V.y + SIDE_LEN_LEFT));

                  botRight = sf::Vector2f(
                      (botLeft.x + CORNER_SIZE_BOT_LEFT_V.x + SIDE_LEN_BOT),
                      (topRight.y + CORNER_SIZE_TOP_RIGHT_V.y + SIDE_LEN_RIGHT));

                  return std::make_tuple(topLeft, topRight, botRight, botLeft);
              }();

        const auto
            [SIDE_START_POS_TOP, SIDE_START_POS_RIGHT, SIDE_START_POS_BOT, SIDE_START_POS_LEFT]
            = [&,
               CORNER_POS_TOP_LEFT = CORNER_POS_TOP_LEFT,
               CORNER_POS_TOP_RIGHT = CORNER_POS_TOP_RIGHT,
               CORNER_POS_BOT_LEFT = CORNER_POS_BOT_LEFT]() {
                  const auto TOP_POS_V { sf::Vector2f(
                      CORNER_POS_TOP_LEFT.x + CORNER_SIZE_TOP_LEFT_V.x, CORNER_POS_TOP_LEFT.y) };

                  const auto BOT_POS_V { sf::Vector2f(
                      CORNER_POS_BOT_LEFT.x + CORNER_SIZE_BOT_LEFT_V.x,
                      (CORNER_POS_BOT_LEFT.y + CORNER_SIZE_BOT_LEFT_V.y)
                          - TEXTURE_RECT_SIDE_BOT.height) };

                  const auto RIGHT_POS_V { sf::Vector2f(
                      (CORNER_POS_TOP_RIGHT.x + CORNER_SIZE_TOP_RIGHT_V.x)
                          - TEXTURE_RECT_SIDE_RIGHT.width,
                      CORNER_POS_TOP_RIGHT.y + CORNER_SIZE_TOP_RIGHT_V.y) };

                  const auto LEFT_POS_V { sf::Vector2f(
                      CORNER_POS_TOP_LEFT.x, CORNER_POS_TOP_LEFT.y + CORNER_SIZE_TOP_LEFT_V.y) };

                  return std::make_tuple(TOP_POS_V, RIGHT_POS_V, BOT_POS_V, LEFT_POS_V);
              }();

        const auto [SIDE_HEIGHT_TOP, SIDE_WIDTH_RIGHT, SIDE_HEIGHT_BOT, SIDE_WIDTH_LEFT] =
            [&,
             SIDE_START_POS_TOP = SIDE_START_POS_TOP,
             SIDE_START_POS_RIGHT = SIDE_START_POS_RIGHT,
             SIDE_START_POS_BOT = SIDE_START_POS_BOT,
             SIDE_START_POS_LEFT = SIDE_START_POS_LEFT]() {
                float heightTop { TEXTURE_RECT_SIDE_TOP.height };
                float heightBot { TEXTURE_RECT_SIDE_BOT.height };

                const auto HEIGHT_OVERLAP {
                    SIDE_START_POS_BOT.y - (SIDE_START_POS_TOP.y + TEXTURE_RECT_SIDE_TOP.height)
                };

                if (HEIGHT_OVERLAP < 0.0f)
                {
                    const auto VERT_SHRINK { (std::abs(HEIGHT_OVERLAP) * 0.5f) };
                    heightTop -= VERT_SHRINK;
                    heightBot -= VERT_SHRINK;
                }

                float widthLeft { TEXTURE_RECT_SIDE_LEFT.width };
                float widthRight { TEXTURE_RECT_SIDE_RIGHT.width };

                const auto WIDTH_OVERLAP {
                    SIDE_START_POS_RIGHT.x - (SIDE_START_POS_LEFT.x + TEXTURE_RECT_SIDE_LEFT.width)
                };

                if (WIDTH_OVERLAP < 0.0f)
                {
                    const auto HORIZ_SHRINK { (std::abs(WIDTH_OVERLAP) * 0.5f) };
                    widthLeft -= HORIZ_SHRINK;
                    widthRight -= HORIZ_SHRINK;
                }

                return std::make_tuple(heightTop, widthRight, heightBot, widthLeft);
            }();

        if (((CORNER_SIZE_TOP_LEFT_V.x < 1.0f) == false)
            || ((CORNER_SIZE_TOP_LEFT_V.y < 1.0f) == false))
        {
            sfutil::AppendVertexesForQuad(
                vertexArray_,
                CORNER_POS_TOP_LEFT,
                TEXTURE_RECT_CORNER_TOP_LEFT,
                sf::Color::White,
                CORNER_SIZE_TOP_LEFT_V);
        }

        sfutil::AppendVertexesForQuadRepeatedOverLength(
            vertexArray_,
            SIDE_START_POS_TOP,
            TEXTURE_RECT_SIDE_TOP,
            Orientation::Horiz,
            SIDE_LEN_TOP,
            sf::Color::White,
            SIDE_HEIGHT_TOP);

        if (((CORNER_SIZE_TOP_RIGHT_V.x < 1.0f) == false)
            || ((CORNER_SIZE_TOP_RIGHT_V.y < 1.0f) == false))
        {
            sfutil::AppendVertexesForQuad(
                vertexArray_,
                CORNER_POS_TOP_RIGHT,
                TEXTURE_RECT_CORNER_TOP_RIGHT,
                sf::Color::White,
                CORNER_SIZE_TOP_RIGHT_V,
                Orientation::Horiz);
        }

        sfutil::AppendVertexesForQuadRepeatedOverLength(
            vertexArray_,
            SIDE_START_POS_RIGHT,
            TEXTURE_RECT_SIDE_RIGHT,
            Orientation::Vert,
            SIDE_LEN_RIGHT,
            sf::Color::White,
            SIDE_WIDTH_RIGHT,
            Orientation::Horiz);

        if (((CORNER_SIZE_BOT_RIGHT_V.x < 1.0f) == false)
            || ((CORNER_SIZE_BOT_RIGHT_V.y < 1.0f) == false))
        {
            sfutil::AppendVertexesForQuad(
                vertexArray_,
                CORNER_POS_BOT_RIGHT,
                TEXTURE_RECT_CORNER_BOT_RIGHT,
                sf::Color::White,
                CORNER_SIZE_BOT_RIGHT_V,
                Orientation::Both);
        }

        sfutil::AppendVertexesForQuadRepeatedOverLength(
            vertexArray_,
            SIDE_START_POS_BOT,
            TEXTURE_RECT_SIDE_BOT,
            Orientation::Horiz,
            SIDE_LEN_BOT,
            sf::Color::White,
            SIDE_HEIGHT_BOT,
            Orientation::Vert);

        if (((CORNER_SIZE_BOT_LEFT_V.x < 1.0f) == false)
            || ((CORNER_SIZE_BOT_LEFT_V.y < 1.0f) == false))
        {
            sfutil::AppendVertexesForQuad(
                vertexArray_,
                CORNER_POS_BOT_LEFT,
                TEXTURE_RECT_CORNER_BOT_LEFT,
                sf::Color::White,
                CORNER_SIZE_BOT_LEFT_V,
                Orientation::Vert);
        }

        sfutil::AppendVertexesForQuadRepeatedOverLength(
            vertexArray_,
            SIDE_START_POS_LEFT,
            TEXTURE_RECT_SIDE_LEFT,
            Orientation::Vert,
            SIDE_LEN_LEFT,
            sf::Color::White,
            SIDE_WIDTH_LEFT);

        const sf::Vector2f OUTER_TOP_LEFT_POS_V(
            std::floor(CORNER_POS_TOP_LEFT.x), std::floor(CORNER_POS_TOP_LEFT.y));

        const sf::Vector2f OUTER_BOT_RIGHT_POS_V(
            std::floor(CORNER_POS_BOT_RIGHT.x + CORNER_SIZE_BOT_RIGHT_V.x),
            std::floor(CORNER_POS_BOT_RIGHT.y + CORNER_SIZE_BOT_RIGHT_V.y));

        const auto OUTER_SIZE_V { OUTER_BOT_RIGHT_POS_V - OUTER_TOP_LEFT_POS_V };

        if (misc::IsRealZero(OUTER_SIZE_V.x) && misc::IsRealZero(OUTER_SIZE_V.y))
        {
            outerRegion_ = sf::FloatRect(OUTER_TOP_LEFT_POS_V, sf::Vector2f(0.0f, 0.0f));
            innerRegion_ = outerRegion_;
        }
        else
        {
            outerRegion_ = sf::FloatRect(OUTER_TOP_LEFT_POS_V, OUTER_SIZE_V);

            const auto OUTER_REGION_CENTER_POS_V { sfutil::CenterOf(outerRegion_) };

            // assume that all corner texture rects are square and the same...cause they are.

            if (OUTER_SIZE_V.x < ((SIDE_WIDTH_LEFT + SIDE_WIDTH_RIGHT) + 1.0f))
            {
                innerRegion_.left = OUTER_REGION_CENTER_POS_V.x;
                innerRegion_.width = 0.0f;
            }
            else
            {
                innerRegion_.left = outerRegion_.left + SIDE_WIDTH_LEFT;
                innerRegion_.width = OUTER_SIZE_V.x - (SIDE_WIDTH_LEFT + SIDE_WIDTH_RIGHT);
            }

            if (OUTER_SIZE_V.y < ((SIDE_HEIGHT_TOP + SIDE_HEIGHT_BOT) + 1.0f))
            {
                innerRegion_.top = OUTER_REGION_CENTER_POS_V.y;
                innerRegion_.height = 0.0f;
            }
            else
            {
                innerRegion_.top = outerRegion_.top + SIDE_HEIGHT_TOP;
                innerRegion_.height = OUTER_SIZE_V.y - (SIDE_HEIGHT_TOP + SIDE_HEIGHT_BOT);
            }
        }
    }

    void GoldBar::SetupLengthToDefault(const Orientation::Enum ORIENTATION)
    {
        if (ORIENTATION == Orientation::Horiz)
        {
            length_ = outerRegion_.width;
        }
        else if (ORIENTATION == Orientation::Vert)
        {
            length_ = outerRegion_.height;
        }
        else
        {
            length_ = std::max(outerRegion_.width, outerRegion_.height);
        }
    }

} // namespace gui
} // namespace heroespath
