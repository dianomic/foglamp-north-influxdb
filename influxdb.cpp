/*
 * FogLAMP InfluxDB north plugin.
 *
 * Copyright (c) 2020 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch
 */
#include <influxdb.h>
#include <unistd.h>
#include <logger.h>
#include <chrono>

using namespace	std;
using namespace	influxdb;

InfluxDBPlugin::InfluxDBPlugin() : m_connected(false)
{
}

InfluxDBPlugin::~InfluxDBPlugin()
{
}

/**
 * Send the readings to the InfluxDB channel
 *
 * @param readings	The Readings to send
 * @return	The number of readings sent
 */
uint32_t
InfluxDBPlugin::send(const vector<Reading *> readings)
{
ostringstream	payload;
int	sent = 0;

	try
	{
		if (!m_connected)
		{
			try {
				if ((m_influxdb = influxdb::InfluxDBFactory::Get(getURL())) == NULL)
				{
					Logger::getLogger()->fatal("Unable to connect to influxDB server");
					return 0;
				}
				m_connected = true;

			} catch (const runtime_error& re) {
				Logger::getLogger()->fatal("Failed to connect to influxdb: %s %s", getURL().c_str(), re.what());
				return 0;

			} catch (exception &e) {
				Logger::getLogger()->fatal("Failed to connect to influxdb: %s", e.what());
				return 0;
			}
			Logger::getLogger()->info("Connected to %s", getURL().c_str());
			m_influxdb->batchOf(100);
		}

		for (auto it = readings.cbegin(); it != readings.cend(); ++it)
		{
			string assetName = (*it)->getAssetName();
			auto point = Point(assetName);

			struct timeval tv;
			(*it)->getUserTimestamp(&tv);
			chrono::time_point<chrono::system_clock> timestamp;
			timestamp = chrono::system_clock::time_point{chrono::seconds{tv.tv_sec} + chrono::milliseconds{tv.tv_usec / 1000}};
			point.setTimestamp(timestamp);
			vector<Datapoint *> datapoints = (*it)->getReadingData();
			for (auto dit = datapoints.cbegin(); dit != datapoints.cend();
				 ++dit)
			{
				string name = (*dit)->getName();
				if ((*dit)->getData().getType() == DatapointValue::dataTagType::T_INTEGER)
				{
					// sending data as integers generates some errors
					//point.addField(name.c_str(), (int)((*dit)->getData().toInt()));
					point.addField(name.c_str(), (double)((*dit)->getData().toInt()));
				}
				else if ((*dit)->getData().getType() == DatapointValue::dataTagType::T_FLOAT)
				{
					point.addField(name.c_str(), (*dit)->getData().toDouble());
				}
				else
				{
					point.addField(name.c_str(), (*dit)->getData().toString().c_str());
				}

			}
			m_influxdb->write(forward<Point&&>(point));
			sent++;
		}
		m_influxdb->flushBuffer();

	}
	catch (const std::exception &e)
	{
		// the write operation raises an error only at the end of the block defined with batchOf
		sent = 0;

		Logger::getLogger()->error("Error while sending data %s" , e.what());
	}

	return sent;
}

string InfluxDBPlugin::getURL()
{
	string rval = "http://";

	if (m_username.compare(""))
	{
		rval += m_username;
		rval += ":";
		rval += m_password;
		rval += "@";
	}

	rval += m_host;
	rval += ":";
	rval += m_port;

	rval += "/?db=";
	rval += m_db;

	Logger::getLogger()->info("db is %s, URL %s", m_db.c_str(), rval.c_str());

	return rval;
}
