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

#include "sfml-util/gui/gui-images.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sfml-util-size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        GoldBar::GoldBar()
            : length_(0.0f)
            , innerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
            , outerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
            , vertexArray_(sf::PrimitiveType::Quads)
            , guiElementsCachedTexture_("media-images-gui-elements")
            , willGrowBorderToContainRegion_(false)
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
            , guiElementsCachedTexture_("media-images-gui-elements")
            , willGrowBorderToContainRegion_(false)
        {
            Setup(POS_LEFT, POS_TOP, LENGTH, ORIENTATION, SIDE, WILL_CAP_ENDS);
        }

        GoldBar::GoldBar(
            const std::vector<GoldBar> & GOLD_BARS, const Orientation::Enum ORIENTATION)
            : length_(0.0f)
            , innerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
            , outerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
            , vertexArray_(sf::PrimitiveType::Quads)
            , guiElementsCachedTexture_("media-images-gui-elements")
            , willGrowBorderToContainRegion_(false)
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

            outerRegion_ = MininallyEnclosing(outerRegions);
            innerRegion_ = MininallyEnclosing(innerRegions);
            SetupLengthToDefault(ORIENTATION);
        }

        GoldBar::GoldBar(const sf::VertexArray & VERTEX_ARRAY, const Orientation::Enum ORIENTATION)
            : length_(0.0f)
            , innerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
            , outerRegion_(0.0f, 0.0f, 0.0f, 0.0f)
            , vertexArray_(sf::PrimitiveType::Quads)
            , guiElementsCachedTexture_("media-images-gui-elements")
            , willGrowBorderToContainRegion_(false)
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
            , guiElementsCachedTexture_("media-images-gui-elements")
            , willGrowBorderToContainRegion_(WILL_GROW_BORDER_TO_CONTAIN_REGION)
        {
            Setup(REGION, ORIENTATION, WILL_GROW_BORDER_TO_CONTAIN_REGION);
        }

        void GoldBar::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            states.texture = &guiElementsCachedTexture_.Get();
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
                return defaults::None;
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
            const auto DIFF_V { sf::Vector2f(POS_LEFT, POS_TOP)
                                - sfml_util::Position(outerRegion_) };

            MovePos(DIFF_V.x, DIFF_V.y);
        }

        void GoldBar::MovePos(const float HORIZ, const float VERT)
        {
            const auto DIFF_V { sf::Vector2f(HORIZ, VERT) };

            outerRegion_ = sf::FloatRect(Position(outerRegion_) + DIFF_V, Size(outerRegion_));
            innerRegion_ = sf::FloatRect(Position(innerRegion_) + DIFF_V, Size(innerRegion_));

            const auto VERTEX_COUNT { vertexArray_.getVertexCount() };
            for (std::size_t i(0); i < VERTEX_COUNT; ++i)
            {
                vertexArray_[i].position += DIFF_V;
            }
        }

        const std::tuple<sf::FloatRect, sf::FloatRect, sf::FloatRect> GoldBar::GetTextureRectsBar(
            const Orientation::Enum ORIENTATION_PARAM, const Side::Enum SIDE) const
        {
            const auto ORIENTATION {
                ((ORIENTATION_PARAM == Orientation::Count) ? Orientation::Horiz : ORIENTATION_PARAM)
            };

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

        const std::tuple<sf::FloatRect, sf::FloatRect, sf::FloatRect, sf::FloatRect>
            GoldBar::GetTextureRectsCorners() const
        {
            return std::make_tuple(
                GuiImages::LineSmallCornerTopLeftF(),
                GuiImages::LineSmallCornerTopRightF(),
                GuiImages::LineSmallCornerBotLeftF(),
                GuiImages::LineSmallCornerBotRightF());
        }

        const std::tuple<sf::FloatRect, sf::FloatRect, sf::FloatRect, sf::FloatRect>
            GoldBar::GetTextureRectsSides() const
        {
            return std::make_tuple(
                GuiImages::LineSmallHorizontalTopF(),
                GuiImages::LineSmallVerticalRightF(),
                GuiImages::LineSmallHorizontalBotF(),
                GuiImages::LineSmallVerticalLeftF());
        }

        void GoldBar::SetupVertexesAndRegionsForSingleBar(
            const float LEFT,
            const float TOP,
            const float LENGTH,
            Orientation::Enum ORIENTATION_PARAM,
            const Side::Enum SIDE,
            const bool WILL_CAP_ENDS)
        {
            const auto ORIENTATION {
                ((ORIENTATION_PARAM == Orientation::Count) ? Orientation::Horiz : ORIENTATION_PARAM)
            };

            length_ = LENGTH;

            vertexArray_.clear();

            if ((LENGTH > 0.0f) == false)
            {
                return;
            }

            const auto POS_V { sf::Vector2f(LEFT, TOP) };

            const auto [TEXTURE_RECT_TOP_OR_LEFT, TEXTURE_RECT_BOT_OR_RIGHT, TEXTURE_RECT_MIDDLE]
                = GetTextureRectsBar(ORIENTATION, SIDE);

            const auto [END_CAP_LENGTH_TOP_OR_LEFT, END_CAP_LENGTH_BOT_OR_RIGHT]
                = [&,
                   TEXTURE_RECT_TOP_OR_LEFT = TEXTURE_RECT_TOP_OR_LEFT,
                   TEXTURE_RECT_BOT_OR_RIGHT = TEXTURE_RECT_BOT_OR_RIGHT]() {
                      float lenTopOrLeft { 0.0f };
                      float lenBotOrRight { 0.0f };

                      if (WILL_CAP_ENDS)
                      {
                          lenTopOrLeft
                              = ((ORIENTATION == Orientation::Horiz)
                                     ? TEXTURE_RECT_TOP_OR_LEFT.width
                                     : TEXTURE_RECT_TOP_OR_LEFT.height);

                          lenBotOrRight
                              = ((ORIENTATION == Orientation::Horiz)
                                     ? TEXTURE_RECT_BOT_OR_RIGHT.width
                                     : TEXTURE_RECT_BOT_OR_RIGHT.height);

                          if (LENGTH < (lenTopOrLeft + lenBotOrRight))
                          {
                              const auto END_CAP_LENGTH_PARTIAL {
                                  (LENGTH < (lenTopOrLeft + lenBotOrRight)) * 0.5f
                              };

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

            sfml_util::AppendVertexesForQuad(
                vertexArray_,
                POS_V,
                TEXTURE_RECT_TOP_OR_LEFT,
                sf::Color::White,
                CreateVector(ORIENTATION, END_CAP_LENGTH_TOP_OR_LEFT, 0.0f));

            const auto MIDDLE_LENGTH {
                LENGTH - (END_CAP_LENGTH_TOP_OR_LEFT + END_CAP_LENGTH_BOT_OR_RIGHT)
            };

            const auto MIDDLE_POS_V
                = [&,
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

            sfml_util::AppendVertexesForQuadRepeatedOverLength(
                vertexArray_, MIDDLE_POS_V, TEXTURE_RECT_MIDDLE, ORIENTATION, MIDDLE_LENGTH);

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

            sfml_util::AppendVertexesForQuad(
                vertexArray_,
                BOT_OR_RIGHT_POS_V,
                TEXTURE_RECT_BOT_OR_RIGHT,
                sf::Color::White,
                CreateVector(ORIENTATION, END_CAP_LENGTH_BOT_OR_RIGHT, 0.0f),
                Orientation::Both);

            outerRegion_ = innerRegion_ = vertexArray_.getBounds();
        }

        void GoldBar::SetupVertexesAndRegionsForSquare(
            const sf::FloatRect & REGION, const bool WILL_GROW_BORDER_TO_CONTAIN_REGION)
        {
            if ((REGION.width < 1.0f) || (REGION.height < 1.0f))
            {
                return;
            }

            const auto POS_V { sfml_util::Position(REGION) };

            const auto
                [TEXTURE_RECT_SIDE_TOP,
                 TEXTURE_RECT_SIDE_RIGHT,
                 TEXTURE_RECT_SIDE_BOT,
                 TEXTURE_RECT_SIDE_LEFT]
                = GetTextureRectsSides();

            const auto
                [TEXTURE_RECT_CORNER_TOP_LEFT,
                 TEXTURE_RECT_CORNER_TOP_RIGHT,
                 TEXTURE_RECT_CORNER_BOT_RIGHT,
                 TEXTURE_RECT_CORNER_BOT_LEFT]
                = GetTextureRectsCorners();

            const sf::Vector2f CORNER_SIZE_TOP_LEFT_V {
                std::min(
                    TEXTURE_RECT_CORNER_TOP_LEFT.width,
                    (REGION.width - TEXTURE_RECT_CORNER_TOP_RIGHT.width)),
                std::min(
                    TEXTURE_RECT_CORNER_TOP_LEFT.height,
                    (REGION.height - TEXTURE_RECT_CORNER_BOT_LEFT.height))
            };

            const sf::Vector2f CORNER_SIZE_TOP_RIGHT_V {
                std::min(
                    TEXTURE_RECT_CORNER_TOP_RIGHT.width,
                    (REGION.width - TEXTURE_RECT_CORNER_TOP_LEFT.width)),
                std::min(
                    TEXTURE_RECT_CORNER_TOP_RIGHT.height,
                    (REGION.height - TEXTURE_RECT_CORNER_BOT_RIGHT.height))
            };

            const sf::Vector2f CORNER_SIZE_BOT_RIGHT_V {
                std::min(
                    TEXTURE_RECT_CORNER_BOT_RIGHT.width,
                    (REGION.width - TEXTURE_RECT_CORNER_BOT_LEFT.width)),
                std::min(
                    TEXTURE_RECT_CORNER_BOT_RIGHT.height,
                    (REGION.height - TEXTURE_RECT_CORNER_TOP_RIGHT.height))
            };

            const sf::Vector2f CORNER_SIZE_BOT_LEFT_V {
                std::min(
                    TEXTURE_RECT_CORNER_BOT_LEFT.width,
                    (REGION.width - TEXTURE_RECT_CORNER_BOT_RIGHT.width)),
                std::min(
                    TEXTURE_RECT_CORNER_BOT_LEFT.height,
                    (REGION.height - TEXTURE_RECT_CORNER_TOP_LEFT.height))
            };

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
                          topLeftV = { -1.0f
                                           * std::min(
                                                 GuiImages::GetLineSmallBoxPadLeft(),
                                                 CORNER_SIZE_TOP_LEFT_V.x),
                                       -1.0f
                                           * std::min(
                                                 GuiImages::GetLineSmallBoxPadTop(),
                                                 CORNER_SIZE_TOP_LEFT_V.y) };

                          topRightV = { std::min(
                                            GuiImages::GetLineSmallBoxPadRight(),
                                            CORNER_SIZE_TOP_RIGHT_V.x),
                                        -1.0f
                                            * std::min(
                                                  GuiImages::GetLineSmallBoxPadTop(),
                                                  CORNER_SIZE_TOP_RIGHT_V.y) };

                          botRightV = {
                              std::min(
                                  GuiImages::GetLineSmallBoxPadRight(), CORNER_SIZE_BOT_RIGHT_V.x),
                              std::min(
                                  GuiImages::GetLineSmallBoxPadBot(), CORNER_SIZE_BOT_RIGHT_V.y)
                          };

                          botLeftV = {
                              -1.0f
                                  * std::min(
                                        GuiImages::GetLineSmallBoxPadLeft(),
                                        CORNER_SIZE_BOT_LEFT_V.x),
                              std::min(GuiImages::GetLineSmallBoxPadBot(), CORNER_SIZE_BOT_LEFT_V.y)
                          };
                      }

                      return std::make_tuple(topLeftV, topRightV, botRightV, botLeftV);
                  }();

            const auto [SIDE_LEN_TOP, SIDE_LEN_RIGHT, SIDE_LEN_BOT, SIDE_LEN_LEFT] =
                [&,
                 OUTER_MOVE_TOP_LEFT_V = OUTER_MOVE_TOP_LEFT_V,
                 OUTER_MOVE_TOP_RIGHT_V = OUTER_MOVE_TOP_RIGHT_V,
                 OUTER_MOVE_BOT_RIGHT_V = OUTER_MOVE_BOT_RIGHT_V,
                 OUTER_MOVE_BOT_LEFT_V = OUTER_MOVE_BOT_LEFT_V,
                 CORNER_SIZE_TOP_LEFT_V = CORNER_SIZE_TOP_LEFT_V,
                 CORNER_SIZE_TOP_RIGHT_V = CORNER_SIZE_TOP_RIGHT_V,
                 CORNER_SIZE_BOT_LEFT_V = CORNER_SIZE_BOT_LEFT_V,
                 CORNER_SIZE_BOT_RIGHT_V = CORNER_SIZE_BOT_RIGHT_V]() {
                    float top { 0.0f };
                    float right { 0.0f };
                    float bot { 0.0f };
                    float left { 0.0f };

                    top = std::max(
                        0.0f,
                        ((REGION.width - CORNER_SIZE_TOP_LEFT_V.x) - CORNER_SIZE_TOP_RIGHT_V.x));

                    bot = std::max(
                        0.0f,
                        ((REGION.width - CORNER_SIZE_BOT_LEFT_V.x) - CORNER_SIZE_BOT_RIGHT_V.x));

                    left = std::max(
                        0.0f,
                        ((REGION.height - CORNER_SIZE_TOP_LEFT_V.y) - CORNER_SIZE_BOT_LEFT_V.y));

                    right = std::max(
                        0.0f,
                        ((REGION.height - CORNER_SIZE_TOP_RIGHT_V.y) - CORNER_SIZE_BOT_RIGHT_V.y));

                    top += (std::abs(OUTER_MOVE_TOP_LEFT_V.x) + std::abs(OUTER_MOVE_TOP_RIGHT_V.x));
                    bot += (std::abs(OUTER_MOVE_BOT_LEFT_V.x) + std::abs(OUTER_MOVE_BOT_RIGHT_V.x));
                    left += (std::abs(OUTER_MOVE_TOP_LEFT_V.y) + std::abs(OUTER_MOVE_BOT_LEFT_V.y));
                    right
                        += (std::abs(OUTER_MOVE_TOP_RIGHT_V.y)
                            + std::abs(OUTER_MOVE_BOT_RIGHT_V.y));

                    return std::make_tuple(top, right, bot, left);
                }();

            const auto
                [CORNER_POS_TOP_LEFT,
                 CORNER_POS_TOP_RIGHT,
                 CORNER_POS_BOT_RIGHT,
                 CORNER_POS_BOT_LEFT]
                = [&,
                   SIDE_LEN_TOP = SIDE_LEN_TOP,
                   SIDE_LEN_RIGHT = SIDE_LEN_RIGHT,
                   SIDE_LEN_BOT = SIDE_LEN_BOT,
                   SIDE_LEN_LEFT = SIDE_LEN_LEFT,
                   OUTER_MOVE_TOP_LEFT_V = OUTER_MOVE_TOP_LEFT_V,
                   OUTER_MOVE_TOP_RIGHT_V = OUTER_MOVE_TOP_RIGHT_V,
                   OUTER_MOVE_BOT_RIGHT_V = OUTER_MOVE_BOT_RIGHT_V,
                   OUTER_MOVE_BOT_LEFT_V = OUTER_MOVE_BOT_LEFT_V]() {
                      sf::Vector2f topLeft { 0.0f, 0.0f };
                      sf::Vector2f topRight { 0.0f, 0.0f };
                      sf::Vector2f botRight { 0.0f, 0.0f };
                      sf::Vector2f botLeft { 0.0f, 0.0f };

                      topLeft = POS_V;

                      topRight = sf::Vector2f(
                          (topLeft.x + CORNER_SIZE_TOP_LEFT_V.x + SIDE_LEN_TOP), POS_V.y);

                      botLeft = sf::Vector2f(
                          POS_V.x, (topLeft.y + CORNER_SIZE_TOP_LEFT_V.y + SIDE_LEN_LEFT));

                      botRight = sf::Vector2f(
                          (botLeft.x + CORNER_SIZE_BOT_LEFT_V.x + SIDE_LEN_BOT),
                          (topRight.y + CORNER_SIZE_TOP_RIGHT_V.y + SIDE_LEN_RIGHT));

                      topLeft += OUTER_MOVE_TOP_LEFT_V;
                      topRight += OUTER_MOVE_TOP_RIGHT_V;
                      botRight += OUTER_MOVE_BOT_RIGHT_V;
                      botLeft += OUTER_MOVE_BOT_LEFT_V;

                      return std::make_tuple(topLeft, topRight, botRight, botLeft);
                  }();

            const auto
                [SIDE_START_POS_TOP, SIDE_START_POS_RIGHT, SIDE_START_POS_BOT, SIDE_START_POS_LEFT]
                = [&,
                   CORNER_POS_TOP_LEFT = CORNER_POS_TOP_LEFT,
                   CORNER_POS_TOP_RIGHT = CORNER_POS_TOP_RIGHT,
                   CORNER_POS_BOT_LEFT = CORNER_POS_BOT_LEFT]() {
                      const auto TOP_POS_V { sf::Vector2f(
                          CORNER_POS_TOP_LEFT.x + CORNER_SIZE_TOP_LEFT_V.x,
                          CORNER_POS_TOP_LEFT.y) };

                      const auto BOT_POS_V { sf::Vector2f(
                          CORNER_POS_BOT_LEFT.x + CORNER_SIZE_BOT_LEFT_V.x,
                          CORNER_POS_BOT_LEFT.y + GuiImages::GetLineSmallBoxPadBot()) };

                      const auto RIGHT_POS_V { sf::Vector2f(
                          CORNER_POS_TOP_RIGHT.x + GuiImages::GetLineSmallBoxPadRight(),
                          CORNER_POS_TOP_RIGHT.y + CORNER_SIZE_TOP_RIGHT_V.y) };

                      const auto LEFT_POS_V { sf::Vector2f(
                          CORNER_POS_TOP_LEFT.x + GuiImages::GetLineSmallBoxPadLeft(),
                          CORNER_POS_TOP_LEFT.y + CORNER_SIZE_TOP_LEFT_V.y) };

                      return std::make_tuple(TOP_POS_V, RIGHT_POS_V, BOT_POS_V, LEFT_POS_V);
                  }();

            const auto [SIDE_HEIGHT_TOP, SIDE_WIDTH_RIGHT, SIDE_HEIGHT_BOT, SIDE_WIDTH_LEFT] =
                [&,
                 SIDE_START_POS_TOP = SIDE_START_POS_TOP,
                 SIDE_START_POS_RIGHT = SIDE_START_POS_RIGHT,
                 SIDE_START_POS_BOT = SIDE_START_POS_BOT,
                 SIDE_START_POS_LEFT = SIDE_START_POS_LEFT,
                 TEXTURE_RECT_SIDE_TOP = TEXTURE_RECT_SIDE_TOP,
                 TEXTURE_RECT_SIDE_RIGHT = TEXTURE_RECT_SIDE_RIGHT,
                 TEXTURE_RECT_SIDE_BOT = TEXTURE_RECT_SIDE_BOT,
                 TEXTURE_RECT_SIDE_LEFT = TEXTURE_RECT_SIDE_LEFT]() {
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

                    const auto WIDTH_OVERLAP { SIDE_START_POS_RIGHT.x
                                               - (SIDE_START_POS_LEFT.x
                                                  + TEXTURE_RECT_SIDE_LEFT.width) };

                    if (WIDTH_OVERLAP < 0.0f)
                    {
                        const auto HORIZ_SHRINK { (std::abs(WIDTH_OVERLAP) * 0.5f) };
                        widthLeft -= HORIZ_SHRINK;
                        widthRight -= HORIZ_SHRINK;
                    }

                    return std::make_tuple(heightTop, widthRight, heightBot, widthLeft);
                }();

            sfml_util::AppendVertexesForQuad(
                vertexArray_,
                CORNER_POS_TOP_LEFT,
                TEXTURE_RECT_CORNER_TOP_LEFT,
                sf::Color::White,
                CORNER_SIZE_TOP_LEFT_V);

            sfml_util::AppendVertexesForQuadRepeatedOverLength(
                vertexArray_,
                SIDE_START_POS_TOP,
                TEXTURE_RECT_SIDE_TOP,
                Orientation::Horiz,
                SIDE_LEN_TOP,
                sf::Color::White,
                SIDE_HEIGHT_TOP);

            sfml_util::AppendVertexesForQuad(
                vertexArray_,
                CORNER_POS_TOP_RIGHT,
                TEXTURE_RECT_CORNER_TOP_RIGHT,
                sf::Color::White,
                CORNER_SIZE_TOP_RIGHT_V,
                Orientation::Horiz);

            sfml_util::AppendVertexesForQuadRepeatedOverLength(
                vertexArray_,
                SIDE_START_POS_RIGHT,
                TEXTURE_RECT_SIDE_RIGHT,
                Orientation::Vert,
                SIDE_LEN_RIGHT,
                sf::Color::White,
                SIDE_WIDTH_RIGHT,
                Orientation::Horiz);

            sfml_util::AppendVertexesForQuad(
                vertexArray_,
                CORNER_POS_BOT_RIGHT,
                TEXTURE_RECT_CORNER_BOT_RIGHT,
                sf::Color::White,
                CORNER_SIZE_BOT_RIGHT_V,
                Orientation::Both);

            sfml_util::AppendVertexesForQuadRepeatedOverLength(
                vertexArray_,
                SIDE_START_POS_BOT,
                TEXTURE_RECT_SIDE_BOT,
                Orientation::Horiz,
                SIDE_LEN_BOT,
                sf::Color::White,
                SIDE_HEIGHT_BOT,
                Orientation::Vert);

            sfml_util::AppendVertexesForQuad(
                vertexArray_,
                CORNER_POS_BOT_LEFT,
                TEXTURE_RECT_CORNER_BOT_LEFT,
                sf::Color::White,
                CORNER_SIZE_BOT_LEFT_V,
                Orientation::Vert);

            sfml_util::AppendVertexesForQuadRepeatedOverLength(
                vertexArray_,
                SIDE_START_POS_LEFT,
                TEXTURE_RECT_SIDE_LEFT,
                Orientation::Vert,
                SIDE_LEN_LEFT,
                sf::Color::White,
                SIDE_WIDTH_LEFT);

            if (WILL_GROW_BORDER_TO_CONTAIN_REGION)
            {
                innerRegion_ = REGION;

                outerRegion_ = sf::FloatRect(
                    CORNER_POS_TOP_LEFT, CORNER_POS_BOT_RIGHT + CORNER_SIZE_BOT_RIGHT_V);
            }
            else
            {
                outerRegion_ = REGION;

                const sf::Vector2f INNER_POS_V(
                    SIDE_START_POS_LEFT.x + SIDE_WIDTH_LEFT,
                    SIDE_START_POS_TOP.y + SIDE_HEIGHT_TOP);

                const sf::Vector2f INNER_SIZE_V(
                    SIDE_START_POS_RIGHT.x - INNER_POS_V.x, SIDE_START_POS_BOT.y - INNER_POS_V.y);

                innerRegion_ = sf::FloatRect(INNER_POS_V, INNER_SIZE_V);
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
} // namespace sfml_util
} // namespace heroespath
