//
// combat-tree.cpp
//
#include "combat-tree.hpp"

#include "heroespath/combat/combat-node.hpp"
#include "heroespath/creature/creature.hpp"
#include "heroespath/log-macros.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace combat
{

    const std::string EdgeType::ToString(const EdgeType::Enum E)
    {
        switch(E)
        {
            case EdgeType::Blocking:            { return "Blocking"; }
            case EdgeType::Obstruction:         { return "Obstruction"; }
            case EdgeType::ShoulderToShoulder:  { return "ShoulderToShoulder"; }
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
    :
        edgeList_       (),
        vertexList_     ()
    {}


    CombatTree::~CombatTree()
    {}


    CombatTree::Id_t CombatTree::NextAvailableId() const
    {
        const Id_t NUM_VERTEXES(vertexList_.size());
        for(Id_t i(0); i<NUM_VERTEXES; ++i)
            if (false == DoesVertexExist(i))
                return i;

        return NUM_VERTEXES;
    }


    CombatNodeSPtr_t CombatTree::GetNode(const Id_t ID) const
    {
        for(auto const & NEXT_VERTEX : vertexList_)
            if (NEXT_VERTEX.first == ID)
                return NEXT_VERTEX.second;

        std::ostringstream ss;
        ss << "CombatTree::GetNode(id=" << ID << ") -but that vertex does not exist.";
        throw std::logic_error(ss.str());
    }


    CombatNodeSPtr_t CombatTree::GetNode(creature::CreatureCPtrC_t CREATURE_CPTRC) const
    {
        for (auto const & NEXT_VERTEX : vertexList_)
            if (NEXT_VERTEX.second->Creature() == CREATURE_CPTRC)
                return NEXT_VERTEX.second;

        return CombatNodeSPtr_t();
    }


    void CombatTree::SetNode(const Id_t ID, const CombatNodeSPtr_t NODE_SPTR)
    {
        for(auto & nextVertex : vertexList_)
        {
            if (nextVertex.first == ID)
            {
                nextVertex.second = NODE_SPTR;
                return;
            }
        }

        std::ostringstream ss;
        ss << "CombatTree::SetNode(id=" << ID << ", node=" << NODE_SPTR->ToString() << ") -but that vertex does not exist.";
        throw std::logic_error(ss.str());
    }


    CombatTree::Id_t CombatTree::GetNodeId(const CombatNodeSPtr_t & ICOMBATNODE_SPTR) const
    {
        for (auto const & NEXT_VERTEX : vertexList_)
            if (NEXT_VERTEX.second == ICOMBATNODE_SPTR)
                return NEXT_VERTEX.first;

        std::ostringstream ss;
        ss << "CombatTree::GetNodeId(ICOMBATNODE_SPTR=" << ICOMBATNODE_SPTR.get() << ") -but that combat node was not found.";
        throw std::logic_error(ss.str());
    }


    CombatTree::Id_t CombatTree::GetNodeId(creature::CreatureCPtrC_t CREATURE_CPTRC) const
    {
        for (auto const & NEXT_VERTEX : vertexList_)
            if (NEXT_VERTEX.second->Creature() == CREATURE_CPTRC)
                return NEXT_VERTEX.first;

        std::ostringstream ss;
        ss << "CombatTree::GetNodeId(CREATURE_SPTR=" << CREATURE_CPTRC->Name() << ") -but that creature sptr was not found";
        throw std::logic_error(ss.str());
    }


    std::size_t CombatTree::GetNodeIds(CombatTree::IdVec_t & IdVec_OutParam, const creature::role::Enum ROLE)
    {
        std::size_t count(0);
        for (auto const & NEXT_VERTEX : vertexList_)
        {
            if ((NEXT_VERTEX.second->Creature() != nullptr) && (NEXT_VERTEX.second->Creature()->Role().Which() == ROLE))
            {
                ++count;
                IdVec_OutParam.push_back(NEXT_VERTEX.first);
            }
        }

        return count;
    }


    std::size_t CombatTree::GetNodeIds(CombatTree::IdVec_t & IdVec_OutParam, const creature::race::Enum RACE)
    {
        std::size_t count(0);
        for (auto const & NEXT_VERTEX : vertexList_)
        {
            if ((NEXT_VERTEX.second->Creature() != nullptr) && (NEXT_VERTEX.second->Creature()->Race().Which() == RACE))
            {
                ++count;
                IdVec_OutParam.push_back(NEXT_VERTEX.first);
            }
        }

        return count;
    }


    CombatNodeSPtr_t CombatTree::GetNode(const float POS_X, const float POS_Y) const
    {
        for (auto const & NEXT_VERTEX : vertexList_)
            if (NEXT_VERTEX.second->GetEntityRegion().contains(POS_X, POS_Y))
                return NEXT_VERTEX.second;

        return nullptr;
    }


    std::size_t CombatTree::VertexCountByNodeType(const NodeType::Enum TYPE) const
    {
        if (TYPE == NodeType::All)
        {
            return vertexList_.size();
        }
        else
        {
            std::size_t count(0);
            for(auto const & NEXT_VERTEX : vertexList_)
                if (NEXT_VERTEX.second->GetType() == TYPE)
                    ++count;
            return count;
        }
    }


    std::size_t CombatTree::VertexCountByBlockingPos(const int BLOCKING_POS) const
    {
        std::size_t count(0);
        for (auto const & NEXT_VERTEX : vertexList_)
            if (NEXT_VERTEX.second->GetBlockingPos() == BLOCKING_POS)
                ++count;

        return count;
    }


    const CombatTree::VertexVec_t CombatTree::Vertexes(const NodeType::Enum TYPE) const
    {
        CombatTree::VertexVec_t v;
        for(auto const & NEXT_VERTEX : vertexList_)
            if ((TYPE == NodeType::All) || (NEXT_VERTEX.second->GetType() == TYPE))
                v.push_back(NEXT_VERTEX);

        return v;
    }


    const std::string CombatTree::VertexesString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        if (WILL_WRAP) ss << "[";

        for(auto const & NEXT_VERTEX : vertexList_)
            ss << NEXT_VERTEX.first << ":" << NEXT_VERTEX.second->ToString() << ", ";

        if (WILL_WRAP) ss << "]";
        return ss.str();
    }


    CombatTree::Id_t CombatTree::AddVertex(const CombatNodeSPtr_t & NODE_SPTR)
    {
        const Id_t ID_TO_USE( NextAvailableId() );
        vertexList_.push_back( std::make_pair(ID_TO_USE, NODE_SPTR) );
        return ID_TO_USE;
    }


    void CombatTree::AddVertex(const Id_t ID, const CombatNodeSPtr_t & NODE_SPTR)
    {
        if (DoesVertexExist(ID, NODE_SPTR->GetType()))
        {
            std::ostringstream ss;
            ss << "CombatTree::AddVertex(id=" << ID << ", node=" << NODE_SPTR->ToString() << ") -but that ID already exists.";
            throw std::logic_error(ss.str());
        }
        else
        {
            vertexList_.push_back( std::make_pair(ID, NODE_SPTR));
        }
    }


    const CombatTree::IdVec_t CombatTree::RemoveVertex(const Id_t ID, const bool WILL_REMOVE_DANGLING_EDGES)
    {
        VertexListIter_t iter(vertexList_.begin());
        for(; iter != vertexList_.end(); ++iter)
            if (ID == iter->first)
                break;

        if (iter == vertexList_.end())
        {
            std::ostringstream ss;
            ss << "CombatTree::RemoveVertex(id=" << ID << ", will_remove_dangling_edges=" << std::boolalpha << WILL_REMOVE_DANGLING_EDGES << ") -but that vertex does not exist.";
            throw std::range_error(ss.str());
        }
        else
        {
            vertexList_.erase(iter);
        }

        CombatTree::EdgeList_t edgesToBeRemovedList;
        for(auto const & NEXT_EDGE : edgeList_)
            if ((std::get<0>(NEXT_EDGE) == ID) || (std::get<1>(NEXT_EDGE) == ID))
                edgesToBeRemovedList.push_back(NEXT_EDGE);

        CombatTree::IdVec_t orphanedVertexIdVec;
        for(auto const & NEXT_EDGE_TO_BE_REMOVED : edgesToBeRemovedList)
        {
            const IdVec_t NEXT_ORPHANED_VERT_ID_VEC( RemoveEdge(std::get<0>(NEXT_EDGE_TO_BE_REMOVED),
                                                                std::get<1>(NEXT_EDGE_TO_BE_REMOVED),
                                                                ! WILL_REMOVE_DANGLING_EDGES) );

            for(auto const & NEXT_ORPHANED_VERT_ID : NEXT_ORPHANED_VERT_ID_VEC)
                if (NEXT_ORPHANED_VERT_ID != ID) orphanedVertexIdVec.push_back(NEXT_ORPHANED_VERT_ID);
        }

        return orphanedVertexIdVec;
    }


    std::size_t CombatTree::EdgeCount(const EdgeType::Enum TYPE) const
    {
        if (TYPE == EdgeType::All)
        {
            return edgeList_.size();
        }
        else
        {
            Id_t count(0);
            for(auto const & NEXT_EDGE : edgeList_)
                if (std::get<2>(NEXT_EDGE) == TYPE)
                    ++count;

            return count;
        }
    }


    const CombatTree::EdgeVec_t CombatTree::Edges(const EdgeType::Enum TYPE) const
    {
        CombatTree::EdgeVec_t v;
        for(auto const & NEXT_EDGE : edgeList_)
        {
            if (TYPE == EdgeType::All)
            {
                v.push_back(NEXT_EDGE);
            }
            else
            {
                if (TYPE == std::get<2>(NEXT_EDGE))
                    v.push_back(NEXT_EDGE);
            }
        }

        return v;
    }


    const std::string CombatTree::EdgesString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        if (WILL_WRAP) ss << "(";

        for(auto const & NEXT_EDGE : edgeList_)
            ss << std::get<0>(NEXT_EDGE) << "-" << std::get<1>(NEXT_EDGE) << ":" << EdgeType::ToString(std::get<2>(NEXT_EDGE)) << ", ";

        if (WILL_WRAP) ss << ")";

        return ss.str();
    }


    void CombatTree::AddEdge(const Id_t           ID1,
                             const Id_t           ID2,
                             const EdgeType::Enum TYPE)
    {
        if (DoesEdgeExist(ID1, ID2))
        {
            std::ostringstream ss;
            ss << "CombatTree::AddEdge(id1=" << ID1 << ", id2=" << ID2 << ", type=" << EdgeType::ToString(TYPE) << ") -but that edge already exists.";
            throw std::logic_error(ss.str());
        }

        if (false == DoesVertexExist(ID1))
        {
            std::ostringstream ss;
            ss << "CombatTree::AddEdge(id1=" << ID1 << ", id2=" << ID2 << ", type=" << EdgeType::ToString(TYPE) << ") -but that edge ID1==" << ID1 << " does not exist.";
            throw std::logic_error(ss.str());
        }

        if (false == DoesVertexExist(ID2))
        {
            std::ostringstream ss;
            ss << "CombatTree::AddEdge(id1=" << ID1 << ", id2=" << ID2 << ", type=" << EdgeType::ToString(TYPE) << ") -but that edge ID2==" << ID2 << " does not exist.";
            throw std::logic_error(ss.str());
        }

        edgeList_.push_back( std::make_tuple(ID1, ID2, TYPE) );
    }


    const CombatTree::IdVec_t CombatTree::RemoveEdge(const Id_t ID1, const Id_t ID2, const bool IS_DRY_RUN)
    {
        if (false == DoesEdgeExist(ID1, ID2))
        {
            std::ostringstream ss;
            ss << "CombatTree::RemoveEdge(id1=" << ID1 << ", id2=" << ID2 << ", dry_run=" << std::boolalpha << IS_DRY_RUN << ") called when that edge does not exist.";
            throw std::logic_error(ss.str());
        }

        const EdgeType::Enum TYPE(GetEdgeType(ID1, ID2));

        edgeList_.erase(std::remove(edgeList_.begin(), edgeList_.end(), std::make_tuple(ID1, ID2, TYPE)), edgeList_.end());
        edgeList_.erase(std::remove(edgeList_.begin(), edgeList_.end(), std::make_tuple(ID2, ID1, TYPE)), edgeList_.end());

        CombatTree::IdVec_t orphanedVertexIdVec;

        if (false == AreAnyAdjacent(ID1))
            orphanedVertexIdVec.push_back(ID1);

        if (false == AreAnyAdjacent(ID2))
            orphanedVertexIdVec.push_back(ID2);

        if (IS_DRY_RUN)
            AddEdge(ID1, ID2, TYPE);

        return orphanedVertexIdVec;
    }


    bool CombatTree::DoesVertexExist(const Id_t ID, const NodeType::Enum TYPE) const
    {
        for(auto const & NEXT_VERTEX : vertexList_)
            if (((TYPE == NodeType::All) || (NEXT_VERTEX.second->GetType() == TYPE)) && (NEXT_VERTEX.first == ID))
                return true;

        return false;
    }


    bool CombatTree::DoesEdgeExist(const Id_t ID1, const Id_t ID2, const EdgeType::Enum TYPE) const
    {
        for(auto const & NEXT_EDGE : edgeList_)
        {
            if (((std::get<0>(NEXT_EDGE) == ID1) && (std::get<1>(NEXT_EDGE) == ID2)) ||
                ((std::get<0>(NEXT_EDGE) == ID2) && (std::get<1>(NEXT_EDGE) == ID1)))
            {
                if (TYPE == EdgeType::All)
                {
                    return true;
                }
                else
                {
                    if (TYPE == std::get<2>(NEXT_EDGE))
                        return true;
                }
            }
        }

        return false;
    }


    EdgeType::Enum CombatTree::GetEdgeType(const Id_t ID1, const Id_t ID2) const
    {
        for(auto const & NEXT_EDGE : edgeList_)
            if (((std::get<0>(NEXT_EDGE) == ID1) && (std::get<1>(NEXT_EDGE) == ID2)) ||
                ((std::get<0>(NEXT_EDGE) == ID2) && (std::get<1>(NEXT_EDGE) == ID1)))
                return std::get<2>(NEXT_EDGE);

        std::ostringstream ss;
        ss << "CombatTree::GetEdgeType(id1=" << ID1 << ", id2=" << ID2 << ") -but that edge does not exist.";
        throw std::logic_error(ss.str());
    }


    void CombatTree::SetEdgeType(const Id_t ID1, const Id_t ID2, const EdgeType::Enum TYPE)
    {
        for(auto & nextEdge : edgeList_)
        {
            if (((std::get<0>(nextEdge) == ID1) && (std::get<1>(nextEdge) == ID2)) ||
                ((std::get<0>(nextEdge) == ID2) && (std::get<1>(nextEdge) == ID1)))
            {
                std::get<2>(nextEdge) = TYPE;
                break;
            }
        }

        std::ostringstream ss;
        ss << "CombatTree::SetEdgeType(id1=" << ID1 << ", id2=" << ID2 << ", type=" << EdgeType::ToString(TYPE) << ") -but that edge does not exist.";
        throw std::logic_error(ss.str());
    }


    bool CombatTree::FindAdjacentByEdgeType(const Id_t           ID,
                                            IdVec_t &            idVec_OutParam,
                                            const EdgeType::Enum TYPE) const
    {
        const Id_t ORIG_SIZE(idVec_OutParam.size());

        for(auto const & NEXT_EDGE : edgeList_)
        {
            if (TYPE == EdgeType::All)
            {
                if (std::get<0>(NEXT_EDGE) == ID)
                    idVec_OutParam.push_back( std::get<1>(NEXT_EDGE) );
                else
                    if (std::get<1>(NEXT_EDGE) == ID)
                        idVec_OutParam.push_back( std::get<0>(NEXT_EDGE) );
            }
            else
            {
                if ((std::get<0>(NEXT_EDGE) == ID) && (std::get<2>(NEXT_EDGE) == TYPE))
                    idVec_OutParam.push_back( std::get<1>(NEXT_EDGE) );
                else
                    if ((std::get<1>(NEXT_EDGE) == ID) && (std::get<2>(NEXT_EDGE) == TYPE))
                        idVec_OutParam.push_back( std::get<0>(NEXT_EDGE) );
            }
        }

        const Id_t FINAL_SIZE(idVec_OutParam.size());

        return (ORIG_SIZE != FINAL_SIZE);
    }


    bool CombatTree::FindAdjacentByNodeType(const Id_t           ID,
                                            IdVec_t &            idVec_OutParam,
                                            const NodeType::Enum TYPE) const
    {
        auto const ORIG_SIZE(idVec_OutParam.size());

        for(auto const & NEXT_EDGE : edgeList_)
        {
            if (TYPE == NodeType::All)
            {
                if (std::get<0>(NEXT_EDGE) == ID)
                    idVec_OutParam.push_back( std::get<1>(NEXT_EDGE) );
                else
                    if (std::get<1>(NEXT_EDGE) == ID)
                        idVec_OutParam.push_back( std::get<0>(NEXT_EDGE) );
            }
            else
            {
                if ((std::get<0>(NEXT_EDGE) == ID) && (GetNode(std::get<1>(NEXT_EDGE))->GetType() == TYPE))
                    idVec_OutParam.push_back( std::get<1>(NEXT_EDGE) );
                else
                    if ((std::get<1>(NEXT_EDGE) == ID) && (GetNode(std::get<0>(NEXT_EDGE))->GetType() == TYPE))
                        idVec_OutParam.push_back( std::get<0>(NEXT_EDGE) );
            }
        }

        auto const FINAL_SIZE(idVec_OutParam.size());

        return (ORIG_SIZE != FINAL_SIZE);
    }


    std::size_t CombatTree::CountAdjacent(const Id_t           ID,
                                     const EdgeType::Enum EDGE_TYPE,
                                     const NodeType::Enum VERTEX_TYPE) const
    {
        CombatTree::IdVec_t v;
        FindAdjacentByEdgeType(ID, v, EDGE_TYPE);
        FindAdjacentByNodeType(ID, v, VERTEX_TYPE);
        return v.size();
    }


    const std::string CombatTree::ToString() const
    {
        std::ostringstream ss;
        ss << "Vertexes: " << VertexesString() << "  Edges: " << EdgesString();
        return ss.str();
    }


    void CombatTree::ConnectAllAtPosition(const int            POS,
                                          const EdgeType::Enum CONNECTION_TYPE,
                                          const bool           WILL_BREAK_PRE_EXISTING)
    {
        //establish a vec of all verts at POS
        IdVec_t vertIDsAtPosVec;
        for(auto const & NEXT_VERTEX : vertexList_)
            if (NEXT_VERTEX.second->GetBlockingPos() == POS)
                vertIDsAtPosVec.push_back(NEXT_VERTEX.first);

        //remove pre-existing edges if needed
        if (WILL_BREAK_PRE_EXISTING)
        {
            EdgeVec_t edgesToRemoveVec;
            for(auto const & NEXT_VERTEX_ID : vertIDsAtPosVec)
                for(auto const & NEXT_EDGE : edgeList_)
                    if ((std::get<2>(NEXT_EDGE) == CONNECTION_TYPE) && ((std::get<0>(NEXT_EDGE) == NEXT_VERTEX_ID) || (std::get<1>(NEXT_EDGE) == NEXT_VERTEX_ID)))
                        edgesToRemoveVec.push_back(NEXT_EDGE);

            for(auto const & NEXT_EDGE_TO_REMOVE : edgesToRemoveVec)
            {
                for(auto const & NEXT_EDGE : edgeList_)
                {
                    if (NEXT_EDGE == NEXT_EDGE_TO_REMOVE)
                    {
                        edgeList_.erase(remove(edgeList_.begin(), edgeList_.end(), NEXT_EDGE_TO_REMOVE), edgeList_.end());
                        break;
                    }
                }
            }
        }

        //attach all verts at position with an edge of type CONNECTION_TYPE
        const Id_t NUM_VERTS_AT_POS(vertIDsAtPosVec.size());
        if (NUM_VERTS_AT_POS > 0)
            for(Id_t i(0); i<NUM_VERTS_AT_POS - 1; ++i)
                for(Id_t j(i+1); j<NUM_VERTS_AT_POS - 1; ++j)
                    edgeList_.push_back( std::make_tuple(vertIDsAtPosVec[i], vertIDsAtPosVec[j], CONNECTION_TYPE) );
    }


    int CombatTree::GetBlockingPosMin() const
    {
        if (vertexList_.empty())
            return 0;

        int min(vertexList_.begin()->second->GetBlockingPos());
        for (auto const & NEXT_VERTEX : vertexList_)
        {
            const int NEXT_BLOCKING_POS(NEXT_VERTEX.second->GetBlockingPos());
            if (min > NEXT_BLOCKING_POS)
                min = NEXT_BLOCKING_POS;
        }

        return min;
    }


    int CombatTree::GetBlockingPosMax() const
    {
        if (vertexList_.empty())
            return 0;

        int max(vertexList_.begin()->second->GetBlockingPos());
        for (auto const & NEXT_VERTEX : vertexList_)
        {
            const int NEXT_BLOCKING_POS(NEXT_VERTEX.second->GetBlockingPos());
            if (max < NEXT_BLOCKING_POS)
                max = NEXT_BLOCKING_POS;
        }

        return max;
    }


    std::size_t CombatTree::GetNodeIDsAtBlockingPos(IdVec_t & IdVec_OutParam, const int BLOCKING_POS) const
    {
        std::size_t count(0);
        for (auto const & NEXT_VERTEX : vertexList_)
        {
            if (NEXT_VERTEX.second->GetBlockingPos() == BLOCKING_POS)
            {
                ++count;
                IdVec_OutParam.push_back(NEXT_VERTEX.first);
            }
        }

        return count;
    }


    std::size_t CombatTree::GetNodeIDsAllAroundBlockingPos(IdVec_t & IdVec_OutParam, const int BLOCKING_POS) const
    {
        const std::size_t ORIG_COUNT(IdVec_OutParam.size());

        IdVec_t tempIdVec;
        GetNodeIDsAtBlockingPos(tempIdVec, BLOCKING_POS);
        if (tempIdVec.empty() == false)
            std::copy(tempIdVec.begin(), tempIdVec.end(), back_inserter(IdVec_OutParam));

        tempIdVec.clear();
        GetNodeIDsAtBlockingPos(tempIdVec, BLOCKING_POS + 1);
        if (tempIdVec.empty() == false)
            std::copy(tempIdVec.begin(), tempIdVec.end(), back_inserter(IdVec_OutParam));

        tempIdVec.clear();
        GetNodeIDsAtBlockingPos(tempIdVec, BLOCKING_POS - 1);
        if (tempIdVec.empty() == false)
            std::copy(tempIdVec.begin(), tempIdVec.end(), back_inserter(IdVec_OutParam));

        if (IdVec_OutParam.size() > 1)
            std::unique(IdVec_OutParam.begin(), IdVec_OutParam.end());

        auto const NEW_COUNT(IdVec_OutParam.size());

        if (NEW_COUNT < ORIG_COUNT)
            return 0;
        else
            return NEW_COUNT - ORIG_COUNT;
    }


    std::size_t CombatTree::GetNodesAtBlockingPos(CombatNodeSVec_t & NodeSVec_OutParam, const int BLOCKING_POS) const
    {
        auto const ORIG_COUNT(NodeSVec_OutParam.size());

        IdVec_t idVec;
        GetNodeIDsAtBlockingPos(idVec, BLOCKING_POS);

        for (auto const NEXT_ID : idVec)
            NodeSVec_OutParam.push_back(GetNode(NEXT_ID));

        if (NodeSVec_OutParam.size() > 1)
            std::unique(NodeSVec_OutParam.begin(), NodeSVec_OutParam.end());

        auto const NEW_COUNT(NodeSVec_OutParam.size());

        if (NEW_COUNT < ORIG_COUNT)
            return 0;
        else
            return NEW_COUNT - ORIG_COUNT;
    }


    std::size_t CombatTree::GetNodesAllAroundBlockingPos(CombatNodeSVec_t & NodeSVec_OutParam, const int BLOCKING_POS) const
    {
        auto const ORIG_COUNT(NodeSVec_OutParam.size());

        IdVec_t idVec;
        GetNodeIDsAllAroundBlockingPos(idVec, BLOCKING_POS);

        for (auto const NEXT_ID : idVec)
            NodeSVec_OutParam.push_back(GetNode(NEXT_ID));

        if (NodeSVec_OutParam.size() > 1)
            std::unique(NodeSVec_OutParam.begin(), NodeSVec_OutParam.end());

        auto const NEW_COUNT(NodeSVec_OutParam.size());

        if (NEW_COUNT < ORIG_COUNT)
            return 0;
        else
            return NEW_COUNT - ORIG_COUNT;
    }


    void CombatTree::GetCombatNodes(CombatNodeSVec_t & combatNodesSVec) const
    {
        for (auto const & NEXT_VERTEX : vertexList_)
            combatNodesSVec.push_back(NEXT_VERTEX.second);
    }


    int CombatTree::GetBlockingDistanceBetween(creature::CreatureCPtrC_t CREATURE_A_CPTRC, creature::CreatureCPtrC_t CREATURE_B_CPTRC) const
    {
        return GetNode(CREATURE_B_CPTRC)->GetBlockingPos() - GetNode(CREATURE_A_CPTRC)->GetBlockingPos();
    }


    int CombatTree::GetClosestBlockingDistanceByType(creature::CreatureCPtrC_t CREATURE_CPTRC, const bool WILL_FIND_PLAYERS) const
    {
        CombatNodeSPtr_t closestNodeSPtr{ nullptr };
        auto closestBlockingDistanceABS{ (std::abs(GetBlockingPosMin()) + std::abs(GetBlockingPosMax())) * 2 };

        for (auto const & NEXT_VERTEX : vertexList_)
        {
            if ((NEXT_VERTEX.second->Creature() != CREATURE_CPTRC) &&
                (NEXT_VERTEX.second->Creature()->IsPlayerCharacter() == WILL_FIND_PLAYERS))
            {
                auto const ABS_DISTANCE{ std::abs(GetBlockingDistanceBetween(CREATURE_CPTRC, NEXT_VERTEX.second->Creature())) };
                if (ABS_DISTANCE < closestBlockingDistanceABS)
                {
                    closestBlockingDistanceABS = ABS_DISTANCE;
                    closestNodeSPtr = NEXT_VERTEX.second;
                }
            }
        }

        if (closestNodeSPtr.get() == nullptr)
        {
            return 0;
        }
        else
        {
            return GetBlockingDistanceBetween(CREATURE_CPTRC, closestNodeSPtr->Creature());
        }
    }


    void CombatTree::UpdateTimeOfAllCombatNodes(const float ELAPSED_TIME_SECONDS)
    {
        for (auto & nextVertex : vertexList_)
            nextVertex.second->UpdateTime(ELAPSED_TIME_SECONDS);
    }

}
}
