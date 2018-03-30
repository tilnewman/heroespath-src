// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// combat-tree.cpp
//
#include "combat-tree.hpp"

#include "combat/combat-node.hpp"
#include "creature/creature.hpp"
#include "log/log-macros.hpp"

#include <algorithm>
#include <exception>
#include <set>
#include <sstream>

namespace heroespath
{
namespace combat
{

    const std::string EdgeType::ToString(const Enum E)
    {
        switch (E)
        {
            case EdgeType::Blocking:
            {
                return "Blocking";
            }
            case EdgeType::ShoulderToShoulder:
            {
                return "Shoulder-To-Shoulder";
            }
            case EdgeType::All:
            default:
            {
                std::ostringstream ss;
                ss << "CombatTree::EdgeType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        };
    }

    CombatTree::CombatTree()
        : edges_()
        , vertexes_()
    {}

    ID_t CombatTree::NextAvailableId() const
    {
        auto const NUM_VERTEXES{ vertexes_.size() };
        for (std::size_t i(0); i < NUM_VERTEXES; ++i)
        {
            if (false == DoesVertexExist(ID_t(i)))
            {
                return ID_t(i);
            }
        }

        return ID_t(NUM_VERTEXES);
    }

    CombatNodePtr_t CombatTree::GetNode(const ID_t & ID) const
    {
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.id == ID)
            {
                return VERTEX.node_sptr.get();
            }
        }

        std::ostringstream ss;
        ss << "CombatTree::GetNode(id=" << ID << ") -but that vertex does not exist.";
        throw std::invalid_argument(ss.str());
    }

    CombatNodeSPtr_t CombatTree::GetNodeSPtr(const ID_t & ID) const
    {
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.id == ID)
            {
                return VERTEX.node_sptr;
            }
        }

        std::ostringstream ss;
        ss << "CombatTree::GetNodeSPtr(id=" << ID << ") -but that vertex does not exist.";
        throw std::invalid_argument(ss.str());
    }

    CombatNodePtr_t CombatTree::GetNode(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->Creature() == CREATURE_PTR)
            {
                return VERTEX.node_sptr.get();
            }
        }

        return nullptr;
    }

    CombatNodeSPtr_t CombatTree::GetNodeSPtr(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->Creature() == CREATURE_PTR)
            {
                return VERTEX.node_sptr;
            }
        }

        return nullptr;
    }

    void CombatTree::SetNode(const ID_t & ID, const CombatNodeSPtr_t & NODE_SPTR)
    {
        for (auto & vertex : vertexes_)
        {
            if (vertex.id == ID)
            {
                vertex.node_sptr = NODE_SPTR;
                return;
            }
        }

        std::ostringstream ss;
        ss << "CombatTree::SetNode(id=" << ID
           << ", node=" << ((NODE_SPTR.get() == nullptr) ? "nullptr" : NODE_SPTR->ToString())
           << ") -but that vertex does not exist.";

        throw std::invalid_argument(ss.str());
    }

    ID_t CombatTree::GetNodeId(const CombatNodePtr_t COMBATNODE_PTR) const
    {
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr.get() == COMBATNODE_PTR)
            {
                return VERTEX.id;
            }
        }

        std::ostringstream ss;
        ss << "CombatTree::GetNodeId(COMBATNODE_PTR="
           << ((COMBATNODE_PTR == nullptr) ? "nullptr" : COMBATNODE_PTR->ToString())
           << ") -but that combat node ptr was not found.";

        throw std::invalid_argument(ss.str());
    }

    ID_t CombatTree::GetNodeId(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->Creature() == CREATURE_PTR)
            {
                return VERTEX.id;
            }
        }

        std::ostringstream ss;
        ss << "CombatTree::GetNodeId(creature={" << CREATURE_PTR->ToString()
           << "}) -but that creature was not found.";

        throw std::invalid_argument(ss.str());
    }

    std::size_t CombatTree::GetNodeIds(IDVec_t & IDVec_OutParam, const creature::role::Enum ROLE)
    {
        std::size_t count(0);
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->Creature()->Role() == ROLE)
            {
                ++count;
                IDVec_OutParam.emplace_back(VERTEX.id);
            }
        }

        return count;
    }

    std::size_t CombatTree::GetNodeIds(IDVec_t & IdVec_OutParam, const creature::race::Enum RACE)
    {
        std::size_t count(0);
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->Creature()->Race() == RACE)
            {
                ++count;
                IdVec_OutParam.emplace_back(VERTEX.id);
            }
        }

        return count;
    }

    CombatNodePtr_t CombatTree::GetNode(const float POS_X, const float POS_Y) const
    {
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->GetEntityRegion().contains(POS_X, POS_Y))
            {
                return VERTEX.node_sptr.get();
            }
        }

        return nullptr;
    }

    std::size_t CombatTree::VertexCountByBlockingPos(const int BLOCKING_POS) const
    {
        std::size_t count(0);
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->GetBlockingPos() == BLOCKING_POS)
            {
                ++count;
            }
        }

        return count;
    }

    const CombatTree::VertexVec_t CombatTree::Vertexes() const
    {
        CombatTree::VertexVec_t vVec;
        std::copy(vertexes_.begin(), vertexes_.end(), back_inserter(vVec));
        return vVec;
    }

    const std::string CombatTree::VertexesString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        if (WILL_WRAP)
        {
            ss << "[";
        }

        for (auto const & VERTEX : vertexes_)
        {
            ss << VERTEX.id << ":" << VERTEX.node_sptr->ToString() << ", ";
        }

        if (WILL_WRAP)
        {
            ss << "]";
        }

        return ss.str();
    }

    ID_t CombatTree::AddVertex(const CombatNodeSPtr_t & NODE_SPTR)
    {
        const ID_t ID_tO_USE(NextAvailableId());
        vertexes_.emplace_back(Vertex(ID_tO_USE, NODE_SPTR));
        return ID_tO_USE;
    }

    void CombatTree::AddVertex(const ID_t & ID, const CombatNodeSPtr_t & NODE_SPTR)
    {
        if (DoesVertexExist(ID))
        {
            std::ostringstream ss;
            ss << "CombatTree::AddVertex(id=" << ID
               << ", node=" << ((NODE_SPTR.get() == nullptr) ? "nullptr" : NODE_SPTR->ToString())
               << ") -but that ID already exists.";

            throw std::invalid_argument(ss.str());
        }
        else
        {
            vertexes_.emplace_back(Vertex(ID, NODE_SPTR));
        }
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
        for (auto const & EDGE : edges_)
        {
            if ((EDGE.a == ID) || (EDGE.b == ID))
            {
                edgesToBeRemoved.emplace_back(EDGE);
            }
        }

        // remove all edges connected to the removed vert,
        // while keeping track of a list (set) of blocking IDs with orphaned verts
        std::set<int> orphanedBlockingIdSet;
        for (auto const & EDGE_TO_BE_REMOVED : edgesToBeRemoved)
        {
            auto const ORPHANED_VERT_ID_VEC{ RemoveEdge(
                EDGE_TO_BE_REMOVED.a, EDGE_TO_BE_REMOVED.b, !WILL_REMOVE_DANGLING_EDGES) };

            for (auto const & ORPHANED_VERT_ID : ORPHANED_VERT_ID_VEC)
            {
                if (ORPHANED_VERT_ID != ID)
                {
                    orphanedBlockingIdSet.insert(GetNode(ORPHANED_VERT_ID)->GetBlockingPos());
                }
            }
        }

        // reconnect orphaned verts
        for (auto const NEXT_ORPHANED_BLOCKING_POS : orphanedBlockingIdSet)
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
            ID_t::type count{ 0 };
            for (auto const & EDGE : edges_)
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
        for (auto const & EDGE : edges_)
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
        std::ostringstream ss;

        if (WILL_WRAP)
        {
            ss << "(";
        }

        for (auto const & EDGE : edges_)
        {
            ss << EDGE.a << "-" << EDGE.b << ":" << EdgeType::ToString(EDGE.type) << ", ";
        }

        if (WILL_WRAP)
        {
            ss << ")";
        }

        return ss.str();
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

        const EdgeType::Enum TYPE{ GetEdgeType(ID1, ID2) };

        edges_.erase(std::remove(edges_.begin(), edges_.end(), Edge(ID1, ID2, TYPE)), edges_.end());

        edges_.erase(std::remove(edges_.begin(), edges_.end(), Edge(ID2, ID1, TYPE)), edges_.end());

        IDVec_t orphanedVertexIdVec;

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
        for (auto const & VERTEX : vertexes_)
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
        for (auto const & EDGE : edges_)
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
        for (auto const & EDGE : edges_)
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
        const ID_t ORIG_SIZE{ idVec_OutParam.size() };

        for (auto const & EDGE : edges_)
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

        const ID_t FINAL_SIZE{ idVec_OutParam.size() };
        return (ORIG_SIZE != FINAL_SIZE);
    }

    bool CombatTree::FindAdjacent(const ID_t & ID, IDVec_t & idVec_OutParam) const
    {
        auto const ORIG_SIZE{ idVec_OutParam.size() };

        for (auto const & EDGE : edges_)
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

        auto const FINAL_SIZE(idVec_OutParam.size());
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
        std::ostringstream ss;
        ss << "Vertexes: " << VertexesString() << "  Edges: " << EdgesString();
        return ss.str();
    }

    void CombatTree::ConnectAllAtPosition(const int POS, const EdgeType::Enum CONNECTION_TYPE)
    {
        // establish a vec of all verts at POS
        IDVec_t vertIDsAtPosVec;
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->GetBlockingPos() == POS)
            {
                vertIDsAtPosVec.emplace_back(VERTEX.id);
            }
        }

        // remove pre-existing edges
        EdgeVec_t edgesToRemoveVec;
        for (auto const & VERTEX_ID : vertIDsAtPosVec)
        {
            for (auto const & EDGE : edges_)
            {
                if ((EDGE.type == CONNECTION_TYPE)
                    && ((EDGE.a == VERTEX_ID) || (EDGE.b == VERTEX_ID)))
                {
                    edgesToRemoveVec.emplace_back(EDGE);
                }
            }
        }

        for (auto const & EDGE_TO_REMOVE : edgesToRemoveVec)
        {
            for (auto const & EDGE : edges_)
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
        auto const NUM_VERTS_AT_POS{ vertIDsAtPosVec.size() };
        if (NUM_VERTS_AT_POS > 0)
        {
            for (ID_t::type i(0); i < NUM_VERTS_AT_POS - 1; ++i)
            {
                for (ID_t::type j(i + 1); j < NUM_VERTS_AT_POS - 1; ++j)
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
        for (auto const & VERTEX : vertexes_)
        {
            const int NEXT_BLOCKING_POS{ VERTEX.node_sptr->GetBlockingPos() };
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
        for (auto const & VERTEX : vertexes_)
        {
            const int NEXT_BLOCKING_POS{ VERTEX.node_sptr->GetBlockingPos() };
            if (max < NEXT_BLOCKING_POS)
            {
                max = NEXT_BLOCKING_POS;
            }
        }

        return max;
    }

    int CombatTree::GetBlockingDistanceMax() const
    {
        return std::abs(GetBlockingPosMin()) - std::abs(GetBlockingPosMax());
    }

    std::size_t
        CombatTree::GetNodeIDsAtBlockingPos(IDVec_t & IdVec_OutParam, const int BLOCKING_POS) const
    {
        std::size_t count(0);
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->GetBlockingPos() == BLOCKING_POS)
            {
                ++count;
                IdVec_OutParam.emplace_back(VERTEX.id);
            }
        }

        return count;
    }

    std::size_t CombatTree::GetNodeIDsAllAroundBlockingPos(
        IDVec_t & IdVec_OutParam, const int BLOCKING_POS) const
    {
        auto const ORIG_COUNT{ IdVec_OutParam.size() };

        IDVec_t tempIdVec;
        GetNodeIDsAtBlockingPos(tempIdVec, BLOCKING_POS);
        if (tempIdVec.empty() == false)
        {
            std::copy(tempIdVec.begin(), tempIdVec.end(), back_inserter(IdVec_OutParam));
        }

        tempIdVec.clear();
        GetNodeIDsAtBlockingPos(tempIdVec, BLOCKING_POS + 1);
        if (tempIdVec.empty() == false)
        {
            std::copy(tempIdVec.begin(), tempIdVec.end(), back_inserter(IdVec_OutParam));
        }

        tempIdVec.clear();
        GetNodeIDsAtBlockingPos(tempIdVec, BLOCKING_POS - 1);
        if (tempIdVec.empty() == false)
        {
            std::copy(tempIdVec.begin(), tempIdVec.end(), back_inserter(IdVec_OutParam));
        }

        if (IdVec_OutParam.size() > 1)
        {
            std::sort(IdVec_OutParam.begin(), IdVec_OutParam.end());
            IdVec_OutParam.erase(
                std::unique(IdVec_OutParam.begin(), IdVec_OutParam.end()), IdVec_OutParam.end());
        }

        auto const NEW_COUNT{ IdVec_OutParam.size() };

        if (NEW_COUNT < ORIG_COUNT)
        {
            return 0;
        }
        else
        {
            return NEW_COUNT - ORIG_COUNT;
        }
    }

    std::size_t CombatTree::GetNodesAtBlockingPos(
        CombatNodePVec_t & NodePVec_OutParam, const int BLOCKING_POS) const
    {
        auto const ORIG_COUNT{ NodePVec_OutParam.size() };

        IDVec_t idVec;
        GetNodeIDsAtBlockingPos(idVec, BLOCKING_POS);

        for (auto const & NEXT_ID : idVec)
        {
            NodePVec_OutParam.emplace_back(GetNode(NEXT_ID));
        }

        if (NodePVec_OutParam.size() > 1)
        {
            std::sort(NodePVec_OutParam.begin(), NodePVec_OutParam.end());

            NodePVec_OutParam.erase(
                std::unique(NodePVec_OutParam.begin(), NodePVec_OutParam.end()),
                NodePVec_OutParam.end());
        }

        auto const NEW_COUNT{ NodePVec_OutParam.size() };

        if (NEW_COUNT < ORIG_COUNT)
        {
            return 0;
        }
        else
        {
            return NEW_COUNT - ORIG_COUNT;
        }
    }

    std::size_t CombatTree::GetNodesAllAroundBlockingPos(
        CombatNodePVec_t & NodePVec_OutParam, const int BLOCKING_POS) const
    {
        auto const ORIG_COUNT{ NodePVec_OutParam.size() };

        IDVec_t idVec;
        GetNodeIDsAllAroundBlockingPos(idVec, BLOCKING_POS);

        for (auto const & NEXT_ID : idVec)
        {
            NodePVec_OutParam.emplace_back(GetNode(NEXT_ID));
        }

        if (NodePVec_OutParam.size() > 1)
        {
            std::sort(NodePVec_OutParam.begin(), NodePVec_OutParam.end());
            NodePVec_OutParam.erase(
                std::unique(NodePVec_OutParam.begin(), NodePVec_OutParam.end()),
                NodePVec_OutParam.end());
        }

        auto const NEW_COUNT{ NodePVec_OutParam.size() };

        if (NEW_COUNT < ORIG_COUNT)
        {
            return 0;
        }
        else
        {
            return NEW_COUNT - ORIG_COUNT;
        }
    }

    void CombatTree::GetCombatNodes(CombatNodePVec_t & combatNodesPVec) const
    {
        for (auto const & VERTEX : vertexes_)
        {
            combatNodesPVec.emplace_back(VERTEX.node_sptr.get());
        }
    }

    void CombatTree::GetCombatNodesOfPlayerType(
        CombatNodePVec_t & combatNodesPVec, const bool FIND_PLAYERS) const
    {
        for (auto const & VERTEX : vertexes_)
        {
            if (VERTEX.node_sptr->Creature()->IsPlayerCharacter() == FIND_PLAYERS)
            {
                combatNodesPVec.emplace_back(VERTEX.node_sptr.get());
            }
        }
    }

    int CombatTree::GetBlockingDistanceBetween(
        creature::CreaturePtr_t CREATURE_A_PTR, creature::CreaturePtr_t CREATURE_B_PTR) const
    {
        return GetNode(CREATURE_B_PTR)->GetBlockingPos()
            - GetNode(CREATURE_A_PTR)->GetBlockingPos();
    }

    int CombatTree::GetClosestBlockingDistanceByType(
        creature::CreaturePtr_t CREATURE_PTR, const bool WILL_FIND_PLAYERS) const
    {
        CombatNodeSPtr_t closestNodeSPtr{ nullptr };
        auto closestBlockingDistanceABS{ GetBlockingDistanceMax() + 1 };

        for (auto const & VERTEX : vertexes_)
        {
            if ((VERTEX.node_sptr->Creature() != CREATURE_PTR)
                && (VERTEX.node_sptr->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS))
            {
                auto const ABS_DISTANCE{ std::abs(
                    GetBlockingDistanceBetween(CREATURE_PTR, VERTEX.node_sptr->Creature())) };

                if (ABS_DISTANCE < closestBlockingDistanceABS)
                {
                    closestBlockingDistanceABS = ABS_DISTANCE;
                    closestNodeSPtr = VERTEX.node_sptr;
                }
            }
        }

        if (closestNodeSPtr.get() == nullptr)
        {
            return 0;
        }
        else
        {
            return GetBlockingDistanceBetween(CREATURE_PTR, closestNodeSPtr->Creature());
        }
    }

    const CombatNodePVec_t CombatTree::FindNodesClosestOfType(
        const int ORIGIN_BLOCKING_POS, const bool WILL_FIND_PLAYERS) const
    {
        auto closestBlockingDistanceABS{ GetBlockingDistanceMax() + 1 };

        // find closest distance
        for (auto const & VERTEX : vertexes_)
        {
            auto const NEXT_BLOCKING_DISTANCE_ABS{ std::abs(
                VERTEX.node_sptr->GetBlockingPos() - ORIGIN_BLOCKING_POS) };

            if ((VERTEX.node_sptr->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS)
                && (NEXT_BLOCKING_DISTANCE_ABS < closestBlockingDistanceABS))
            {
                closestBlockingDistanceABS = NEXT_BLOCKING_DISTANCE_ABS;
            }
        }

        CombatNodePVec_t closestCombatNodesPVec;

        // find all nodes with that closest distance
        for (auto const & VERTEX : vertexes_)
        {
            auto const NEXT_BLOCKING_DISTANCE_ABS{ std::abs(
                VERTEX.node_sptr->GetBlockingPos() - ORIGIN_BLOCKING_POS) };

            if ((VERTEX.node_sptr->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS)
                && (NEXT_BLOCKING_DISTANCE_ABS == closestBlockingDistanceABS))
            {
                closestCombatNodesPVec.emplace_back(VERTEX.node_sptr.get());
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS(
            (closestCombatNodesPVec.empty() == false),
            "combat::CombatTree::FindNodesClosestOfType(orig_blocking_pos="
                << ORIGIN_BLOCKING_POS << ", will_find_players=" << std::boolalpha
                << WILL_FIND_PLAYERS
                << ")  unbale to find any CombatNodes closest.  This should never happen.");

        return closestCombatNodesPVec;
    }

    void CombatTree::ResetAllEdges()
    {
        edges_.clear();

        for (auto const & VERTEX : vertexes_)
        {
            ConnectAllAtPosition(
                VERTEX.node_sptr->GetBlockingPos(), combat::EdgeType::ShoulderToShoulder);
        }
    }
} // namespace combat
} // namespace heroespath
