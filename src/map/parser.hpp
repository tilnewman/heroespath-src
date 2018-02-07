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
#include "misc/types.hpp"

#include "sfml-util/collision-quad-tree.hpp"

#include <string>


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

    //Responsible for parsing map .tmx files.
    class Parser
    {
        Parser(const Parser &) = delete;
        Parser & operator=(const Parser &) = delete;

    public:
        Parser() {}

        void Parse(
            const std::string & FILE_PATH_STR,
            Layout &,
            sfml_util::QuadTree &);

    private:
        void Parse_Implementation(
            const std::string & FILE_PATH_STR,
            Layout &,
            sfml_util::QuadTree &);

        const boost::property_tree::ptree Parse_XML(
            const std::string & MAP_FILE_PATH_STR) const;

        void Parse_MapSizes(const boost::property_tree::ptree &, Layout &);
        void Parse_Layer_Tileset(const boost::property_tree::ptree &, Layout & layout);
        void Parse_Layer_Collisions(const boost::property_tree::ptree &);
        void Prase_Layer_Generic(const boost::property_tree::ptree::value_type &, Layout & layout);
        void Parse_Layer_Generic_Tiles(std::vector<int> &, std::stringstream &);
        bool WillParseLayer(const std::string & NODENAME_LOWERCASE) const;
        void SetupEmptyTexture(Layout &);

    public:
        //these are the names of XML nodes used in parsing the .tmx map files
        static const std::string XML_NODE_NAME_TILE_LAYER_;
        static const std::string XML_NODE_NAME_OBJECT_LAYER_;
        static const std::string XML_NODE_NAME_TILESET_;

        //these are the sub-names of XML nodes used in parsing the .tmx map files
        static const std::string XML_ATTRIB_NAME_OBJECTS_;
        static const std::string XML_ATTRIB_NAME_SHADOWS_;
        
    private:
        sfml_util::FloatRectVec_t collisionRects_;
    };

}
}

#endif //HEROESPATH_MAP_PARSER_HPP_INCLUDED
