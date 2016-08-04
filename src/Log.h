#ifndef LOG_H
#define LOG_H

#ifdef LOGGING_ENABLED

#include <fstream>
#include <stdexcept>
#include <string>

#include <boost/unordered_map.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/utility/empty_deleter.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

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
    BOOST_LOG_ATTRIBUTE_KEYWORD(attrSeverity, "Severity", Severity);

    //Add a custom level
    inline void addLevel(int level, std::string label) {
        levels.insert(std::make_pair(level, label));
        levelsReversed.insert(std::make_pair(label, level));
    }

    //Global format specifier
    logging::formatter format = expr::stream
        << "[" << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%m-%d-%Y %H:%M:%S.%f") << "]"
        << "[" << attrSeverity << "] "
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

    //Call to create a file log
    inline void addFileLog(std::string filename) {
        typedef logging::sinks::synchronous_sink<logging::sinks::text_ostream_backend> text_sink;
        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

        boost::shared_ptr<std::ostream> fileStream(new std::ofstream(filename));
        sink->locked_backend()->add_stream(fileStream);
        sink->set_formatter(format);

        logging::core::get()->add_sink(sink);
    }

    //Add common levels instead of defining custom ones
    inline void addCommonLevels() {
        addLevel(-1, " unknown");
        addLevel(0,  "periodic");
        addLevel(1,  "   trace");
        addLevel(2,  " startup");
        addLevel(3,  "teardown");
        addLevel(4,  "    info");
        addLevel(5,  "   error");
        addLevel(6,  "   debug");
    }

    //Set log filter to be greater than some value
    inline void setLogFilter(Severity sev) {
        logging::core::get()->set_filter(attrSeverity >= sev);
    }

    //Create a log source
    static logging::sources::severity_logger_mt<logger::Severity> logSource;
}

#define INIT_LOG logger::init();
#define ADD_CONSOLE_LOG logger::addConsoleLog();
#define ADD_FILE_LOG(x) logger::addFileLog(x);

#define ADD_LOG_LEVEL(x,y) logger::addLevel(x,y);
#define ADD_COMMON_LOG_LEVELS logger::addCommonLevels();

#define SEVERITY_FILTER(x)       logger::setLogFilter(logger::Severity(x));
#define SEVERITY_FILTER_PERIODIC logger::setLogFilter(logger::Severity(0));
#define SEVERITY_FILTER_TRACE    logger::setLogFilter(logger::Severity(1));
#define SEVERITY_FILTER_STARTUP  logger::setLogFilter(logger::Severity(2));
#define SEVERITY_FILTER_TEARDOWN logger::setLogFilter(logger::Severity(3));
#define SEVERITY_FILTER_INFO     logger::setLogFilter(logger::Severity(4));
#define SEVERITY_FILTER_ERROR    logger::setLogFilter(logger::Severity(5));
#define SEVERITY_FILTER_DEBUG    logger::setLogFilter(logger::Severity(6));

#define LOG(x)       BOOST_LOG_SEV(logger::logSource,logger::Severity(x))
#define LOG_PERIODIC BOOST_LOG_SEV(logger::logSource,logger::Severity(0))
#define LOG_TRACE    BOOST_LOG_SEV(logger::logSource,logger::Severity(1))
#define LOG_STARTUP  BOOST_LOG_SEV(logger::logSource,logger::Severity(2))
#define LOG_TEARDOWN BOOST_LOG_SEV(logger::logSource,logger::Severity(3))
#define LOG_INFO     BOOST_LOG_SEV(logger::logSource,logger::Severity(4))
#define LOG_ERROR    BOOST_LOG_SEV(logger::logSource,logger::Severity(5))
#define LOG_DEGUG    BOOST_LOG_SEV(logger::logSource,logger::Severity(6))

#else

#include <ostream>

#define INIT_LOG
#define ADD_CONSOLE_LOG
#define ADD_FILE_LOG(x)

#define ADD_LOG_LEVEL(x,y)
#define ADD_COMMON_LOG_LEVELS

#define SEVERITY_FILTER(x)
#define SEVERITY_FILTER_PERIODIC
#define SEVERITY_FILTER_TRACE
#define SEVERITY_FILTER_STARTUP
#define SEVERITY_FILTER_TEARDOWN
#define SEVERITY_FILTER_INFO
#define SEVERITY_FILTER_ERROR
#define SEVERITY_FILTER_DEBUG

#define LOG(x)       std::ostream(0)
#define LOG_PERIODIC std::ostream(0)
#define LOG_TRACE    std::ostream(0)
#define LOG_STARTUP  std::ostream(0)
#define LOG_TEARDOWN std::ostream(0)
#define LOG_INFO     std::ostream(0)
#define LOG_ERROR    std::ostream(0)
#define LOG_DEGUG    std::ostream(0)

#endif

#endif //LOG_H
