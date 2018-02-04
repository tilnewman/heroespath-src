//This is an open source non-commercial project. Dear PVS-Studio, please check it.
//PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
//Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
//Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from
//the use of this software.
//
//Permission is granted to anyone to use this software for any purpose,
//including commercial applications, and to alter it and redistribute it
//freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software.  If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// parser.cpp
//
#include "parser.hpp"

#include "map/tiles-panel.hpp"
#include "map/shadow-masker.hpp"
#include "log/log-macros.hpp"
#include "game/game-data-file.hpp"
#include "sfml-util/loaders.hpp"
#include "misc/assertlogandthrow.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>


namespace heroespath
{
namespace map
{

    const std::string Parser::XML_NODE_NAME_TILE_LAYER_  { "layer" };
    const std::string Parser::XML_NODE_NAME_OBJECT_LAYER_{ "objectgroup" };
    const std::string Parser::XML_NODE_NAME_TILESET_     { "tileset" };
    //
    const std::string Parser::XML_ATTRIB_NAME_GROUND_    { "ground" };
    const std::string Parser::XML_ATTRIB_NAME_OBJECTS_   { "object" };
    const std::string Parser::XML_ATTRIB_NAME_SHADOWS_   { "shadow" };
    const std::string Parser::XML_ATTRIB_NAME_COLLISION_ { "collision" };


    void Parser::Parse(const std::string & FILE_PATH_STR, Layout & layout)
    {
        try
        {
            Parse_Implementation(FILE_PATH_STR, layout);
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_ERR("map::Parser::Parse(" << FILE_PATH_STR
                << ") boost::property_tree functions threw '" << E.what() << "' exception.");

            throw E;
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << "map::Parser::Parse(" << FILE_PATH_STR
                << ") boost::property_tree functions threw an unknown exception.";

            M_HP_LOG_ERR(ss.str());
            throw std::runtime_error(ss.str());
        }
    }


    void Parser::Parse_Implementation(const std::string & MAP_FILE_PATH_STR, Layout & layout)
    {
        collisionRects_.clear();
        collisionRects_.reserve(4096);//found by experiment to be a good upper bound

        layout.Reset();

        auto const XML_PROPERTY_TREE{ Parse_XML(MAP_FILE_PATH_STR) };

        Parse_MapSizes(XML_PROPERTY_TREE, layout);

        SetupEmptyTexture(layout);

        using BPTreeValue_t = boost::property_tree::ptree::value_type;

        //loop over all layers in the map file and parse them separately
        for (const BPTreeValue_t & PROPTREE_CHILD_PAIR : XML_PROPERTY_TREE.get_child("map"))
        {
            namespace ba = boost::algorithm;
            auto const NODENAME_LOWER{ ba::to_lower_copy(PROPTREE_CHILD_PAIR.first) };

            if (WillParseLayer(NODENAME_LOWER) == false)
            {
                continue;
            }
            else if (ba::contains(NODENAME_LOWER, ba::to_lower_copy(XML_NODE_NAME_OBJECT_LAYER_)))
            {
                Parse_Layer_Collisions(PROPTREE_CHILD_PAIR.second);
                continue;
            }
            else if (ba::contains(NODENAME_LOWER, ba::to_lower_copy(XML_NODE_NAME_TILESET_)))
            {
                Parse_Layer_Tileset(PROPTREE_CHILD_PAIR.second, layout);
                continue;
            }
            else
            {
                Prase_Layer_Generic(PROPTREE_CHILD_PAIR, layout);
            }
        }

        layout.collision_qtree.Setup(
            static_cast<float>(layout.tile_size_x * layout.tile_count_x),
            static_cast<float>(layout.tile_size_y * layout.tile_count_y),
            collisionRects_);

        collisionRects_.clear();

        ShadowMasker shadowMasker;
        shadowMasker.ChangeColors(XML_ATTRIB_NAME_SHADOWS_, layout);
    }


    const boost::property_tree::ptree Parser::Parse_XML(
        const std::string & MAP_FILE_PATH_STR) const
    {
        boost::property_tree::ptree xmlPropertyTree;

        try
        {
            boost::property_tree::read_xml(MAP_FILE_PATH_STR, xmlPropertyTree);
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_ERR("map::Parser::Parse_XML(\"" << MAP_FILE_PATH_STR
                << "\") boost::property_tree::read_xml() threw '" << E.what() << "' exception.");

            throw;
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << "map::Parser::Parse_XML(\"" << MAP_FILE_PATH_STR
                << "\") boost::property_tree::read_xml() threw an unknown exception.";

            M_HP_LOG_ERR(ss.str());
            throw std::runtime_error(ss.str());
        }

        return xmlPropertyTree;
    }


    void Parser::Parse_MapSizes(
        const boost::property_tree::ptree & XML_PROPERTY_TREE,
        Layout & layout)
    {
        auto const XML_PROPERTY_TREE_MAPIMAGE{ XML_PROPERTY_TREE.get_child("map") };

        try
        {
            layout.tile_size_x = XML_PROPERTY_TREE_MAPIMAGE.get<int>("<xmlattr>.tilewidth");
            layout.tile_size_y = XML_PROPERTY_TREE_MAPIMAGE.get<int>("<xmlattr>.tileheight");
            layout.tile_count_x = XML_PROPERTY_TREE_MAPIMAGE.get<int>("<xmlattr>.width");
            layout.tile_count_y = XML_PROPERTY_TREE_MAPIMAGE.get<int>("<xmlattr>.height");
        }
        catch (const std::exception & E)
        {
            std::string name("");
            try
            {
                name = XML_PROPERTY_TREE_MAPIMAGE.get<std::string>("<xmlattr>.name");
            }
            catch (...)
            {
                name = "(no name error)";
            }

            M_HP_LOG_FAT("map::Parser::Parse_MapSizes() threw exception when parsing \""
                << name << "\".  what=\"" << E.what() << "\".");

            throw E;
        }
    }


    void Parser::Parse_Layer_Tileset(
        const boost::property_tree::ptree & TILESET_PROPTREE,
        Layout & layout)
    {
        auto const IMAGE_PROPTREE{ TILESET_PROPTREE.get_child("image") };

        layout.texture_vec.push_back(sf::Texture());
        auto const TEXTURE_INDEX{ layout.texture_vec.size() - 1 };

        layout.tiles_panel_vec.push_back( TilesPanel(
            TILESET_PROPTREE.get<std::string>("<xmlattr>.name"),
            IMAGE_PROPTREE.get<std::string>("<xmlattr>.source"),
            TILESET_PROPTREE.get<int>("<xmlattr>.firstgid"),
            TILESET_PROPTREE.get<int>("<xmlattr>.tilecount"),
            TILESET_PROPTREE.get<int>("<xmlattr>.columns"),
            TEXTURE_INDEX));

        TilesPanel & tilesPanel{ layout.tiles_panel_vec[layout.tiles_panel_vec.size() - 1] };

        namespace bfs = boost::filesystem;
        tilesPanel.path_obj = bfs::system_complete(
            bfs::path(game::GameDataFile::Instance()->GetMediaPath("media-maps-tile-dir")) /
                bfs::path(tilesPanel.path_rel).leaf());

        sfml_util::LoadTexture(layout.texture_vec[TEXTURE_INDEX], tilesPanel.path_obj.string());
    }


    void Parser::Parse_Layer_Collisions(const boost::property_tree::ptree & OBJ_GROUP_PT)
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD : OBJ_GROUP_PT)
        {
            if (ba::contains(ba::to_lower_copy(CHILD.first), ba::to_lower_copy(XML_ATTRIB_NAME_OBJECTS_)))
            {
                try
                {
                    //Not sure why these magic numbers 20.0f and 40.0f are here...zTn 2016-11-1
                    const float LEFT(CHILD.second.get<float>("<xmlattr>.x") - 20.0f);
                    const float TOP(CHILD.second.get<float>("<xmlattr>.y") - 40.0f);

                    const float WIDTH(CHILD.second.get<float>("<xmlattr>.width"));
                    const float HEIGHT(CHILD.second.get<float>("<xmlattr>.height"));

                    const sf::FloatRect RECT(LEFT, TOP, WIDTH, HEIGHT);

                    collisionRects_.push_back(RECT);
                }
                catch (const std::exception & E)
                {
                    std::string name("(no name)");
                    try
                    {
                        name = CHILD.second.get<std::string>("<xmlattr>.name");
                    }
                    catch (...)
                    {
                        name = CHILD.first;
                    }

                    M_HP_LOG_FAT("Parser::ParseMapFile_ParseLayerCollisions() threw "
                        << "std::exception when parsing \"" << name << "\".  what=\""
                        << E.what() << "\".");

                    throw E;
                }
            }
        }
    }


    void Parser::Prase_Layer_Generic(
        const boost::property_tree::ptree::value_type & PROPTREE_CHILD_PAIR,
        Layout & layout)
    {
        std::stringstream ssAllData;
        ssAllData << PROPTREE_CHILD_PAIR.second.get_child("data").data();

        layout.layer_vec.push_back( Layer() );
        Layer & layer{ layout.layer_vec[layout.layer_vec.size() - 1] };
        Parse_Layer_Generic_Tiles(layer.mapid_vec, ssAllData);
    }


    void Parser::Parse_Layer_Generic_Tiles(
        std::vector<int> & mapIDs,
        std::stringstream & tileLayerDataSS)
    {
        //found by experiment to be a reliable upper bound for typical maps
        mapIDs.reserve(20'000);

        std::string nextLine("");
        while (tileLayerDataSS)
        {
            std::getline(tileLayerDataSS, nextLine, '\n');

            if (nextLine.empty())
            {
                continue;
            }

            std::stringstream ssLine;
            ssLine << nextLine;
            std::string nextValStr("");

            //loop through each line reading numbers between the commas
            while (ssLine)
            {
                std::getline(ssLine, nextValStr, ',');

                if (nextValStr.empty())
                {
                    continue;
                }

                try
                {
                    //using boost::lexical_cast instead of std::stoi because it tested faster
                    mapIDs.push_back( boost::lexical_cast<int>(nextValStr) );
                }
                catch (const std::exception & E)
                {
                    M_HP_LOG("sfml_util::Parser::ParseMapFile_ParseGenericTileLayer("
                        << nextLine << ") " << "boost::lexical_cast<int>("
                        << "\"" << nextValStr << "\") threw '" << E.what() << "' exception.");

                    throw;
                }
                catch (...)
                {
                    std::ostringstream ss;
                    ss << "sfml_util::Parser::ParseMapFile_ParseGenericTileLayer("
                        << nextLine << ") " << "boost::lexical_cast<int>("
                        << "\"" << nextValStr << "\") threw unknown exception.";

                    M_HP_LOG(ss.str());
                    throw std::runtime_error(ss.str());
                }
            }
        }
    }


    bool Parser::WillParseLayer(const std::string & NODENAME_LOWERCASE) const
    {
        namespace ba = boost::algorithm;

        auto const DOES_NODENAME_CONTAIN_TILELAYER{
            ba::contains(NODENAME_LOWERCASE, ba::to_lower_copy(XML_NODE_NAME_TILE_LAYER_)) };

        auto const DOES_NODENAME_CONTAIN_OBJECTLAYER{
            ba::contains(NODENAME_LOWERCASE, ba::to_lower_copy(XML_NODE_NAME_OBJECT_LAYER_)) };

        auto const DOES_NODENAME_CONTAIN_TILESET{
            ba::contains(NODENAME_LOWERCASE, ba::to_lower_copy(XML_NODE_NAME_TILESET_)) };

        return (
            DOES_NODENAME_CONTAIN_TILELAYER ||
            DOES_NODENAME_CONTAIN_OBJECTLAYER ||
            DOES_NODENAME_CONTAIN_TILESET);
    }


    void Parser::SetupEmptyTexture(Layout & layout)
    {
        auto const TILE_WIDTH{ static_cast<unsigned>(layout.tile_size_x) };
        auto const TILE_HEIGHT{ static_cast<unsigned>(layout.tile_size_y) };

        M_ASSERT_OR_LOGANDTHROW_SS(layout.empty_texture.create(TILE_WIDTH, TILE_HEIGHT),
            "TileMap::SetupEmptyTexture() sf::RenderTexture::create(" << layout.tile_size_x << "x"
            << layout.tile_size_y << ") failed.");

        layout.empty_texture.clear(sf::Color::Transparent);
        layout.empty_texture.display();

        const std::size_t EMPTY_TEXTURE_INDEX{ 0 };
        layout.texture_vec.resize(EMPTY_TEXTURE_INDEX + 1);
        layout.texture_vec[EMPTY_TEXTURE_INDEX] = layout.empty_texture.getTexture();

        layout.tiles_panel_vec.push_back( map::TilesPanel(
            layout.EmptyTilesPanelName(),
            "",//the empty/transparent texture has no file
            0,
            1,
            1,
            EMPTY_TEXTURE_INDEX));
    }

}
}
