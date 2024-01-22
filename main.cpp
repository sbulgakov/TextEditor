#include <QApplication>
#include <QIcon>

#ifdef HAVE_SETTINGS
#include "settings.h"
#endif
#include "mainwindow.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  app.setWindowIcon(QIcon(":/images/icon.png"));
  
#ifdef HAVE_SETTINGS
  Settings::instance()->readSettings("txtedit.ini");
#endif
  
  MainWindow w;
  w.show();

  return app.exec();
}
