/**
 * Copyright (C) 2009-2013  Przemysław Dobrowolski
 *
 * This file is part of the Configuration Space Library (libcs), a library
 * for creating configuration spaces of various motion planning problems.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "qlog4cxx.h"
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/appenderskeleton.h>
#include <log4cxx/spi/loggingevent.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <cassert>

using namespace log4cxx;
using namespace log4cxx::helpers;

namespace // anonymous
{
QLog4cxx *g_qLog4cxx = 0;
} // namespace anonymous

typedef boost::function<void (QString, QString, long long, QString)> LogMessageProc;

class QtSignalAppender
    : public AppenderSkeleton
{
public:
    DECLARE_LOG4CXX_OBJECT(QtSignalAppender)
    BEGIN_LOG4CXX_CAST_MAP()
            LOG4CXX_CAST_ENTRY(QtSignalAppender)
            LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
    END_LOG4CXX_CAST_MAP()

    void setLogMessageProc(LogMessageProc logMessageProc)
    {
        m_logMessageProc = logMessageProc;
    }

    void append(const spi::LoggingEventPtr& event, log4cxx::helpers::Pool& p)
    {
        (void)p;

        m_logMessageProc(QString::fromUtf8(event->getLoggerName().c_str()),
                         QString::fromUtf8(event->getLevel()->toString().c_str()),
                         static_cast<long long>(event->getTimeStamp() - event->getStartTime()),
                         QString::fromUtf8(event->getMessage().c_str()));
    }

    void close()
    {
        if (closed)
            return;

        closed = true;
    }

    virtual bool requiresLayout() const
    {
        return false;
    }

private:
    LogMessageProc m_logMessageProc;
};

typedef helpers::ObjectPtrT<QtSignalAppender> QtSignalAppenderPtr;

IMPLEMENT_LOG4CXX_OBJECT(QtSignalAppender)

QLog4cxx::QLog4cxx(QObject *parent) :
    QObject(parent)
{
    // configure by property pages
    //log4cxx::PropertyConfigurator::configure("logger.conf");

    // configure internal appender
    QtSignalAppenderPtr appender(new QtSignalAppender());
    appender->setLogMessageProc(boost::bind(&QLog4cxx::internalLogMessage, this, _1, _2, _3, _4));
    log4cxx::BasicConfigurator::configure(appender);
}

QLog4cxx::~QLog4cxx()
{
    // remove logger
    log4cxx::BasicConfigurator::resetConfiguration();
}

void QLog4cxx::configure()
{
    assert(!g_qLog4cxx);
    g_qLog4cxx = new QLog4cxx();
}

void QLog4cxx::deconfigure()
{
    assert(g_qLog4cxx);
    delete g_qLog4cxx;
    g_qLog4cxx = 0;
}

QLog4cxx *QLog4cxx::instance()
{
    assert(g_qLog4cxx);
    return g_qLog4cxx;
}

void QLog4cxx::internalLogMessage(QString loggerName, QString level, long long deltaTimeMicroSec, QString message)
{
    emit logMessage(loggerName, level, deltaTimeMicroSec, message);
}
