// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_MAP_PARSER_HPP_INCLUDED
#define HEROESPATH_MAP_PARSER_HPP_INCLUDED
//
// parser.hpp
//
#include "map/layer-type-enum.hpp"
#include "map/layout.hpp"
#include "map/map-anim.hpp"
#include "map/parse-packet.hpp"
#include "map/transition.hpp"
#include "map/walk-sfx.hpp"

#include <SFML/Graphics/Rect.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <string>
#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for parsing map .tmx files.
    class Parser
    {
    public:
        Parser(const Parser &) = delete;
        Parser(Parser &&) = delete;
        Parser & operator=(const Parser &) = delete;
        Parser & operator=(Parser &&) = delete;

        Parser() = default;

        static void SetupFilesystemPaths();

        void Parse(ParsePacket &) const;

    private:
        void Parse_Implementation(ParsePacket &) const;

        const boost::property_tree::ptree Parse_XML(const std::string & MAP_FILE_PATH_STR) const;

        void Parse_MapSizes(const boost::property_tree::ptree &, Layout &) const;

        void Parse_Layer_Tileset(const boost::property_tree::ptree &, Layout &) const;

        void Parse_Layer_Collisions(
            const boost::property_tree::ptree &, std::vector<sf::FloatRect> &) const;

        void Prase_Layer_Generic(
            const boost::property_tree::ptree &, Layout &, const LayerType::Enum) const;

        void Parse_Layer_Generic_Tiles(std::vector<int> &, std::stringstream &) const;

        void Parse_Layer_WalkBounds(const boost::property_tree::ptree &, WalkRectMap_t &) const;

        void Parse_Layer_Animations(const boost::property_tree::ptree &, MapAnimVec_t &) const;

        void Parse_Layer_Transitions(const boost::property_tree::ptree &, TransitionVec_t &) const;

        const Transition Parse_Transition(const boost::property_tree::ptree &) const;

        void Parse_Transition_Properties(
            const boost::property_tree::ptree &,
            bool &,
            Level::Enum &,
            gui::sound_effect::MapTransition &) const;

        void Parse_Layer_WalkSfxs(const boost::property_tree::ptree &, WalkSfxRegionLayers &) const;

        void Parse_WalkSfx(const boost::property_tree::ptree &, WalkSfxRegionLayers &) const;

        template <typename T>
        T FetchXMLAttribute(
            const boost::property_tree::ptree & PTREE, const std::string & NAME) const
        {
            return PTREE.get<T>(XML_ATTRIB_FETCH_PREFIX_ + NAME);
        }

        const std::string FetchXMLAttributeName(const boost::property_tree::ptree &) const;

        void Parse_Rects(
            const boost::property_tree::ptree &,
            const std::string & NODE_NAME,
            std::vector<sf::FloatRect> &) const;

        LayerType::Enum LayerTypeFromName(const std::string &) const;

    public:
        // these are the names of XML nodes used in parsing the .tmx map files
        static const std::string XML_NODE_NAME_MAP_;
        static const std::string XML_NODE_NAME_TILE_LAYER_;
        static const std::string XML_NODE_NAME_OBJECTS_LAYER_;
        static const std::string XML_NODE_NAME_OBJECT_;
        static const std::string XML_NODE_NAME_TILESET_;
        static const std::string XML_NODE_NAME_PROPERTIES_;
        static const std::string XML_NODE_NAME_PROPERTY_;
        static const std::string XML_NODE_NAME_ANIMATIONS_;
        static const std::string XML_NODE_NAME_WALKSFX_;

        // these are the sub-names of XML nodes used in parsing the .tmx map files
        static const std::string XML_ATTRIB_FETCH_PREFIX_;
        static const std::string XML_ATTRIB_NAME_COLLISIONS_;
        static const std::string XML_ATTRIB_NAME_SHADOW_;
        static const std::string XML_ATTRIB_NAME_GROUND_;
        static const std::string XML_ATTRIB_NAME_TYPE_;
        static const std::string XML_ATTRIB_NAME_LEVEL_;
        static const std::string XML_ATTRIB_NAME_TRANSITIONS_;
        static const std::string XML_ATTRIB_NAME_VALUE_;
        static const std::string XML_ATTRIB_NAME_WALKBOUNDS_;
        static const std::string XML_ATTRIB_NAME_NAME_;
        static const std::string XML_ATTRIB_NAME_DOORSFX_;

        static std::string tileTextureDirectoryPath_;
    };
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_PARSER_HPP_INCLUDED
