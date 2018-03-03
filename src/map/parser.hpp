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
#ifndef HEROESPATH_MAP_PARSER_HPP_INCLUDED
#define HEROESPATH_MAP_PARSER_HPP_INCLUDED
//
// parser.hpp
//
#include "map/layout.hpp"
#include "map/layer-type-enum.hpp"
#include "map/transition.hpp"
#include "map/map-anim.hpp"
#include "map/walk-sfx.hpp"
#include "misc/types.hpp"
#include "misc/vector-map.hpp"

#include <SFML/Graphics/Rect.hpp>

#include <map>
#include <string>
#include <vector>


//suppress warnings that are safe to ignore in boost
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__WINDOWS__)
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wswitch-enum"
#endif

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem/path.hpp>

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__) && !defined(__WINDOWS__)
#pragma GCC diagnostic warning "-Wundef"
#pragma GCC diagnostic warning "-Wswitch-enum"
#endif


namespace heroespath
{
namespace map
{

    using WalkRectMap_t = misc::VectorMap<std::size_t, std::vector<sf::FloatRect> >;

    //Responsible for wrapping all the data needed to parse a map file.
    struct ParsePacket
    {
        ParsePacket(
            const std::string & FILE_PATH_PARAM,
            Layout & layoutParam,
            std::vector<sf::FloatRect> & collisionVecParam,
            TransitionVec_t & transitionVecParam,
            WalkRectMap_t & walkVecMapParam,
            MapAnimVec_t & mapAnimVecParam,
            WalkSfxRegionLayers & walkSfxLayersParam)
        :
            file_path(FILE_PATH_PARAM),
            layout(layoutParam),
            collision_vec(collisionVecParam),
            transition_vec(transitionVecParam),
            walk_region_vecmap(walkVecMapParam),
            animation_vec(mapAnimVecParam),
            walkSfxLayers(walkSfxLayersParam)
        {}

        std::string file_path;
        Layout & layout;
        std::vector<sf::FloatRect> & collision_vec;
        TransitionVec_t & transition_vec;
        WalkRectMap_t & walk_region_vecmap;
        MapAnimVec_t & animation_vec;
        WalkSfxRegionLayers & walkSfxLayers;
    };


    //Responsible for parsing map .tmx files.
    class Parser
    {
        Parser(const Parser &) = delete;
        Parser & operator=(const Parser &) = delete;

    public:
        Parser() {}

        void Parse(ParsePacket &) const;

    private:
        void Parse_Implementation(ParsePacket &) const;

        const boost::property_tree::ptree Parse_XML(
            const std::string & MAP_FILE_PATH_STR) const;

        void Parse_MapSizes(const boost::property_tree::ptree &, Layout &) const;

        void Parse_Layer_Tileset(const boost::property_tree::ptree &, Layout &) const;

        void Parse_Layer_Collisions(
            const boost::property_tree::ptree &,
            std::vector<sf::FloatRect> &) const;

        void Prase_Layer_Generic(
            const boost::property_tree::ptree &,
            Layout &,
            const LayerType::Enum) const;

        void Parse_Layer_Generic_Tiles(std::vector<int> &, std::stringstream &) const;

        void Parse_Layer_WalkBounds(
            const boost::property_tree::ptree &,
            WalkRectMap_t &) const;

        void Parse_Layer_Animations(
            const boost::property_tree::ptree &,
            MapAnimVec_t &) const;

        void Parse_Layer_Transitions(
            const boost::property_tree::ptree &,
            TransitionVec_t &) const;

        const Transition Parse_Transition(const boost::property_tree::ptree &) const;

        void Parse_Transition_Properties(
            const boost::property_tree::ptree &,
            bool &,
            Level::Enum &,
            sfml_util::sound_effect::MapTransition &) const;

        void Parse_Layer_WalkSfxs(
            const boost::property_tree::ptree &,
            WalkSfxRegionLayers &) const;

        void Parse_WalkSfx(
            const boost::property_tree::ptree &,
            WalkSfxRegionLayers &) const;

        void SetupEmptyTexture(Layout &) const;

        template<typename T>
        T FetchXMLAttribute(
            const boost::property_tree::ptree & PTREE,
            const std::string & NAME) const
        {
            return PTREE.get<T>(XML_ATTRIB_FETCH_PREFIX_ + NAME);
        }

        const std::string FetchXMLAttributeName(
            const boost::property_tree::ptree &) const;

        void Parse_Rects(
            const boost::property_tree::ptree &,
            const std::string & NODE_NAME,
            std::vector<sf::FloatRect> &) const;

        LayerType::Enum LayerTypeFromName(const std::string &) const;

    public:
        //these are the names of XML nodes used in parsing the .tmx map files
        static const std::string XML_NODE_NAME_MAP_;
        static const std::string XML_NODE_NAME_TILE_LAYER_;
        static const std::string XML_NODE_NAME_OBJECTS_LAYER_;
        static const std::string XML_NODE_NAME_OBJECT_;
        static const std::string XML_NODE_NAME_TILESET_;
        static const std::string XML_NODE_NAME_PROPERTIES_;
        static const std::string XML_NODE_NAME_PROPERTY_;
        static const std::string XML_NODE_NAME_ANIMATIONS_;
        static const std::string XML_NODE_NAME_WALKSFX_;

        //these are the sub-names of XML nodes used in parsing the .tmx map files
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
    };

}
}

#endif //HEROESPATH_MAP_PARSER_HPP_INCLUDED
