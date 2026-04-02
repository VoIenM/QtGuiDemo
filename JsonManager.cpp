#include "JsonManager.h"

QJsonObject JsonManager::loadJson(const QString& filename)
{
	try
	{
		QFile file(filename);
		
		if (!file.open(QIODevice::ReadOnly))
			throw std::runtime_error("Failed to open file: " + filename.toStdString());
		
		QByteArray data = file.readAll();
		file.close();
		
		QJsonParseError err;
		QJsonDocument doc = QJsonDocument::fromJson(data, &err);
		
		if (err.error != QJsonParseError::NoError)
			throw std::runtime_error("JSON parse error: " + err.errorString().toStdString());
		
		if (!doc.isObject())
			throw std::runtime_error("JSON is not an object");
		
		return doc.object();
	}
	catch (const std::exception& e)
	{
		qWarning() << "JsonManager::loadJson error:" << e.what();
		return {};
	}
}

void JsonManager::saveJson(const QString& filename, const QJsonObject& obj)
{
	try
	{
		QFile file(filename);
		
		if (!file.open(QIODevice::WriteOnly))
			throw std::runtime_error("Failed to open file for writing");
		
		QJsonDocument doc(obj);
		file.write(doc.toJson(QJsonDocument::Indented));
		file.close();
	}
	catch (const std::exception& e)
	{
		qWarning() << "JsonManager::saveJson error:" << e.what();
	}
}