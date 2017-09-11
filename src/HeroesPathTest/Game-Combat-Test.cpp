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
#define BOOST_TEST_MODULE "HeroesPathTestModule_Game_Combat"

#include <boost/test/unit_test.hpp>

#include "Test-stuff.hpp"

#include "game/startup-shutdown.hpp"
#include "game/state/game-state-factory.hpp"
#include "game/player/fake-party.hpp"
#include "game/combat/encounter.hpp"
#include "game/combat/combat-tree.hpp"
#include "game/creature/creature.hpp"

#include <exception>

using namespace game;
namespace ts = test_stuff;


BOOST_AUTO_TEST_CASE(CombatTree_Construction)
{
    BOOST_CHECK_MESSAGE(game::StartupShutdown::Setup("Heroes' Path Unit Tests", 0, nullptr),
        "StartupShutdown::Setup() failed");

    combat::CombatTree combatTree;

    BOOST_CHECK(combatTree.NextAvailableId() == 0);

    BOOST_CHECK_THROW(combatTree.GetNode(
        static_cast<combat::CombatTree::Id_t>(0)), std::invalid_argument);

    BOOST_CHECK_THROW(combatTree.GetNodeSPtr(
        static_cast<combat::CombatTree::Id_t>(0)), std::invalid_argument);

    BOOST_CHECK(combatTree.GetNode(nullptr) == nullptr);
    BOOST_CHECK(combatTree.GetNodeSPtr(nullptr) == nullptr);

    BOOST_CHECK_THROW(combatTree.SetNode(0, combat::CombatNodeSPtr_t()), std::invalid_argument);

    BOOST_CHECK_THROW(combatTree.GetNodeId(
        combat::CombatNodePtr_t(nullptr)), std::invalid_argument);

    BOOST_CHECK_THROW(combatTree.GetNodeId(
        creature::CreaturePtr_t(nullptr)), std::invalid_argument);

    {
        combat::CombatTree::IdVec_t ids;
        combatTree.GetNodeIds(ids, creature::role::Count);
        BOOST_CHECK(ids.empty());
    }

    {
        combat::CombatTree::IdVec_t ids;
        combatTree.GetNodeIds(ids, creature::race::Count);
        BOOST_CHECK(ids.empty());
    }

    BOOST_CHECK(combatTree.GetNode(0.0f, 0.0f) == nullptr);

    BOOST_CHECK(combatTree.VertexCount() == 0);
    BOOST_CHECK(combatTree.VertexCountByBlockingPos(0) == 0);

    BOOST_CHECK(combatTree.Vertexes().empty());
    BOOST_CHECK(combatTree.VertexesString(false).empty());

    BOOST_CHECK_THROW(combatTree.RemoveVertex(0, false), std::invalid_argument);
    BOOST_CHECK_THROW(combatTree.RemoveVertex(0, true), std::invalid_argument);

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

    BOOST_CHECK_THROW(combatTree.AddEdge(0, 0, combat::EdgeType::Blocking), std::invalid_argument);
    BOOST_CHECK_THROW(combatTree.AddEdge(0, 1, combat::EdgeType::Blocking), std::invalid_argument);

    BOOST_CHECK_THROW(combatTree.RemoveEdge(0, 0), std::invalid_argument);
    BOOST_CHECK_THROW(combatTree.RemoveEdge(0, 1), std::invalid_argument);

    BOOST_CHECK(combatTree.DoesVertexExist(0) == false);

    BOOST_CHECK(combatTree.DoesEdgeExist(0, 1) == false);
    
    BOOST_CHECK_THROW(combatTree.GetEdgeType(0, 1), std::invalid_argument);

    BOOST_CHECK_THROW(combatTree.SetEdgeType(
        0, 1, combat::EdgeType::Blocking), std::invalid_argument);

    {
        combat::CombatTree::IdVec_t ids;
        combatTree.FindAdjacentByEdgeType(0, ids);
        BOOST_CHECK(ids.empty());
    }

    {
        combat::CombatTree::IdVec_t ids;
        combatTree.FindAdjacent(0, ids);
        BOOST_CHECK(ids.empty());
    }

    BOOST_CHECK(combatTree.CountAdjacent(0) == 0);

    BOOST_CHECK(combatTree.AreAnyAdjacent(0) == false);

    BOOST_CHECK(combatTree.GetBlockingPosMin() == 0);
    BOOST_CHECK(combatTree.GetBlockingPosMax() == 0);
    BOOST_CHECK(combatTree.GetBlockingDistanceMax() == 0);

    {
        combat::CombatTree::IdVec_t ids;
        BOOST_CHECK(combatTree.GetNodeIDsAtBlockingPos(ids, 0) == false);
        BOOST_CHECK(ids.empty());
    }

    {
        combat::CombatTree::IdVec_t ids;
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

    game::StartupShutdown::Teardown();
}


BOOST_AUTO_TEST_CASE(CombatTree_DefaultParty)
{
    BOOST_CHECK_MESSAGE(game::StartupShutdown::Setup("Heroes' Path Unit Tests", 0, nullptr),
        "StartupShutdown::Setup() failed");

    state::GameStateFactory::Instance()->NewGame(player::FakeParty::Make());

    combat::Encounter::Instance()->BeginCombatTasks();

    //TODO

    game::StartupShutdown::Teardown();
}
