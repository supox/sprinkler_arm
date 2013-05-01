#include "Serialization.h"
#include "Storage.h"
#include "StringBuffer.h"
#include "SensorFactory.h"

#include <stddef.h>
#include <string.h>

namespace Serialization
{
	enum StorageFiles
	{
		eSensors,
		eSprinkler,
		eValves
	};
		
	#define SENSORS_IDENTIFIER "SENSORS"
	bool Save(Vector<SensorPtr>& sensors)
	{
		StringBuffer sb;
		const size_t sensors_size = sensors.size();
		sb.Write(SENSORS_IDENTIFIER, sizeof(SENSORS_IDENTIFIER));
		sb.Write((const char*)&sensors_size, sizeof(sensors_size));
		for(size_t sensor_index = 0 ; sensor_index < sensors_size ; sensor_index++)
		{
			SensorPtr& current_sensor = sensors[sensor_index];
			const int data[] = {current_sensor->id, current_sensor->port_index, current_sensor->GetLastReadingValue(), current_sensor->GetType()};
			sb.Write((char*) data, sizeof(data));
		}
		Storage::Write(eSensors * Storage::PageSize, (const unsigned char*)sb.GetBuffer(), sb.GetBufferSize());
		
		return true;
	}
	
	bool Load(Vector<SensorPtr>& sensors)
	{
		const unsigned char* data = Storage::Read(eSensors * Storage::PageSize);
		if(strncmp((const char*)data, SENSORS_IDENTIFIER, sizeof(SENSORS_IDENTIFIER)))
			return false;
		data += sizeof(SENSORS_IDENTIFIER);
		size_t sensors_size = *((const size_t*)data);
		if(sensors_size > 30)
			return false;
		data += sizeof(size_t);
		for(size_t sensor_index = 0 ; sensor_index < sensors_size ; sensor_index++)
		{
			int id = *((const int*)data);
			data+= sizeof(int);
			int port_index = *((const int*)data);
			data+= sizeof(int);
			int last_reading_value = *((const int*)data);
			data+= sizeof(int);
			int sensor_type = *((const int*)data);
			data+= sizeof(int);
			SensorPtr sensor(SensorFactory::CreateSensor((SensorType)sensor_type, id, port_index, last_reading_value, NULL));
			sensors.Add(sensor);
		}
		return true;
	}
	
	bool Update(Vector<SensorPtr>& sensors, size_t sensor_index_to_update)
	{
		return false;
	}
	
};
