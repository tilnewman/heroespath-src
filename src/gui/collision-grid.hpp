// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_COLLISION_GRID_HPP_INCLUDED
#define HEROESPATH_GUI_COLLISION_GRID_HPP_INCLUDED
//
// collision-grid.hpp
//
#include "misc/strings.hpp"
#include "misc/timing.hpp"
#include "misc/vectors.hpp"
#include "sfutil/common.hpp"
#include "sfutil/vector-and-rect.hpp"

#include <SFML/Graphics/Rect.hpp>

#include <cmath>
#include <vector>

namespace heroespath
{
namespace gui
{

    // Responsible for implementing a collision detection grid map.
    // Assumes the map starts at zero,zero and that call values given are >= zero.
    class CollisionGrid
    {
        using Pos_t = int; // needs to be signed
        using Index_t = std::size_t;

        using PosV_t = sf::Vector2<Pos_t>;
        using IndexV_t = sf::Vector2<Index_t>;

        using PosRect_t = sf::Rect<Pos_t>;
        using PosRectVec_t = std::vector<PosRect_t>;

    public:
        CollisionGrid()
            : cellSize_(0)
            , cellCountV_(0, 0)
            , collRectVecs_()
            , collRectVecIndexes_()
        {
            // no player/npc character rect will ever match more than four grids at once, but
            // collision rects being added might hit many grid cells
            collRectVecIndexes_.reserve(1024);
        }

        CollisionGrid(const CollisionGrid &) = delete;
        CollisionGrid(CollisionGrid &&) = delete;
        CollisionGrid & operator=(const CollisionGrid &) = delete;
        CollisionGrid & operator=(CollisionGrid &&) = delete;

        void Setup(
            const sf::Vector2f & TOTAL_SIZE_VF,
            const std::vector<sf::FloatRect> & COLL_RECTS_TO_ADD)
        {
            // erase old values
            collRectVecs_.clear();
            collRectVecIndexes_.clear();

            // initial calculations
            cellSize_ = Pos_t(100);
            const auto CELL_SIZE_F { static_cast<float>(cellSize_) };

            // use ceil() to make sure we round up on the total size of the map
            const sf::Vector2f CELL_COUNT_VF_CEIL(
                (std::ceil(TOTAL_SIZE_VF.x) / CELL_SIZE_F),
                (std::ceil(TOTAL_SIZE_VF.y) / CELL_SIZE_F));

            // use ceil()+1 to make sure we round up on the number of cells in the map grid
            const auto CELL_COUNT_VF_FINAL {
                sf::Vector2f(std::ceil(CELL_COUNT_VF_CEIL.x), std::ceil(CELL_COUNT_VF_CEIL.y))
                + sf::Vector2f(1.0f, 1.0f)
            };

            // convert the final cell counts to Index_t (std::size_t)
            cellCountV_ = IndexV_t { CELL_COUNT_VF_FINAL };

            // use the final (overestimated) cell counts to get the total size of the map grid
            gridSizeV_.x = (cellSize_ * static_cast<Pos_t>(cellCountV_.x));
            gridSizeV_.y = (cellSize_ * static_cast<Pos_t>(cellCountV_.y));

            // create cells/collision_rects
            collRectVecs_.resize((cellCountV_.x * cellCountV_.y));
            for (auto & rectVec : collRectVecs_)
            {
                rectVec.reserve(64);
            }

            std::vector<TimeCount_t> cellsPerCollRectVec;

            // populate cells/collision_rects
            for (const auto & COLL_RECT_TO_ADD : COLL_RECTS_TO_ADD)
            {
                SetupCollRectVecIndexesForRect(PosRect_t { COLL_RECT_TO_ADD });

                cellsPerCollRectVec.emplace_back(
                    static_cast<TimeCount_t>(collRectVecIndexes_.size()));

                for (const auto INDEX : collRectVecIndexes_)
                {
                    collRectVecs_[INDEX].emplace_back(COLL_RECT_TO_ADD);
                }
            }

            M_HP_LOG_WRN(
                "Collision Grid Setup: " + M_HP_VAR_STR(TOTAL_SIZE_VF) + M_HP_VAR_STR(gridSizeV_)
                + M_HP_VAR_STR(cellSize_) + M_HP_VAR_STR(cellCountV_)
                + M_HP_VAR_STR(collRectVecs_.size()));

            misc::TimeStats cellsPerCollRectStats(cellsPerCollRectVec, 0.0, 0);
            M_HP_LOG_WRN("cellsPerCollRectStats=" << cellsPerCollRectStats.ToString(true, false));

            std::vector<TimeCount_t> collRectsPerCellVec;
            for (const auto & COLL_RECT_VEC : collRectVecs_)
            {
                collRectsPerCellVec.emplace_back(static_cast<TimeCount_t>(COLL_RECT_VEC.size()));
            }

            misc::TimeStats collRectsPerCellStats(collRectsPerCellVec, 0.0, 0);
            M_HP_LOG_WRN("collRectsPerCellStats=" << collRectsPerCellStats.ToString(true, false));

            // TestingMaddnessPoints();
            // TestingMaddnessRects();
        }

        void TestingMaddnessPoints()
        {
            M_HP_LOG_WRN("TestingMaddnessPoints");

            auto prevIndex = std::numeric_limits<Index_t>::max();
            auto posRect = PosRect_t(0, 0, 0, 0);
            for (Pos_t i(0); i <= gridSizeV_.x; ++i)
            {
                posRect.left = i;
                SetupCollRectVecIndexesForRect(posRect);

                if (collRectVecIndexes_.size() != 1)
                {
                    M_HP_LOG_ERR(
                        "\t point_rect=" << posRect
                                         << " point test ended up with something other than one ="
                                         << collRectVecIndexes_.size());
                }

                const auto CURRENT_INDEX { collRectVecIndexes_.at(0) };
                if (CURRENT_INDEX != prevIndex)
                {
                    M_HP_LOG_DBG("\t point_rect=" << posRect << ", index=" << CURRENT_INDEX);
                    prevIndex = CURRENT_INDEX;
                }
            }

            M_HP_LOG_WRN("spacer")
            prevIndex = std::numeric_limits<Index_t>::max();
            posRect = PosRect_t(0, gridSizeV_.y, 0, 0);
            for (Pos_t i(0); i <= gridSizeV_.x; ++i)
            {
                posRect.left = i;
                SetupCollRectVecIndexesForRect(posRect);

                if (collRectVecIndexes_.size() != 1)
                {
                    M_HP_LOG_ERR(
                        "\t point_rect=" << posRect
                                         << " point test ended up with something other than one ="
                                         << collRectVecIndexes_.size());
                }

                const auto CURRENT_INDEX { collRectVecIndexes_.at(0) };
                if (CURRENT_INDEX != prevIndex)
                {
                    M_HP_LOG_DBG("\t  point_rect=" << posRect << ", index=" << CURRENT_INDEX);
                    prevIndex = CURRENT_INDEX;
                }
            }

            M_HP_LOG_WRN("spacer")
            prevIndex = std::numeric_limits<Index_t>::max();
            posRect = PosRect_t(0, 0, 0, 0);
            for (Pos_t i(0); i <= gridSizeV_.y; ++i)
            {
                posRect.top = i;
                SetupCollRectVecIndexesForRect(posRect);

                if (collRectVecIndexes_.size() != 1)
                {
                    M_HP_LOG_ERR(
                        "\t point_rect=" << posRect
                                         << " point test ended up with something other than one ="
                                         << collRectVecIndexes_.size());
                }

                const auto CURRENT_INDEX { collRectVecIndexes_.at(0) };
                if (CURRENT_INDEX != prevIndex)
                {
                    M_HP_LOG_DBG("\t  point_rect=" << posRect << ", index=" << CURRENT_INDEX);
                    prevIndex = CURRENT_INDEX;
                }
            }

            M_HP_LOG_WRN("spacer")
            prevIndex = std::numeric_limits<Index_t>::max();
            posRect = PosRect_t(gridSizeV_.x, 0, 0, 0);
            for (Pos_t i(0); i <= gridSizeV_.y; ++i)
            {
                posRect.top = i;
                SetupCollRectVecIndexesForRect(posRect);

                if (collRectVecIndexes_.size() != 1)
                {
                    M_HP_LOG_ERR(
                        "\t point_rect=" << posRect
                                         << " point test ended up with something other than one ="
                                         << collRectVecIndexes_.size());
                }

                const auto CURRENT_INDEX { collRectVecIndexes_.at(0) };
                if (CURRENT_INDEX != prevIndex)
                {
                    M_HP_LOG_DBG("\t  point_rect=" << posRect << ", index=" << CURRENT_INDEX);
                    prevIndex = CURRENT_INDEX;
                }
            }

            M_HP_LOG_WRN("spacer")
            prevIndex = std::numeric_limits<Index_t>::max();
            posRect = PosRect_t(0, 0, 0, 0);
            for (Pos_t i(0); i <= misc::Max(gridSizeV_.x, gridSizeV_.y); ++i)
            {
                posRect.left = misc::Min(i, gridSizeV_.x);
                posRect.top = misc::Min(i, gridSizeV_.y);

                SetupCollRectVecIndexesForRect(posRect);

                if (collRectVecIndexes_.size() != 1)
                {
                    M_HP_LOG_ERR(
                        "\t point_rect=" << posRect
                                         << " point test ended up with something other than one ="
                                         << collRectVecIndexes_.size());
                }

                const auto CURRENT_INDEX { collRectVecIndexes_.at(0) };
                if (CURRENT_INDEX != prevIndex)
                {
                    M_HP_LOG_DBG("\t  point_rect=" << posRect << ", index=" << CURRENT_INDEX);
                    prevIndex = CURRENT_INDEX;
                }
            }
        }

        void TestingMaddnessRects()
        {
            M_HP_LOG_WRN("TestingMaddnessRects");

            auto posRect = PosRect_t(0, 0, 90, 90);
            for (Pos_t i(0); i <= gridSizeV_.x; ++i)
            {
                posRect.left = i;
                SetupCollRectVecIndexesForRect(posRect);
            }

            M_HP_LOG_WRN("spacer")
            posRect = PosRect_t(0, gridSizeV_.y, 90, 90);
            for (Pos_t i(0); i <= gridSizeV_.x; ++i)
            {
                posRect.left = i;
                SetupCollRectVecIndexesForRect(posRect);
            }

            M_HP_LOG_WRN("spacer")
            posRect = PosRect_t(0, 0, 90, 90);
            for (Pos_t i(0); i <= gridSizeV_.y; ++i)
            {
                posRect.top = i;
                SetupCollRectVecIndexesForRect(posRect);
            }

            M_HP_LOG_WRN("spacer")
            posRect = PosRect_t(gridSizeV_.x, 0, 90, 90);
            for (Pos_t i(0); i <= gridSizeV_.y; ++i)
            {
                posRect.top = i;
                SetupCollRectVecIndexesForRect(posRect);
            }

            M_HP_LOG_WRN("spacer")
            posRect = PosRect_t(0, 0, 90, 90);
            for (Pos_t i(0); i <= misc::Max(gridSizeV_.x, gridSizeV_.y); ++i)
            {
                posRect.left = misc::Min(i, gridSizeV_.x);
                posRect.top = misc::Min(i, gridSizeV_.y);
                SetupCollRectVecIndexesForRect(posRect);
            }
        }

        bool DoesPointCollide(const sf::Vector2f & POS_VF_ORIG) const
        {
            const PosV_t POS_V_FINAL { POS_VF_ORIG };
            SetupCollRectVecIndexesForPoint(POS_V_FINAL);

            for (const auto INDEX : collRectVecIndexes_)
            {
                for (const auto & COLL_RECT : collRectVecs_[INDEX])
                {
                    if (COLL_RECT.contains(POS_V_FINAL))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        bool DoesRectCollide(const sf::FloatRect & RECT_TO_CHECK_F_ORIG) const
        {
            const PosRect_t RECT_TO_CHECK_FINAL { RECT_TO_CHECK_F_ORIG };

            SetupCollRectVecIndexesForRect(RECT_TO_CHECK_FINAL);

            for (const auto INDEX : collRectVecIndexes_)
            {
                for (const auto & COLL_RECT : collRectVecs_[INDEX])
                {
                    if (COLL_RECT.intersects(RECT_TO_CHECK_FINAL))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

    private:
        Index_t CalcCollRectVecIndexFromIndexV(const IndexV_t & CELL_INDEX_POS_V) const
        {
            return ((CELL_INDEX_POS_V.y * cellCountV_.y) + CELL_INDEX_POS_V.x);
        }

        void SetupCollRectVecIndexesForPoint(const PosV_t & POS_VG) const
        {
            collRectVecIndexes_.clear();

            collRectVecIndexes_.emplace_back(
                CalcCollRectVecIndexFromIndexV(CalcCellIndexFromPosV(POS_VG)));
        }

        void SetupCollRectVecIndexesForRect(const PosRect_t & POS_RECT) const
        {
            collRectVecIndexes_.clear();

            const auto GRID_POS_V_TOP_LEFT { CalcCellIndexFromPosV(sfutil::TopLeft(POS_RECT)) };

            const auto GRID_POS_V_BOT_RIGHT { CalcCellIndexFromPosV(
                sfutil::BottomRight(POS_RECT)) };

            if (GRID_POS_V_TOP_LEFT == GRID_POS_V_BOT_RIGHT)
            {
                collRectVecIndexes_.emplace_back(
                    CalcCollRectVecIndexFromIndexV(GRID_POS_V_TOP_LEFT));
            }
            else
            {
                const auto VERT_CELL_COUNT { (GRID_POS_V_BOT_RIGHT.y - GRID_POS_V_TOP_LEFT.y) + 1 };

                const auto HORIZ_CELL_COUNT { (GRID_POS_V_BOT_RIGHT.x - GRID_POS_V_TOP_LEFT.x)
                                              + 1 };

                for (Index_t vertIndex(0); vertIndex < VERT_CELL_COUNT; ++vertIndex)
                {
                    for (Index_t horizIndex(0); horizIndex < HORIZ_CELL_COUNT; ++horizIndex)
                    {
                        const IndexV_t GRID_POS_V_NEXT(
                            (GRID_POS_V_TOP_LEFT.x + horizIndex),
                            (GRID_POS_V_TOP_LEFT.y + vertIndex));

                        collRectVecIndexes_.emplace_back(
                            CalcCollRectVecIndexFromIndexV(GRID_POS_V_NEXT));
                    }
                }
            }
        }

        const IndexV_t CalcCellIndexFromPosV(const PosV_t & POS_V) const
        {
            const IndexV_t INDEX_V_RAW { PosV_t((POS_V.x / cellSize_), (POS_V.y / cellSize_)) };

            // clamp cell indexes to valid values since they will be used to directly index a vector
            const IndexV_t GRID_POS_V_FINAL {
                std::clamp(INDEX_V_RAW.x, Index_t(0), (cellCountV_.x - 1)),
                std::clamp(INDEX_V_RAW.y, Index_t(0), (cellCountV_.y - 1))
            };

            return GRID_POS_V_FINAL;
        }

    private:
        Pos_t cellSize_;
        PosV_t gridSizeV_;
        IndexV_t cellCountV_;
        std::vector<PosRectVec_t> collRectVecs_;
        mutable std::vector<Index_t> collRectVecIndexes_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_COLLISION_GRID_HPP_INCLUDED
