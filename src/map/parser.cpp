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
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "misc/assertlogandthrow.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

#include <algorithm>


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
    const std::string Parser::XML_NODE_NAME_PROPERTIES_     { "properties" };
    const std::string Parser::XML_NODE_NAME_PROPERTY_       { "property" };
    const std::string Parser::XML_NODE_NAME_ANIMATIONS_     { "animations" };
    const std::string Parser::XML_ATTRIB_FETCH_PREFIX_      { "<xmlattr>." };
    const std::string Parser::XML_ATTRIB_NAME_COLLISIONS_   { "collision" };
    const std::string Parser::XML_ATTRIB_NAME_SHADOW_       { "shadow" };
    const std::string Parser::XML_ATTRIB_NAME_GROUND_       { "ground" };
    const std::string Parser::XML_ATTRIB_NAME_TYPE_         { "type" };
    const std::string Parser::XML_ATTRIB_NAME_LEVEL_        { "level" };
    const std::string Parser::XML_ATTRIB_NAME_TRANSITIONS_  { "transitions" };
    const std::string Parser::XML_ATTRIB_NAME_VALUE_        { "value" };
    const std::string Parser::XML_ATTRIB_NAME_WALKBOUNDS_   { "walk-bounds" };
    const std::string Parser::XML_ATTRIB_NAME_NAME_         { "name" };
    const std::string Parser::XML_ATTRIB_NAME_DOORSFX_      { "doorsfx" };


    void Parser::Parse(ParsePacket & packet) const
    {
        try
        {
            Parse_Implementation(packet);
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_ERR("map::Parser::Parse(" << packet.file_path << ") threw '"
                << E.what() << "' exception.");

            throw E;
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << "map::Parser::Parse(" << packet.file_path << ") threw an unknown exception.";
            M_HP_LOG_ERR(ss.str());
            throw std::runtime_error(ss.str());
        }
    }


    void Parser::Parse_Implementation(ParsePacket & packet) const
    {
        packet.collision_vec.clear();
        packet.collision_vec.reserve(1024);//found by experiment to be a good upper bound

        packet.transition_vec.clear();
        packet.transition_vec.reserve(8);//found by experiment to be a good upper bound

        packet.walk_region_vecmap.Clear();

        packet.layout.Reset();

        auto const XML_PTREE_ROOT{ Parse_XML(packet.file_path) };

        Parse_MapSizes(XML_PTREE_ROOT.get_child(XML_NODE_NAME_MAP_), packet.layout);

        SetupEmptyTexture(packet.layout);

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
                    Parse_Layer_Collisions(CHILD_PAIR.second, packet.collision_vec);
                }
                else if (ba::contains(OBJECT_LAYER_NAME_LOWER, XML_ATTRIB_NAME_TRANSITIONS_))
                {
                    Parse_Layer_Transitions(CHILD_PAIR.second, packet.transition_vec);
                }
                else if (ba::contains(OBJECT_LAYER_NAME_LOWER, XML_ATTRIB_NAME_WALKBOUNDS_))
                {
                    Parse_Layer_WalkBounds(CHILD_PAIR.second, packet.walk_region_vecmap);
                }
                else if (ba::contains(OBJECT_LAYER_NAME_LOWER, XML_NODE_NAME_ANIMATIONS_))
                {
                    Parse_Layer_Animations(CHILD_PAIR.second, packet.animation_vec);
                }
                continue;
            }
            else if (ba::contains(NODENAME_LOWER, XML_NODE_NAME_TILESET_))
            {
                Parse_Layer_Tileset(CHILD_PAIR.second, packet.layout);
                continue;
            }
            else if (ba::contains(NODENAME_LOWER, XML_NODE_NAME_TILE_LAYER_))
            {
                auto const LAYER_TYPE{ LayerTypeFromName(ba::to_lower_copy(
                    FetchXMLAttributeName(CHILD_PAIR.second))) };

                Prase_Layer_Generic(CHILD_PAIR.second, packet.layout, LAYER_TYPE);
            }
        }

        std::sort(std::begin(packet.collision_vec), std::end(packet.collision_vec));

        ShadowMasker::ChangeColors(XML_ATTRIB_NAME_SHADOW_, packet.layout);
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
        Layout & layout) const
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
        Layout & layout) const
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


    void Parser::Parse_Layer_Collisions(
        const boost::property_tree::ptree & OBJGROUP_PTREE,
        std::vector<sf::FloatRect> & collisionRects) const
    {
        Parse_Rects(OBJGROUP_PTREE, XML_NODE_NAME_OBJECT_, collisionRects);
    }


    void Parser::Prase_Layer_Generic(
        const boost::property_tree::ptree & PTREE,
        Layout & layout,
        const LayerType::Enum TYPE) const
    {
        std::stringstream ssAllData;
        ssAllData << PTREE.get_child("data").data();

        layout.layer_vec.push_back( Layer() );
        Layer & layer{ layout.layer_vec[layout.layer_vec.size() - 1] };

        layer.type = TYPE;

        Parse_Layer_Generic_Tiles(layer.mapid_vec, ssAllData);
    }


    void Parser::Parse_Layer_Generic_Tiles(
        std::vector<int> & mapIDs,
        std::stringstream & tileLayerDataSS) const
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
                    M_HP_LOG("map::Parser::Parse_Layer_Generic_Tiles("
                        << nextLine << ") " << "boost::lexical_cast<int>("
                        << "\"" << nextValStr << "\") threw '" << E.what() << "' exception.");

                    throw;
                }
                catch (...)
                {
                    std::ostringstream ss;
                    ss << "map::Parser::Parse_Layer_Generic_Tiles("
                        << nextLine << ") " << "boost::lexical_cast<int>("
                        << "\"" << nextValStr << "\") threw unknown exception.";

                    M_HP_LOG(ss.str());
                    throw std::runtime_error(ss.str());
                }
            }
        }
    }


    void Parser::Parse_Layer_WalkBounds(
        const boost::property_tree::ptree & PTREE,
        WalkRectMap_t & walkRectMap) const
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(ba::to_lower_copy(CHILD_PAIR.first), XML_NODE_NAME_OBJECT_))
            {
                auto const OBJECT_PTREE{ CHILD_PAIR.second };

                //the index is stored in an attribute field named "type"
                auto const WALK_RECT_INDEX{
                    FetchXMLAttribute<std::size_t>(OBJECT_PTREE, XML_ATTRIB_NAME_TYPE_) };

                sf::FloatRect rect;

                try
                {
                    rect.left = FetchXMLAttribute<float>(OBJECT_PTREE, "x");
                    rect.top = FetchXMLAttribute<float>(OBJECT_PTREE, "y");
                    rect.width = FetchXMLAttribute<float>(OBJECT_PTREE, "width");
                    rect.height = FetchXMLAttribute<float>(OBJECT_PTREE, "height");
                }
                catch (const std::exception & E)
                {
                    M_HP_LOG_FAT("map::Parser::Parse_Layer_WalkBounds() threw "
                        << "std::exception when parsing the rect from a node named \""
                        << FetchXMLAttributeName(OBJECT_PTREE) << "\".  what=\""
                        << E.what() << "\".");

                    throw E;
                }

                walkRectMap[WALK_RECT_INDEX].push_back(rect);
            }
        }
    }


    void Parser::Parse_Layer_Animations(
        const boost::property_tree::ptree & PTREE,
        MapAnimVec_t & anims_) const
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(ba::to_lower_copy(CHILD_PAIR.first), XML_NODE_NAME_OBJECT_))
            {
                auto const OBJECT_PTREE{ CHILD_PAIR.second };

                //the anim enum name is stored in an attribute field named "name"
                auto const ANIM_NAME{
                    FetchXMLAttribute<std::string>(OBJECT_PTREE, XML_ATTRIB_NAME_NAME_) };

                auto const ANIM_ENUM{ sfml_util::Animations::FromString(ANIM_NAME) };

                M_ASSERT_OR_LOGANDTHROW_SS((ANIM_ENUM != sfml_util::Animations::Count),
                    "map::Parser::Parse_Layer_Animations() got an invalid animation name \""
                    << ANIM_NAME << "\"");

                sf::FloatRect rect;

                try
                {
                    rect.left = FetchXMLAttribute<float>(OBJECT_PTREE, "x");
                    rect.top = FetchXMLAttribute<float>(OBJECT_PTREE, "y");
                    rect.width = FetchXMLAttribute<float>(OBJECT_PTREE, "width");
                    rect.height = FetchXMLAttribute<float>(OBJECT_PTREE, "height");
                }
                catch (const std::exception & E)
                {
                    M_HP_LOG_FAT("map::Parser::Parse_Layer_Animations() threw "
                        << "std::exception when parsing the rect from a node named \""
                        << FetchXMLAttributeName(OBJECT_PTREE) << "\".  what=\""
                        << E.what() << "\".");

                    throw E;
                }

                anims_.push_back( MapAnim(
                    ANIM_ENUM,
                    rect,
                    sfml_util::Animations::Sfx(ANIM_ENUM)) );
            }
        }
    }


    void Parser::Parse_Layer_Transitions(
        const boost::property_tree::ptree & PTREE,
        TransitionVec_t & transitionVec) const
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(ba::to_lower_copy(CHILD_PAIR.first), XML_NODE_NAME_OBJECT_))
            {
                transitionVec.push_back(Parse_Transition(CHILD_PAIR.second));
            }
        }
    }


    const Transition Parser::Parse_Transition(
        const boost::property_tree::ptree & PTREE) const
    {
        namespace ba = boost::algorithm;

        sf::FloatRect rect;

        try
        {
            rect.left = FetchXMLAttribute<float>(PTREE, "x");
            rect.top = FetchXMLAttribute<float>(PTREE, "y");
            rect.width = FetchXMLAttribute<float>(PTREE, "width");
            rect.height = FetchXMLAttribute<float>(PTREE, "height");
        }
        catch (const std::exception & E)
        {
            M_HP_LOG_FAT("map::Parser::Parse_Transition() threw "
                << "std::exception when parsing the rect from a node named \""
                << FetchXMLAttributeName(PTREE) << "\".  what=\""
                << E.what() << "\".");

            throw E;
        }

        auto isEntry{ false };
        Level::Enum level{ Level::Count };
        sfml_util::sound_effect::DoorType doorSfxType{ sfml_util::sound_effect::DoorType::Count };

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(ba::to_lower_copy(CHILD_PAIR.first), XML_NODE_NAME_PROPERTIES_))
            {
                Parse_Transition_Properties(CHILD_PAIR.second, isEntry, level, doorSfxType);
            }
        }

        return Transition(isEntry, level, rect, doorSfxType);
    }


    void Parser::Parse_Transition_Properties(
        const boost::property_tree::ptree & PTREE,
        bool & isEntry,
        Level::Enum & level,
        sfml_util::sound_effect::DoorType & doorSfxType) const
    {
        namespace ba = boost::algorithm;

        auto wasEntrySet{ false };

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(ba::to_lower_copy(CHILD_PAIR.first), XML_NODE_NAME_PROPERTY_) == false)
            {
                continue;
            }

            auto const NAME_STR{ FetchXMLAttributeName(CHILD_PAIR.second) };

            auto const VALUE_STR{
                FetchXMLAttribute<std::string>(CHILD_PAIR.second, XML_ATTRIB_NAME_VALUE_) };

            if (NAME_STR == XML_ATTRIB_NAME_TYPE_)
            {
                if (VALUE_STR == "entry")
                {
                    isEntry = true;
                    wasEntrySet = true;
                }
                else if (VALUE_STR == "exit")
                {
                    isEntry = false;
                    wasEntrySet = true;
                }
            }
            else if (NAME_STR == XML_ATTRIB_NAME_LEVEL_)
            {
                try
                {
                    level = Level::FromString(VALUE_STR);
                }
                catch(...)
                {
                    std::ostringstream ss;

                    ss << "map::Parser::Parse_Transition_Properties() found a level "
                        "property in a map file \"" << VALUE_STR
                        << "\", but that is not a valid level name.";

                    throw std::runtime_error(ss.str());
                }
            }
            else if (NAME_STR == XML_ATTRIB_NAME_DOORSFX_)
            {
                try
                {
                    doorSfxType = sfml_util::sound_effect::DoorTypeFromString(VALUE_STR);
                }
                catch (...)
                {
                    std::ostringstream ss;

                    ss << "map::Parser::Parse_Transition_Properties() found a doorSfxType "
                        "property in a map file \"" << VALUE_STR
                        << "\", but that is not a valid name.";

                    throw std::runtime_error(ss.str());
                }
            }
        }

        M_ASSERT_OR_LOGANDTHROW_SS((wasEntrySet),
            "map::Parser::Parse_Transition_Properties() was unable to parse an entry type.");

        M_ASSERT_OR_LOGANDTHROW_SS((level != Level::Count),
            "map::Parser::Parse_Transition_Properties() was unable to parse a Level::Enum.");
    }


    void Parser::SetupEmptyTexture(Layout & layout) const
    {
        auto const TILE_WIDTH{ static_cast<unsigned>(layout.tile_size_v.x) };
        auto const TILE_HEIGHT{ static_cast<unsigned>(layout.tile_size_v.y) };

        M_ASSERT_OR_LOGANDTHROW_SS(layout.empty_texture.create(TILE_WIDTH, TILE_HEIGHT),
            "map::Parser::SetupEmptyTexture() sf::RenderTexture::create(" << layout.tile_size_v.x << "x"
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
            return "";
        }
    }


    void Parser::Parse_Rects(
        const boost::property_tree::ptree & PTREE,
        const std::string & NODE_NAME,
        std::vector<sf::FloatRect> & rectsVec) const
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
                M_HP_LOG_FAT("map::Parser::Parse_Rects() threw "
                    << "std::exception when parsing \"" << FetchXMLAttributeName(CHILD_PAIR.second)
                    << "\".  what=\"" << E.what() << "\".");

                throw E;
            }
        }
    }


    LayerType::Enum Parser::LayerTypeFromName(const std::string & NAME) const
    {
        namespace ba = boost::algorithm;

        if (ba::contains(NAME, XML_ATTRIB_NAME_GROUND_))
        {
            return LayerType::Ground;
        }
        else if (ba::contains(NAME, XML_ATTRIB_NAME_SHADOW_))
        {
            return LayerType::Shadow;
        }
        else
        {
            return LayerType::Object;
        }
    }

}
}
