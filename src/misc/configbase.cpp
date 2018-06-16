// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// configbase.cpp
//
#include "configbase.hpp"

#include "misc/boost-string-includes.hpp"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

namespace heroespath
{
namespace misc
{

    // static member initializers
    const std::string ConfigBase::FILE_PATH_DEFAULT_("config.txt");
    const std::string ConfigBase::SEP_STR_DEFAULT_(" ");
    const std::string ConfigBase::COMMENT_STR_DEFAULT_("#");

    // Not implementing default constructor

    ConfigBase::ConfigBase(
        const std::string & FILE_PATH_STR,
        const std::string & SEP_STR,
        const std::string & COMMENT_STR)
        : filePathStr_((FILE_PATH_STR.empty()) ? FILE_PATH_DEFAULT_ : FILE_PATH_STR)
        , sepStr_((SEP_STR.empty()) ? SEP_STR_DEFAULT_ : SEP_STR)
        , commentStr_((COMMENT_STR.empty()) ? COMMENT_STR_DEFAULT_ : COMMENT_STR)
        , map_()
    {}

    bool ConfigBase::Load()
    {
        namespace bfs = boost::filesystem;

        const bfs::path PATH(GetFullPath(filePathStr_));

        try
        {
            // verify the file exists
            if (false == bfs::exists(PATH))
            {
                return false;
            }

            // verify the file is actually a file
            if (false == bfs::is_regular_file(PATH))
            {
                std::ostringstream ss;
                ss << "Config file was found but not a regular file: \"" << PATH.string() << "\"";
                HandleLoadSaveError(ss.str());
                return false;
            }

            // open
            std::ifstream fileStream;
            fileStream.open(PATH.string().c_str(), std::ios::in);
            if ((false == fileStream.good()) || (false == fileStream.is_open()))
            {
                std::ostringstream ss;
                ss << "Config file could not be opened during load atempt: \"" << PATH.string()
                   << "\"";
                HandleLoadSaveError(ss.str());
                return false;
            }

            // clear all existing data
            map_.Clear();

            // read in all entries
            std::size_t lineNum(0);
            std::string nextLine("");
            while (getline(fileStream, nextLine))
            {
                LoadNextLine(lineNum, nextLine);
                ++lineNum;
                nextLine.clear();
            }

            // sorting only because it will increase lookup performance
            std::sort(std::begin(map_), std::end(map_));

            return (false == map_.Empty());
        }
        catch (const std::exception & EX)
        {
            std::ostringstream ss;
            ss << "Exception Error \"" << EX.what() << "\" during load of config file \""
               << PATH.string() << "\"";
            HandleLoadSaveError(ss.str());
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << R"(Exception Error "UNKNOWN" during load of config file )" << PATH.string()
               << "\"";
            HandleLoadSaveError(ss.str());
        }

        return false;
    }

    bool ConfigBase::Save(const std::string & FILENAME) const
    {
        namespace bfs = boost::filesystem;

        // use the given FILENAME unless it is empty
        const bfs::path PATH(GetFullPath((FILENAME.empty()) ? filePathStr_ : FILENAME));

        try
        {
            // open the file (closes with scope)
            std::ofstream fileStream;
            fileStream.open(PATH.string().c_str(), std::ios::out);
            if ((false == fileStream.good()) || (false == fileStream.is_open()))
            {
                std::ostringstream ss;
                ss << "Config file could not be opened for saving: \"" << PATH.string() << "\"";
                HandleLoadSaveError(ss.str());
                return false;
            }

            // write
            for (auto const & KEY_VALUE_PAIR : map_)
            {
                SaveNextLine(KEY_VALUE_PAIR, fileStream);
            }

            return true;
        }
        catch (const std::exception & EX)
        {
            std::ostringstream ss;
            ss << "Exception Error \"" << EX.what() << "\" during save of file \"" << PATH.string()
               << "\"";
            HandleLoadSaveError(ss.str());
        }
        catch (...)
        {
            std::ostringstream ss;
            ss << R"(Exception Error "UNKNOWN" during save of file )" << PATH.string() << "\"";
            HandleLoadSaveError(ss.str());
        }

        return false;
    }

    bool ConfigBase::GetWithDefaultBool(const std::string & KEY, const bool DEFAULT) const
    {
        try
        {
            bool b;
            GetBool(b, KEY);
            return b;
        }
        catch (...)
        {
            return DEFAULT;
        }
    }

    void ConfigBase::GetBool(bool & b, const std::string & KEY) const
    {
        std::string s;
        Get<std::string>(s, KEY);

        if (StringToBool(s, b) == false)
        {
            try
            {
                b = boost::lexical_cast<bool>(s);
            }
            catch (...)
            {
                std::ostringstream ss;
                ss << "misc::ConfigBase::GetBool(key=\"" << KEY << "\", str=\"" << s
                   << "\") str could not be interpreted as a bool.";
                throw std::runtime_error(ss.str());
            }
        }
    }

    void ConfigBase::GetStr(std::string & value, const std::string & KEY) const
    {
        if (map_.Find(KEY, value))
        {
            boost::algorithm::trim(value);
        }
        else
        {
            std::ostringstream ss;
            ss << "misc::ConfigBase::GetStr(key=\"" << KEY << "\") was unable to find that key.";
            throw std::runtime_error(ss.str());
        }
    }

    void ConfigBase::SetStr(const std::string & KEY, const std::string & VALUE)
    {
        map_[KEY] = VALUE;
    }

    void ConfigBase::HandleLoadSaveError(const std::string & ERR_MSG) const
    {
        std::cerr << ERR_MSG << std::endl;
    }

    void ConfigBase::HandleLoadInvalidLineError(const std::string & ERR_MSG) const
    {
        HandleLoadSaveError(ERR_MSG);
    }

    const boost::filesystem::path
        ConfigBase::GetFullPath(const std::string & USER_SPEC_PATH_STR) const
    {
        namespace bfs = boost::filesystem;

        // if no path is given, then use the one set in the constructor
        const std::string FILE_PATH_STR_TO_USE(
            (USER_SPEC_PATH_STR.empty()) ? filePathStr_ : USER_SPEC_PATH_STR);
        return bfs::path(bfs::current_path() / bfs::path(FILE_PATH_STR_TO_USE));
    }

    bool ConfigBase::LoadNextLine(const std::size_t LINE_NUM, const std::string & NEXT_LINE)
    {
        const std::size_t NEXT_LINE_LEN(NEXT_LINE.size());

        // skip empty lines
        if (2 >= NEXT_LINE_LEN)
        {
            return false;
        }

        // skip comment lines
        if (IsCommentLine(NEXT_LINE))
        {
            return false;
        }

        // find the separator position
        const std::size_t SEP_POS(NEXT_LINE.find(sepStr_));
        const std::size_t VAL_POS(SEP_POS + sepStr_.size());

        // check for problems in the position
        std::string errStr("");
        if (std::string::npos == SEP_POS)
        {
            errStr = "Separator not found on";
        }
        else if (0 == SEP_POS)
        {
            errStr = "Separator at start of";
        }
        else if (VAL_POS >= NEXT_LINE_LEN)
        {
            errStr = "Separator at end of";
        }

        const std::string NEXT_KEY(NEXT_LINE.substr(0, SEP_POS));
        std::string nextValue(NEXT_LINE.substr(VAL_POS));

        // remove newline chars if any
        if (boost::algorithm::ends_with(nextValue, "\n"))
        {
            nextValue = nextValue.substr(0, nextValue.size() - 1);
        }
        if (boost::algorithm::ends_with(nextValue, "\r"))
        {
            nextValue = nextValue.substr(0, nextValue.size() - 1);
        }

        if (errStr.empty())
        {
            map_[NEXT_KEY] = boost::algorithm::trim_copy(nextValue);
            return true;
        }
        else
        {
            std::ostringstream ss;
            ss << "Invalid config file line.  " << errStr << " line #" << LINE_NUM
               << ", and will be ignored.  (Separator=\"" << sepStr_ << "\")  (Line=\"" << NEXT_LINE
               << "\")";
            HandleLoadInvalidLineError(ss.str());
            return false;
        }
    }

    bool ConfigBase::SaveNextLine(
        const std::pair<std::string, std::string> & KEY_VALUE_PAIR, std::ostream & stream) const
    {
        stream << KEY_VALUE_PAIR.first << sepStr_ << KEY_VALUE_PAIR.second << std::endl;
        return true;
    }

    void ConfigBase::Dump(std::ostream & stream_OutParam)
    {
        for (auto const & KEY_VALUE_PAIR : map_)
        {
            stream_OutParam << KEY_VALUE_PAIR.first << sepStr_ << KEY_VALUE_PAIR.second
                            << std::endl;
        }
    }

    bool ConfigBase::IsCommentLine(const std::string & LINE) const
    {
        // skip if no comment string
        if (commentStr_.empty() || LINE.empty())
        {
            return false;
        }
        else
        {
            return (LINE.compare(0, commentStr_.length(), commentStr_) == 0);
        }
    }

    bool ConfigBase::StringToBool(const std::string & S, bool & result) const
    {
        namespace ba = boost::algorithm;

        auto const BOOL_STR{ ba::trim_copy(ba::to_lower_copy(S)) };

        if ((BOOL_STR == "true") || (BOOL_STR == "yes") || (BOOL_STR == "1"))
        {
            result = true;
            return true;
        }
        else if ((BOOL_STR == "false") || (BOOL_STR == "no") || (BOOL_STR == "0"))
        {
            result = false;
            return true;
        }
        else
        {
            return false;
        }
    }

    const std::string ConfigBase::GetFileNameStr() const { return filePathStr_; }

    const std::string ConfigBase::GetFileNameFullPathStr() const
    {
        return (GetFullPath(filePathStr_)).string();
    }

    const std::string ConfigBase::GetSeparatorStr() const { return sepStr_; }

    const std::string ConfigBase::GetCommentStr() const { return commentStr_; }
} // namespace misc
} // namespace heroespath
