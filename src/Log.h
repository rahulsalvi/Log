#ifndef LOG_H
#define LOG_H

#ifdef LOGGING_ENABLED

#define PERIODIC 0
#define TRACE    1
#define STARTUP  2
#define TEARDOWN 3
#define INFO     4
#define ERROR    5
#define DEBUG    6

#define INIT_LOG logger::init();

#define SEVERITY logger::severity
#define LEVEL(x) logger::Severity(x)

#define ADD_CONSOLE_LOG             logger::addConsoleLog();
#define ADD_FILTERED_CONSOLE_LOG(x) logger::addConsoleLog(x);

#define ADD_FILE_LOG(x)             logger::addFileLog(x);
#define ADD_FILTERED_FILE_LOG(x,y)  logger::addFileLog(x,y);

#define ADD_LOG_LEVEL(x,y)          logger::addLevel(x,y);
#define ADD_COMMON_LOG_LEVELS       logger::addCommonLevels();

#define GLOBAL_FILTER(x)            logger::setGlobalFilter(x);
#define RESET_GLOBAL_FILTER         logger::resetGlobalFilter();

#define LOG(x) BOOST_LOG_SEV(logger::logSource,logger::Severity(x))

#include <fstream>
#include <stdexcept>
#include <string>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/unordered_map.hpp>
#include <boost/utility/empty_deleter.hpp>

namespace logging = boost::log;
namespace expr = boost::log::expressions;

namespace logger {
    //Containers for holding int to string and vice-versa log level mapping
    static boost::unordered::unordered_map<int, std::string> levels;
    static boost::unordered::unordered_map<std::string, int> levelsReversed;

    //A type for determining levels and printing the level correctly
    struct Severity {
        Severity(int level) : level(level) {}
        Severity(std::string s) {
            try {
                level = levelsReversed.at(s);
            } catch (const std::out_of_range) {
                level = -1;
            }
        }
        int level;
    };

    //Override << operator to print the level as a string
    inline std::ostream& operator << (std::ostream& stream, const Severity& sev) {
        std::string label;
        try {
            label = levels.at(sev.level);
        } catch (const std::out_of_range) {
            label = std::to_string(sev.level);
        }
        stream << label;
        return stream;
    }

    //Override comparison operators for filters
    inline bool operator == (const Severity& s1, const Severity& s2) {
        return (s1.level == s2.level);
    }

    inline bool operator != (const Severity& s1, const Severity& s2) {
        return (s1.level != s2.level);
    }

    inline bool operator < (const Severity& s1, const Severity& s2) {
        return (s1.level < s2.level);
    }

    inline bool operator > (const Severity& s1, const Severity& s2) {
        return (s1.level > s2.level);
    }

    inline bool operator >= (const Severity& s1, const Severity& s2) {
        return (s1.level >= s2.level);
    }

    inline bool operator <= (const Severity& s1, const Severity& s2) {
        return (s1.level <= s2.level);
    }

    //Register as an attribute
    BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", Severity);

    //Add a custom level
    inline void addLevel(int level, std::string label) {
        levels.insert(std::make_pair(level, label));
        levelsReversed.insert(std::make_pair(label, level));
    }

    //Add common levels instead of defining custom ones
    inline void addCommonLevels() {
        addLevel(-1,             " unknown");
        addLevel(PERIODIC, "periodic");
        addLevel(TRACE,    "   trace");
        addLevel(STARTUP,  " startup");
        addLevel(TEARDOWN, "teardown");
        addLevel(INFO,     "    info");
        addLevel(ERROR,    "   error");
        addLevel(DEBUG,    "   debug");
    }

    //Global format specifier
    logging::formatter format = expr::stream
        << "[" << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%m-%d-%Y %H:%M:%S.%f") << "]"
        << "[" << severity << "] "
        << expr::smessage;

    //Required to make TimeStamp work currently
    inline void init() {
        logging::add_common_attributes();
    }

    //Call to print logs to console
    inline void addConsoleLog() {
        typedef logging::sinks::synchronous_sink<logging::sinks::text_ostream_backend> text_sink;
        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

        boost::shared_ptr<std::ostream> stream(&std::clog, boost::empty_deleter());
        sink->locked_backend()->add_stream(stream);
        sink->set_formatter(format);

        logging::core::get()->add_sink(sink);
    }

    inline void addConsoleLog(const logging::filter& filter) {
        typedef logging::sinks::synchronous_sink<logging::sinks::text_ostream_backend> text_sink;
        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

        boost::shared_ptr<std::ostream> stream(&std::clog, boost::empty_deleter());
        sink->locked_backend()->add_stream(stream);
        sink->set_formatter(format);
        sink->set_filter(filter);

        logging::core::get()->add_sink(sink);
    }

    //Call to create a file log
    inline void addFileLog(std::string filename) {
        typedef logging::sinks::synchronous_sink<logging::sinks::text_ostream_backend> text_sink;
        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

        boost::shared_ptr<std::ostream> fileStream(new std::ofstream(filename));
        sink->locked_backend()->add_stream(fileStream);
        sink->set_formatter(format);

        logging::core::get()->add_sink(sink);
    }

    inline void addFileLog(std::string filename, const logging::filter& filter) {
        typedef logging::sinks::synchronous_sink<logging::sinks::text_ostream_backend> text_sink;
        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

        boost::shared_ptr<std::ostream> fileStream(new std::ofstream(filename));
        sink->locked_backend()->add_stream(fileStream);
        sink->set_formatter(format);
        sink->set_filter(filter);

        logging::core::get()->add_sink(sink);
    }

    //Set log filter to be greater than some value
    inline void setGlobalFilter(const logging::filter& filter) {
        logging::core::get()->set_filter(filter);
    }

    inline void resetGlobalFilter() {
        logging::core::get()->reset_filter();
    }

    //Create a log source
    static logging::sources::severity_logger_mt<logger::Severity> logSource;
}

#else

#include <ostream>

#define INIT_LOG

#define ADD_CONSOLE_LOG
#define ADD_FILTERED_CONSOLE_LOG(x)

#define ADD_FILE_LOG(x)
#define ADD_FILTERED_FILE_LOG(x,y)

#define ADD_LOG_LEVEL(x,y)
#define ADD_COMMON_LOG_LEVELS

#define GLOBAL_FILTER(x)
#define RESET_GLOBAL_FILTER

#define LOG(x) std::ostream(0)

#endif

#endif //LOG_H
