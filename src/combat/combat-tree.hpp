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
#ifndef HEROESPATH_GAME_COMBAT_COMBATTREE_HPP_INCLUDED
#define HEROESPATH_GAME_COMBAT_COMBATTREE_HPP_INCLUDED
//
// combat-tree.hpp
//  A class that manages an undirected graph of verticies with ID_t IDs called the combat tree.
//  Boost Graph was considered but rejected because the graphs required will be very small and
//  because Boost Graph would not provide required features.  So this implementation may be slower
//  than Boost Graph, but not in a noticable way, and will have a much simpler interface.
//
#include "creature/race-enum.hpp"
#include "creature/role-enum.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/types.hpp"

#include <list>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature
namespace combat
{

    class CombatNode;
    using CombatNodePtr_t = misc::NotNull<CombatNode *>;
    using CombatNodePtrOpt_t = boost::optional<CombatNodePtr_t>;
    using CombatNodeSPtr_t = std::shared_ptr<CombatNode>;
    using CombatNodePVec_t = std::vector<CombatNodePtr_t>;

    // Note: For player-characters (facing right), 'Forward' is moving to
    //      the right on the battlefield (increasing blocking pos), and
    //      'Backward' is moving to the left (decreasing blocking pos).
    //      Non-player-characters have the inverse concept of forward and
    //      backward.
    // using BlockingPos_t = int;

    struct EdgeType
    {
        enum Enum
        {
            // This is the left/right or horizontal or blocking position
            // connection between nodes or creatures on the battlefield.
            // Every node connects to every other that is directly left
            // or right of it.  Put another way, every creature that is
            // separated by one blocking position is connected.
            Blocking = 0,

            // This is the up/down or vertical connection between nodes or
            // creatures  on the battlefield.  Every creature stacked vertically
            // has this edge type link to every other creature that is stacked
            // vertically.  Put another way, every creature that shares a
            // blocking position is linked shoulder-to-shoulder.
            ShoulderToShoulder,

            Count,
            All = Count
        };

        static const std::string ToString(const Enum);
    };

    // Manages the combat tree and provides functions to ease common operations
    class CombatTree
    {
        // types
    public:
        struct Edge
        {
            Edge(const ID_t & A, const ID_t & B, const EdgeType::Enum TYPE)
                : a(A)
                , b(B)
                , type(TYPE)
            {}

            ID_t a;
            ID_t b;
            EdgeType::Enum type;
        };

        using EdgeVec_t = std::vector<Edge>;

        struct Vertex
        {
            Vertex(const ID_t & ID, const CombatNodeSPtr_t & NODE_SPTR)
                : id(ID)
                , node_sptr(NODE_SPTR)
            {}

            ID_t id;
            CombatNodeSPtr_t node_sptr;
        };

        using VertexVec_t = std::vector<Vertex>;

    public:
        CombatTree(const CombatTree &) = delete;
        CombatTree(CombatTree &&) = delete;
        CombatTree & operator=(const CombatTree &) = delete;
        CombatTree & operator=(CombatTree &&) = delete;

    public:
        CombatTree();

        // reuses IDs that were preveiously removed
        ID_t NextAvailableId() const;

        // throws std::invalid_argument if ID does not exist
        CombatNodePtr_t GetNodePtr(const ID_t & ID) const;

        // returns nullptr if creature does not exist
        CombatNodePtr_t GetNodePtr(const creature::CreaturePtr_t) const;

        // throws std::invalid_argument if CombatNode does not exist
        ID_t GetNodeId(const CombatNodePtr_t) const;

        // throws std::invalid_argument if creature does not exist
        ID_t GetNodeId(const creature::CreaturePtr_t) const;

        // returns nullptr if there is no node at the position given
        const CombatNodePtrOpt_t GetNodePtrOpt(const float POS_X, const float POS_Y) const;

        std::size_t VertexCount() const { return vertexes_.size(); }
        std::size_t VertexCountByBlockingPos(const int BLOCKING_POS) const;

        const std::string VertexesString(const bool WILL_WRAP = true) const;

        // returns the id number of the vertex added
        CombatNodePtr_t AddVertex(const creature::CreaturePtr_t CREATURE_PTR);

        // returns a vector of vertex IDs that were left without edges
        // throws std::invalid_argument if ID is not an existing vertex
        void RemoveVertex(const ID_t & ID, const bool WILL_REMOVE_DANGLING_EDGES = false);

        std::size_t EdgeCount(const EdgeType::Enum TYPE = EdgeType::All) const;

        const EdgeVec_t Edges(const EdgeType::Enum TYPE = EdgeType::All) const;

        const std::string EdgesString(const bool WILL_WRAP = true) const;

        // throws std::invalid_argument if edge already exists, or if the Verticies do not exist
        void AddEdge(const ID_t & ID1, const ID_t & ID2, const EdgeType::Enum E);

        // order independant matching
        // returns a vector of vertex IDs that were left without edges
        // if DRY_RUN == true, then no edges will be removed
        // throws std::invalid_argument if the edge does not exist
        const IDVec_t RemoveEdge(const ID_t & ID1, const ID_t & ID2, const bool IS_DRY_RUN = false);

        // verticies may exist without edges
        bool DoesVertexExist(const ID_t & ID) const;

        // order independant matching
        bool DoesEdgeExist(
            const ID_t & ID1, const ID_t & ID2, const EdgeType::Enum TYPE = EdgeType::All) const;

        // throws std::invalid_argument if the edge does not exist
        EdgeType::Enum GetEdgeType(const ID_t & ID1, const ID_t & ID2) const;

        // throws std::invalid_argument if the edge does not exist
        void SetEdgeType(const ID_t & ID1, const ID_t & ID2, const EdgeType::Enum);

        // returns false if no IDs were added to the output vector
        bool FindAdjacentByEdgeType(
            const ID_t & ID,
            IDVec_t & idVec_OutParam,
            const EdgeType::Enum TYPE = EdgeType::All) const;

        bool FindAdjacent(const ID_t & ID, IDVec_t & idVec_OutParam) const;

        std::size_t
            CountAdjacent(const ID_t & ID, const EdgeType::Enum EDGE_TYPE = EdgeType::All) const;

        bool AreAnyAdjacent(const ID_t & ID, const EdgeType::Enum EDGE_TYPE = EdgeType::All) const
        {
            return (CountAdjacent(ID, EDGE_TYPE) > 0);
        }

        bool IsVertexLeaf(const ID_t & ID) const { return (CountAdjacent(ID) == 1); }

        const std::string ToString() const;

        // only breaks pre-existing edges of type CONNECTION_TYPE
        void ConnectAllAtPosition(const int POS, const EdgeType::Enum CONNECTION_TYPE);

        int GetBlockingPosMin() const;
        int GetBlockingPosMax() const;
        int GetBlockingDistanceMax() const;

        const IDVec_t GetNodeIDsAtBlockingPos(const int BLOCKING_POS) const;

        const IDVec_t GetNodeIDsAllAroundBlockingPos(const int BLOCKING_POS) const;

        const CombatNodePVec_t GetNodesAtBlockingPos(const int BLOCKING_POS) const;

        const CombatNodePVec_t GetNodesAllAroundBlockingPos(const int BLOCKING_POS) const;

        const CombatNodePVec_t GetCombatNodes() const;

        const CombatNodePVec_t GetCombatNodesOfPlayerType(const bool FIND_PLAYERS) const;

        int GetBlockingDistanceBetween(
            const creature::CreaturePtr_t, const creature::CreaturePtr_t) const;

        int GetClosestBlockingDistanceByType(
            const creature::CreaturePtr_t, const bool WILL_FIND_PLAYERS) const;

        const CombatNodePVec_t FindNodesClosestOfType(
            const int ORIGIN_BLOCKING_POS, const bool WILL_FIND_PLAYERS) const;

        void ResetAllEdges();

        void ChangeBlockingPositionAndUpdateTree(
            const ID_t ID_OF_VERTEX_TO_CHANGE, const int NEW_BLOCKING_POS);

        const IDVec_t GetAllIds() const;

    private:
        EdgeVec_t edges_;
        VertexVec_t vertexes_;
    };

    inline bool operator==(const CombatTree::Edge & L, const CombatTree::Edge & R)
    {
        return std::tie(L.a, L.b, L.type) == std::tie(R.a, R.b, R.type);
    }

    inline bool operator!=(const CombatTree::Edge & L, const CombatTree::Edge & R)
    {
        return !(L == R);
    }

} // namespace combat
} // namespace heroespath

#endif // HEROESPATH_GAME_COMBAT_COMBATTREE_HPP_INCLUDED
