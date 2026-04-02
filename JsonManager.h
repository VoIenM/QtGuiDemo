/* date = April 2nd 2026 1:56 pm */

#ifndef _JSON_MANAGER_H
#define _JSON_MANAGER_H

#include <stdexcept>

class JsonManager
{
	public:
    QJsonObject loadJson(const QString& filename);
    void saveJson(const QString& filename, const QJsonObject& obj);
};

#endif //_JSON_MANAGER_H
