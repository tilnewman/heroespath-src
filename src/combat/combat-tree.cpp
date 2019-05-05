// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-tree.cpp
//
#include "combat-tree.hpp"

#include "combat/combat-node.hpp"
#include "creature/creature.hpp"
#include "misc/log-macros.hpp"

#include <algorithm>

namespace heroespath
{
namespace combat
{

    const std::string EdgeType::ToString(const Enum ENUM)
    {
        switch (ENUM)
        {
            case EdgeType::Blocking: return "Blocking";
            case EdgeType::ShoulderToShoulder: return "Shoulder-To-Shoulder";
            case EdgeType::All: return "All";
            default:
            {
                M_HP_LOG_ERR(
                    "edge_type_enum=" << EnumUnderlying_t(ENUM) << " was an invalid value.");

                return "";
            }
        };
    }

    CombatTree::CombatTree()
        : edges_()
        , vertexes_()
    {}

    ID_t CombatTree::NextAvailableId() const
    {
        const auto NUM_VERTEXES { ID_t::Make(vertexes_.size()) };
        for (auto id(0_id); id < NUM_VERTEXES; ++id)
        {
            if (false == DoesVertexExist(id))
            {
                return id;
            }
        }

        return NUM_VERTEXES;
    }

    CombatNodePtr_t CombatTree::GetNodePtr(const ID_t & ID) const
    {
        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.id == ID)
            {
                return CombatNodePtr_t(VERTEX.node_sptr.get());
            }
        }

        throw std::invalid_argument(
            "CombatTree::GetNodePtr(id=" + ID.ToString() + ") -but that vertex does not exist.");
    }

    CombatNodePtr_t CombatTree::GetNodePtr(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->Creature() == CREATURE_PTR)
            {
                return CombatNodePtr_t(VERTEX.node_sptr.get());
            }
        }

        throw std::invalid_argument(
            "CombatTree::GetNodePtr(creature=" + CREATURE_PTR->ToString()
            + ") -but a vertex for that creature does not exist.");
    }

    ID_t CombatTree::GetNodeId(const CombatNodePtr_t COMBAT_NODE_PTR) const
    {
        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr.get() == COMBAT_NODE_PTR)
            {
                return VERTEX.id;
            }
        }

        throw std::invalid_argument(
            "CombatTree::GetNodeId(combat_node=" + COMBAT_NODE_PTR->ToString()
            + ") -but that combat_node was not found.");
    }

    ID_t CombatTree::GetNodeId(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->Creature() == CREATURE_PTR)
            {
                return VERTEX.id;
            }
        }

        throw std::invalid_argument(
            "CombatTree::GetNodeId(creature={" + CREATURE_PTR->ToString()
            + "}) -but that creature was not found.");
    }

    const CombatNodePtrOpt_t CombatTree::GetNodePtrOpt(const float POS_X, const float POS_Y) const
    {
        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->GetEntityRegion().contains(POS_X, POS_Y))
            {
                return CombatNodePtrOpt_t(CombatNodePtr_t(VERTEX.node_sptr.get()));
            }
        }

        return boost::none;
    }

    std::size_t CombatTree::VertexCountByBlockingPos(const int BLOCKING_POS) const
    {
        std::size_t count(0);
        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->GetBlockingPos() == BLOCKING_POS)
            {
                ++count;
            }
        }

        return count;
    }

    const std::string CombatTree::VertexesString(const bool WILL_WRAP) const
    {
        std::string result;
        result.reserve(128);

        if (WILL_WRAP)
        {
            result += "[";
        }

        for (const auto & VERTEX : vertexes_)
        {
            result += VERTEX.id.ToString() + ":" + VERTEX.node_sptr->ToString() + ", ";
        }

        if (WILL_WRAP)
        {
            result += "]";
        }

        return result;
    }

    CombatNodePtr_t CombatTree::AddVertex(const creature::CreaturePtr_t CREATURE_PTR)
    {
        const auto COMBAT_NODE_SPTR { std::make_shared<combat::CombatNode>(CREATURE_PTR) };
        vertexes_.emplace_back(Vertex(NextAvailableId(), COMBAT_NODE_SPTR));
        return CombatNodePtr_t(COMBAT_NODE_SPTR.get());
    }

    void CombatTree::RemoveVertex(const ID_t & ID, const bool WILL_REMOVE_DANGLING_EDGES)
    {
        auto iter(vertexes_.begin());
        for (; iter != vertexes_.end(); ++iter)
        {
            if (ID == iter->id)
            {
                break;
            }
        }

        if (iter == vertexes_.end())
        {
            std::ostringstream ss;
            ss << "CombatTree::RemoveVertex(id=" << ID
               << ", will_remove_dangling_edges=" << std::boolalpha << WILL_REMOVE_DANGLING_EDGES
               << ") -but that vertex does not exist.";

            throw std::invalid_argument(ss.str());
        }
        else
        {
            vertexes_.erase(iter);
        }

        CombatTree::EdgeVec_t edgesToBeRemoved;
        edgesToBeRemoved.reserve(edges_.size());

        for (const auto & EDGE : edges_)
        {
            if ((EDGE.a == ID) || (EDGE.b == ID))
            {
                edgesToBeRemoved.emplace_back(EDGE);
            }
        }

        // remove all edges connected to the removed vert,
        // while keeping track of a list (set) of blocking IDs with orphaned verts
        std::vector<int> orphanedBlockingIds;
        orphanedBlockingIds.reserve(edgesToBeRemoved.size() * 4);

        for (const auto & EDGE_TO_BE_REMOVED : edgesToBeRemoved)
        {
            const auto ORPHANED_VERT_ID_VEC { RemoveEdge(
                EDGE_TO_BE_REMOVED.a, EDGE_TO_BE_REMOVED.b, !WILL_REMOVE_DANGLING_EDGES) };

            for (const auto & ORPHANED_VERT_ID : ORPHANED_VERT_ID_VEC)
            {
                if (ORPHANED_VERT_ID != ID)
                {
                    orphanedBlockingIds.emplace_back(
                        GetNodePtr(ORPHANED_VERT_ID)->GetBlockingPos());
                }
            }
        }

        // reconnect orphaned verts
        for (const auto NEXT_ORPHANED_BLOCKING_POS : orphanedBlockingIds)
        {
            ConnectAllAtPosition(NEXT_ORPHANED_BLOCKING_POS, EdgeType::ShoulderToShoulder);
        }
    }

    std::size_t CombatTree::EdgeCount(const EdgeType::Enum TYPE) const
    {
        if (TYPE == EdgeType::All)
        {
            return edges_.size();
        }
        else
        {
            std::size_t count { 0 };
            for (const auto & EDGE : edges_)
            {
                if (EDGE.type == TYPE)
                {
                    ++count;
                }
            }

            return count;
        }
    }

    const CombatTree::EdgeVec_t CombatTree::Edges(const EdgeType::Enum TYPE) const
    {
        CombatTree::EdgeVec_t v;
        v.reserve(edges_.size());

        for (const auto & EDGE : edges_)
        {
            if (TYPE == EdgeType::All)
            {
                v.emplace_back(EDGE);
            }
            else
            {
                if (EDGE.type == TYPE)
                {
                    v.emplace_back(EDGE);
                }
            }
        }

        return v;
    }

    const std::string CombatTree::EdgesString(const bool WILL_WRAP) const
    {
        std::string result;
        result.reserve(255);

        if (WILL_WRAP)
        {
            result += "(";
        }

        for (const auto & EDGE : edges_)
        {
            result += EDGE.a.ToString() + "-" + EDGE.b.ToString() + ":"
                + EdgeType::ToString(EDGE.type) + ", ";
        }

        if (WILL_WRAP)
        {
            result += ")";
        }

        return result;
    }

    void CombatTree::AddEdge(const ID_t & ID1, const ID_t & ID2, const EdgeType::Enum TYPE)
    {
        if (ID1 == ID2)
        {
            std::ostringstream ss;
            ss << "CombatTree::AddEdge(id1=" << ID1 << ", id2=" << ID2
               << ", type=" << EdgeType::ToString(TYPE) << ") -but the two IDs are the same.";

            throw std::invalid_argument(ss.str());
        }

        if (DoesEdgeExist(ID1, ID2))
        {
            std::ostringstream ss;
            ss << "CombatTree::AddEdge(id1=" << ID1 << ", id2=" << ID2
               << ", type=" << EdgeType::ToString(TYPE) << ") -but that edge already exists.";

            throw std::invalid_argument(ss.str());
        }

        if (false == DoesVertexExist(ID1))
        {
            std::ostringstream ss;
            ss << "CombatTree::AddEdge(id1=" << ID1 << ", id2=" << ID2
               << ", type=" << EdgeType::ToString(TYPE) << ") -but that edge ID1==" << ID1
               << " does not exist.";

            throw std::invalid_argument(ss.str());
        }

        if (false == DoesVertexExist(ID2))
        {
            std::ostringstream ss;
            ss << "CombatTree::AddEdge(id1=" << ID1 << ", id2=" << ID2
               << ", type=" << EdgeType::ToString(TYPE) << ") -but that edge ID2==" << ID2
               << " does not exist.";

            throw std::invalid_argument(ss.str());
        }

        edges_.emplace_back(Edge(ID1, ID2, TYPE));
    }

    const IDVec_t CombatTree::RemoveEdge(const ID_t & ID1, const ID_t & ID2, const bool IS_DRY_RUN)
    {
        if (ID1 == ID2)
        {
            std::ostringstream ss;
            ss << "CombatTree::RemoveEdge(id1=" << ID1 << ", id2=" << ID2
               << ", dry_run=" << std::boolalpha << IS_DRY_RUN
               << ") but the two IDs were the same.";

            throw std::invalid_argument(ss.str());
        }

        if (DoesEdgeExist(ID1, ID2) == false)
        {
            std::ostringstream ss;
            ss << "CombatTree::RemoveEdge(id1=" << ID1 << ", id2=" << ID2
               << ", dry_run=" << std::boolalpha << IS_DRY_RUN
               << ") called when that edge does not exist.";

            throw std::invalid_argument(ss.str());
        }

        const EdgeType::Enum TYPE { GetEdgeType(ID1, ID2) };

        edges_.erase(std::remove(edges_.begin(), edges_.end(), Edge(ID1, ID2, TYPE)), edges_.end());

        edges_.erase(std::remove(edges_.begin(), edges_.end(), Edge(ID2, ID1, TYPE)), edges_.end());

        IDVec_t orphanedVertexIdVec;
        orphanedVertexIdVec.reserve(2);

        if (AreAnyAdjacent(ID1) == false)
        {
            orphanedVertexIdVec.emplace_back(ID1);
        }

        if (AreAnyAdjacent(ID2) == false)
        {
            orphanedVertexIdVec.emplace_back(ID2);
        }

        if (IS_DRY_RUN)
        {
            AddEdge(ID1, ID2, TYPE);
        }

        return orphanedVertexIdVec;
    }

    bool CombatTree::DoesVertexExist(const ID_t & ID) const
    {
        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.id == ID)
            {
                return true;
            }
        }

        return false;
    }

    bool CombatTree::DoesEdgeExist(
        const ID_t & ID1, const ID_t & ID2, const EdgeType::Enum TYPE) const
    {
        for (const auto & EDGE : edges_)
        {
            if (((EDGE.a == ID1) && (EDGE.b == ID2)) || ((EDGE.a == ID2) && (EDGE.b == ID1)))
            {
                if (TYPE == EdgeType::All)
                {
                    return true;
                }
                else
                {
                    if (EDGE.type == TYPE)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    EdgeType::Enum CombatTree::GetEdgeType(const ID_t & ID1, const ID_t & ID2) const
    {
        for (const auto & EDGE : edges_)
        {
            if (((EDGE.a == ID1) && (EDGE.b == ID2)) || ((EDGE.a == ID2) && (EDGE.b == ID1)))
            {
                return EDGE.type;
            }
        }

        std::ostringstream ss;
        ss << "CombatTree::GetEdgeType(id1=" << ID1 << ", id2=" << ID2
           << ") -but that edge does not exist.";

        throw std::invalid_argument(ss.str());
    }

    void CombatTree::SetEdgeType(const ID_t & ID1, const ID_t & ID2, const EdgeType::Enum TYPE)
    {
        for (auto & edge : edges_)
        {
            if (((edge.a == ID1) && (edge.b == ID2)) || ((edge.a == ID2) && (edge.b == ID1)))
            {
                edge.type = TYPE;
                return;
            }
        }

        std::ostringstream ss;
        ss << "CombatTree::SetEdgeType(id1=" << ID1 << ", id2=" << ID2
           << ", type=" << EdgeType::ToString(TYPE) << ") -but that edge does not exist.";

        throw std::invalid_argument(ss.str());
    }

    bool CombatTree::FindAdjacentByEdgeType(
        const ID_t & ID, IDVec_t & idVec_OutParam, const EdgeType::Enum TYPE) const
    {
        const auto ORIG_SIZE = ID_t::Make(idVec_OutParam.size());

        for (const auto & EDGE : edges_)
        {
            if (TYPE == EdgeType::All)
            {
                if (EDGE.a == ID)
                {
                    idVec_OutParam.emplace_back(EDGE.b);
                }
                else
                {
                    if (EDGE.b == ID)
                    {
                        idVec_OutParam.emplace_back(EDGE.a);
                    }
                }
            }
            else
            {
                if ((EDGE.a == ID) && (EDGE.type == TYPE))
                {
                    idVec_OutParam.emplace_back(EDGE.b);
                }
                else
                {
                    if ((EDGE.b == ID) && (EDGE.type == TYPE))
                    {
                        idVec_OutParam.emplace_back(EDGE.a);
                    }
                }
            }
        }

        const auto FINAL_SIZE = ID_t::Make(idVec_OutParam.size());
        return (ORIG_SIZE != FINAL_SIZE);
    }

    bool CombatTree::FindAdjacent(const ID_t & ID, IDVec_t & idVec_OutParam) const
    {
        const auto ORIG_SIZE { idVec_OutParam.size() };

        for (const auto & EDGE : edges_)
        {
            if (EDGE.a == ID)
            {
                idVec_OutParam.emplace_back(EDGE.b);
            }
            else
            {
                if (EDGE.b == ID)
                {
                    idVec_OutParam.emplace_back(EDGE.a);
                }
            }
        }

        const auto FINAL_SIZE(idVec_OutParam.size());
        return (ORIG_SIZE != FINAL_SIZE);
    }

    std::size_t CombatTree::CountAdjacent(const ID_t & ID, const EdgeType::Enum EDGE_TYPE) const
    {
        IDVec_t v;
        FindAdjacentByEdgeType(ID, v, EDGE_TYPE);
        return v.size();
    }

    const std::string CombatTree::ToString() const
    {
        return "Vertexes: " + VertexesString() + "  Edges: " + EdgesString();
    }

    void CombatTree::ConnectAllAtPosition(const int POS, const EdgeType::Enum CONNECTION_TYPE)
    {
        // establish a vec of all verts at POS
        IDVec_t vertIDsAtPosVec;
        vertIDsAtPosVec.reserve(vertexes_.size());

        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->GetBlockingPos() == POS)
            {
                vertIDsAtPosVec.emplace_back(VERTEX.id);
            }
        }

        // remove pre-existing edges
        EdgeVec_t edgesToRemoveVec;
        edgesToRemoveVec.reserve(vertIDsAtPosVec.size());

        for (const auto & VERTEX_ID : vertIDsAtPosVec)
        {
            for (const auto & EDGE : edges_)
            {
                if ((EDGE.type == CONNECTION_TYPE)
                    && ((EDGE.a == VERTEX_ID) || (EDGE.b == VERTEX_ID)))
                {
                    edgesToRemoveVec.emplace_back(EDGE);
                }
            }
        }

        for (const auto & EDGE_TO_REMOVE : edgesToRemoveVec)
        {
            for (const auto & EDGE : edges_)
            {
                if (EDGE == EDGE_TO_REMOVE)
                {
                    edges_.erase(
                        remove(edges_.begin(), edges_.end(), EDGE_TO_REMOVE), edges_.end());

                    break;
                }
            }
        }

        // attach all verts at position with an edge of type CONNECTION_TYPE
        const auto NUM_VERTS_AT_POS { vertIDsAtPosVec.size() };
        if (NUM_VERTS_AT_POS > 0)
        {
            for (std::size_t i(0); i < NUM_VERTS_AT_POS - 1; ++i)
            {
                for (std::size_t j(i + 1); j < NUM_VERTS_AT_POS - 1; ++j)
                {
                    edges_.emplace_back(
                        Edge(vertIDsAtPosVec[i], vertIDsAtPosVec[j], CONNECTION_TYPE));
                }
            }
        }
    }

    int CombatTree::GetBlockingPosMin() const
    {
        if (vertexes_.empty())
        {
            return 0;
        }

        int min(vertexes_.begin()->node_sptr->GetBlockingPos());
        for (const auto & VERTEX : vertexes_)
        {
            const int NEXT_BLOCKING_POS { VERTEX.node_sptr->GetBlockingPos() };
            if (min > NEXT_BLOCKING_POS)
            {
                min = NEXT_BLOCKING_POS;
            }
        }

        return min;
    }

    int CombatTree::GetBlockingPosMax() const
    {
        if (vertexes_.empty())
        {
            return 0;
        }

        int max(vertexes_.begin()->node_sptr->GetBlockingPos());
        for (const auto & VERTEX : vertexes_)
        {
            const int NEXT_BLOCKING_POS { VERTEX.node_sptr->GetBlockingPos() };
            if (max < NEXT_BLOCKING_POS)
            {
                max = NEXT_BLOCKING_POS;
            }
        }

        return max;
    }

    int CombatTree::GetBlockingDistanceMax() const
    {
        return misc::Abs(GetBlockingPosMin()) - misc::Abs(GetBlockingPosMax());
    }

    const IDVec_t CombatTree::GetNodeIDsAtBlockingPos(const int BLOCKING_POS) const
    {
        IDVec_t ids;
        ids.reserve(vertexes_.size());

        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->GetBlockingPos() == BLOCKING_POS)
            {
                ids.emplace_back(VERTEX.id);
            }
        }

        return ids;
    }

    const IDVec_t CombatTree::GetNodeIDsAllAroundBlockingPos(const int BLOCKING_POS) const
    {
        IDVec_t finalIds;
        finalIds.reserve(vertexes_.size());

        {
            const auto IDS_AT_POS_VEC { GetNodeIDsAtBlockingPos(BLOCKING_POS) };

            std::copy(
                std::begin(IDS_AT_POS_VEC), std::end(IDS_AT_POS_VEC), back_inserter(finalIds));
        }

        {
            const auto IDS_AFTER_POS_VEC { GetNodeIDsAtBlockingPos(BLOCKING_POS + 1) };

            std::copy(
                std::begin(IDS_AFTER_POS_VEC),
                std::end(IDS_AFTER_POS_VEC),
                back_inserter(finalIds));
        }

        {
            const auto IDS_BEFORE_POS_VEC { GetNodeIDsAtBlockingPos(BLOCKING_POS - 1) };

            std::copy(
                std::begin(IDS_BEFORE_POS_VEC),
                std::end(IDS_BEFORE_POS_VEC),
                back_inserter(finalIds));
        }

        if (finalIds.size() > 1)
        {
            std::sort(std::begin(finalIds), std::end(finalIds));

            finalIds.erase(
                std::unique(std::begin(finalIds), std::end(finalIds)), std::end(finalIds));
        }

        return finalIds;
    }

    const CombatNodePVec_t CombatTree::GetNodesAtBlockingPos(const int BLOCKING_POS) const
    {
        CombatNodePVec_t combatNodesPVec;
        combatNodesPVec.reserve(vertexes_.size());

        for (const auto & ID : GetNodeIDsAtBlockingPos(BLOCKING_POS))
        {
            combatNodesPVec.emplace_back(GetNodePtr(ID));
        }

        if (combatNodesPVec.size() > 1)
        {
            std::sort(std::begin(combatNodesPVec), std::end(combatNodesPVec));

            combatNodesPVec.erase(
                std::unique(std::begin(combatNodesPVec), std::end(combatNodesPVec)),
                std::end(combatNodesPVec));
        }

        return combatNodesPVec;
    }

    const CombatNodePVec_t CombatTree::GetNodesAllAroundBlockingPos(const int BLOCKING_POS) const
    {
        CombatNodePVec_t combatNodesPVec;
        combatNodesPVec.reserve(vertexes_.size());

        for (const auto & ID : GetNodeIDsAllAroundBlockingPos(BLOCKING_POS))
        {
            combatNodesPVec.emplace_back(GetNodePtr(ID));
        }

        if (combatNodesPVec.size() > 1)
        {
            std::sort(std::begin(combatNodesPVec), std::end(combatNodesPVec));

            combatNodesPVec.erase(
                std::unique(std::begin(combatNodesPVec), std::end(combatNodesPVec)),
                std::end(combatNodesPVec));
        }

        return combatNodesPVec;
    }

    const CombatNodePVec_t CombatTree::GetCombatNodes() const
    {
        CombatNodePVec_t combatNodesPVec;
        combatNodesPVec.reserve(vertexes_.size());

        for (const auto & VERTEX : vertexes_)
        {
            combatNodesPVec.emplace_back(VERTEX.node_sptr.get());
        }

        return combatNodesPVec;
    }

    const CombatNodePVec_t CombatTree::GetCombatNodesOfPlayerType(const bool FIND_PLAYERS) const
    {
        CombatNodePVec_t combatNodesPvec;
        combatNodesPvec.reserve(vertexes_.size());

        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->Creature()->IsPlayerCharacter() == FIND_PLAYERS)
            {
                combatNodesPvec.emplace_back(VERTEX.node_sptr.get());
            }
        }

        return combatNodesPvec;
    }

    int CombatTree::GetBlockingDistanceBetween(
        const creature::CreaturePtr_t CREATURE_A_PTR,
        const creature::CreaturePtr_t CREATURE_B_PTR) const
    {
        return GetNodePtr(CREATURE_B_PTR)->GetBlockingPos()
            - GetNodePtr(CREATURE_A_PTR)->GetBlockingPos();
    }

    int CombatTree::GetClosestBlockingDistanceByType(
        const creature::CreaturePtr_t CREATURE_PTR, const bool WILL_FIND_PLAYERS) const
    {
        CombatNodePtrOpt_t closestNodePtrOpt { boost::none };
        auto closestBlockingDistanceABS { GetBlockingDistanceMax() + 1 };

        for (const auto & VERTEX : vertexes_)
        {
            if ((VERTEX.node_sptr->Creature() != CREATURE_PTR)
                && (VERTEX.node_sptr->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS))
            {
                const auto ABS_DISTANCE { misc::Abs(
                    GetBlockingDistanceBetween(CREATURE_PTR, VERTEX.node_sptr->Creature())) };

                if (ABS_DISTANCE < closestBlockingDistanceABS)
                {
                    closestBlockingDistanceABS = ABS_DISTANCE;
                    closestNodePtrOpt = CombatNodePtr_t(VERTEX.node_sptr.get());
                }
            }
        }

        if (closestNodePtrOpt)
        {
            return GetBlockingDistanceBetween(CREATURE_PTR, closestNodePtrOpt.value()->Creature());
        }
        else
        {
            return 0;
        }
    }

    const CombatNodePVec_t CombatTree::FindNodesClosestOfType(
        const int ORIGIN_BLOCKING_POS, const bool WILL_FIND_PLAYERS) const
    {
        auto closestBlockingDistanceABS { GetBlockingDistanceMax() + 1 };

        // find closest distance
        for (const auto & VERTEX : vertexes_)
        {
            const auto NEXT_BLOCKING_DISTANCE_ABS { misc::Abs(
                VERTEX.node_sptr->GetBlockingPos() - ORIGIN_BLOCKING_POS) };

            if ((VERTEX.node_sptr->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS)
                && (NEXT_BLOCKING_DISTANCE_ABS < closestBlockingDistanceABS))
            {
                closestBlockingDistanceABS = NEXT_BLOCKING_DISTANCE_ABS;
            }
        }

        CombatNodePVec_t closestCombatNodesPVec;
        closestCombatNodesPVec.reserve(vertexes_.size());

        // find all nodes with that closest distance
        for (const auto & VERTEX : vertexes_)
        {
            const auto NEXT_BLOCKING_DISTANCE_ABS { misc::Abs(
                VERTEX.node_sptr->GetBlockingPos() - ORIGIN_BLOCKING_POS) };

            if ((VERTEX.node_sptr->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS)
                && (NEXT_BLOCKING_DISTANCE_ABS == closestBlockingDistanceABS))
            {
                closestCombatNodesPVec.emplace_back(VERTEX.node_sptr.get());
            }
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (closestCombatNodesPVec.empty() == false),
            "combat::CombatTree::FindNodesClosestOfType(orig_blocking_pos="
                << ORIGIN_BLOCKING_POS << ", will_find_players=" << std::boolalpha
                << WILL_FIND_PLAYERS
                << ")  unable to find any CombatNodes closest.  This should never happen.");

        return closestCombatNodesPVec;
    }

    void CombatTree::ResetAllEdges()
    {
        edges_.clear();

        for (const auto & VERTEX : vertexes_)
        {
            ConnectAllAtPosition(
                VERTEX.node_sptr->GetBlockingPos(), combat::EdgeType::ShoulderToShoulder);
        }
    }

    void CombatTree::ChangeBlockingPositionAndUpdateTree(
        const ID_t ID_OF_VERTEX_TO_CHANGE, const int NEW_BLOCKING_POS)
    {
        for (const auto & VERTEX : vertexes_)
        {
            if (VERTEX.id == ID_OF_VERTEX_TO_CHANGE)
            {
                const Vertex VERTEX_COPY { VERTEX };
                VERTEX_COPY.node_sptr->SetBlockingPos(NEW_BLOCKING_POS);
                RemoveVertex(ID_OF_VERTEX_TO_CHANGE, true);
                vertexes_.emplace_back(VERTEX_COPY);
                ConnectAllAtPosition(NEW_BLOCKING_POS, EdgeType::ShoulderToShoulder);
                return;
            }
        }

        std::ostringstream ss;
        ss << "combat::CombatTree::ChangeBlockingPositionAndUpdateTree(id="
           << ID_OF_VERTEX_TO_CHANGE << ", new_blocking_id=" << NEW_BLOCKING_POS
           << ") was unable to find a vertex with that id.";

        throw std::runtime_error(ss.str());
    }

    const IDVec_t CombatTree::GetAllIds() const
    {
        IDVec_t ids;
        ids.reserve(vertexes_.size());

        for (const auto & VERTEX : vertexes_)
        {
            ids.emplace_back(VERTEX.id);
        }

        return ids;
    }

} // namespace combat
} // namespace heroespath
