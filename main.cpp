#include <QApplication>

#include "textedit.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  
  TextEdit w;
  w.show();

  return app.exec();
}
