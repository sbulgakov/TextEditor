#include "settings.h"

#include <QFileInfo>
#include <QStringList>

Settings* Settings::obj = 0;

Settings::Settings()
{
  values.insert("Editor/fontSize", 8);
  values.insert("Editor/fontFamily", "Sans");
  values.insert("Editor/showLineNumbers", true);
}

Settings::Settings(const Settings& o)
{
  Q_UNUSED(o)
}

Settings::~Settings()
{
  if(obj)
  {
    delete obj;
  }
  obj = 0;
}

Settings* Settings::instance()
{
  if(!obj)
  {
    obj = new Settings();
  }
  
  return obj;
}

void Settings::readSettings(const QString& path)
{
  if(QFileInfo(path).exists())
  {
    QSettings settings(path, QSettings::IniFormat);
    QStringList keys = settings.allKeys();
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    foreach (const QString& key, keys) {
#else
    for(const QString& key : keys) {
#endif
      if(values.contains(key))
      {
        values.insert(key, settings.value(key));
      }
    }
    file = path;
  }
}

void Settings::writeSettings(const QString& path)
{
  QString store = path;
  if(store.isEmpty()) store = file;
  if(store.isEmpty()) return;
  QSettings settings(store, QSettings::IniFormat);
  QStringList keys = values.keys();
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  foreach (const QString& key, keys) {
#else
  for(const QString& key : keys) {
#endif
    settings.setValue(key, values[key]);
  }
}

QStringList Settings::allKeys () const
{
  return values.keys();
}

QString Settings::fileName() const
{
  return file;
}

bool Settings::isWritable() const
{
  if(file.isEmpty()) return false;
  
  return true;
}

void Settings::setValue(const QString& key, const QVariant& value)
{
  values.insert(key, value);
}


QVariant Settings::value(const QString& key) const
{
  QSettings::SettingsMap::const_iterator i = values.find(key);
  if(i != values.end()) return *i;
  
  return QVariant();
}
