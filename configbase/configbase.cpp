//
// configbase.cpp
//
#include "configbase.hpp"
#include "common/assertorthrow.hpp"

#include <sstream>
#include <fstream>
#include <exception>


namespace appbase
{
    //static member initializers
    const std::string ConfigBase::FILE_PATH_DEFAULT_    ("config.txt");
    const std::string ConfigBase::SEP_STR_DEFAULT_      (" ");
    const std::string ConfigBase::COMMENT_STR_DEFAULT_  ("#");

    //Not implementing default constructor

    ConfigBase::ConfigBase( const std::string & FILE_PATH_STR,
                            const std::string & SEP_STR,
                            const std::string & COMMENT_STR)
    :
        dataAccessMutex_(),
        filePathStr_    ( (FILE_PATH_STR.empty()) ? FILE_PATH_DEFAULT_ : FILE_PATH_STR ),
        sepStr_         ( (SEP_STR.empty()) ? SEP_STR_DEFAULT_ : SEP_STR ),
        commentStr_     ( (COMMENT_STR.empty()) ? COMMENT_STR_DEFAULT_ : COMMENT_STR ),
        data_           ()
    {}


    ConfigBase::~ConfigBase()
    {
        //MUST CALL SAVE - not saved automatically upon destruction
    }


    bool ConfigBase::Load()
    {
        boost::recursive_mutex::scoped_lock lock(dataAccessMutex_);

        namespace bfs = boost::filesystem;

        const bfs::path PATH( GetFullPath(filePathStr_) );

        try
        {
            //verify the file exists
            if (false == bfs::exists(PATH))
            {
                std::ostringstream ss;
                ss << "Config file not found for loading: \"" << PATH.string() << "\"";
                HandleLoadSaveError( ss.str() );
                return false;
            }

            //open
            std::ifstream fileStream;
            fileStream.open(PATH.string().c_str(),std::ios::in);
            if ((false == fileStream.good()) || (false == fileStream.is_open()))
            {
                std::ostringstream ss;
                ss << "Config file could not be opened for loading: \"" << PATH.string() << "\"";
                HandleLoadSaveError( ss.str() );
                return false;
            }

            //clear all existing data
            data_.clear();

            //read in all entries
            std::size_t lineNum(0);
            std::string nextLine("");
            while(getline(fileStream, nextLine))
            {
                LoadNextLine(lineNum, nextLine);
                ++lineNum;
                nextLine.clear();
            }

            return (false == data_.empty());
        }
        catch(const std::exception & EX)
        {
            std::ostringstream ss;
            ss << "Exception Error \"" << EX.what() << "\" during load of config file \"" << PATH.string() << "\"";
            HandleLoadSaveError( ss.str() );
        }
        catch(...)
        {
            std::ostringstream ss;
            ss << "Exception Error \"UNKNOWN\" during load of config file \"" << PATH.string() << "\"";
            HandleLoadSaveError( ss.str() );
        }

        return false;
    }


    bool ConfigBase::Save(const std::string & FILENAME) const
    {
        boost::recursive_mutex::scoped_lock lock(dataAccessMutex_);

        namespace bfs = boost::filesystem;

        //use the given FILENAME unless it is empty
        const bfs::path PATH( GetFullPath( (FILENAME.empty()) ? filePathStr_ : FILENAME ) );

        try
        {
            //open the file (closes with scope)
            std::ofstream fileStream;
            fileStream.open(PATH.string().c_str(), std::ios::out);
            if ((false == fileStream.good()) || (false == fileStream.is_open()))
            {
                std::ostringstream ss;
                ss << "Config file could not be opened for saving: \"" << PATH.string() << "\"";
                HandleLoadSaveError( ss.str() );
                return false;
            }

            //write
            for(ConfigMap::const_iterator i(data_.begin()); data_.end() != i; ++i)
            {
                SaveNextLine(i, fileStream);
            }

            return true;
        }
        catch(const std::exception & EX)
        {
            std::ostringstream ss;
            ss << "Exception Error \"" << EX.what() << "\" during save of file \"" << PATH.string() << "\"";
            HandleLoadSaveError( ss.str() );
        }
        catch(...)
        {
            std::ostringstream ss;
            std::cerr << "Exception Error \"UNKNOWN\" during save of file \"" << PATH.string() << "\"";
            HandleLoadSaveError( ss.str() );
        }

        return false;
    }


    const std::string ConfigBase::GetCopyStr(const std::string & KEY, const std::string & DEFAULT) const
    {
        std::string temp(DEFAULT);
        GetStr(temp, KEY);
        return temp;
    }


    void ConfigBase::GetStr(std::string & val, const std::string & KEY) const
    {
        boost::recursive_mutex::scoped_lock lock(dataAccessMutex_);

        const ConfigMap::const_iterator ITER( data_.find(KEY) );
        if (ITER == data_.end())
        {
            std::ostringstream ss;
            ss << "appbase::ConfigBase::GetStr(key=\"" << KEY << "\") was unable to find that key.";
            throw std::runtime_error(ss.str());
        }
        else
        {
            val = boost::algorithm::trim_copy(ITER->second);
        }
    }


    void ConfigBase::SetStr(const std::string & KEY, const std::string & VALUE)
    {
        boost::recursive_mutex::scoped_lock lock(dataAccessMutex_);
        data_[KEY] = VALUE;
    }


    void ConfigBase::HandleLoadSaveError(const std::string & ERR_MSG) const
    {
        std::cerr << ERR_MSG << std::endl;
    }


    void ConfigBase::HandleLoadInvalidLineError(const std::string & ERR_MSG) const
    {
        HandleLoadSaveError(ERR_MSG);
    }


    const boost::filesystem::path ConfigBase::GetFullPath(const std::string & USER_SPEC_PATH_STR) const
    {
        namespace bfs = boost::filesystem;

        //if no path is given, then use the one set in the constructor
        const std::string FILE_PATH_STR_TO_USE( (USER_SPEC_PATH_STR.empty()) ? filePathStr_ : USER_SPEC_PATH_STR );
        return bfs::path(bfs::current_path() / bfs::path(FILE_PATH_STR_TO_USE));
    }


    bool ConfigBase::LoadNextLine(const std::size_t LINE_NUM, const std::string & NEXT_LINE)
    {
        const std::size_t NEXT_LINE_LEN( NEXT_LINE.size() );

        //skip empty lines
        if (2 >= NEXT_LINE_LEN)
        {
            return false;
        }

        //skip comment lines
        if (IsCommentLine(NEXT_LINE))
        {
            return false;
        }

        //find the separator position
        const std::size_t SEP_POS( NEXT_LINE.find(sepStr_) );
        const std::size_t VAL_POS(SEP_POS + sepStr_.size());

        //check for problems in the position
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

        const std::string NEXT_KEY( NEXT_LINE.substr(0, SEP_POS) );
        std::string nextValue( NEXT_LINE.substr(VAL_POS) );

        //remove newline chars if any
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
            data_[NEXT_KEY] = boost::algorithm::trim_copy(nextValue);
            return true;
        }
        else
        {
            std::ostringstream ss;
            ss  << "Invalid config file line.  " << errStr
                << " line #" << LINE_NUM
                << ", and will be ignored.  (Separator=\"" << sepStr_
                << "\")  (Line=\"" << NEXT_LINE << "\")";
            HandleLoadInvalidLineError( ss.str() );
            return false;
        }
    }


    bool ConfigBase::SaveNextLine(const ConfigMap::const_iterator & NEXT_ENTRY_ITR, std::ostream & stream) const
    {
        stream << NEXT_ENTRY_ITR->first << sepStr_ << NEXT_ENTRY_ITR->second << std::endl;
        return true;
    }


    void ConfigBase::Dump(std::ostream & stream_OutParam)
    {
        for(ConfigMapCIter i(data_.begin()); data_.end() != i; ++i)
        {
            stream_OutParam << i->first << sepStr_ << i->second << std::endl;
        }
    }


    bool ConfigBase::IsCommentLine(const std::string & LINE) const
    {
        //skip if no comment string
        if (commentStr_.empty() || LINE.empty())
        {
            return false;
        }

        //only check if first char matches
        if (commentStr_[0] == LINE[0])
        {
            if (LINE.size() == 1)
            {
                return true;
            }
            else
            {
                if (0 == LINE.find(commentStr_))
                {
                    return true;
                }
            }
        }

        return false;
    }


    const std::string ConfigBase::GetFileNameStr() const
    {
        return filePathStr_;
    }


    const std::string ConfigBase::GetFileNameFullPathStr() const
    {
        return (GetFullPath(filePathStr_)).string();
    }


    const std::string ConfigBase::GetSeparatorStr() const
    {
        return sepStr_;
    }


    const std::string ConfigBase::GetCommentStr() const
    {
        return commentStr_;
    }

}//end of namespace appbase
