#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{
    static Settings* obj;
    
    QSettings::SettingsMap values;
    QString                file;
    
    Settings();
    Settings(const Settings& o);
    
public:
   ~Settings();
    static Settings* instance();
    
    void readSettings(const QString& path);
    void writeSettings(const QString& path = QString());
    
    QStringList allKeys () const;
    QString fileName() const;
    bool isWritable() const;
    void setValue(const QString& key, const QVariant& value);
    QVariant value(const QString& key) const;
};

#endif // SETTINGS_H
