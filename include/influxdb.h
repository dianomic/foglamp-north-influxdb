#ifndef _INFLUXDB_H
#define _INFLUXDB_H
/*
 * FogLAMP InfluxDB north plugin.
 *
 * Copyright (c) 2019 Dianomic Systems
 *      
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch
 */
#include <string>
#include <variant>
#include <reading.h>
#include <strings.h>
#include <InfluxDBFactory.h>

class InfluxDBPlugin
{
	public:
		InfluxDBPlugin();
		~InfluxDBPlugin();
		uint32_t	send(const std::vector<Reading *> readings);
		void		setHost(const std::string& host) { m_host = host; };
		void		setPort(const std::string& port) { m_port = port; };
		void		setUsername(const std::string& username) { m_username = username; };
		void		setPassword(const std::string& password) { m_password = password; };
		void		setDB(const std::string& database) { m_db = database; };
	private:
		std::string		getURL();
		std::unique_ptr<influxdb::InfluxDB>
					m_influxdb;
		bool			m_connected;
		std::string		m_host;
		std::string		m_port;
		std::string		m_username;
		std::string		m_password;
		std::string		m_db;
};
#endif
