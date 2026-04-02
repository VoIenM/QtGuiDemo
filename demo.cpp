// TODO: Add JsonFileToCoding. Convert a JsonFile to a byte sequence
// TODO: Dont forget to add a mapping between the country codes and some example countries that belong to different continents but have the same bit sequence

// NOTE: (0,0) top left, (inf, inf) bottom right i.e y++: go down, x++: go right
// NOTE: MASKS for 16bit wide coding data value
//	0000 0000 0000 0111 = continent
//	0000 0000 1111 0000 = country
//	0000 0000 0000 1000 = running_lights
//	0000 0001 0000 0000 = comfort_lock
//	0000 0010 0000 0000 = light_indication
//	0000 0100 0000 0000 = sound_indication
//	0011 0000 0000 0000 = tank_size
// NOTE: We are exporting the modified text that is in the QTextEdit box to an "export.json" file
// 	  That later gets reloaded for the viewer to see the modified data

#include <stdlib.h>
#include <stdio.h>

// QT Framework
#include <QtGui>
#include <QApplication>
#include <QMainWindow>

#include <QFile>
#include <QDebug>
#include <QTextEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDialogButtonBox>
#include <QWidget>
#include <QLayout>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>

#include "Demo.h"

// Bit masks
#define CONTINENT_MASK 0x07
#define COUNTRY_MASK 0xF0
#define RUNNING_LIGHTS_MASK 0x08
#define COMFORT_LOCK_MASK 0x100
#define TANK_MASK 0x3000
#define LIGHT_INDICATION_MASK 0x200
#define SOUND_INDICATION_MASK 0x400

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 1024

void Demo::ExportToJson()
{
	QJsonObject output;
	QString plain_text = file_viewer->toPlainText();
	output["data"] = plain_text;
	export_file_viewer->setPlainText(plain_text);
	
	QFile file("export.json");
	
	if(file.open(QIODevice::WriteOnly))
	{
		QJsonDocument doc(output);
		QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
		file.write(jsonData);
		file.close();				
	}
}

// Supposed to convert a json file to a hexadecimal number
void Demo::ImportCoding(QString filename, unsigned int *result)
{
	QFile file(filename);
	QFile map_file("mapping.json");
	
    if(file.open(QIODevice::ReadOnly | QIODevice::Text) && 
	   map_file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QJsonParseError perr;
		QByteArray jsonData = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(jsonData, &perr);
		
		QByteArray json_map = map_file.readAll();
		QJsonDocument map_doc = QJsonDocument::fromJson(json_map, &perr);
		
		if(perr.error != QJsonParseError::NoError) 
		{
			return;
		}
		
		if(doc.isObject()) 
		{
			QJsonObject obj = doc.object();
			QJsonObject map_obj = map_doc.object();
			
			if(obj["continent"].isString() && 
			   obj["country_code"].isString() &&
			   obj["tank_size"].isString() && 
			   obj["comfort_lock"].isString() &&
			   obj["light_indication"].isString() && 
			   obj["sound_indication"].isString() &&
			   obj["running_lights"].isString())
			{
				QString continent_id = obj["continent"].toString();
				QString country_id   = obj["country_code"].toString();
				QString tank_id 	 = obj["tank_size"].toString();
				
				int continent_bits = 0;
				int tank_bits = 0;
				
				//NOTE: Now we need to look up into mapping.json to translate the strings to binary strings
				//NOTE: No point looking up the country code until we introduce a mapping between country codes and countries
				
				QJsonObject continent = map_obj["continentMapping"].toObject();
				QJsonObject country   = map_obj["country_code"].toObject();
				QJsonObject tank 	 = map_obj["tank_sizes"].toObject();
				
				QString key;
				QJsonValue val;
				
				bool check_state;
				
				for(auto it = continent.begin(); it != continent.end(); ++it) 
				{
					key = it.key();
					val = it.value();
					
					if(val.isString() && continent_id == key)
					{
						continent_id = val.toString();

						continent_bits = continent_id.toInt(&check_state, 2); 
						break;
					}
					else
					{
						continent_id = "N/A";
					}
				}
				
				for(auto it = tank.begin(); it != tank.end(); ++it) 
				{
					key = it.key();
					val = it.value();
					
					if(val.isString() && tank_id == key)
					{
						tank_id = val.toString();
						tank_bits = tank_id.toInt(&check_state, 2); 
						break;
					}
					else
					{
						tank_id = "N/A";
					}
				}
				
				bool comfort_lock 	 = false;
				bool light_indication  = false;
				bool sound_indication  = false;
				bool running_lights    = false;
				
				if(obj["comfort_lock"].toString() == "true")
				{
					comfort_lock = true;
				}
				if(obj["light_indication"].toString() == "true")
				{
					light_indication = true;
				}
				if(obj["sound_indication"].toString() == "true")
				{
					sound_indication = true;
				}
				if(obj["running_lights"].toString() == "true")
				{
					running_lights = true;
				}
				
				*result = (comfort_lock << 8) | (light_indication << 9) | (sound_indication << 10) | (sound_indication << 3) | continent_bits | (tank_bits << 12);
				QMessageBox::information(nullptr, "Coding Byte Sequence: ", QString::number(*result, 2));
			}
		}		
	}
}	

bool Demo::WriteCodingToJson()
{
    QFile file("mapping.json");
	
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) 
	{
		QJsonParseError perr;
		QByteArray jsonData = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(jsonData, &perr);
		
		if(perr.error == QJsonParseError::NoError) 
		{
			if (doc.isObject()) 
			{
				QJsonObject obj = doc.object();
				if(obj["continent_mapping"].isObject() &&
				   obj["country_code"].isObject() &&
				   obj["tank_sizes"].isObject())
				{
					QString device_continent = QString::number(dcd.continent, 2).rightJustified(3, '0');
					QString device_country   = QString::number(dcd.country, 2);
					QString device_tank_size = QString::number(dcd.tank_size, 2).rightJustified(2, '0');
					
					
					QJsonObject continent = obj["continentMapping"].toObject();
					QJsonObject country   = obj["country_code"].toObject();
					QJsonObject tank 	 = obj["tank_sizes"].toObject();
					
					QString key;
					QJsonValue val;
					
					for(auto it = continent.begin(); it != continent.end(); ++it) 
					{
						key = it.key();
						val = it.value();
						
						if(val.isString())
						{
							if(device_continent == val.toString())
							{
								device_continent = key;
								break;
							}
							else
							{
								device_continent = "xxx";
							}
						}
					}
					
					bool stop_loop = false;
					
					for(auto it = country.begin(); it != country.end(); ++it) 
					{
						if(device_continent == it.key())
						{
							QJsonArray codes = it.value().toArray(); // Continent
							if(stop_loop)
							{
								break;
							}
							for(const auto &v : codes)
							{
								QString code = v.toString();
								if(device_country == code)
								{

									device_country = code;
									stop_loop = true;
									break;
								}
								else
								{
									device_country = "xxx";
								}
							}
						}
					}
					
					for(auto it = tank.begin(); it != tank.end(); ++it) 
					{
						key = it.key();
						val = it.value();
						QString nval = val.toString();
						
						if(device_tank_size == nval)
						{
							device_tank_size = key;
							break;
						}
					}
					
					QJsonObject output;
					output["country_code"] = device_country;
					output["continent"]    = device_continent;
					output["tank_size"]	= device_tank_size;
					dcd.running_lights   ? output["running_lights"] = "true"   : output["running_lights"] = "false";
					dcd.comfort_lock 	? output["comfort_lock"] = "true" 	: output["comfort_lock"] = "false";
					dcd.light_indication ? output["light_indication"] = "true" : output["light_indication"] = "false";
					dcd.sound_indication ? output["sound_indication"] = "true" : output["sound_indication"] = "false";

					QFile file("output.json");
					if(file.open(QIODevice::WriteOnly))
					{
						QJsonDocument doc(output);
						QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
						file.write(jsonData);
						file.close();				
					}
				}
			}
			else
			{
				qWarning() << "JSON is not an object!";
				return false;			
			}
		}
		else
		{
			qWarning() << "JSON parse error:" << perr.errorString();
			return false;			
		}
		
		file.close();
        return true;
    }
	
	QMessageBox::information(nullptr, "", "Cannot open file!");
	
	return false;
}

bool Demo::LoadConfigFile()
{
    QFile file("mapping.json");
	
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) 
	{
		QJsonParseError perr;
		QByteArray jsonData = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(jsonData, &perr);
		
		if(perr.error == QJsonParseError::NoError) 
		{
			if(doc.isObject()) 
			{
				QString qt_string = doc.toJson(QJsonDocument::Indented);
				QJsonObject obj = doc.object();
				file_viewer->setPlainText(qt_string);
			}
			else
			{
				qWarning() << "JSON is not an object!";
				return false;			
			}
		}
		else
		{
			qWarning() << "JSON parse error:" << perr.errorString();
			return false;			
		}
		
		file.close();
        return true;
    }
	
	QMessageBox::information(nullptr, "", "Cannot open file!");
	
	return false;
}

bool Demo::LoadJsonFile()
{
	DeviceFile = dropdown_accessor->currentText();
	
	if(DeviceFile == "device_1")
	{
		DeviceFile = "data.json";
	}
	else if(DeviceFile == "device_2")
	{
		DeviceFile = "data_2.json";
	}
	else if(DeviceFile == "device_3")
	{
		DeviceFile = "data_3.json";
	}
	
	if(view_coding_data)
	{
		DeviceFile = "output.json";
		view_coding_data = false;
	}
	
    QFile file(DeviceFile);
	
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) 
	{
		QJsonParseError perr;
		QByteArray jsonData = file.readAll();
		QJsonDocument doc = QJsonDocument::fromJson(jsonData, &perr);

		if(perr.error == QJsonParseError::NoError) 
		{
			if (doc.isObject()) 
			{
				QString qt_string = doc.toJson(QJsonDocument::Indented);
				QJsonObject obj = doc.object();
				
				file_viewer->setPlainText(qt_string);
				file.close();
				return true;
			}
			else
			{
				qWarning() << "JSON is not an object!";
			}
		}
		else
		{
			qWarning() << "JSON parse error:" << perr.errorString();
		}
    }
	
	QMessageBox::information(nullptr, "", "Cannot open file!" + DeviceFile);
	
	return false;
}

void Demo::LoadCodingData()
{
	QString users_input_text = user_input->text();
	bool conversion_state = false;
	// NOTE: toInt(...) fails for values that requires 4 bytes to represent. Need to check if strings containing 0x are correctly interpreted as hex by toInt(...)
	int coding_data = user_input->text().toInt(&conversion_state, 16); 
	
	if(conversion_state)
	{
		dcd.continent 	   = coding_data & CONTINENT_MASK;
		dcd.running_lights   = coding_data & RUNNING_LIGHTS_MASK;
		dcd.country 		 = (coding_data & COUNTRY_MASK) >> 4;
		dcd.light_indication = (coding_data & LIGHT_INDICATION_MASK) >> 8;
		dcd.sound_indication = (coding_data & SOUND_INDICATION_MASK) >> 8;
		dcd.comfort_lock	 = (coding_data & COMFORT_LOCK_MASK) >> 8;
		dcd.tank_size 	   = (coding_data & TANK_MASK) >> 12;
		
		WriteCodingToJson();
		view_coding_data = true;
		LoadJsonFile();
	}
	else
	{
		QMessageBox::information(nullptr, "", "Conversion Failed");
	}
}



int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QMainWindow window;
	QWidget centralWidget(&window);
	
    window.resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    window.setWindowTitle("Simple Qt Window");
	window.setCentralWidget(&centralWidget);
	
	QComboBox dropdown(&window); 
	dropdown.addItem("device_1");
    dropdown.addItem("device_2");
    dropdown.addItem("device_3");
	
	dropdown_accessor = &dropdown;
	
	user_input = new QLineEdit(&window);
	user_input->move(0, dropdown.height());
	user_input->setFixedWidth(WINDOW_WIDTH / 4);
	user_input->setPlaceholderText("Enter Hex Coding (Max 2 bytes: e.g 0xFABF)");
	
	file_viewer = new QTextEdit(&centralWidget);
	file_viewer->setGeometry(user_input->width() + 1, 20, 200, 400);
	file_viewer->setReadOnly(false);
	
	QPushButton button("Load Mapping Data", &window);
	button.setFixedWidth(200);
	button.move(file_viewer->pos().x(), file_viewer->height() + 1);
	
	export_file_viewer = new QTextEdit(&centralWidget);
	export_file_viewer->setGeometry(file_viewer->pos().x() + file_viewer->width() + 1, 20, 200, 400);
	export_file_viewer->setReadOnly(true);
	
	QPushButton export_label("Exported Data", &window);
	export_label.setFixedWidth(200);
	export_label.move(export_file_viewer->pos().x(), export_file_viewer->height() + 1);
	
	QPushButton import_button("Import Coding Data", &window);
	import_button.setFixedWidth(200);
	import_button.move(20, file_viewer->height());
	
	unsigned int coding_byte_sequence = 0;
	
	// Events
	window.connect(user_input, &QLineEdit::returnPressed, &window, &Demo::LoadCodingData);
	window.connect(&dropdown, &QComboBox::currentTextChanged, &window, &Demo::LoadJsonFile);
	window.connect(&button, &QPushButton::clicked, &window, &Demo::LoadConfigFile);
	window.connect(file_viewer, &QTextEdit::textChanged, &window, &Demo::ExportToJson);
	window.connect(&import_button, &QPushButton::clicked, &window, [&]() {ImportCoding("data_2.json", &coding_byte_sequence);});
    window.show(); 
	
    return app.exec();
}