/*!
 * (C) 2009 Andrey Semashev
 *
 * Use, modification and distribution is subject to the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 * \file   trivial.cpp
 * \author Andrey Semashev
 * \date   07.11.2009
 *
 * \brief  This header is the Boost.Log library implementation, see the library documentation
 *         at http://www.boost.org/libs/log/doc/log.html.
 */

#include <boost/log/detail/prologue.hpp>

#if defined(BOOST_LOG_USE_CHAR)

#include <stdio.h> // FILENAME_MAX
#include <ios>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/formatters/stream.hpp>
#include <boost/log/formatters/attr.hpp>
#include <boost/log/formatters/date_time.hpp>
#include <boost/log/formatters/message.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/detail/universal_path.hpp>

#if !defined(BOOST_LOG_NO_THREADS)
#include <boost/log/sinks/sync_frontend.hpp>
#else
#include <boost/log/sinks/unlocked_frontend.hpp>
#endif

#if defined(BOOST_WINDOWS)
#include "windows_version.hpp"
#include <windows.h>
#elif defined(BOOST_HAS_UNISTD_H)
#include <unistd.h> // readlink
#endif

namespace boost {

namespace BOOST_LOG_NAMESPACE {

namespace trivial {

namespace aux {

namespace {

    // By default we try to compose the log file name based on the application executable file name

#if defined(BOOST_WINDOWS)

    log::aux::universal_path suggest_file_name()
    {
#if !defined(BOOST_FILESYSTEM_NARROW_ONLY)
        const wchar_t ext[] = L".log";
        wchar_t buf[FILENAME_MAX];
        std::size_t len = GetModuleFileNameW(NULL, buf, sizeof(buf) / sizeof(*buf));
#else
        const char ext[] = ".log";
        char buf[FILENAME_MAX];
        std::size_t len = GetModuleFileNameA(NULL, buf, sizeof(buf) / sizeof(*buf));
#endif
        if (len > 0)
        {
            // Extract the file name from the full path and replace extension with .log
            return log::aux::universal_path(
                filesystem::basename(log::aux::to_universal_path(buf)) + ext);
        }
        else
        {
            return log::aux::to_universal_path("application.log");
        }
    }

#else // defined(BOOST_WINDOWS)

    log::aux::universal_path suggest_file_name()
    {
#if defined(BOOST_HAS_UNISTD_H)

        // Attempt to read the executable name from the proc filesystem
        char buf[FILENAME_MAX];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf));
        if (len > 0)
        {
            std::string full_name(buf, buf + len);

            // Extract the file name from the full path and replace extension with .log
            return log::aux::universal_path(
                filesystem::basename(log::aux::to_universal_path(full_name)) + ".log");
        }

#endif // defined(BOOST_HAS_UNISTD_H)

        return log::aux::to_universal_path("application.log");
    }

#endif // defined(BOOST_WINDOWS)

} // namespace

//! Initialization routine
BOOST_LOG_EXPORT void init()
{
    log::add_common_attributes< char >();

#if !defined(BOOST_LOG_NO_THREADS)
    typedef sinks::synchronous_sink< sinks::text_file_backend > sink_type;
#else
    typedef sinks::unlocked_sink< sinks::text_file_backend > sink_type;
#endif

    shared_ptr< sink_type > sink = boost::make_shared< sink_type >(
        keywords::file_name = suggest_file_name(),
        keywords::open_mode = (std::ios::out | std::ios::app),
        keywords::auto_flush = true
    );

    typedef log::aux::add_common_attributes_constants< char > traits_t;
    sink->locked_backend()->set_formatter(
        formatters::stream
            << formatters::attr< unsigned int >(traits_t::line_id_attr_name())
            << " [" << formatters::date_time< posix_time::ptime >(traits_t::time_stamp_attr_name())
#if !defined(BOOST_LOG_NO_THREADS)
            << "] [" << formatters::attr< attributes::current_thread_id::held_type >(traits_t::thread_id_attr_name())
#endif
            << "] [" << formatters::attr< severity_level >(sources::aux::severity_attribute_name< char >::get())
            << "] " << formatters::message< char >()
    );

    log::core::get()->add_sink(sink);
}

} // namespace aux

} // namespace trivial

} // namespace log

} // namespace boost

#endif // defined(BOOST_LOG_USE_CHAR)
