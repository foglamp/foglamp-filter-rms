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

/**
 * Constructor for the RMS Filter class
 *
 * The filter is called with each number data point/reading and
 * will calculate the RMS (Root Mean Squared) value of each data point
 * in the reading over a specified number of samples.
 *
 * The RMS value is calculated by taking a cumulative squared value from
 * each reading up to the point that enough readings have been taken.
 * Once enough readings have been taken the cumulative value is divided
 * by the sample count to get the RMS value. This RMS value is passed up
 * the filter chain as a new reading. The cumulative value is reset to
 * zero and the process starts again for the next RMS value.
 *
 * The filter can also optionally pass the raw data along the filter
 * pipeline if it has been configured to send raw data.
 */
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
		m_sampleSize = strtol(filterConfig.getValue("samples").c_str(), NULL, 10);
	}
	else
	{
		m_sampleSize = 10;
	}
	if (filterConfig.itemExists("rawData"))
	{
		m_sendRawData = filterConfig.getValue("rawData").compare("true") == 0 ? true : false;
	}
	else
	{
		m_sendRawData = false;
	}
	if (filterConfig.itemExists("peak"))
	{
		m_sendPeak = filterConfig.getValue("peak").compare("true") == 0 ? true : false;
	}
	else
	{
		m_sendPeak = false;
	}
}

/**
 * Add a sample value to the RMS cumulative values
 * @param name	The name of the value, i.e. the datapoint
 * @param value	The value to add
 */
void
RMSFilter::addValue(const string& asset, const string& dpname, long value)
{
double	dvalue = (double)value;

	addValue(asset, dpname, dvalue);
}

/**
 * Add a sample value to the RMS cumulative values
 * @param name	The name of the value, i.e. the datapoint
 * @param value	The value to add
 */
void
RMSFilter::addValue(const string& asset, const string& dpname, double value)
{
map<pair<string, string>, RMSData *>::iterator it;
pair<string, string>	key = make_pair(asset, dpname);

	if ((it = m_values.find(key)) == m_values.end())
	{
		m_values.insert(pair<pair<string, string>, RMSData *>(key, new RMSData));
		it = m_values.find(key);
		it->second->peak_max = value;
		it->second->peak_min = value;
	}
	it->second->cumulative += (value * value);
	if (it->second->samples == 0 || it->second->peak_max < value)
		it->second->peak_max = value;
	if (it->second->samples == 0 || it->second->peak_min > value)
		it->second->peak_min = value;
	it->second->samples++;
}

/**
 * Called to output any RMS values that can be sent.
 * We only output values when we get the number defined as
 * the sample size.
 *
 * The output mechanism involves appending RMS values to a
 * ReadingSet instance. Not every call will result in new RMS
 * values being appended to the ReadingSet.
 *
 * @param readingSet	A reading set to which any RMS values are appened
 */
void
RMSFilter::outputData(ReadingSet& readingSet)
{
vector<Datapoint *>	dataPoints;
map<string, Reading *>	readings;

	for (auto it = m_values.cbegin(); it != m_values.cend(); it++)
	{
		if (it->second->samples >= m_sampleSize)
		{
			double value = it->second->cumulative / m_sampleSize;
			value = sqrt(value);
			it->second->cumulative = 0.0;
			it->second->samples = 0;
			DatapointValue	dpv(value);
			DatapointValue  peak(it->second->peak_max - it->second->peak_min);

			string assetName = m_assetName;
			if (assetName.find("%%a") != string::npos)
			{
				assetName.replace(assetName.find("%%a"), 2, it->first.first);
			}
			
			map<string, Reading *>::iterator ait = readings.find(it->first.first);
			if (ait != readings.end())
			{
				ait->second->addDatapoint(new Datapoint(it->first.second, dpv));
				if (m_sendPeak)
				{
					ait->second->addDatapoint(new Datapoint(it->first.second + "peak", peak));
				}
			}
			else
			{
				Reading *tmpReading = new Reading(assetName, new Datapoint(it->first.second, dpv));
				tmpReading->addDatapoint(new Datapoint(it->first.second + "peak", peak));
				if (m_sendPeak)
				{
					readings.insert(pair<string, Reading *>(it->first.first, tmpReading));
				}
			}
		}
	}

	vector<Reading *> readingVec;
	for (auto it = readings.cbegin(); it != readings.cend(); it++)
	{
		readingVec.push_back(it->second);
	}
	if (readingVec.size() > 0)
	{
		readingSet.append(readingVec);
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
