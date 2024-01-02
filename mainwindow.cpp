#include "mainwindow.h"

#include "textedit.h"

#include <QMenuBar>
#include <QApplication>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  setWindowTitle(tr("[No Name] - TextEditor"));
  
  textEdit = new TextEdit(this);
  setCentralWidget(textEdit);
  
  fileName = "";
  filePath = "";
  
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileActNew  = fileMenu->addAction(tr("&New"),
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(fileNew()),
#else
    &MainWindow::fileNew,
#endif
    QKeySequence::New
    );
  fileActOpen = fileMenu->addAction(tr("&Open..."),
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(fileOpen()),
#else
    &MainWindow::fileOpen,
#endif
    QKeySequence::Open
    );
  fileActSave = fileMenu->addAction(tr("&Save"),
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(fileSave()),
#else
    &MainWindow::fileSave,
#endif
    QKeySequence::Save
    );
  fileActSaveAs = fileMenu->addAction(tr("Save &As"),
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(fileSaveAs())
#else
    &MainWindow::fileSaveAs
#endif
    );
  /* ----- */   fileMenu->addSeparator();
  fileActExit = fileMenu->addAction(tr("E&xit"),
    qApp,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(quit())
#else
    &QApplication::quit
#endif
    );
}

MainWindow::~MainWindow()
{

}

void MainWindow::fileNew()
{
  fileName = "";
  textEdit->Open("");
  setWindowTitle(tr("[No Name] - TextEditor"));
}

void MainWindow::fileOpen()
{
  fileName = QFileDialog::getOpenFileName(this, tr("Open file"), filePath,
    tr("Text files (*.txt)"));
  if(fileName.isEmpty()) return;
  if(textEdit->Open(fileName))
  {
    filePath = fileName;
    fileName = QFileInfo(fileName).fileName();
    filePath.remove(QRegExp(fileName+"$"));
    setWindowTitle(fileName + QString(" - TextEditor"));
  }
}

void MainWindow::fileSave()
{
  if(fileName == "") fileSaveAs();
  else
  {
    textEdit->Save(filePath + fileName);
  }
}

void MainWindow::fileSaveAs()
{
  fileName = QFileDialog::getSaveFileName(this, tr("Save file"), filePath,
    tr("All files (*)"));
  if(fileName.isEmpty()) return;
  if(textEdit->Save(fileName))
  {
    filePath = fileName;
    fileName = QFileInfo(fileName).fileName();
    filePath.remove(QRegExp(fileName+"$"));
    setWindowTitle(fileName + QString(" - TextEditor"));
  }
}