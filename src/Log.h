#ifndef LOG_H
#define LOG_H

#include <string>
#include <fstream>

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

namespace log {
    enum severityLevel {
        periodic,
        trace,
        startup,
        teardown,
        info,
        error,
        debug
    };

    BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severityLevel);

    inline std::ostream& operator<<(std::ostream& stream, severityLevel level)
    {
        static const char* strings[] =
        {
            "periodic",
            "   trace",
            " startup",
            "teardown",
            "    info",
            "   error",
            "   debug"
        };
        if (level >= 0 && level < 7) {
            stream << strings[level];
        } else {
            stream << (int)(level);
        }
        return stream;
    }

    inline void initializeLog(std::string filename = "") {
        logging::add_common_attributes();

        typedef logging::sinks::synchronous_sink<logging::sinks::text_ostream_backend> text_sink;
        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

        boost::shared_ptr<std::ostream> stream(&std::clog, boost::empty_deleter());
        sink->locked_backend()->add_stream(stream);

        if (filename != "") {
            boost::shared_ptr<std::ostream> fileStream(new std::ofstream(filename));
            sink->locked_backend()->add_stream(fileStream);
        }

        logging::formatter format = logging::expressions::stream <<
            "[" << logging::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%m-%d-%Y %H:%M:%S") << "]" <<
            "[" << severity << "] " <<
            logging::expressions::smessage;
        sink->set_formatter(format);

        logging::core::get()->add_sink(sink);
    }

    inline void setLogFilter(severityLevel level) {
        logging::core::get()->set_filter(severity >= level);
    }
}

#define LOG(x,y) BOOST_LOG_SEV(x,y)

#endif //LOG_H
