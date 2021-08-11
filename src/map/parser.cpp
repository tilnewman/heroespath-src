// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// parser.cpp
//
#include "parser.hpp"

#include "gui/cached-texture.hpp"
#include "map/tiles-panel.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/boost-string-includes.hpp"
#include "misc/config-file.hpp"
#include "misc/enum-util.hpp"
#include "misc/filesystem.hpp"
#include "misc/log-macros.hpp"
#include "misc/strings.hpp"

#include <boost/lexical_cast.hpp>

#include <algorithm>

namespace heroespath
{
namespace map
{
    // all of these strings must be lower case
    const std::string Parser::XML_NODE_NAME_MAP_ { "map" };
    const std::string Parser::XML_NODE_NAME_TILE_LAYER_ { "layer" };
    const std::string Parser::XML_NODE_NAME_OBJECTS_LAYER_ { "objectgroup" };
    const std::string Parser::XML_NODE_NAME_OBJECT_ { "object" };
    const std::string Parser::XML_NODE_NAME_TILESET_ { "tileset" };
    const std::string Parser::XML_NODE_NAME_PROPERTIES_ { "properties" };
    const std::string Parser::XML_NODE_NAME_PROPERTY_ { "property" };
    const std::string Parser::XML_NODE_NAME_ANIMATIONS_ { "animations" };
    const std::string Parser::XML_NODE_NAME_WALKSFX_ { "walk-sfx" };
    const std::string Parser::XML_ATTRIB_FETCH_PREFIX_ { "<xmlattr>." };
    const std::string Parser::XML_ATTRIB_NAME_COLLISIONS_ { "collision" };
    const std::string Parser::XML_ATTRIB_NAME_SHADOW_ { "shadow" };
    const std::string Parser::XML_ATTRIB_NAME_GROUND_ { "ground" };
    const std::string Parser::XML_ATTRIB_NAME_TYPE_ { "type" };
    const std::string Parser::XML_ATTRIB_NAME_LEVEL_ { "level" };
    const std::string Parser::XML_ATTRIB_NAME_TRANSITIONS_ { "transitions" };
    const std::string Parser::XML_ATTRIB_NAME_VALUE_ { "value" };
    const std::string Parser::XML_ATTRIB_NAME_WALKBOUNDS_ { "walk-bounds" };
    const std::string Parser::XML_ATTRIB_NAME_NAME_ { "name" };
    const std::string Parser::XML_ATTRIB_NAME_DOORSFX_ { "doorsfx" };

    std::string Parser::tileTextureDirectoryPath_ { "" };

    void Parser::SetupFilesystemPaths()
    {
        tileTextureDirectoryPath_
            = misc::ConfigFile::Instance()->GetMediaPath("media-image-map-dir");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            misc::filesystem::ExistsAndIsDirectory(tileTextureDirectoryPath_),
            "Map tile images directory does not exist or is not a directory."
                + M_HP_VAR_STR(tileTextureDirectoryPath_));
    }

    void Parser::Parse(ParsePacket & packet) const
    {
        try
        {
            Parse_Implementation(packet);
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_ERR(
                "map::Parser::Parse(" << packet.file_path << ") threw '" << EXCEPTION.what()
                                      << "' exception.");

            throw;
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
        packet.collision_vec.reserve(1024); // found by experiment to be a good upper bound

        packet.transition_vec.clear();
        packet.transition_vec.reserve(16); // found by experiment to be a good upper bound

        packet.walk_region_vecmap.Clear();

        packet.layout.Reset();

        const auto XML_PTREE_ROOT { Parse_XML(packet.file_path) };

        Parse_MapSizes(XML_PTREE_ROOT.get_child(XML_NODE_NAME_MAP_), packet.layout);

        Parse_MapBackgroundColor(XML_PTREE_ROOT.get_child(XML_NODE_NAME_MAP_), packet.layout);

        using BPTreeValue_t = boost::property_tree::ptree::value_type;

        // loop over all layers in the map file and parse them separately
        for (const BPTreeValue_t & CHILD_PAIR : XML_PTREE_ROOT.get_child(XML_NODE_NAME_MAP_))
        {
            namespace ba = boost::algorithm;
            const auto NODENAME_LOWER { misc::ToLowerCopy(CHILD_PAIR.first) };

            if (ba::contains(NODENAME_LOWER, XML_NODE_NAME_OBJECTS_LAYER_))
            {
                const auto OBJECT_LAYER_NAME_LOWER { misc::ToLowerCopy(
                    FetchXMLAttributeName(CHILD_PAIR.second)) };

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
                else if (ba::contains(OBJECT_LAYER_NAME_LOWER, XML_NODE_NAME_WALKSFX_))
                {
                    Parse_Layer_WalkSfxs(CHILD_PAIR.second, packet.walkSfxLayers);
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
                const auto LAYER_TYPE { LayerTypeFromName(
                    misc::ToLowerCopy(FetchXMLAttributeName(CHILD_PAIR.second))) };

                Prase_Layer_Generic(CHILD_PAIR.second, packet.layout, LAYER_TYPE);
            }
        }

        std::sort(std::begin(packet.collision_vec), std::end(packet.collision_vec));

        std::sort(
            std::begin(packet.walkSfxLayers.bottom_layers),
            std::end(packet.walkSfxLayers.bottom_layers));

        std::sort(
            std::begin(packet.walkSfxLayers.top_layers), std::end(packet.walkSfxLayers.top_layers));

        std::size_t layerIndex { 0 };
        for (auto & layer : packet.layout.layer_vec)
        {
            layer.index = layerIndex++;
        }
    }

    const boost::property_tree::ptree Parser::Parse_XML(const std::string & MAP_FILE_PATH_STR) const
    {
        boost::property_tree::ptree xmlPropertyTree;

        try
        {
            boost::property_tree::read_xml(MAP_FILE_PATH_STR, xmlPropertyTree);
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_ERR(
                "map::Parser::Parse_XML(\"" << MAP_FILE_PATH_STR
                                            << "\") boost::property_tree::read_xml() threw '"
                                            << EXCEPTION.what() << "' exception.");

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

    void
        Parser::Parse_MapSizes(const boost::property_tree::ptree & MAP_PTREE, Layout & layout) const
    {
        try
        {
            layout.tile_size_v.x = FetchXMLAttribute<int>(MAP_PTREE, "tilewidth");
            layout.tile_size_v.y = FetchXMLAttribute<int>(MAP_PTREE, "tileheight");
            layout.tile_count_v.x = FetchXMLAttribute<int>(MAP_PTREE, "width");
            layout.tile_count_v.y = FetchXMLAttribute<int>(MAP_PTREE, "height");
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(
                "map::Parser::Parse_MapSizes() threw exception when parsing \""
                << FetchXMLAttributeName(MAP_PTREE) << "\".  what=\"" << EXCEPTION.what() << "\".");

            throw;
        }
    }

    void Parser::Parse_MapBackgroundColor(
        const boost::property_tree::ptree & MAP_PTREE, Layout & layout) const
    {
        layout.background_color = sf::Color::Transparent;

        std::string hexColorString;

        try
        {
            hexColorString = FetchXMLAttribute<std::string>(MAP_PTREE, "backgroundcolor");
        }
        catch (...)
        {
            return;
        }

        if ((hexColorString.size() != 7) || (hexColorString[0] != '#'))
        {
            return;
        }

        // remove the hash symbol that the Tiled app prefixes
        hexColorString.erase(0, 1);

        // add 255 to the end for the alpha value
        hexColorString += "ff";

        std::stringstream ss;
        ss << std::hex << hexColorString;

        unsigned colorNumber { 0 };
        ss >> colorNumber;

        layout.background_color = sf::Color(colorNumber);
    }

    void Parser::Parse_Layer_Tileset(
        const boost::property_tree::ptree & TILESET_PTREE, Layout & layout) const
    {
        const auto IMAGE_PROPTREE { TILESET_PTREE.get_child("image") };

        const std::string FILE_RELATIVE_PATH { misc::filesystem::Filename(
            FetchXMLAttribute<std::string>(IMAGE_PROPTREE, "source")) };

        const std::string FILE_COMPLETE_PATH { misc::filesystem::CombinePaths(
            tileTextureDirectoryPath_, FILE_RELATIVE_PATH) };

        const std::string TILES_PANEL_NAME { FetchXMLAttribute<std::string>(
            TILESET_PTREE, "name") };

        layout.tiles_panel_vec.emplace_back(TilesPanel(
            TILES_PANEL_NAME,
            FILE_COMPLETE_PATH,
            FetchXMLAttribute<int>(TILESET_PTREE, "firstgid"),
            FetchXMLAttribute<int>(TILESET_PTREE, "tilecount"),
            FetchXMLAttribute<int>(TILESET_PTREE, "columns"),
            layout.texture_vec.size()));

        namespace ba = boost::algorithm;

        const auto IS_SHADOW_IMAGE { ba::contains(
            misc::ToLowerCopy(TILES_PANEL_NAME), misc::ToLowerCopy(XML_ATTRIB_NAME_SHADOW_)) };

        gui::ImageOpt::Enum imageOptionsEnum { gui::ImageOpt::Default };

        if (IS_SHADOW_IMAGE)
        {
            imageOptionsEnum |= gui::ImageOpt::ShadowMaskForShadowImage;
        }
        else
        {
            imageOptionsEnum |= gui::ImageOpt::ShadowMaskNormal;
        }

        layout.texture_vec.emplace_back(
            gui::CachedTexture(PathWrapper(FILE_COMPLETE_PATH), imageOptionsEnum));
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

        layout.layer_vec.emplace_back(Layer());
        Layer & layer { layout.layer_vec[layout.layer_vec.size() - 1] };

        layer.type = TYPE;

        Parse_Layer_Generic_Tiles(layer.mapid_vec, ssAllData);
    }

    void Parser::Parse_Layer_Generic_Tiles(
        std::vector<int> & mapIDs, std::stringstream & tileLayerDataSS) const
    {
        // found by experiment to be a reliable upper bound for typical maps
        mapIDs.reserve(20000);

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

            // loop through each line reading numbers between the commas
            while (ssLine)
            {
                std::getline(ssLine, nextValStr, ',');

                if (nextValStr.empty())
                {
                    continue;
                }

                try
                {
                    // std::stoi tested faster than boost::lexical_cast 2018-3-19
                    mapIDs.emplace_back(std::stoi(nextValStr));
                }
                catch (const std::exception & EXCEPTION)
                {
                    M_HP_LOG(
                        "map::Parser::Parse_Layer_Generic_Tiles("
                        << nextLine << ") "
                        << "boost::lexical_cast<int>("
                        << "\"" << nextValStr << "\") threw '" << EXCEPTION.what()
                        << "' exception.");

                    throw;
                }
                catch (...)
                {
                    std::ostringstream ss;
                    ss << "map::Parser::Parse_Layer_Generic_Tiles(" << nextLine << ") "
                       << "boost::lexical_cast<int>("
                       << "\"" << nextValStr << "\") threw unknown exception.";

                    M_HP_LOG(ss.str());
                    throw std::runtime_error(ss.str());
                }
            }
        }
    }

    void Parser::Parse_Layer_WalkBounds(
        const boost::property_tree::ptree & PTREE, WalkRectMap_t & walkRectMap) const
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(misc::ToLowerCopy(CHILD_PAIR.first), XML_NODE_NAME_OBJECT_))
            {
                const auto OBJECT_PTREE { CHILD_PAIR.second };

                // the index is stored in an attribute field named "type"
                const auto WALK_RECT_INDEX { FetchXMLAttribute<std::size_t>(
                    OBJECT_PTREE, XML_ATTRIB_NAME_TYPE_) };

                sf::FloatRect rect;

                try
                {
                    rect.left = FetchXMLAttribute<float>(OBJECT_PTREE, "x");
                    rect.top = FetchXMLAttribute<float>(OBJECT_PTREE, "y");
                    rect.width = FetchXMLAttribute<float>(OBJECT_PTREE, "width");
                    rect.height = FetchXMLAttribute<float>(OBJECT_PTREE, "height");
                }
                catch (const std::exception & EXCEPTION)
                {
                    M_HP_LOG_FAT(
                        "map::Parser::Parse_Layer_WalkBounds() threw "
                        << "std::exception when parsing the rect from a node named \""
                        << FetchXMLAttributeName(OBJECT_PTREE) << "\".  what=\"" << EXCEPTION.what()
                        << "\".");

                    throw;
                }

                walkRectMap[WALK_RECT_INDEX].emplace_back(rect);
            }
        }
    }

    void Parser::Parse_Layer_Animations(
        const boost::property_tree::ptree & PTREE, MapAnimVec_t & anims_) const
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(misc::ToLowerCopy(CHILD_PAIR.first), XML_NODE_NAME_OBJECT_))
            {
                const auto OBJECT_PTREE { CHILD_PAIR.second };

                // the anim enum name is stored in an attribute field named "name"
                const auto ANIM_NAME { FetchXMLAttribute<std::string>(
                    OBJECT_PTREE, XML_ATTRIB_NAME_NAME_) };

                const auto ANIM_ENUM { EnumUtil<gui::Animations>::FromString(ANIM_NAME) };

                M_HP_ASSERT_OR_LOG_AND_THROW(
                    (ANIM_ENUM != gui::Animations::Count),
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
                catch (const std::exception & EXCEPTION)
                {
                    M_HP_LOG_FAT(
                        "map::Parser::Parse_Layer_Animations() threw "
                        << "std::exception when parsing the rect from a node named \""
                        << FetchXMLAttributeName(OBJECT_PTREE) << "\".  what=\"" << EXCEPTION.what()
                        << "\".");

                    throw;
                }

                anims_.emplace_back(MapAnim(ANIM_ENUM, rect, gui::Animations::Sfx(ANIM_ENUM)));
            }
        }
    }

    void Parser::Parse_Layer_Transitions(
        const boost::property_tree::ptree & PTREE, TransitionVec_t & transitionVec) const
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(misc::ToLowerCopy(CHILD_PAIR.first), XML_NODE_NAME_OBJECT_))
            {
                transitionVec.emplace_back(Parse_Transition(CHILD_PAIR.second));
            }
        }
    }

    const Transition Parser::Parse_Transition(const boost::property_tree::ptree & PTREE) const
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
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(
                "map::Parser::Parse_Transition() threw "
                << "std::exception when parsing the rect from a node named \""
                << FetchXMLAttributeName(PTREE) << "\".  what=\"" << EXCEPTION.what() << "\".");

            throw;
        }

        auto isEntry { false };
        Level::Enum level { Level::Count };

        gui::sound_effect::MapTransition transSfxType { gui::sound_effect::MapTransition::Count };

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(misc::ToLowerCopy(CHILD_PAIR.first), XML_NODE_NAME_PROPERTIES_))
            {
                Parse_Transition_Properties(CHILD_PAIR.second, isEntry, level, transSfxType);
            }
        }

        return Transition(isEntry, level, rect, transSfxType);
    }

    void Parser::Parse_Transition_Properties(
        const boost::property_tree::ptree & PTREE,
        bool & isEntry,
        Level::Enum & level,
        gui::sound_effect::MapTransition & transSfxType) const
    {
        namespace ba = boost::algorithm;

        auto wasEntrySet { false };

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(misc::ToLowerCopy(CHILD_PAIR.first), XML_NODE_NAME_PROPERTY_) == false)
            {
                continue;
            }

            const auto NAME_STR { FetchXMLAttributeName(CHILD_PAIR.second) };

            const auto VALUE_STR { FetchXMLAttribute<std::string>(
                CHILD_PAIR.second, XML_ATTRIB_NAME_VALUE_) };

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
                    level = EnumUtil<Level>::FromString(VALUE_STR);
                }
                catch (...)
                {
                    std::ostringstream ss;

                    ss << "map::Parser::Parse_Transition_Properties() found a level "
                          "property in a map file \""
                       << VALUE_STR << "\", but that is not a valid level name.";

                    throw std::runtime_error(ss.str());
                }
            }
            else if (NAME_STR == XML_ATTRIB_NAME_DOORSFX_)
            {
                try
                {
                    transSfxType = gui::sound_effect::MapTransitionFromString(VALUE_STR);
                }
                catch (...)
                {
                    std::ostringstream ss;

                    ss << "map::Parser::Parse_Transition_Properties() found a doorSfxType "
                          "property in a map file \""
                       << VALUE_STR << "\", but that is not a valid name.";

                    throw std::runtime_error(ss.str());
                }
            }
        }

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (wasEntrySet),
            "map::Parser::Parse_Transition_Properties() was unable to parse an entry type.");

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (level != Level::Count),
            "map::Parser::Parse_Transition_Properties() was unable to parse a Level::Enum.");
    }

    void Parser::Parse_Layer_WalkSfxs(
        const boost::property_tree::ptree & PTREE, WalkSfxRegionLayers & walkSfxLayers) const
    {
        namespace ba = boost::algorithm;

        for (const boost::property_tree::ptree::value_type & CHILD_PAIR : PTREE)
        {
            if (ba::contains(misc::ToLowerCopy(CHILD_PAIR.first), XML_NODE_NAME_OBJECT_))
            {
                Parse_WalkSfx(CHILD_PAIR.second, walkSfxLayers);
            }
        }
    }

    void Parser::Parse_WalkSfx(
        const boost::property_tree::ptree & PTREE, WalkSfxRegionLayers & walkSfxLayers) const
    {
        namespace ba = boost::algorithm;

        sf::FloatRect rect;
        std::string footstepName;

        try
        {
            rect.left = FetchXMLAttribute<float>(PTREE, "x");
            rect.top = FetchXMLAttribute<float>(PTREE, "y");
            rect.width = FetchXMLAttribute<float>(PTREE, "width");
            rect.height = FetchXMLAttribute<float>(PTREE, "height");
            footstepName = FetchXMLAttributeName(PTREE);
        }
        catch (const std::exception & EXCEPTION)
        {
            M_HP_LOG_FAT(
                "map::Parser::Parse_WalkSfx() threw "
                << "std::exception when parsing the rect from a node named \""
                << FetchXMLAttributeName(PTREE) << "\".  what=\"" << EXCEPTION.what() << "\".");

            throw;
        }

        const auto SFX { gui::sound_effect::FootstepToSfx(
            EnumUtil<gui::Footstep>::FromString(footstepName)) };

        if (SFX == gui::sound_effect::Count)
        {
            std::ostringstream ss;
            ss << "map::Parser::Parse_WalkSfx() failed to translate footstep sfx name \""
               << footstepName << "\" into a valid Footstep and then a valid sound_effect::Enum.";

            throw std::runtime_error(ss.str());
        }

        std::string typeStr { "" };
        try
        {
            typeStr = FetchXMLAttribute<std::string>(PTREE, XML_ATTRIB_NAME_TYPE_);
        }
        catch (...)
        {
            typeStr = "";
        }

        if ("0" == typeStr)
        {
            walkSfxLayers.bottom_layers.emplace_back(WalkSfxRegion(rect, SFX));
        }
        else
        {
            walkSfxLayers.top_layers.emplace_back(WalkSfxRegion(rect, SFX));
        }
    }

    const std::string Parser::FetchXMLAttributeName(const boost::property_tree::ptree & PTREE) const
    {
        try
        {
            return FetchXMLAttribute<std::string>(PTREE, "name");
        }
        catch (...)
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
            if (ba::contains(misc::ToLowerCopy(CHILD_PAIR.first), NODE_NAME) == false)
            {
                continue;
            }

            const auto CHILD_PTREE { CHILD_PAIR.second };

            try
            {
                const auto LEFT { FetchXMLAttribute<float>(CHILD_PTREE, "x") };
                const auto TOP { FetchXMLAttribute<float>(CHILD_PTREE, "y") };
                const auto WIDTH { FetchXMLAttribute<float>(CHILD_PTREE, "width") };
                const auto HEIGHT { FetchXMLAttribute<float>(CHILD_PTREE, "height") };

                const sf::FloatRect RECT(LEFT, TOP, WIDTH, HEIGHT);
                rectsVec.emplace_back(RECT);
            }
            catch (const std::exception & EXCEPTION)
            {
                M_HP_LOG_FAT(
                    "map::Parser::Parse_Rects() threw "
                    << "std::exception when parsing \"" << FetchXMLAttributeName(CHILD_PAIR.second)
                    << "\".  what=\"" << EXCEPTION.what() << "\".");

                throw;
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
} // namespace map
} // namespace heroespath
