// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------

#define BOOST_TEST_MODULE "HeroesPathTestModule_Game_Combat"

#include <boost/test/unit_test.hpp>

#include "unit-test-test-stuff.hpp"

#include "combat/combat-node.hpp"
#include "combat/combat-tree.hpp"
#include "combat/encounter.hpp"
#include "creature/creature.hpp"
#include "creature/player-party-factory.hpp"
#include "creature/player-party.hpp"
#include "game/game-state-factory.hpp"
#include "game/startup-shutdown.hpp"

#include <stdexcept>

using namespace heroespath;
using namespace heroespath::game;

namespace ts = test_stuff;

BOOST_AUTO_TEST_CASE(CombatTree_Construction)
{
    {
        StartupShutdown startupShutdown("Heroes' Path Combat Tests", 0, nullptr);

        {
            combat::CombatTree combatTree;

            BOOST_CHECK(combatTree.NextAvailableId() == 0_id);

            BOOST_CHECK_THROW(combatTree.GetNodePtr(0_id), std::invalid_argument);

            BOOST_CHECK(combatTree.GetNodePtrOpt(0.0f, 0.0f) == boost::none);

            BOOST_CHECK(combatTree.VertexCount() == 0);
            BOOST_CHECK(combatTree.VertexCountByBlockingPos(0) == 0);

            BOOST_CHECK(combatTree.VertexesString(false).empty());

            BOOST_CHECK_THROW(combatTree.RemoveVertex(0_id, false), std::invalid_argument);
            BOOST_CHECK_THROW(combatTree.RemoveVertex(0_id, true), std::invalid_argument);

            BOOST_CHECK(combatTree.EdgeCount() == 0);
            BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::All) == 0);
            BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::Blocking) == 0);
            BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::ShoulderToShoulder) == 0);
            BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::Count) == 0);

            BOOST_CHECK(combatTree.Edges().empty());
            BOOST_CHECK(combatTree.Edges(combat::EdgeType::All).empty());
            BOOST_CHECK(combatTree.Edges(combat::EdgeType::Blocking).empty());
            BOOST_CHECK(combatTree.Edges(combat::EdgeType::ShoulderToShoulder).empty());
            BOOST_CHECK(combatTree.Edges(combat::EdgeType::Count).empty());

            BOOST_CHECK(combatTree.EdgesString(false).empty());

            BOOST_CHECK_THROW(
                combatTree.AddEdge(0_id, 0_id, combat::EdgeType::Blocking), std::invalid_argument);

            BOOST_CHECK_THROW(
                combatTree.AddEdge(0_id, 1_id, combat::EdgeType::Blocking), std::invalid_argument);

            BOOST_CHECK_THROW(combatTree.RemoveEdge(0_id, 0_id), std::invalid_argument);
            BOOST_CHECK_THROW(combatTree.RemoveEdge(0_id, 1_id), std::invalid_argument);

            BOOST_CHECK(combatTree.DoesVertexExist(0_id) == false);

            BOOST_CHECK(combatTree.DoesEdgeExist(0_id, 1_id) == false);

            BOOST_CHECK_THROW(combatTree.GetEdgeType(0_id, 1_id), std::invalid_argument);

            BOOST_CHECK_THROW(
                combatTree.SetEdgeType(0_id, 1_id, combat::EdgeType::Blocking),
                std::invalid_argument);

            {
                heroespath::combat::IDVec_t ids;
                combatTree.FindAdjacentByEdgeType(0_id, ids);
                BOOST_CHECK(ids.empty());
            }

            {
                heroespath::combat::IDVec_t ids;
                combatTree.FindAdjacent(0_id, ids);
                BOOST_CHECK(ids.empty());
            }

            BOOST_CHECK(combatTree.CountAdjacent(0_id) == 0);
            BOOST_CHECK(combatTree.AreAnyAdjacent(0_id) == false);
            BOOST_CHECK(combatTree.GetBlockingPosMin() == 0);
            BOOST_CHECK(combatTree.GetBlockingPosMax() == 0);
            BOOST_CHECK(combatTree.GetBlockingDistanceMax() == 0);
            BOOST_CHECK(combatTree.GetNodeIDsAtBlockingPos(0).empty());
            BOOST_CHECK(combatTree.GetNodeIDsAllAroundBlockingPos(0).empty());
            BOOST_CHECK(combatTree.GetNodesAtBlockingPos(0).empty());
            BOOST_CHECK(combatTree.GetNodesAllAroundBlockingPos(0).empty());
            BOOST_CHECK(combatTree.GetCombatNodes().empty());
        }
    }
}

BOOST_AUTO_TEST_CASE(CombatTree_DefaultParty)
{
    /*
    std::cout << "begin" << std::endl;
    BOOST_CHECK_MESSAGE(
        StartupShutdown::Setup("Heroes' Path Unit Tests", 0, nullptr),
        "StartupShutdown::Setup() failed");

    std::cout << "1" << std::endl;
    //setup the default party
    auto partyPtr{ player::PartyFactory::MakeFakeForTesting() };
    game::GameStateFactory::Instance()->NewGame(partyPtr);
    combat::Encounter::Instance()->BeginCombatTasks();
    std::cout << "2" << std::endl;
    //make a CombatNode for the creature at position zero
    const auto CREATURE_PTR0{
        static_cast<creature::CreaturePtr_t>(partyPtr->Characters().at(0)) };
    std::cout << "3" << std::endl;
    auto combatNodeSPtr0{ std::make_shared<combat::CombatNode>(CREATURE_PTR0) };
    const int BLOCKING_POS0{ -13 };
    combatNodeSPtr0->SetBlockingPos(BLOCKING_POS0);
    std::cout << "4" << std::endl;
    //add that CombatNodeSPtr_t as a Vertex to the CombatTree
    combat::CombatTree combatTree;
    const auto VERT_ID0{ combatTree.AddVertex(combatNodeSPtr0) };
    std::cout << "5" << std::endl;
    //tests after the single vertex insert
    BOOST_CHECK(combatTree.NextAvailableId() == 1_id);
    BOOST_CHECK(combatTree.GetNodePtr(VERT_ID0) == combatNodeSPtr0.get());
    BOOST_CHECK(combatTree.GetNodeSPtr(VERT_ID0) == combatNodeSPtr0);
    BOOST_CHECK(combatTree.GetNodePtr(CREATURE_PTR0) == combatNodeSPtr0.get());
    BOOST_CHECK(combatTree.GetNodeSPtr(CREATURE_PTR0) == combatNodeSPtr0);
    BOOST_CHECK(combatTree.GetNodeId(combatNodeSPtr0.get()) == VERT_ID0);
    BOOST_CHECK(combatTree.GetNodeId(CREATURE_PTR0) == VERT_ID0);
    BOOST_CHECK(combatTree.VertexCount() == 1);
    BOOST_CHECK(combatTree.VertexCountByBlockingPos(0) == 0);
    BOOST_CHECK(combatTree.VertexCountByBlockingPos(BLOCKING_POS0) == 1);
    BOOST_CHECK(combatTree.Vertexes().size() == 1);
    BOOST_CHECK(combatTree.VertexesString(false).empty() == false);
    BOOST_CHECK(combatTree.EdgeCount() == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::All) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::Blocking) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::ShoulderToShoulder) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::Count) == 0);
    BOOST_CHECK(combatTree.Edges().empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::All).empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::Blocking).empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::ShoulderToShoulder).empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::Count).empty());
    BOOST_CHECK(combatTree.EdgesString(false).empty());
    BOOST_CHECK(combatTree.DoesVertexExist(VERT_ID0) == true);
    BOOST_CHECK(combatTree.DoesVertexExist(VERT_ID0 + 1_id) == false);
    BOOST_CHECK(combatTree.CountAdjacent(VERT_ID0) == 0);
    BOOST_CHECK(combatTree.AreAnyAdjacent(VERT_ID0) == false);
    BOOST_CHECK(combatTree.GetBlockingPosMin() == BLOCKING_POS0);
    BOOST_CHECK(combatTree.GetBlockingPosMax() == BLOCKING_POS0);
    const IDVec_t ID_VEC_SINGLE_ZERO = { 0_id };
    {
        IDVec_t ids;
        combatTree.GetNodeIds(ids, CREATURE_PTR0->Role());
        BOOST_CHECK(ids == ID_VEC_SINGLE_ZERO);
    }
    {
        IDVec_t ids;
        combatTree.GetNodeIds(ids, CREATURE_PTR0->Race());
        BOOST_CHECK(ids == ID_VEC_SINGLE_ZERO);
    }
    {
        IDVec_t ids;
        combatTree.FindAdjacentByEdgeType(VERT_ID0, ids);
        BOOST_CHECK(ids.empty());
    }
    {
        IDVec_t ids;
        combatTree.FindAdjacent(VERT_ID0, ids);
        BOOST_CHECK(ids.empty());
    }
    {
        IDVec_t ids;
        BOOST_CHECK(combatTree.GetNodeIDsAtBlockingPos(ids, BLOCKING_POS0));
        BOOST_CHECK(ids == ID_VEC_SINGLE_ZERO);
    }
    {
        IDVec_t ids;
        BOOST_CHECK(combatTree.GetNodeIDsAllAroundBlockingPos(ids, BLOCKING_POS0));
        BOOST_CHECK(ids == ID_VEC_SINGLE_ZERO);
    }
    {
        combat::CombatNodePVec_t combatNodePtrs;
        BOOST_CHECK(combatTree.GetNodesAtBlockingPos(combatNodePtrs, BLOCKING_POS0));
        BOOST_CHECK(combatNodePtrs.size() == 1);
        BOOST_CHECK(combatNodePtrs.at(0) == combatNodeSPtr0.get());
    }
    {
        combat::CombatNodePVec_t combatNodePtrs;
        BOOST_CHECK(combatTree.GetNodesAllAroundBlockingPos(combatNodePtrs, BLOCKING_POS0));
        BOOST_CHECK(combatNodePtrs.size() == 1);
        BOOST_CHECK(combatNodePtrs.at(0) == combatNodeSPtr0.get());
    }
    {
        combat::CombatNodePVec_t combatNodePtrs;
        combatTree.GetCombatNodes(combatNodePtrs);
        BOOST_CHECK(combatNodePtrs.size() == 1);
        BOOST_CHECK(combatNodePtrs.at(0) == combatNodeSPtr0.get());
    }

    //make a CombatNode for the creature at position zero
    const auto CREATURE_PTR1{
        static_cast<creature::CreaturePtr_t>(partyPtr->Characters().at(1)) };

    auto combatNodeSPtr1{ std::make_shared<combat::CombatNode>(CREATURE_PTR1) };
    const int BLOCKING_POS1{ BLOCKING_POS0 };
    combatNodeSPtr1->SetBlockingPos(BLOCKING_POS1);

    //add that CombatNodeSPtr_t as a Vertex to the CombatTree
    const auto VERT_ID1{ combatTree.AddVertex(combatNodeSPtr1) };

    //tests after the second vertex insert
    BOOST_CHECK(combatTree.NextAvailableId() == 2_id);
    BOOST_CHECK(combatTree.GetNodePtr(VERT_ID1) == combatNodeSPtr1.get());
    BOOST_CHECK(combatTree.GetNodeSPtr(VERT_ID1) == combatNodeSPtr1);
    BOOST_CHECK(combatTree.GetNodePtr(CREATURE_PTR1) == combatNodeSPtr1.get());
    BOOST_CHECK(combatTree.GetNodeSPtr(CREATURE_PTR1) == combatNodeSPtr1);
    BOOST_CHECK(combatTree.GetNodeId(combatNodeSPtr1.get()) == VERT_ID1);
    BOOST_CHECK(combatTree.GetNodeId(CREATURE_PTR1) == VERT_ID1);
    BOOST_CHECK(combatTree.VertexCount() == 2);
    BOOST_CHECK(combatTree.VertexCountByBlockingPos(0) == 0);
    BOOST_CHECK(combatTree.VertexCountByBlockingPos(BLOCKING_POS1) == 2);
    BOOST_CHECK(combatTree.Vertexes().size() == 2);
    BOOST_CHECK(combatTree.VertexesString(false).empty() == false);
    BOOST_CHECK(combatTree.EdgeCount() == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::All) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::Blocking) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::ShoulderToShoulder) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::Count) == 0);
    BOOST_CHECK(combatTree.Edges().empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::All).empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::Blocking).empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::ShoulderToShoulder).empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::Count).empty());
    BOOST_CHECK(combatTree.EdgesString(false).empty());
    BOOST_CHECK(combatTree.DoesVertexExist(VERT_ID1) == true);
    BOOST_CHECK(combatTree.DoesVertexExist(VERT_ID1 + 1_id) == false);
    BOOST_CHECK(combatTree.CountAdjacent(VERT_ID1) == 0);
    BOOST_CHECK(combatTree.AreAnyAdjacent(VERT_ID1) == false);
    BOOST_CHECK(combatTree.GetBlockingPosMin() == BLOCKING_POS1);
    BOOST_CHECK(combatTree.GetBlockingPosMax() == BLOCKING_POS1);
    {
        IDVec_t ids;
        BOOST_CHECK(combatTree.GetNodeIDsAtBlockingPos(ids, BLOCKING_POS1));
        BOOST_CHECK(ids.size() == 2);
    }
    {
        IDVec_t ids;
        BOOST_CHECK(combatTree.GetNodeIDsAllAroundBlockingPos(ids, BLOCKING_POS1));
        BOOST_CHECK(ids.size() == 2);
    }
    {
        combat::CombatNodePVec_t combatNodePtrs;
        BOOST_CHECK(combatTree.GetNodesAtBlockingPos(combatNodePtrs, BLOCKING_POS1));
        BOOST_CHECK(combatNodePtrs.size() == 2);
    }
    {
        combat::CombatNodePVec_t combatNodePtrs;
        BOOST_CHECK(combatTree.GetNodesAllAroundBlockingPos(combatNodePtrs, BLOCKING_POS1));
        BOOST_CHECK(combatNodePtrs.size() == 2);
    }
    {
        combat::CombatNodePVec_t combatNodePtrs;
        combatTree.GetCombatNodes(combatNodePtrs);
        BOOST_CHECK(combatNodePtrs.size() == 2);
    }

    //add an edge to link the two creatures shoulder-to-shoulder
    combatTree.AddEdge(VERT_ID0, VERT_ID1, combat::EdgeType::ShoulderToShoulder);

    BOOST_CHECK(combatTree.DoesEdgeExist(VERT_ID0, VERT_ID1) == true);
    BOOST_CHECK(combatTree.EdgeCount() == 1);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::All) == 1);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::Blocking) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::ShoulderToShoulder) == 1);
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::Blocking).empty());

    {
        const auto EDGES_ALL{ combatTree.Edges(combat::EdgeType::All) };
        BOOST_CHECK(EDGES_ALL.size() == 1);

        BOOST_CHECK(
            ((EDGES_ALL[0].a == 0_id) && (EDGES_ALL[0].b == 1_id)) ||
            ((EDGES_ALL[0].a == 1_id) && (EDGES_ALL[0].b == 0_id)) );

        BOOST_CHECK(EDGES_ALL[0].type == combat::EdgeType::ShoulderToShoulder);
    }

    {
        const auto EDGES_STS{ combatTree.Edges(combat::EdgeType::ShoulderToShoulder) };
        BOOST_CHECK(EDGES_STS.size() == 1);

        BOOST_CHECK(
            ((EDGES_STS[0].a == 0_id) && (EDGES_STS[0].b == 1_id)) ||
            ((EDGES_STS[0].a == 1_id) && (EDGES_STS[0].b == 0_id)));

        BOOST_CHECK(EDGES_STS[0].type == combat::EdgeType::ShoulderToShoulder);
    }

    combatTree.RemoveVertex(VERT_ID1, true);
    BOOST_CHECK(combatTree.DoesEdgeExist(VERT_ID0, VERT_ID1) == false);
    BOOST_CHECK(combatTree.DoesVertexExist(VERT_ID1) == false);
    BOOST_CHECK(combatTree.DoesVertexExist(VERT_ID0) == true);
    BOOST_CHECK(combatTree.EdgeCount() == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::All) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::ShoulderToShoulder) == 0);

    BOOST_CHECK_THROW(combatTree.RemoveVertex(VERT_ID1), std::invalid_argument);

    combatTree.RemoveVertex(VERT_ID0);

    BOOST_CHECK(combatTree.DoesVertexExist(VERT_ID0) == false);
    BOOST_CHECK_THROW(combatTree.RemoveVertex(VERT_ID0), std::invalid_argument);

    //whole set of checks to be sure the combatTree is back in its initial (empty) state
    BOOST_CHECK(combatTree.NextAvailableId() == 0_id);

    BOOST_CHECK_THROW(combatTree.GetNodePtr(0_id), std::invalid_argument);
    BOOST_CHECK_THROW(combatTree.GetNodeSPtr(0_id), std::invalid_argument);

    BOOST_CHECK(combatTree.GetNodePtr(nullptr) == nullptr);
    BOOST_CHECK(combatTree.GetNodeSPtr(nullptr) == nullptr);

    BOOST_CHECK_THROW(combatTree.SetNode(0_id, combat::CombatNodeSPtr_t()), std::invalid_argument);

    BOOST_CHECK_THROW(combatTree.GetNodeId(
        combat::CombatNodePtr_t(nullptr)), std::invalid_argument);

    BOOST_CHECK_THROW(combatTree.GetNodeId(
        creature::CreaturePtr_t(nullptr)), std::invalid_argument);

    BOOST_CHECK(combatTree.GetNodePtr(0.0f, 0.0f) == nullptr);
    BOOST_CHECK(combatTree.VertexCount() == 0);
    BOOST_CHECK(combatTree.VertexCountByBlockingPos(0) == 0);
    BOOST_CHECK(combatTree.Vertexes().empty());
    BOOST_CHECK(combatTree.VertexesString(false).empty());
    BOOST_CHECK_THROW(combatTree.RemoveVertex(0_id, false), std::invalid_argument);
    BOOST_CHECK_THROW(combatTree.RemoveVertex(0_id, true), std::invalid_argument);
    BOOST_CHECK(combatTree.EdgeCount() == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::All) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::Blocking) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::ShoulderToShoulder) == 0);
    BOOST_CHECK(combatTree.EdgeCount(combat::EdgeType::Count) == 0);
    BOOST_CHECK(combatTree.Edges().empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::All).empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::Blocking).empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::ShoulderToShoulder).empty());
    BOOST_CHECK(combatTree.Edges(combat::EdgeType::Count).empty());
    BOOST_CHECK(combatTree.EdgesString(false).empty());
    BOOST_CHECK_THROW(combatTree.AddEdge(0_id, 0_id, combat::EdgeType::Blocking),
    std::invalid_argument); BOOST_CHECK_THROW(combatTree.AddEdge(0_id, 1_id,
    combat::EdgeType::Blocking), std::invalid_argument);
    BOOST_CHECK_THROW(combatTree.RemoveEdge(0_id, 0_id), std::invalid_argument);
    BOOST_CHECK_THROW(combatTree.RemoveEdge(0_id, 1_id), std::invalid_argument);
    BOOST_CHECK(combatTree.DoesVertexExist(0_id) == false);
    BOOST_CHECK(combatTree.DoesEdgeExist(0_id, 1_id) == false);
    BOOST_CHECK_THROW(combatTree.GetEdgeType(0_id, 1_id), std::invalid_argument);

    BOOST_CHECK_THROW(combatTree.SetEdgeType(
        0_id, 1_id, combat::EdgeType::Blocking), std::invalid_argument);

    BOOST_CHECK(combatTree.CountAdjacent(0_id) == 0);
    BOOST_CHECK(combatTree.AreAnyAdjacent(0_id) == false);
    BOOST_CHECK(combatTree.GetBlockingPosMin() == 0);
    BOOST_CHECK(combatTree.GetBlockingPosMax() == 0);
    BOOST_CHECK(combatTree.GetBlockingDistanceMax() == 0);

    {
        IDVec_t ids;
        combatTree.GetNodeIds(ids, creature::role::Count);
        BOOST_CHECK(ids.empty());
    }
    {
        IDVec_t ids;
        combatTree.GetNodeIds(ids, creature::race::Count);
        BOOST_CHECK(ids.empty());
    }
    {
        IDVec_t ids;
        combatTree.FindAdjacentByEdgeType(0_id, ids);
        BOOST_CHECK(ids.empty());
    }
    {
        IDVec_t ids;
        combatTree.FindAdjacent(0_id, ids);
        BOOST_CHECK(ids.empty());
    }
    {
        IDVec_t ids;
        BOOST_CHECK(combatTree.GetNodeIDsAtBlockingPos(ids, 0) == false);
        BOOST_CHECK(ids.empty());
    }
    {
        IDVec_t ids;
        BOOST_CHECK(combatTree.GetNodeIDsAllAroundBlockingPos(ids, 0) == false);
        BOOST_CHECK(ids.empty());
    }
    {
        combat::CombatNodePVec_t combatNodePtrs;
        BOOST_CHECK(combatTree.GetNodesAtBlockingPos(combatNodePtrs, 0) == false);
        BOOST_CHECK(combatNodePtrs.empty());
    }
    {
        combat::CombatNodePVec_t combatNodePtrs;
        BOOST_CHECK(combatTree.GetNodesAllAroundBlockingPos(combatNodePtrs, 0) == false);
        BOOST_CHECK(combatNodePtrs.empty());
    }
    {
        combat::CombatNodePVec_t combatNodePtrs;
        combatTree.GetCombatNodes(combatNodePtrs);
        BOOST_CHECK(combatNodePtrs.empty());
    }

    StartupShutdown::Teardown();

    std::cout << "end" << std::endl;
    */
}
