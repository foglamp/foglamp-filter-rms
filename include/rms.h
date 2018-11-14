/*
 * FogLAMP "RMS" filter plugin.
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch
 */
#include <filter.h>
#include <reading_set.h>
#include <string>
#include <map>

class RMSFilter : public FogLampFilter {
	public:
		RMSFilter(const std::string& filterName,
			ConfigCategory& filterConfig,
			OUTPUT_HANDLE *outHandle,
			OUTPUT_STREAM out);
		void	setSampleSize(unsigned int size)
		{
			m_sampleSize = size;
		};
		void	addValue(const std::string& name, long value);
		void	addValue(const std::string& name, double value);
		void	outputData(ReadingSet *);
		bool	sendRawData();
	private:
		class RMSData {
			public:
				RMSData() : samples(0), cumulative(0.0)
				{
				};
				unsigned int samples;
				double	      cumulative;
		};
		unsigned int	m_sampleSize;
		bool		m_sendRawData;
		std::string	m_assetName;
		std::map<std::string, RMSData *>
				m_values;
};
