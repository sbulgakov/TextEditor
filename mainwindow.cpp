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
#ifdef HAVE_ICONS
  fileActNew  = fileMenu->addAction(QIcon(":/images/new.png"), tr("&New"),
#else
  fileActNew  = fileMenu->addAction(tr("&New"),
#endif
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(fileNew()),
#else
    &MainWindow::fileNew,
#endif
    QKeySequence::New
    );
#ifdef HAVE_ICONS
  fileActOpen = fileMenu->addAction(QIcon(":/images/open.png"), tr("&Open..."),
#else
  fileActOpen = fileMenu->addAction(tr("&Open..."),
#endif
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(fileOpen()),
#else
    &MainWindow::fileOpen,
#endif
    QKeySequence::Open
    );
#ifdef HAVE_ICONS
  fileActSave = fileMenu->addAction(QIcon(":/images/save.png"), tr("&Save"),
#else
  fileActSave = fileMenu->addAction(tr("&Save"),
#endif
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(fileSave()),
#else
    &MainWindow::fileSave,
#endif
    QKeySequence::Save
    );
#ifdef HAVE_ICONS
  fileActSaveAs = fileMenu->addAction(QIcon(":/images/saveas.png"), tr("Save &As"),
#else
  fileActSaveAs = fileMenu->addAction(tr("Save &As"),
#endif
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(fileSaveAs())
#else
    &MainWindow::fileSaveAs
#endif
    );
  /* ----- */   fileMenu->addSeparator();
#ifdef HAVE_ICONS
  fileActExit = fileMenu->addAction(QIcon(":/images/exit.png"), tr("E&xit"),
#else
  fileActExit = fileMenu->addAction(tr("E&xit"),
#endif
    qApp,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(quit())
#else
    &QApplication::quit
#endif
    );
  
  editMenu = menuBar()->addMenu(tr("&Edit"));
#ifdef HAVE_ICONS
  editActFind  = editMenu->addAction(QIcon(":/images/find.png"), tr("&Find"),
#else
  editActFind  = editMenu->addAction(tr("&Find"),
#endif
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(editFind()),
#else
    &MainWindow::editFind,
#endif
    QKeySequence::Find
    );
#ifdef FINDDIALOG_REPLACE
#ifdef HAVE_ICONS
    editActReplace = editMenu->addAction(QIcon(":/images/replace.png"), tr("&Replace"),
#else
    editActReplace = editMenu->addAction(tr("&Replace"),
#endif
      this,
  #if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
      SLOT(editReplace()),
  #else
      &MainWindow::editReplace,
  #endif
      QKeySequence::Replace
      );
#endif
  
  helpMenu = menuBar()->addMenu(tr("&Help"));
#ifdef HAVE_ICONS
  helpActAbout  = helpMenu->addAction(QIcon(":/images/about.png"), tr("&About"),
#else
  helpActAbout  = helpMenu->addAction(tr("&About"),
#endif
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

#ifdef FINDDIALOG_REPLACE
void MainWindow::editReplace()
{
  textEdit->showReplaceDialog();
}
#endif

void MainWindow::helpAbout()
{
  QString version;
  
  QString git_tag(GIT_TAG);
  QString git_date(GIT_DATE);
  QString git_hash(GIT_HASH);
  QString git_abbr(GIT_ABBR);
  
  if(git_tag == git_hash)
    version = "[" + git_hash + "@" + git_date + "]";
  else if(git_tag != git_abbr)
    version = git_abbr + " [" + git_hash + "@" + git_date + "]";
  else
    version = git_tag;
  
  version.append(" (" __DATE__ " " __TIME__ ")<br/>");
  
  QMessageBox about;
  
  about.setTextFormat(Qt::RichText);
#if (QT_VERSION >= QT_VERSION_CHECK(5,1,0))
  about.setTextInteractionFlags(Qt::NoTextInteraction|Qt::LinksAccessibleByMouse);
#endif
  about.setIconPixmap(QPixmap(":/images/icon.png"));
  about.setWindowTitle(tr("About TextEditor"));
  about.setText(
    tr(
      "TextEditor is a simple application for editing text documents.<br/>"
      "<br/>"
      "&copy; 2024 Stanislav Bulgakov<br/>"
      ) + version + tr(
      "<br/>"
      "Distributed freely under <a href='www.opensource.org/license/mit/'>The MIT License</a>.<br/>"
      "<br/>"
      "<a href='www.qt.io'>Qt application framework</a> v" QT_VERSION_STR " by The Qt company."
#ifdef HAVE_ICONS
      "<br/>"
      "Some icons used are from "
        "<a href='www.flaticon.com/authors/super-basic-orbit/outline'>"
        "Super Basic Orbit Outline</a> style."
#endif
      )
  );
  
  about.exec();
}
