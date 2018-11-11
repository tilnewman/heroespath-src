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
#include "misc/vectors.hpp"
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
        using Grid_t = int;

        using Vector2g_t = sf::Vector2<Grid_t>;
        using Vector2s_t = sf::Vector2<std::size_t>;

        using GridRect_t = sf::Rect<Grid_t>;
        using GridRectVec_t = std::vector<GridRect_t>;

        using CellIndexPos_t = Vector2s_t;
        using CellIndexPosVec_t = std::vector<CellIndexPos_t>;

    public:
        CollisionGrid()
            : cellSize_(0.0f)
            , cellCountVS_(0, 0)
            , collRectVecs_()
            , cellIndexPosVec_()
            , cellIndexPosCount_(0)
        {
            cellIndexPosVec_.resize(128, CellIndexPos_t(0, 0)); // this value found by experiment
        }

        CollisionGrid(const CollisionGrid &) = delete;
        CollisionGrid(CollisionGrid &&) = delete;
        CollisionGrid & operator=(const CollisionGrid &) = delete;
        CollisionGrid & operator=(CollisionGrid &&) = delete;

        void Setup(
            const sf::Vector2f & TOTAL_SIZE_VF,
            const std::vector<sf::FloatRect> & COLL_RECTS_TO_ADD)
        {
            collRectVecs_.clear();
            cellIndexPosCount_ = 0;

            // initial calculations
            cellSize_ = Grid_t(100);

            const auto CELL_COUNT_VF { TOTAL_SIZE_VF / static_cast<float>(cellSize_) };

            const auto CELL_COUNT_CEIL_VF { sf::Vector2f(
                std::ceil(CELL_COUNT_VF.x), std::ceil(CELL_COUNT_VF.y)) };

            cellCountVS_ = Vector2s_t { CELL_COUNT_CEIL_VF };

            // create cells
            std::vector<std::vector<GridRectVec_t>> vecVecTemp;
            vecVecTemp.resize(cellCountVS_.y);
            for (auto & rectVecVec : vecVecTemp)
            {
                rectVecVec.resize(cellCountVS_.x);

                for (auto & rectVec : rectVecVec)
                {
                    // found by experiment to be a good guess for most game maps
                    rectVec.reserve(64);
                }
            }

            collRectVecs_.resize(
                (cellCountVS_.x * cellCountVS_.y)); // + (cellCountVS_.x + cellCountVS_.y) + 1);

            for (auto & rectVec : collRectVecs_)
            {
                rectVec.reserve(64);
            }

            // populate cells into vecVecTemp
            for (const auto & COLL_RECT_TO_ADD : COLL_RECTS_TO_ADD)
            {
                SetupCellIndexPositionsForRect(GridRect_t { COLL_RECT_TO_ADD });

                for (std::size_t index(0); index < cellIndexPosCount_; ++index)
                {
                    const auto & CELL_INDEX_POS_V { cellIndexPosVec_.at(index) };

                    vecVecTemp.at(CELL_INDEX_POS_V.y)
                        .at(CELL_INDEX_POS_V.x)
                        .emplace_back(COLL_RECT_TO_ADD);
                }
            }

            // copy collision rects into collRectVecs_
            for (std::size_t vertIndex(0); vertIndex < cellCountVS_.y; ++vertIndex)
            {
                for (std::size_t horizIndex(0); horizIndex < cellCountVS_.x; ++horizIndex)
                {
                    const CellIndexPos_t CELL_INDEX_POS_V(horizIndex, vertIndex);

                    const auto FROM_GRID {
                        vecVecTemp.at(CELL_INDEX_POS_V.y).at(CELL_INDEX_POS_V.x)
                    };

                    const auto OTHER_INDEX { CalcIndexFromCellPos(CELL_INDEX_POS_V) };

                    collRectVecs_.at(OTHER_INDEX) = FROM_GRID;
                    collRectVecs_.at(OTHER_INDEX).reserve(128);
                }
            }
        }

        bool DoesPointCollide(const sf::Vector2f & POS_VF)
        {
            const Vector2g_t POS_VG { POS_VF };
            SetupCellIndexPositionsForPoint(POS_VG);

            for (std::size_t index(0); index < cellIndexPosCount_; ++index)
            {
                const auto & CELL_INDEX_POS_V { cellIndexPosVec_.at(index) };

                const auto & COLL_RECTS { collRectVecs_.at(
                    CalcIndexFromCellPos(CELL_INDEX_POS_V)) };

                for (const auto & COLL_RECT : COLL_RECTS)
                {
                    if (COLL_RECT.contains(POS_VG))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        bool DoesRectCollide(const sf::FloatRect & RECT_TO_CHECK_FR)
        {
            const GridRect_t RECT_TO_CHECK_GR { RECT_TO_CHECK_FR };
            SetupCellIndexPositionsForRect(RECT_TO_CHECK_GR);

            for (std::size_t index(0); index < cellIndexPosCount_; ++index)
            {
                const auto & CELL_INDEX_POS_V { cellIndexPosVec_.at(index) };

                const auto & COLL_RECTS { collRectVecs_.at(
                    CalcIndexFromCellPos(CELL_INDEX_POS_V)) };

                for (const auto & COLL_RECT : COLL_RECTS)
                {
                    if (COLL_RECT.intersects(RECT_TO_CHECK_GR))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

    private:
        std::size_t CalcIndexFromCellPos(const CellIndexPos_t & CELL_INDEX_POS_V) const
        {
            return ((CELL_INDEX_POS_V.y * cellCountVS_.y) + CELL_INDEX_POS_V.x);
        }

        void SetupCellIndexPositionsForPoint(const Vector2g_t & POS_VG)
        {
            cellIndexPosVec_.at(0) = CalcCellIndexPosition(POS_VG);
            cellIndexPosCount_ = 1;
        }

        void SetupCellIndexPositionsForRect(const GridRect_t & RECT_G)
        {
            cellIndexPosCount_ = 0;

            const auto TOP_LEFT { CalcCellIndexPosition(sfutil::TopLeft(RECT_G)) };
            const auto BOT_RIGHT { CalcCellIndexPosition(sfutil::BottomRight(RECT_G)) };

            const auto VERT_ITER_COUNT { (BOT_RIGHT.y - TOP_LEFT.y) + 1 };
            const auto HORIZ_ITER_COUNT { (BOT_RIGHT.x - TOP_LEFT.x) + 1 };

            if ((VERT_ITER_COUNT > 1) || (HORIZ_ITER_COUNT > 1))
            {
                const auto ITER_COUNT_TOTAL { (VERT_ITER_COUNT * HORIZ_ITER_COUNT) };

                if (ITER_COUNT_TOTAL > cellIndexPosVec_.size())
                {
                    cellIndexPosVec_.resize(ITER_COUNT_TOTAL, CellIndexPos_t(0, 0));
                }

                for (std::size_t vertIndex(0); vertIndex < VERT_ITER_COUNT; ++vertIndex)
                {
                    for (std::size_t horizIndex(0); horizIndex < HORIZ_ITER_COUNT; ++horizIndex)
                    {
                        cellIndexPosVec_.at(cellIndexPosCount_++)
                            = CellIndexPos_t((TOP_LEFT.x + horizIndex), (TOP_LEFT.y + vertIndex));
                    }
                }
            }
            else
            {
                cellIndexPosVec_.at(0) = TOP_LEFT;
                cellIndexPosVec_.at(1) = BOT_RIGHT;
                cellIndexPosCount_ = 2;
            }
        }

        const CellIndexPos_t CalcCellIndexPosition(const Vector2g_t & POS_VG)
        {
            CellIndexPos_t cellIndexPosV { (POS_VG / cellSize_) };

            {
                const auto CELL_COUNT_MAX_HORIZ { cellCountVS_.x - 1 };
                if (cellIndexPosV.x > CELL_COUNT_MAX_HORIZ)
                {
                    cellIndexPosV.x = CELL_COUNT_MAX_HORIZ;
                }
            }

            {
                const auto CELL_COUNT_MAX_VERT { cellCountVS_.y - 1 };
                if (cellIndexPosV.y > CELL_COUNT_MAX_VERT)
                {
                    cellIndexPosV.y = CELL_COUNT_MAX_VERT;
                }
            }

            return cellIndexPosV;
        }

    private:
        Grid_t cellSize_;
        Vector2s_t cellCountVS_;
        std::vector<GridRectVec_t> collRectVecs_;
        CellIndexPosVec_t cellIndexPosVec_;
        std::size_t cellIndexPosCount_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_COLLISION_GRID_HPP_INCLUDED
