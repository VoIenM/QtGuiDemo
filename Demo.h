#ifndef _DEMO_H
#define _DEMO_H

class device_coding_data
{
	public:
	
	// TODO: getters and setters
	
	private:
	char continent;
	unsigned char country;
	char tank_size;
	bool light_indication;
	bool sound_indication;
	bool running_lights;
	bool comfort_lock;
};


class Demo
{
	public:
	Demo();
	~Demo();
	void ExportToJson();
	void ImportCoding(QString filename, unsigned int *result);
	bool WriteCodingToJson();
	bool LoadConfigFile();
	bool LoadJsonFile();
	void LoadCodingData();
	
	private:
	bool view_coding_data;
	device_coding_data dcd;
	QString DeviceFile;
	QString file_name;
	QLineEdit* user_input;
	QTextEdit* file_viewer;
	QTextEdit* export_file_viewer;
	QComboBox* dropdown_accessor;
}


#endif //_DEMO_H
