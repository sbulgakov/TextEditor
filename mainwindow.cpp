#include "mainwindow.h"

#include "textedit.h"
#ifdef FINDDIALOG_RESULTS
#include "finddialog.h"
#endif

#include <QMenuBar>
#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

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
  
  editMenu = menuBar()->addMenu(tr("&Edit"));
  editActFind  = editMenu->addAction(tr("&Find"),
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(editFind()),
#else
    &MainWindow::editFind,
#endif
    QKeySequence::Find
    );
  
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpActAbout  = helpMenu->addAction(tr("&About"),
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(helpAbout())
#else
    &MainWindow::helpAbout
#endif
    );
  
#ifdef FINDDIALOG_RESULTS
  FindResults *findResults = textEdit->getFindResults();
  QDockWidget *dock = new QDockWidget(this);
  dock->setAllowedAreas(Qt::TopDockWidgetArea);
  dock->setWidget(findResults);
  dock->setWindowTitle(findResults->windowTitle());
  connect(findResults,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(visibilityChanged(bool)),
#else
          &FindResults::visibilityChanged,
#endif
          dock,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(setVisible(bool)));
#else
          &QDockWidget::setVisible);
#endif
  addDockWidget(Qt::TopDockWidgetArea, dock);
  dock->setVisible(false);
#endif // FINDDIALOG_RESULTS
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

void MainWindow::editFind()
{
  textEdit->showFindDialog();
}

void MainWindow::helpAbout()
{
  QMessageBox about;
  
  about.setTextFormat(Qt::RichText);
#if (QT_VERSION >= QT_VERSION_CHECK(5,1,0))
  about.setTextInteractionFlags(Qt::NoTextInteraction|Qt::LinksAccessibleByMouse);
#endif
  about.setIconPixmap(QPixmap(":/images/icon.png"));
  about.setWindowTitle(tr("About TextEditor"));
  about.setText(
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0)) && defined (Q_OS_WIN)
    trUtf8(
#else
    tr(
#endif
      "TextEditor is a simple application for editing text documents.<br/>"
      "<br/>"
      "\u00A9 2024 Stanislav Bulgakov<br/>"
      "" GIT_TAG " (" __DATE__ " " __TIME__ ")<br/>"
      "<br/>"
      "Distributed freely under <a href='www.opensource.org/license/mit/'>The MIT License</a>.<br/>"
      "<br/>"
      "<a href='www.qt.io'>Qt application framework</a> v" QT_VERSION_STR " by The Qt company."
      )
  );
  
  about.exec();
}
