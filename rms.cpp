/*
 * FogLAMP "RMS" filter plugin.
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch
 */
#include <reading.h>
#include <reading_set.h>
#include <utility>
#include <logger.h>
#include <rms.h>

using namespace std;

RMSFilter::RMSFilter(const std::string& filterName,
		     ConfigCategory& filterConfig,
		     OUTPUT_HANDLE *outHandle,
		     OUTPUT_STREAM out) :
				FogLampFilter(filterName, filterConfig,
						outHandle, out)
{
	if (filterConfig.itemExists("asset"))
	{
		m_assetName = filterConfig.getValue("asset");
	}
	else
	{
		m_assetName = "RMS";
	}
	if (filterConfig.itemExists("samples"))
	{
		m_sampleSize = atoi(filterConfig.getValue("samples").c_str());
	}
	else
	{
		m_sampleSize = 10;
	}
	if (filterConfig.itemExists("rawData"))
	{
		m_sendRawData = *(filterConfig.getValue("rawData").c_str()) == 't' ? true : false;
	}
	else
	{
		m_sendRawData = false;
	}
}
/**
 * Add a sample value to the RMS cumulative values
 */
void
RMSFilter::addValue(const string& name, long value)
{
double	dvalue = (double)value;

	addValue(name, dvalue);
}

/**
 * Add a sample value to the RMS cumulative values
 */
void
RMSFilter::addValue(const string& name, double value)
{
map<std::string, RMSData *>::iterator it;

	if ((it = m_values.find(name)) == m_values.end())
	{
		m_values.insert(pair<string, RMSData *>(name, new RMSData));
		it = m_values.find(name);
	}
	it->second->samples++;
	it->second->cumulative += (value * value);
}

/**
 * Called to output any RMS values that can be sent.
 * We only output values when we get the number defined as
 * the sample size.
 */
void
RMSFilter::outputData(ReadingSet *readingSet)
{
vector<Datapoint *>	dataPoints;

	for (auto it = m_values.cbegin(); it != m_values.cend(); it++)
	{
		if (it->second->samples >= m_sampleSize)
		{
			double value = it->second->cumulative / m_sampleSize;
			value = sqrt(value);
			it->second->cumulative = 0.0;
			it->second->samples = 0;
			DatapointValue	dpv(value);
			dataPoints.push_back(new Datapoint(it->first, dpv));
		
			it->first.c_str(), value);
		}
	}
	if (dataPoints.size() > 0)
	{
		vector<Reading *> readingVec;
		readingVec.push_back(new Reading(m_assetName, dataPoints));
		readingSet->append(readingVec);
		
	}
}

/**
 * Indicates if the raw inut data should be output in
 * addition to the RMS values.
 */
bool
RMSFilter::sendRawData()
{
	return m_sendRawData;
}
