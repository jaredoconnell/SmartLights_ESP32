#ifndef SETTING_H
#define SETTING_H

#include <string>
#include <stdexcept>

class Setting {
private:
    std::string settingName;
    std::string strVal;
    int intVal;
    bool _isString;
public:
    Setting(std::string name, std::string value)
        : settingName(name), strVal(value), _isString(true)
    {}
    Setting(std::string name, int value)
        : settingName(name), intVal(value), _isString(false)
    {}

    std::string getSettingName() {
        return settingName;
    }

    bool isString() {
        return _isString;
    }
    int getIntVal() {
        if (_isString)
            throw new std::runtime_error("Requested int for type string");
        return intVal;
    }

    std::string getStrVal() {
        if (!_isString)
            throw new std::runtime_error("Requested string for type int");
        return strVal;
    }
};

#endif