#include <time.h>
#include <CodingCoverter.h>

unsigned int CodingConverter::encode(const DeviceCodingData& d)
{
	return (d.continent) |
	(d.country << 4) |
	(d.running_lights << 3) |
	(d.comfort_lock << 8) |
	(d.light_indication << 9) |
	(d.sound_indication << 10) |
	(d.tank_size << 12);
}

DeviceCodingData CodingConverter::decode(unsigned int value)
{
	DeviceCodingData d;
	
	d.continent        = value & 0x07;
	d.country          = (value & 0xF0) >> 4;
	d.running_lights   = value & 0x08;
	d.comfort_lock     = (value & 0x100) >> 8;
	d.light_indication = (value & 0x200) >> 9;
	d.sound_indication = (value & 0x400) >> 10;
	d.tank_size        = (value & 0x3000) >> 12;
	
	return d;
}