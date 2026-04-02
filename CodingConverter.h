#ifndef _CODING_CONVERTER_H
#define _CODING_CONVERTER_H

class CodingConverter
{
	public:
    static unsigned int encode(const DeviceCodingData& d);
    static DeviceCodingData decode(unsigned int value);
};

#endif //_CODING_CONVERTER_H
