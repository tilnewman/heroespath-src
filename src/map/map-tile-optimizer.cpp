// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// map-tile-optimizer.cpp
//
#include "map-tile-optimizer.hpp"

#include "map/layer.hpp"
#include "map/layout.hpp"
#include "sfutil/vertex.hpp"

#include <SFML/Graphics/RenderTexture.hpp>

#include <algorithm>

namespace heroespath
{
namespace map
{

    const sf::Texture & MapTileOptimizer::Packet::MapTileTexture(const std::size_t INDEX) const
    {
        if (textures.empty())
        {
            return layout.texture_vec.at(INDEX).Get();
        }
        else
        {
            return textures.at(INDEX);
        }
    }

    std::size_t MapTileOptimizer::Packet::MapTileTextureCount() const
    {
        if (textures.empty())
        {
            return layout.texture_vec.size();
        }
        else
        {
            return textures.size();
        }
    }

    const std::size_t MapTileOptimizer::TEXTURE_DIMMENSION_MAX_ { 1024 };

    void MapTileOptimizer::Optimize(Packet & packet)
    {
        packet.textures.clear();
        PopulateTileDraws(packet);
        OptimizeTextures(packet);

        // This pulls as many draws as possible down to the lowest layer possible resulting in the
        // lowest layer having almost all of the draws and then the other upper layers only have
        // whatever draws overlap.  I thought this would speed up MapDisplay, but I've run the
        // numbers and it just doesn't.  So, since this optimization does not actually optimize
        // anything, and since it adds 5-10 seconds to map loads during debug runs on windows, I'm
        // commenting this out. zTn 2018-12-3
        // OptimizeLayers(packet);

        // LogSummary(packet);
    }

    void MapTileOptimizer::PopulateTileDraws(Packet & packet)
    {
        packet.tile_draws_below.clear();
        packet.tile_draws_above.clear();

        for (const auto & LAYER : packet.layout.layer_vec)
        {
            PopulateTileDrawsForLayer(packet, LAYER);
        }

        // a lot of other code depends on this sort by low to high layer and the texture
        std::sort(std::begin(packet.tile_draws_below), std::end(packet.tile_draws_below));
        std::sort(std::begin(packet.tile_draws_above), std::end(packet.tile_draws_above));
    }

    void MapTileOptimizer::PopulateTileDrawsForLayer(Packet & packet, const Layer & LAYER)
    {
        for (int tileX(0); tileX < packet.tile_count_v.x; ++tileX)
        {
            for (int tileY(0); tileY < packet.tile_count_v.y; ++tileY)
            {
                const auto TILE_INDEXES { sf::Vector2i(tileX, tileY) };
                const auto LAYER_TILE_NUMBER { packet.layout.LayerTileNumber(LAYER, TILE_INDEXES) };

                const auto [FIRST_TILE_ID, TEXTURE_INDEX]
                    = packet.layout.FindTileTextureInfo(LAYER_TILE_NUMBER);

                if (TEXTURE_INDEX >= packet.layout.texture_vec.size())
                {
                    continue;
                }

                const auto TEXTURE_TILE_NUMBER { (LAYER_TILE_NUMBER - FIRST_TILE_ID) };

                const TileDraw TILE_DRAW { MakeTileDraw(
                    packet, LAYER, TEXTURE_INDEX, TILE_INDEXES, TEXTURE_TILE_NUMBER) };

                if (LAYER.type == LayerType::Ground)
                {
                    packet.tile_draws_below.emplace_back(TILE_DRAW);
                }
                else
                {
                    packet.tile_draws_above.emplace_back(TILE_DRAW);
                }
            }
        }
    }

    const TileDraw MapTileOptimizer::MakeTileDraw(
        const Packet & PACKET,
        const Layer & LAYER,
        const std::size_t TEXTURE_INDEX,
        const sf::Vector2i & TILE_INDEXES,
        const int TEXTURE_TILE_NUMBER)
    {
        const sf::Texture & TEXTURE_REF { PACKET.layout.texture_vec[TEXTURE_INDEX].Get() };

        const auto TEXTURE_WIDTH { static_cast<int>(TEXTURE_REF.getSize().x) };
        const auto TEXTURE_WIDTH_IN_TILES { TEXTURE_WIDTH / PACKET.tile_size_v.x };

        const int TEXTURE_RECT_TILE_LEFT { (TEXTURE_TILE_NUMBER % TEXTURE_WIDTH_IN_TILES) };
        const int TEXTURE_RECT_TILE_TOP { (TEXTURE_TILE_NUMBER / TEXTURE_WIDTH_IN_TILES) };

        const int TEXTURE_RECT_LEFT { (TEXTURE_RECT_TILE_LEFT * PACKET.tile_size_v.x) };
        const int TEXTURE_RECT_TOP { (TEXTURE_RECT_TILE_TOP * PACKET.tile_size_v.y) };

        const auto TEXTURE_RECT { sf::IntRect(
            sf::Vector2i(TEXTURE_RECT_LEFT, TEXTURE_RECT_TOP), PACKET.tile_size_v) };

        return TileDraw(LAYER.index, TILE_INDEXES, TEXTURE_INDEX, TEXTURE_RECT);
    }

    void MapTileOptimizer::OptimizeTextures(Packet & packet)
    {
        if (packet.layout.texture_vec.empty())
        {
            return;
        }

        const std::size_t MAX_TILES_PER_NEW_TEXTURE { TilesInTexture(
            packet.tile_size_v, TEXTURE_DIMMENSION_MAX_) };

        OptimizeTexturesSet(
            MAX_TILES_PER_NEW_TEXTURE,
            packet.tile_size_v,
            packet.layout.texture_vec,
            packet.textures,
            packet.tile_draws_below);

        OptimizeTexturesSet(
            MAX_TILES_PER_NEW_TEXTURE,
            packet.tile_size_v,
            packet.layout.texture_vec,
            packet.textures,
            packet.tile_draws_above);
    }

    void MapTileOptimizer::OptimizeTexturesSet(
        const std::size_t MAX_TILES_PER_NEW_TEXTURE,
        const sf::Vector2i & TILE_SIZE_V,
        const std::vector<gui::CachedTexture> & TEXTURES_ORIG,
        std::vector<sf::Texture> & texturesNew,
        TileDrawVec_t & tileDraws)
    {
        if (tileDraws.empty())
        {
            return;
        }

        // create a minimally complete collection of unique layer/texture/rects/TileDrawIndexes
        using RectTileDrawIndexesMap_t = misc::VectorMap<sf::IntRect, std::vector<std::size_t>>;
        using TextureRectsMap_t = misc::VectorMap<std::size_t, RectTileDrawIndexesMap_t>;
        using LayerTexturesMap_t = misc::VectorMap<std::size_t, TextureRectsMap_t>;

        LayerTexturesMap_t layerTilesMap;

        bool isFirstUnique { true };
        for (std::size_t index(0); index < tileDraws.size(); ++index)
        {
            const auto & DRAW { tileDraws.at(index) };

            layerTilesMap[DRAW.layer_index][DRAW.texture_index][DRAW.texture_rect].emplace_back(
                index);

            if (isFirstUnique)
            {
                isFirstUnique = false;
                layerTilesMap.Reserve(32);
                layerTilesMap.Front().second.Reserve(32);
                layerTilesMap.Front().second.Front().second.Reserve(1024);
            }
        }

        // create an collection of TileDraw indexes that share texture_index and texture_rect,
        // in draw order, to be transfered to a new texture
        std::vector<std::vector<std::size_t>> tileDrawIndexVecsToTransferOrig;
        tileDrawIndexVecsToTransferOrig.reserve(tileDraws.size());

        std::size_t tileDrawConfirmCount { 0 };
        for (const auto & LAYER_TEXTURES_PAIR : layerTilesMap)
        {
            for (const auto & TEXTURE_RECTS_PAIR : LAYER_TEXTURES_PAIR.second)
            {
                for (const auto & RECT_INDEXES_PAIR : TEXTURE_RECTS_PAIR.second)
                {
                    tileDrawIndexVecsToTransferOrig.emplace_back(RECT_INDEXES_PAIR.second);
                    tileDrawConfirmCount += RECT_INDEXES_PAIR.second.size();
                }
            }
        }

        // eliminate duplicates that will end up on the same texture
        using ImagePair_t = std::pair<std::size_t, sf::IntRect>;
        misc::VectorMap<ImagePair_t, std::size_t> alreadyTransferedImageToUniqueIndexMap;

        std::vector<std::vector<std::size_t>> tileDrawIndexVecsToTransferUnique;

        std::size_t newTextureImageCounter { 0 };
        for (const auto & TILE_DRAW_INDEXES_ORIG : tileDrawIndexVecsToTransferOrig)
        {
            if (MAX_TILES_PER_NEW_TEXTURE >= ++newTextureImageCounter)
            {
                alreadyTransferedImageToUniqueIndexMap.Clear();
            }

            const auto & REFERENCE_TILE_DRAW { tileDraws.at(TILE_DRAW_INDEXES_ORIG.front()) };

            const auto IMAGE_PAIR { std::make_pair(
                REFERENCE_TILE_DRAW.texture_index, REFERENCE_TILE_DRAW.texture_rect) };

            if (alreadyTransferedImageToUniqueIndexMap.Exists(IMAGE_PAIR))
            {
                auto & tileDrawIndexesToTransferUnique { tileDrawIndexVecsToTransferUnique.at(
                    alreadyTransferedImageToUniqueIndexMap[IMAGE_PAIR]) };

                if (tileDrawIndexesToTransferUnique.empty() == false)
                {
                    tileDrawIndexesToTransferUnique.insert(
                        std::end(tileDrawIndexesToTransferUnique),
                        std::begin(TILE_DRAW_INDEXES_ORIG),
                        std::end(TILE_DRAW_INDEXES_ORIG));
                }
            }
            else
            {
                alreadyTransferedImageToUniqueIndexMap[IMAGE_PAIR]
                    = tileDrawIndexVecsToTransferUnique.size();

                tileDrawIndexVecsToTransferUnique.emplace_back(TILE_DRAW_INDEXES_ORIG);
            }
        }

        // transfer all TileDraws to newly created textures
        while (tileDrawIndexVecsToTransferUnique.empty() == false)
        {
            const auto TILE_VEC_COUNT_IN_NEW_TEXTURE { MakeOptimizedTexture(
                MAX_TILES_PER_NEW_TEXTURE,
                TILE_SIZE_V,
                TEXTURES_ORIG,
                tileDraws,
                texturesNew,
                tileDrawIndexVecsToTransferUnique) };

            tileDrawIndexVecsToTransferUnique.erase(
                std::begin(tileDrawIndexVecsToTransferUnique),
                std::begin(tileDrawIndexVecsToTransferUnique)
                    + static_cast<std::ptrdiff_t>(TILE_VEC_COUNT_IN_NEW_TEXTURE));
        }

        // re-sort after transfer to ensure overlapping draws still work/look right
        std::sort(std::begin(tileDraws), std::end(tileDraws));

        tileDraws.erase(
            std::unique(std::begin(tileDraws), std::end(tileDraws)), std::end(tileDraws));
    }

    std::size_t MapTileOptimizer::MakeOptimizedTexture(
        const std::size_t MAX_TILES_PER_NEW_TEXTURE,
        const sf::Vector2i & TILE_SIZE_V,
        const std::vector<gui::CachedTexture> & TEXTURES_ORIG,
        TileDrawVec_t & tileDraws,
        std::vector<sf::Texture> & texturesNew,
        std::vector<std::vector<std::size_t>> & tileDrawIndexVecsToPutInNewImage)
    {
        const auto TILE_DRAW_INDEXES_COUNT_BEFORE { tileDrawIndexVecsToPutInNewImage.size() };

        if (TILE_DRAW_INDEXES_COUNT_BEFORE == 0)
        {
            return 0;
        }

        const auto NEW_TEXTURE_PIXEL_DIMMENSION = [&]() {
            if (TILE_DRAW_INDEXES_COUNT_BEFORE < MAX_TILES_PER_NEW_TEXTURE)
            {
                std::size_t powerOfTwo { 64 };

                while (
                    (powerOfTwo < TEXTURE_DIMMENSION_MAX_)
                    && (TilesInTexture(TILE_SIZE_V, powerOfTwo) < TILE_DRAW_INDEXES_COUNT_BEFORE))
                {
                    powerOfTwo <<= 1;
                }

                return powerOfTwo;
            }
            else
            {
                return TEXTURE_DIMMENSION_MAX_;
            }
        }();

        sf::RenderTexture renderTexture;

        renderTexture.create(
            static_cast<unsigned>(NEW_TEXTURE_PIXEL_DIMMENSION),
            static_cast<unsigned>(NEW_TEXTURE_PIXEL_DIMMENSION));

        renderTexture.clear(sf::Color::Transparent);

        std::vector<sf::Vertex> copyQuadvertexes;

        const auto NEW_TEXTURE_TILE_COUNT { misc::Min(
            TILE_DRAW_INDEXES_COUNT_BEFORE, MAX_TILES_PER_NEW_TEXTURE) };

        const sf::Vector2<std::size_t> TILE_SIZE_T_V { TILE_SIZE_V };
        const std::size_t MAX_COL_COUNT { (NEW_TEXTURE_PIXEL_DIMMENSION / TILE_SIZE_T_V.x) };
        const std::size_t MAX_ROW_COUNT { (NEW_TEXTURE_PIXEL_DIMMENSION / TILE_SIZE_T_V.y) };

        sf::RenderStates renderStates { sf::RenderStates::Default };

        std::size_t transferVecIndex(0);
        for (std::size_t row(0);
             ((row < MAX_ROW_COUNT) && (transferVecIndex < NEW_TEXTURE_TILE_COUNT));
             ++row)
        {
            for (std::size_t col(0);
                 ((col < MAX_COL_COUNT) && (transferVecIndex < NEW_TEXTURE_TILE_COUNT));
                 ++col)
            {
                // copy/draw the image from the old texture to the new texture
                const sf::Vector2<std::size_t> DEST_POS_V(
                    (col * TILE_SIZE_T_V.x), (row * TILE_SIZE_T_V.y));

                const sf::FloatRect DEST_POS_RECT(
                    sf::Vector2f { DEST_POS_V }, sf::Vector2f { TILE_SIZE_T_V });

                // all the TileDraws in tileDrawIndexVecsToPutInNewImage[transferVecIndex] have
                // the same texture_index and texture_rect
                const auto & FIRST_TILE_DRAW_TO_TRNANSER { tileDraws.at(
                    tileDrawIndexVecsToPutInNewImage.at(transferVecIndex).front()) };

                const std::size_t SOURCE_TEXTURE_INDEX {
                    FIRST_TILE_DRAW_TO_TRNANSER.texture_index
                };
                const sf::FloatRect SOURCE_TEXTURE_RECT {
                    FIRST_TILE_DRAW_TO_TRNANSER.texture_rect
                };

                copyQuadvertexes.clear();
                sfutil::AppendVertexesForQuad(copyQuadvertexes, DEST_POS_RECT, SOURCE_TEXTURE_RECT);

                renderStates.texture = &TEXTURES_ORIG.at(SOURCE_TEXTURE_INDEX).Get();

                renderTexture.draw(
                    &copyQuadvertexes[0], copyQuadvertexes.size(), sf::Quads, renderStates);

                // update all TileDraws with new source texture information
                for (const auto & TILE_DRAW_TO_UPDATE_INDEX :
                     tileDrawIndexVecsToPutInNewImage.at(transferVecIndex))
                {
                    auto & tileDrawToUpdate { tileDraws.at(TILE_DRAW_TO_UPDATE_INDEX) };
                    tileDrawToUpdate.texture_index = texturesNew.size();
                    tileDrawToUpdate.texture_rect = sf::IntRect { DEST_POS_RECT };
                }

                ++transferVecIndex;
            }
        }

        renderTexture.display();

        texturesNew.emplace_back(sf::Texture());
        texturesNew.back().loadFromImage(renderTexture.getTexture().copyToImage());

        return NEW_TEXTURE_TILE_COUNT;
    }

    void MapTileOptimizer::OptimizeLayers(Packet & packet)
    {
        OptimizeLayersSet(packet.tile_draws_below);
        OptimizeLayersSet(packet.tile_draws_above);
    }

    void MapTileOptimizer::OptimizeLayersSet(TileDrawVec_t & tileDraws)
    {
        if (tileDraws.empty())
        {
            return;
        }

        LayerInfoMap_t layerMap;

        // this loop is only to call reserve to speed up the loop below
        const auto LAYER_INDEX_HIGHEST { tileDraws.back().layer_index };
        for (std::size_t index(0); index <= LAYER_INDEX_HIGHEST; ++index)
        {
            layerMap[index].pos_indexes_map.Reserve(tileDraws.size());
        }

        const auto TILE_DRAW_COUNT { tileDraws.size() };
        for (std::size_t index(0); index < TILE_DRAW_COUNT; ++index)
        {
            const auto & TILE_DRAW { tileDraws.at(index) };

            auto & layerInfo { layerMap[TILE_DRAW.layer_index] };

            layerInfo.layer_index = TILE_DRAW.layer_index;
            layerInfo.texture_count_map[TILE_DRAW.texture_index]++;
            layerInfo.pos_indexes_map[TILE_DRAW.tile_index_v].emplace_back(index);
        }

        if (layerMap.Size() < 2)
        {
            return;
        }

        MinimizeLayerOverlap(layerMap, tileDraws);
        EliminateSkippedLayerIndexes(layerMap, tileDraws);
        std::sort(std::begin(tileDraws), std::end(tileDraws));
    }

    void
        MapTileOptimizer::MinimizeLayerOverlap(LayerInfoMap_t & layerMap, TileDrawVec_t & tileDraws)
    {
        if (layerMap.Size() < 2)
        {
            return;
        }

        auto iter { std::begin(layerMap) };

        while ((iter + 1) != std::end(layerMap))
        {
            auto const DID_ANY_TILE_POSITIONS_DESCEND { DescendNonOverlappingTiles(
                iter->second, (iter + 1)->second, tileDraws) };

            if (DID_ANY_TILE_POSITIONS_DESCEND && (iter != std::begin(layerMap)))
            {
                auto backTrackIter { iter };

                while (--backTrackIter != std::begin(layerMap))
                {
                    if (DescendNonOverlappingTiles(
                            backTrackIter->second, (backTrackIter + 1)->second, tileDraws)
                        == false)
                    {
                        break;
                    }
                }
            }

            if ((iter + 1)->second.pos_indexes_map.Empty())
            {
                iter = (layerMap.Erase((iter + 1)) - 1);
            }
            else
            {
                ++iter;
            }
        }
    }

    void MapTileOptimizer::EliminateSkippedLayerIndexes(
        LayerInfoMap_t & layerMap, TileDrawVec_t & tileDraws)
    {
        const auto MAP_SIZE_MINUS_ONE { layerMap.Size() - 1 };
        for (std::size_t mapIndex(0); mapIndex < MAP_SIZE_MINUS_ONE; ++mapIndex)
        {
            auto & pairCurr { layerMap.At(mapIndex) };
            auto & pairNext { layerMap.At(mapIndex + 1) };

            const auto NEXT_LAYER_INDEX_EXPECTED { (pairCurr.first + 1) };

            if (pairNext.first != NEXT_LAYER_INDEX_EXPECTED)
            {
                pairNext.first = NEXT_LAYER_INDEX_EXPECTED;
                pairNext.second.layer_index = NEXT_LAYER_INDEX_EXPECTED;

                for (auto & posIndexesPair : pairNext.second.pos_indexes_map)
                {
                    for (const auto TILE_DRAW_INDEX : posIndexesPair.second)
                    {
                        tileDraws.at(TILE_DRAW_INDEX).layer_index = NEXT_LAYER_INDEX_EXPECTED;
                    }
                }
            }
        }
    }

    bool MapTileOptimizer::DescendNonOverlappingTiles(
        LayerInfo & layerUnder, LayerInfo & layerOver, TileDrawVec_t & tileDraws)
    {
        if ((layerUnder.layer_index >= layerOver.layer_index)
            || layerUnder.texture_count_map.Empty() || layerOver.texture_count_map.Empty()
            || layerUnder.pos_indexes_map.Empty() || layerOver.pos_indexes_map.Empty())
        {
            return false;
        }

        // only descend tiles if all of the over textures are also in the under
        for (const auto & OVER_TEXTURE_COUNT_PAIR : layerOver.texture_count_map)
        {
            const auto OVER_TEXTURE_INDEX { OVER_TEXTURE_COUNT_PAIR.first };

            if (layerUnder.texture_count_map.Exists(OVER_TEXTURE_INDEX) == false)
            {
                return false;
            }
        }

        std::size_t tilesDescendCount { 0 };
        for (auto & overPosIndexesPair : layerOver.pos_indexes_map)
        {
            auto & overTileDrawIndexes { overPosIndexesPair.second };

            if (overTileDrawIndexes.empty())
            {
                continue;
            }

            const auto & OVER_POS_V { overPosIndexesPair.first };

            auto posIndexesIter { layerUnder.pos_indexes_map.Find(OVER_POS_V) };

            if ((std::end(layerUnder.pos_indexes_map) != posIndexesIter)
                && (posIndexesIter->second.empty() == false))
            {
                continue;
            }

            for (const auto TILE_DRAW_INDEX : overTileDrawIndexes)
            {
                auto & tileDraw { tileDraws.at(TILE_DRAW_INDEX) };
                tileDraw.layer_index = layerUnder.layer_index;
                layerUnder.texture_count_map[tileDraw.texture_index]++;
            }

            if (std::end(layerUnder.pos_indexes_map) == posIndexesIter)
            {
                layerUnder.pos_indexes_map.Append(
                    std::make_pair(OVER_POS_V, std::vector<std::size_t>()));

                layerUnder.pos_indexes_map.Back().second = std::move(overTileDrawIndexes);
            }
            else
            {
                posIndexesIter->second = std::move(overTileDrawIndexes);
            }

            overTileDrawIndexes.clear();

            ++tilesDescendCount;
        }

        if (tilesDescendCount > 0)
        {
            layerOver.pos_indexes_map.Erase(
                std::remove_if(
                    std::begin(layerOver.pos_indexes_map),
                    std::end(layerOver.pos_indexes_map),
                    [](const auto & POS_INDEXES_PAIR) { return POS_INDEXES_PAIR.second.empty(); }),
                std::end(layerOver.pos_indexes_map));

            return true;
        }
        else
        {
            return false;
        }
    }

    void MapTileOptimizer::LogSummary(const Packet & PACKET)
    {
        LogSummaryAll(PACKET);
        LogSummarySet(PACKET, "BELOW", PACKET.tile_draws_below);
        LogSummarySet(PACKET, "ABOVE", PACKET.tile_draws_above);
    }

    void MapTileOptimizer::LogSummaryAll(const Packet & PACKET)
    {
        std::size_t totalTextureCountPixels { 0 };
        std::size_t totalTextureCountTiles { 0 };
        for (std::size_t index(0); index < PACKET.MapTileTextureCount(); ++index)
        {
            const auto & TEXTURE { PACKET.MapTileTexture(index) };
            totalTextureCountPixels += TilesInTexture(PACKET.tile_size_v, TEXTURE);
            totalTextureCountTiles += TilesInTexture(PACKET.tile_size_v, TEXTURE);
        }

        misc::VectorMap<std::pair<std::size_t, sf::IntRect>, int> uniqueImageMap;
        misc::VectorMap<std::size_t, int> uniqueLayerCountMapper;
        misc::VectorMap<sf::Vector2i, int> uniqueMapTileCountMap;

        for (const auto & TILE_DRAW : PACKET.tile_draws_below)
        {
            uniqueMapTileCountMap[TILE_DRAW.tile_index_v]++;
            uniqueLayerCountMapper[TILE_DRAW.layer_index]++;
            uniqueImageMap[std::make_pair(TILE_DRAW.texture_index, TILE_DRAW.texture_rect)]++;
        }

        for (const auto & TILE_DRAW : PACKET.tile_draws_above)
        {
            uniqueMapTileCountMap[TILE_DRAW.tile_index_v]++;
            uniqueLayerCountMapper[TILE_DRAW.layer_index]++;
            uniqueImageMap[std::make_pair(TILE_DRAW.texture_index, TILE_DRAW.texture_rect)]++;
        }

        auto makePercentString
            = [&](const auto NUMBER_A, const auto NUMBER_B, const std::string & DESCRIPTION) {
                  std::string str;
                  str.reserve(32);

                  const auto NUMBER_A_D { static_cast<double>(NUMBER_A) };
                  const auto NUMBER_B_D { static_cast<double>(NUMBER_B) };

                  if (misc::IsRealZero(NUMBER_B_D))
                  {
                      str += "(div_by_zero_error%)";
                  }
                  else
                  {
                      const auto PERCENT { static_cast<int>((NUMBER_A_D / NUMBER_B_D) * 100.0) };
                      str += "(" + std::to_string(PERCENT) + "% " + DESCRIPTION + ")";
                  }

                  return str;
              };

        const auto LAYER_COUNT { uniqueLayerCountMapper.Size() };
        const auto TILES_PER_LAYER_COUNT { (PACKET.tile_count_v.x * PACKET.tile_count_v.y) };

        const auto TOTAL_TILES_IN_MAP { LAYER_COUNT
                                        * static_cast<std::size_t>(TILES_PER_LAYER_COUNT) };

        const auto TOTAL_UNIQUE_IMAGE_COUNT { uniqueImageMap.Size() };
        const auto TOTAL_DRAW_COUNT { (
            PACKET.tile_draws_below.size() + PACKET.tile_draws_above.size()) };

        std::ostringstream ssPre;
        ssPre << "\nAll Layers/Texture info:  map_size=" << PACKET.tile_count_v
              << ", tiles_per_layer=" << TILES_PER_LAYER_COUNT << ", layer_count=" << LAYER_COUNT
              << ", total_tiles=" << TOTAL_TILES_IN_MAP << ", total_draws=" << TOTAL_DRAW_COUNT
              << makePercentString(
                     TOTAL_DRAW_COUNT,
                     TOTAL_TILES_IN_MAP,
                     "of all possible tiles are actually drawn")
              << ", total_unique_draws=" << uniqueMapTileCountMap.Size()
              << makePercentString(
                     uniqueMapTileCountMap.Size(),
                     TOTAL_DRAW_COUNT,
                     "of all draws are to unique tiles (inverse overlap)")
              << ", texture_count=" << PACKET.MapTileTextureCount()
              << ", total_images=" << totalTextureCountTiles
              << ", unique_images_drawn=" << TOTAL_UNIQUE_IMAGE_COUNT
              << makePercentString(
                     TOTAL_UNIQUE_IMAGE_COUNT,
                     totalTextureCountTiles,
                     "of all possible images are actually drawn")
              << ", each unique image drawn an average of "
              << (TOTAL_DRAW_COUNT / TOTAL_UNIQUE_IMAGE_COUNT) << " times.";

        M_HP_LOG_WRN(ssPre.str());
    }

    void MapTileOptimizer::LogSummarySet(
        const Packet & PACKET, const std::string & NAME, const TileDrawVec_t & DRAWS)
    {
        misc::VectorMap<std::size_t, int> uniqueLayerCountMap;
        misc::VectorMap<std::size_t, int> uniqueTextureCountMap;
        misc::VectorMap<sf::Vector2i, int> uniqueTilePosMap;

        struct TextureTrans
        {
            std::size_t index = 0;
            std::size_t draw_count = 0;
            misc::VectorMap<sf::IntRect, int> unique_image_map;
        };

        struct LayerTran
        {
            std::size_t index = 0;
            misc::VectorMap<sf::Vector2i, int> unique_tile_map;
            std::vector<TextureTrans> textures;
        };

        std::vector<LayerTran> layerTrans;

        for (const auto & TILE_DRAW : DRAWS)
        {
            if ((layerTrans.empty()) || (layerTrans.back().index != TILE_DRAW.layer_index))
            {
                layerTrans.emplace_back(LayerTran());
                layerTrans.back().index = TILE_DRAW.layer_index;

                uniqueLayerCountMap[TILE_DRAW.layer_index]++;
            }

            if ((layerTrans.back().textures.empty())
                || (layerTrans.back().textures.back().index != TILE_DRAW.texture_index))
            {
                layerTrans.back().textures.emplace_back(TextureTrans());
                layerTrans.back().textures.back().index = TILE_DRAW.texture_index;

                uniqueTextureCountMap[TILE_DRAW.texture_index]++;
            }

            layerTrans.back().textures.back().unique_image_map[TILE_DRAW.texture_rect]++;
            layerTrans.back().textures.back().draw_count++;
            layerTrans.back().unique_tile_map[TILE_DRAW.tile_index_v]++;
            uniqueTilePosMap[TILE_DRAW.tile_index_v]++;
        }

        std::size_t uniqueTextureArea { 0 };
        for (const auto & INDEX_COUNT_PAIR : uniqueTextureCountMap)
        {
            uniqueTextureArea += TilesInTexture(
                PACKET.tile_size_v, PACKET.MapTileTexture(INDEX_COUNT_PAIR.first));
        }

        std::size_t overlappingDrawsCount { 0 };
        for (const auto & TILEPOS_COUNT_PAIR : uniqueTilePosMap)
        {
            if (TILEPOS_COUNT_PAIR.second > 1)
            {
                overlappingDrawsCount += static_cast<std::size_t>(TILEPOS_COUNT_PAIR.second - 1);
            }
        }

        std::size_t totalUniqueImageCount { 0 };
        std::size_t textureTransitionCount { 0 };
        std::size_t prevTextureIndex { PACKET.MapTileTextureCount() };
        for (auto & LAYER_INFO : layerTrans)
        {
            for (const auto & TEXTURE_INFO : LAYER_INFO.textures)
            {
                if (TEXTURE_INFO.index != prevTextureIndex)
                {
                    ++textureTransitionCount;
                    prevTextureIndex = TEXTURE_INFO.index;
                    totalUniqueImageCount += TEXTURE_INFO.unique_image_map.Size();
                }
            }
        }
        --textureTransitionCount;

        const auto LAYER_TRANSITION_COUNT { layerTrans.size() };

        std::size_t textureTransitionsThatCanBeEliminatedWithReOrdering { 0 };
        if (LAYER_TRANSITION_COUNT > 1)
        {
            for (std::size_t i(0); i < (LAYER_TRANSITION_COUNT - 1); ++i)
            {
                const auto & CURR_LAYER_INFO { layerTrans.at(i) };
                const auto & NEXT_LAYER_INFO { layerTrans.at(i + 1) };

                const auto CURR_LAST_TEXTURE_INDEX { CURR_LAYER_INFO.textures.back().index };
                const auto NEXT_FIRST_TEXTURE_INDEX { NEXT_LAYER_INFO.textures.front().index };

                if (CURR_LAST_TEXTURE_INDEX != NEXT_FIRST_TEXTURE_INDEX)
                {
                    std::vector<std::size_t> indexes;

                    for (const auto & CURR_TEXTURE_INFO : CURR_LAYER_INFO.textures)
                    {
                        indexes.emplace_back(CURR_TEXTURE_INFO.index);
                    }

                    for (const auto & NEXT_TEXTURE_INFO : NEXT_LAYER_INFO.textures)
                    {
                        indexes.emplace_back(NEXT_TEXTURE_INFO.index);
                    }

                    std::sort(std::begin(indexes), std::end(indexes));

                    if (std::adjacent_find(std::begin(indexes), std::end(indexes))
                        != std::end(indexes))
                    {
                        ++textureTransitionsThatCanBeEliminatedWithReOrdering;
                    }
                }
            }
        }

        const auto TILES_PER_LAYER_COUNT { static_cast<std::size_t>(
            PACKET.tile_count_v.x * PACKET.tile_count_v.y) };

        const auto TOTAL_TILES_IN_SET_LAYERS { uniqueLayerCountMap.Size() * TILES_PER_LAYER_COUNT };
        const auto TOTAL_TEXTURES_COUNT { PACKET.MapTileTextureCount() };

        std::ostringstream ss;

        auto numberRatioToString = [&](const std::string & NUMBERS_NAME,
                                       const auto NUMBER_A,
                                       const auto NUMBER_B,
                                       const std::string & PERCENT_DESCRIPTION) {
            ss << "\n\t" << std::setw(24) << std::right << NUMBERS_NAME << "=" << std::setw(4)
               << NUMBER_A << "/" << std::setw(4) << NUMBER_B;
            const auto NUMBER_A_D { static_cast<double>(NUMBER_A) };
            const auto NUMBER_B_D { static_cast<double>(NUMBER_B) };
            if (misc::IsRealZero(NUMBER_B_D) == false)
            {
                const auto PERCENT { static_cast<int>((NUMBER_A_D / NUMBER_B_D) * 100.0) };
                ss << "\t(" << PERCENT << "% " << PERCENT_DESCRIPTION << ")";
            }
            return ss.str();
        };

        ss << "\nLayer/Texture Info for \"" << NAME
           << "\", layer_count=" << uniqueLayerCountMap.Size()
           << ", texture_transition_optimizations="
           << textureTransitionsThatCanBeEliminatedWithReOrdering;

        numberRatioToString(
            "tiles_unique2total",
            uniqueTilePosMap.Size(),
            TOTAL_TILES_IN_SET_LAYERS,
            "of the map was drawn");

        numberRatioToString(
            "draws_unique2total",
            overlappingDrawsCount,
            uniqueTilePosMap.Size(),
            "of the draws overlapped");

        numberRatioToString(
            "textures_unique2total",
            uniqueTextureCountMap.Size(),
            TOTAL_TEXTURES_COUNT,
            "of ALL textures were used");

        numberRatioToString(
            "(images)_unique2total",
            totalUniqueImageCount,
            uniqueTextureArea,
            "of all used textures were actually drawn");

        numberRatioToString(
            "textures_trans2unique", textureTransitionCount, uniqueTextureCountMap.Size(), "");

        for (std::size_t layerTransIndex(0); layerTransIndex < layerTrans.size(); ++layerTransIndex)
        {
            const auto & LAYER_INFO { layerTrans.at(layerTransIndex) };
            std::size_t layerDrawCount { 0 };
            std::size_t layerUniqueImageCount { 0 };
            misc::VectorMap<std::size_t, int> layerUniqueTextureMap;
            for (const auto & TEXTURE_INFO : LAYER_INFO.textures)
            {
                layerUniqueTextureMap[TEXTURE_INFO.index]++;
                layerDrawCount += TEXTURE_INFO.draw_count;
                layerUniqueImageCount += TEXTURE_INFO.unique_image_map.Size();
            }

            ss << "\n\t\tLayer #" << LAYER_INFO.index << " " << layerUniqueTextureMap.Size() << "(";

            for (const auto & TEXTUREINDEX_COUNT_PAIR : layerUniqueTextureMap)
            {
                ss << TEXTUREINDEX_COUNT_PAIR.first << ",";
            }

            ss << "), images=" << layerUniqueImageCount << ", draws=" << layerDrawCount;

            if (layerTransIndex > 0)
            {
                const auto & PREV_LAYER_INFO { layerTrans.at(layerTransIndex - 1) };

                std::size_t prevLayerOverlappingDrawsCount { 0 };
                for (const auto & CURR_TILE_COUNT_PAIR : LAYER_INFO.unique_tile_map)
                {
                    const auto & CURR_TILE { CURR_TILE_COUNT_PAIR.first };
                    for (const auto & PREV_TILE_COUNT_PAIR : PREV_LAYER_INFO.unique_tile_map)
                    {
                        const auto & PREV_TILE { PREV_TILE_COUNT_PAIR.first };

                        if (CURR_TILE == PREV_TILE)
                        {
                            ++prevLayerOverlappingDrawsCount;
                        }
                    }
                }

                ss << ", overlapping_draws=" << prevLayerOverlappingDrawsCount;
            }
        }

        M_HP_LOG_WRN(ss.str());
    }

    std::size_t MapTileOptimizer::PixelsInTexture(const sf::Texture & TEXTURE)
    {
        return static_cast<std::size_t>(TEXTURE.getSize().x * TEXTURE.getSize().y);
    }

    std::size_t MapTileOptimizer::TilesInTexture(
        const sf::Vector2i & TILE_SIZE_V, const std::size_t WIDTH, const std::size_t HEIGHT)
    {
        const sf::Vector2<std::size_t> TILE_SIZE_T_V { TILE_SIZE_V };
        const std::size_t TILE_COUNT_HORIZ { (WIDTH / TILE_SIZE_T_V.x) };
        const std::size_t TILE_CUONT_VERT { (HEIGHT / TILE_SIZE_T_V.y) };
        return (TILE_COUNT_HORIZ * TILE_CUONT_VERT);
    }

    std::size_t MapTileOptimizer::TilesInTexture(
        const sf::Vector2i & TILE_SIZE_V, const std::size_t SQUARE_PIXEL_DIMMENSION)
    {
        return TilesInTexture(TILE_SIZE_V, SQUARE_PIXEL_DIMMENSION, SQUARE_PIXEL_DIMMENSION);
    }

    std::size_t MapTileOptimizer::TilesInTexture(
        const sf::Vector2i & TILE_SIZE_V, const sf::Texture & TEXTURE)
    {
        return TilesInTexture(
            TILE_SIZE_V,
            static_cast<std::size_t>(TEXTURE.getSize().x),
            static_cast<std::size_t>(TEXTURE.getSize().y));
    }

} // namespace map
} // namespace heroespath
