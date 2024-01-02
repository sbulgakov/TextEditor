#include "mainwindow.h"

#include "textedit.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  setWindowTitle(tr("TextEditor"));
  
  textEdit = new TextEdit(this);
  setCentralWidget(textEdit);
}

MainWindow::~MainWindow()
{

}
