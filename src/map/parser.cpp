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
    //all of these strings must be lower case
    const std::string Parser::XML_NODE_NAME_MAP_            { "map" };
    const std::string Parser::XML_NODE_NAME_TILE_LAYER_     { "layer" };
    const std::string Parser::XML_NODE_NAME_OBJECTS_LAYER_  { "objectgroup" };
    const std::string Parser::XML_NODE_NAME_OBJECT_         { "object" };
    const std::string Parser::XML_NODE_NAME_TILESET_        { "tileset" };
    const std::string Parser::XML_NODE_NAME_SHADOW_         { "shadow" };
    const std::string Parser::XML_ATTRIB_FETCH_PREFIX_      { "<xmlattr>." };
    const std::string Parser::XML_ATTRIB_NAME_COLLISIONS_   { "collisions" };


    void Parser::Parse(
        const std::string & FILE_PATH_STR,
        Layout & layout,
        sfml_util::QuadTree & collisionQTree)
    {
        try
        {
            Parse_Implementation(FILE_PATH_STR, layout, collisionQTree);
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_ERR("map::Parser::Parse(" << FILE_PATH_STR << ") threw '"
                << E.what() << "' exception.");

            throw E;
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << "map::Parser::Parse(" << FILE_PATH_STR << ") threw an unknown exception.";
            M_HP_LOG_ERR(ss.str());
            throw std::runtime_error(ss.str());
        }
    }


    void Parser::Parse_Implementation(
        const std::string & MAP_FILE_PATH_STR,
        Layout & layout,
        sfml_util::QuadTree & collisionQTree)
    {
        collisionRects_.clear();
        collisionRects_.reserve(4096);//found by experiment to be a good upper bound

        collisionQTree.Clear();

        layout.Reset();

        auto const XML_PTREE_ROOT{ Parse_XML(MAP_FILE_PATH_STR) };

        Parse_MapSizes(XML_PTREE_ROOT.get_child(XML_NODE_NAME_MAP_), layout);

        SetupEmptyTexture(layout);

        using BPTreeValue_t = boost::property_tree::ptree::value_type;

        //loop over all layers in the map file and parse them separately
        for (const BPTreeValue_t & CHILD_PAIR : XML_PTREE_ROOT.get_child(XML_NODE_NAME_MAP_))
        {
            namespace ba = boost::algorithm;
            auto const NODENAME_LOWER{ ba::to_lower_copy(CHILD_PAIR.first) };

            if (ba::contains(NODENAME_LOWER, XML_NODE_NAME_OBJECTS_LAYER_))
            {
                auto const OBJECT_LAYER_NAME_LOWER{
                    ba::to_lower_copy(FetchXMLAttributeName(CHILD_PAIR.second)) };

                if (ba::contains(OBJECT_LAYER_NAME_LOWER, XML_ATTRIB_NAME_COLLISIONS_))
                {
                    Parse_Layer_Collisions(CHILD_PAIR.second);
                }
                continue;
            }
            else if (ba::contains(NODENAME_LOWER, XML_NODE_NAME_TILESET_))
            {
                Parse_Layer_Tileset(CHILD_PAIR.second, layout);
                continue;
            }
            else if (ba::contains(NODENAME_LOWER, XML_NODE_NAME_TILE_LAYER_))
            {
                Prase_Layer_Generic(CHILD_PAIR.second, layout);
            }
        }

        collisionQTree.Setup(
            static_cast<float>(layout.tile_size_v.x * layout.tile_count_v.x),
            static_cast<float>(layout.tile_size_v.y * layout.tile_count_v.y),
            collisionRects_);

        collisionRects_.clear();

        ShadowMasker shadowMasker;
        shadowMasker.ChangeColors(XML_NODE_NAME_SHADOW_, layout);
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
        const boost::property_tree::ptree & MAP_PTREE,
        Layout & layout)
    {
        try
        {
            layout.tile_size_v.x = FetchXMLAttribute<int>(MAP_PTREE, "tilewidth");
            layout.tile_size_v.y = FetchXMLAttribute<int>(MAP_PTREE, "tileheight");
            layout.tile_count_v.x = FetchXMLAttribute<int>(MAP_PTREE, "width");
            layout.tile_count_v.y = FetchXMLAttribute<int>(MAP_PTREE, "height");
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_FAT("map::Parser::Parse_MapSizes() threw exception when parsing \""
                << FetchXMLAttributeName(MAP_PTREE) << "\".  what=\"" << E.what() << "\".");

            throw E;
        }
    }


    void Parser::Parse_Layer_Tileset(
        const boost::property_tree::ptree & TILESET_PTREE,
        Layout & layout)
    {
        auto const IMAGE_PROPTREE{ TILESET_PTREE.get_child("image") };

        layout.texture_vec.push_back(sf::Texture());
        auto const TEXTURE_INDEX{ layout.texture_vec.size() - 1 };

        layout.tiles_panel_vec.push_back( TilesPanel(
            FetchXMLAttribute<std::string>(TILESET_PTREE, "name"),
            FetchXMLAttribute<std::string>(IMAGE_PROPTREE, "source"),
            FetchXMLAttribute<int>(TILESET_PTREE, "firstgid"),
            FetchXMLAttribute<int>(TILESET_PTREE, "tilecount"),
            FetchXMLAttribute<int>(TILESET_PTREE, "columns"),
            TEXTURE_INDEX));

        TilesPanel & tilesPanel{ layout.tiles_panel_vec[layout.tiles_panel_vec.size() - 1] };

        namespace bfs = boost::filesystem;
        tilesPanel.path_obj = bfs::system_complete(
            bfs::path(game::GameDataFile::Instance()->GetMediaPath("media-maps-tile-dir")) /
                bfs::path(tilesPanel.path_rel).leaf());

        sfml_util::LoadTexture(layout.texture_vec[TEXTURE_INDEX], tilesPanel.path_obj.string());
    }


    void Parser::Parse_Layer_Collisions(const boost::property_tree::ptree & OBJGROUP_PTREE)
    {
        Parse_Rects(OBJGROUP_PTREE, XML_NODE_NAME_OBJECT_, collisionRects_);
    }


    void Parser::Prase_Layer_Generic(
        const boost::property_tree::ptree & PTREE,
        Layout & layout)
    {
        std::stringstream ssAllData;
        ssAllData << PTREE.get_child("data").data();

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
                    M_HP_LOG("sfml_util::Parser::Parse_Layer_Generic_Tiles("
                        << nextLine << ") " << "boost::lexical_cast<int>("
                        << "\"" << nextValStr << "\") threw '" << E.what() << "' exception.");

                    throw;
                }
                catch (...)
                {
                    std::ostringstream ss;
                    ss << "sfml_util::Parser::Parse_Layer_Generic_Tiles("
                        << nextLine << ") " << "boost::lexical_cast<int>("
                        << "\"" << nextValStr << "\") threw unknown exception.";

                    M_HP_LOG(ss.str());
                    throw std::runtime_error(ss.str());
                }
            }
        }
    }


    void Parser::SetupEmptyTexture(Layout & layout)
    {
        auto const TILE_WIDTH{ static_cast<unsigned>(layout.tile_size_v.x) };
        auto const TILE_HEIGHT{ static_cast<unsigned>(layout.tile_size_v.y) };

        M_ASSERT_OR_LOGANDTHROW_SS(layout.empty_texture.create(TILE_WIDTH, TILE_HEIGHT),
            "TileMap::SetupEmptyTexture() sf::RenderTexture::create(" << layout.tile_size_v.x << "x"
            << layout.tile_size_v.y << ") failed.");

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


    const std::string Parser::FetchXMLAttributeName(
        const boost::property_tree::ptree & PTREE) const
    {
        try
        {
            return FetchXMLAttribute<std::string>(PTREE, "name");
        }
        catch(...)
        {
            return "(no attribute 'name' error)";
        }
    }


    void Parser::Parse_Rects(
        const boost::property_tree::ptree & PTREE,
        const std::string & NODE_NAME,
        sfml_util::FloatRectVec_t & rectsVec) const
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(ba::to_lower_copy(CHILD_PAIR.first), NODE_NAME) == false)
            {
                continue;
            }

            auto const CHILD_PTREE{ CHILD_PAIR.second };

            try
            {
                auto const LEFT{ FetchXMLAttribute<float>(CHILD_PTREE, "x") };
                auto const TOP{ FetchXMLAttribute<float>(CHILD_PTREE, "y") };
                auto const WIDTH{ FetchXMLAttribute<float>(CHILD_PTREE, "width") };
                auto const HEIGHT{ FetchXMLAttribute<float>(CHILD_PTREE, "height") };

                const sf::FloatRect RECT(LEFT, TOP, WIDTH, HEIGHT);
                rectsVec.push_back(RECT);
            }
            catch (const std::exception & E)
            {
                M_HP_LOG_FAT("Parser::Parse_Rects() threw "
                    << "std::exception when parsing \"" << FetchXMLAttributeName(CHILD_PAIR.second)
                    << "\".  what=\"" << E.what() << "\".");

                throw E;
            }
        }
    }

}
}
