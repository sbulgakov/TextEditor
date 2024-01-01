#include <QApplication>
#include <QIcon>

#include "textedit.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  app.setWindowIcon(QIcon(":/images/icon.png"));
  
  TextEdit w;
  w.show();

  return app.exec();
}
