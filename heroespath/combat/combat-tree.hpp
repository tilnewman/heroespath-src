#ifndef APPBASE_HEROESPATH_COMBAT_COMBATTREE_INCLUDED
#define APPBASE_HEROESPATH_COMBAT_COMBATTREE_INCLUDED
//
// combat-tree.hpp
//  A class that manages an undirected graph of verticies with Id_t IDs called the combat tree.
//  Boost Graph was considered but rejected because the graphs required will be very small and
//  because Boost Graph would not provide required features.  So this implementation may be slower
//  than Boost Graph, but not in a noticable way, and will have a much simpler interface.
//
#include "heroespath/creature/race-enum.hpp"
#include "heroespath/creature/role-enum.hpp"

#include <list>
#include <tuple>
#include <string>
#include <vector>
#include <memory>


namespace heroespath
{
namespace creature
{
    //forward declarations
    class Creature;
    using CreatureCPtrC_t = const Creature * const;
}

namespace combat
{

    //forward declarations
    class CombatNode;
    using CombatNodePtr_t  = CombatNode *;
    using CombatNodeSPtr_t = std::shared_ptr<CombatNode>;
    using CombatNodePVec_t = std::vector<CombatNodePtr_t>;


    //Note: For player-characters (facing right), 'Forward' is moving to
    //      the right on the battlefield (increasing blocking pos), and
    //      'Backward' is moving to the left (decreasing blocking pos).
    //      Non-player-characters have the inverse concept of forward and
    //      backward.
    //using BlockingPos_t = int;


    struct EdgeType
    {
        enum Enum
        {
            //This is the left/right or horizontal or blocking position
            //connection between nodes or creatures on the battlefield.
            //Every node connects to every other that is directly left
            //or right of it.  Put another way, every creature that is
            //separated by one blocking position is connected.
            Blocking = 0,

            //This is the up/down or vertical connection between nodes or
            //creatures  on the battlefield.  Every creature stacked vertically
            //has this edge type link to every other creature that is stacked
            //vertically.  Put another way, every creature that shares a
            //blocking position is linked shoulder-to-shoulder.
            ShoulderToShoulder,

            Count,
            All = Count
        };

        static const std::string ToString(const Enum);
    };


    //Manages the combat tree and provides functions to ease common operations
    class CombatTree
    {
        //typedefs
    public:
        using Id_t    = std::size_t;
        using IdVec_t = std::vector<Id_t>;
        //
        using Edge_t    = std::tuple<Id_t, Id_t, EdgeType::Enum>;//vertex_id, vertex_id, edge_type
        using EdgeVec_t = std::vector<Edge_t>;
        //
        using Vertex_t    = std::pair<Id_t, CombatNodeSPtr_t>; //vertex_id, node
        using VertexVec_t = std::vector<Vertex_t>;
    private:
        using EdgeList_t      = std::list<Edge_t>;
        using EdgeListIter_t  = EdgeList_t::iterator;
        using EdgeListCIter_t = EdgeList_t::const_iterator;
        //
        using VertexList_t      = std::list<Vertex_t>;
        using VertexListIter_t  = VertexList_t::iterator;
        using VertexListCIter_t = VertexList_t::const_iterator;

        //prevent copy construction
        CombatTree(const CombatTree &);

        //prevent copy assignment
        CombatTree & operator=(const CombatTree &);

    public:
        CombatTree();
        virtual ~CombatTree();

        //reuses IDs that were preveiously removed
        Id_t NextAvailableId() const;

        //these functions throw logic_error if the vertex does not exist
        CombatNodePtr_t GetNode(const Id_t ID) const;
        CombatNodeSPtr_t GetNodeSPtr(const Id_t ID) const;

        CombatNodePtr_t GetNode(creature::CreatureCPtrC_t) const;
        CombatNodeSPtr_t GetNodeSPtr(creature::CreatureCPtrC_t) const;

        void SetNode(const Id_t ID, const CombatNodeSPtr_t NODE_SPTR);

        Id_t GetNodeId(const CombatNodePtr_t) const;
        Id_t GetNodeId(creature::CreatureCPtrC_t) const;

        std::size_t GetNodeIds(IdVec_t & IdVec_OutParam, const creature::role::Enum ROLE);
        std::size_t GetNodeIds(IdVec_t & IdVec_OutParam, const creature::race::Enum RACE);

        //returns nullptr if there is no node at the position given
        CombatNodePtr_t GetNode(const float POS_X, const float POS_Y) const;

        inline std::size_t VertexCount() const { return vertexList_.size(); }
        std::size_t VertexCountByBlockingPos(const int BLOCKING_POS) const;

        const VertexVec_t Vertexes() const;

        const std::string VertexesString(const bool WILL_WRAP = true) const;

        //returns the id number of the vertex added
        Id_t AddVertex(const CombatNodeSPtr_t & NODE_SPTR);

        //throws logic error if the vertex already existed
        void AddVertex(const Id_t ID, const CombatNodeSPtr_t & NODE_SPTR);

        //returns a vector of vertex IDs that were left without edges
        //throws logic_error if ID is not an existing vertex
        const IdVec_t RemoveVertex(const Id_t ID,
                                   const bool WILL_REMOVE_DANGLING_EDGES = false);

        std::size_t EdgeCount(const EdgeType::Enum TYPE = EdgeType::All) const;

        const EdgeVec_t Edges(const EdgeType::Enum TYPE = EdgeType::All) const;

        const std::string EdgesString(const bool WILL_WRAP = true) const;

        //throws logic_error if edge already exists, or if the Verticies do not exist
        void AddEdge(const Id_t           ID1,
                     const Id_t           ID2,
                     const EdgeType::Enum E);

        //order independant matching
        //returns a vector of vertex IDs that were left without edges
        //if DRY_RUN == true, then no edges will be removed
        //throws logic_error if the edge does not exist
        const IdVec_t RemoveEdge(const Id_t   ID1,
                                 const Id_t   ID2,
                                 const bool   IS_DRY_RUN = false);

        //verticies may exist without edges
        bool DoesVertexExist(const Id_t ID) const;

        //order independant matching
        bool DoesEdgeExist(const Id_t           ID1,
                           const Id_t           ID2,
                           const EdgeType::Enum TYPE = EdgeType::All) const;

        //throws logic_error if the edge does not exist
        EdgeType::Enum GetEdgeType(const Id_t ID1, const Id_t ID2) const;

        //throws logic_error if the edge does not exist
        void SetEdgeType(const Id_t ID1, const Id_t ID2, const EdgeType::Enum);

        //returns false if no IDs were added to the output vector
        bool FindAdjacentByEdgeType(const Id_t           ID,
                                    IdVec_t &            idVec_OutParam,
                                    const EdgeType::Enum TYPE = EdgeType::All) const;

        bool FindAdjacent(const Id_t  ID,
                          IdVec_t & idVec_OutParam) const;

        std::size_t CountAdjacent(const Id_t           ID,
                                  const EdgeType::Enum EDGE_TYPE = EdgeType::All) const;

        inline bool AreAnyAdjacent(const Id_t           ID,
                                   const EdgeType::Enum EDGE_TYPE = EdgeType::All) const { return (CountAdjacent(ID, EDGE_TYPE) > 0); }

        inline bool IsVertexLeaf(const Id_t ID) const { return (CountAdjacent(ID) == 1); }

        const std::string ToString() const;

        //WILL_BREAK_PRE_EXISTING only breaks edges of type CONNECTION_TYPE
        void ConnectAllAtPosition(const int            POS,
                                  const EdgeType::Enum CONNECTION_TYPE,
                                  const bool           WILL_BREAK_PRE_EXISTING = true);

        int GetBlockingPosMin() const;
        int GetBlockingPosMax() const;
        int GetBlockingDistanceMax() const;
        inline float GetBlockingPosMid() const          { return (static_cast<float>(GetBlockingPosMax()) + static_cast<float>(GetBlockingPosMin())) / 2.0f; }
        inline float GetBlockingPosMidPercent() const   { return (50.0f + (GetBlockingPosMid() * 10.0f)); }
        inline float GetBlockingPosMidRatio() const     { return (GetBlockingPosMidPercent() / 100.0f); }
        inline float GetBlockingPosMinPercent() const   { return (50.0f + (static_cast<float>(GetBlockingPosMin()) * 10.0f)); }
        inline float GetBlockingPosMinRatio() const     { return (GetBlockingPosMinPercent() / 100.0f); }
        inline float GetBlockingPosMaxPercent() const   { return (50.0f + (static_cast<float>(GetBlockingPosMax()) * 10.0f)); }
        inline float GetBlockingPosMaxRatio() const     { return (GetBlockingPosMaxPercent() / 100.0f); }

        std::size_t GetNodeIDsAtBlockingPos(IdVec_t & IdVec_OutParam, const int BLOCKING_POS) const;
        std::size_t GetNodeIDsAllAroundBlockingPos(IdVec_t & IdVec_OutParam, const int BLOCKING_POS) const;
        std::size_t GetNodesAtBlockingPos(CombatNodePVec_t & NodePVec_OutParam, const int BLOCKING_POS) const;
        std::size_t GetNodesAllAroundBlockingPos(CombatNodePVec_t & NodePVec_OutParam, const int BLOCKING_POS) const;

        void GetCombatNodes(CombatNodePVec_t & combatNodesPVec) const;

        int GetBlockingDistanceBetween(creature::CreatureCPtrC_t, creature::CreatureCPtrC_t) const;

        int GetClosestBlockingDistanceByType(creature::CreatureCPtrC_t, const bool WILL_FIND_PLAYERS) const;

        const CombatNodePVec_t FindNodesClosestOfType(const int ORIGIN_BLOCKING_POS, const bool WILL_FIND_PLAYERS) const;

        void ResetAllEdges();

    private:
        EdgeList_t   edgeList_;
        VertexList_t vertexList_;
    };

}
}
#endif //APPBASE_HEROESPATH_COMBAT_COMBATTREE_INCLUDED
