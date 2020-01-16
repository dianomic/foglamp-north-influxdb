/*
 * FogLAMP Influxdb north plugin.
 *
 * Copyright (c) 2020 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch
 */
#include <plugin_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <logger.h>
#include <plugin_exception.h>
#include <iostream>
#include <influxdb.h>
#include <simple_https.h>
#include <config_category.h>
#include <storage_client.h>
#include <rapidjson/document.h>
#include <version.h>
#include <InfluxDB.h>
#include <InfluxDBFactory.h>
#include <Point.h>

using namespace std;
using namespace rapidjson;
using namespace influxdb;

#define PLUGIN_NAME "influxdb"

/**
 * Plugin specific default configuration
 */
static const char *default_config = QUOTE({
		"plugin": {
			"description": "Send readings to a Influxdb server",
			"type": "string",
			"default": PLUGIN_NAME,
			"readonly": "true"
			},
		"host": {
			"description": "The hostname of the InfluxDB server",
			"type": "string",
			"default": "influxdb.local",
			"order": "1",
			"displayName": "Host"
			},
		"port": { 
			"description": "The port for this InfluxDB server",
			"type": "integer",
			"default": "8086",
			"order": "2",
			"displayName": "Port"
			},
		"database": {
			"description": "The InfluxDB database name",
			"type": "string",
			"default": "foglamp",
			"order": "3",
			"displayName": "Database"
			},
		"username": {
			"description": "The InfluxDB database user name",
			"type": "string",
			"default": "",
			"order": "4",
			"displayName": "Username"
			},
		"password": {
			"description": "The InfluxDB database user password",
			"type": "password",
			"default": "",
			"order": "5",
			"displayName": "Password"
			},
		"source": {
			"description": "Defines the source of the data to be sent on the stream",
			"type": "enumeration",
			"default": "readings",
			"options": ["readings", "statistics"],
		       	"order": "6",
			"displayName": "Source"
			}
	});


/**
 * The Influxdb plugin interface
 */
extern "C" {

/**
 * The C API plugin information structure
 */
static PLUGIN_INFORMATION info = {
	PLUGIN_NAME,			// Name
	VERSION,			// Version
	0,				// Flags
	PLUGIN_TYPE_NORTH,		// Type
	"1.0.0",			// Interface version
	default_config			// Configuration
};

/**
 * Return the information about this plugin
 */
PLUGIN_INFORMATION *plugin_info()
{
	return &info;
}

/**
 * Initialise the plugin with configuration.
 *
 * This function is called to get the plugin handle.
 */
PLUGIN_HANDLE plugin_init(ConfigCategory* configData)
{
	InfluxDBPlugin *influxdb = new InfluxDBPlugin();

	if (!configData->itemExists("host"))
	{
		Logger::getLogger()->fatal("Influxdb plugin must have a host defined for the Influxdb");
		throw exception();
	}
	influxdb->setHost(configData->getValue("host"));
	if (!configData->itemExists("port"))
	{
		Logger::getLogger()->fatal("Influxdb plugin must have a port defined");
		throw exception();
	}
	influxdb->setPort(configData->getValue("port"));
	if (configData->itemExists("database"))
	{
		influxdb->setDB(configData->getValue("database"));
	}
	if (configData->itemExists("username"))
	{
		influxdb->setUsername(configData->getValue("username"));
	}
	if (configData->itemExists("password"))
	{
		influxdb->setPassword(configData->getValue("password"));
	}

	return (PLUGIN_HANDLE)influxdb;
}

/**
 * Send Readings data to historian server
 */
uint32_t plugin_send(const PLUGIN_HANDLE handle,
		     const vector<Reading *>& readings)
{
InfluxDBPlugin	*influxdb = (InfluxDBPlugin *)handle;

	return influxdb->send(readings);
}

/**
 * Shutdown the plugin
 *
 * Delete allocated data
 *
 * @param handle    The plugin handle
 */
void plugin_shutdown(PLUGIN_HANDLE handle)
{
InfluxDBPlugin	*influxdb = (InfluxDBPlugin *)handle;

        delete influxdb;
}

// End of extern "C"
}
