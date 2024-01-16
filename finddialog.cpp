#include "finddialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint|Qt::WindowTitleHint)
#ifdef FINDDIALOG_REPLACE
  , replaceAll(false), replaceDialog(false)
#endif
{
  label = new QLabel(tr("Find:"),this);
  label->adjustSize();
  
  dummy = new QLabel(this);
  dummy->setMinimumSize(label->size());
  
  edit  = new QLineEdit(this);
  
#ifdef FINDDIALOG_REPLACE
  with = new QLabel(tr("Replace:"),this);
  with->adjustSize();
  with->setMinimumSize(with->size());
  with->setText(tr("With:"));
  with->setVisible(false);
  
  text = new QLineEdit(this);
  text->setVisible(false);
#endif
  
  backCheck  = new QCheckBox(tr("Find backwards"),this);
  
#ifdef FINDDIALOG_ALL
  allCheck   = new QCheckBox(tr("Find all occurrences"),this);
#endif
  
  caseCheck  = new QCheckBox(tr("Case sensitive"),this);
  caseCheck->setChecked(true);
  
  wholeCheck = new QCheckBox(tr("Whole words"),this);
  regexCheck = new QCheckBox(tr("Regular expression"),this);
  
  findButton  = new QPushButton(tr("Find"),this);
  findButton->setDefault(true);
  
#ifdef FINDDIALOG_REPLACE
  replaceButton    = new QPushButton(tr("Replace"),this);
  replaceButton->setVisible(false);
  replaceAllButton = new QPushButton(tr("Replace All"),this);
  replaceAllButton->setVisible(false);
#endif
  
  closeButton = new QPushButton(tr("Cancel"),this);
  
  QHBoxLayout *find = new QHBoxLayout();
  find->addWidget(label);
  find->addWidget(edit);

#ifdef FINDDIALOG_REPLACE
  QHBoxLayout *repl = new QHBoxLayout();
  repl->addWidget(with);
  repl->addWidget(text);
#endif
  
  QGridLayout *checks = new QGridLayout();
  checks->addWidget(backCheck,  0, 0);
#ifdef FINDDIALOG_ALL
  checks->addWidget(allCheck,   1, 0);
#endif
  checks->addWidget(caseCheck,  0, 1);
  checks->addWidget(wholeCheck, 1, 1);
  checks->addWidget(regexCheck, 2, 1);
  checks->addItem(new QSpacerItem(25, 10), 0, 2);
#ifdef FINDDIALOG_REPLACE
  spacer = new QLabel(this);
  spacer->setMinimumSize(25, 10);
  checks->addWidget(spacer, 3, 2);
  spacer->setVisible(false);
#endif
  
  QVBoxLayout *buttons = new QVBoxLayout();
  buttons->addWidget(findButton);
#ifdef FINDDIALOG_REPLACE
  buttons->addWidget(replaceButton);
  buttons->addWidget(replaceAllButton);
#endif
  buttons->addWidget(closeButton);
  buttons->addStretch();
  
  QHBoxLayout *controls = new QHBoxLayout();
  controls->addWidget(dummy);
  controls->addLayout(checks,1);
  controls->addLayout(buttons);
  
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addLayout(find);
#ifdef FINDDIALOG_REPLACE
  layout->addLayout(repl);
#endif
  layout->addLayout(controls);
  
  connect(findButton,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(clicked()),
#else
          &QPushButton::clicked,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(findClicked()));
#else
          &FindDialog::findClicked);
#endif
  
#ifdef FINDDIALOG_REPLACE
  connect(replaceButton,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(clicked()),
#else
          &QPushButton::clicked,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(replaceClicked()));
#else
          &FindDialog::replaceClicked);
#endif
  connect(replaceAllButton,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(clicked()),
#else
          &QPushButton::clicked,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(replaceAllClicked()));
#else
          &FindDialog::replaceAllClicked);
#endif
#endif // FINDDIALOG_REPLACE
  
  connect(closeButton,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(clicked()),
#else
          &QPushButton::clicked,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(close()));
#else
          &FindDialog::reject);
#endif
  
  setWindowTitle(tr("Find"));
  findSize = sizeHint();
  setFixedSize(findSize);
  setModal(true);
  
  setFocusProxy(edit);
}

bool FindDialog::isBackward()
{
  return backCheck->isChecked();
}

#ifdef FINDDIALOG_ALL
bool FindDialog::isAll()
{
  return allCheck->isChecked();
}
#endif

bool FindDialog::isCaseSensitive()
{
  return caseCheck->isChecked();
}

bool FindDialog::isWholeWords()
{
  return wholeCheck->isChecked();
}

bool FindDialog::isRegex()
{
  return regexCheck->isChecked();
}

#ifdef FINDDIALOG_REPLACE
bool FindDialog::isReplace()
{
  return replaceDialog;
}

bool FindDialog::isReplaceAll()
{
  return replaceAll;
}

void FindDialog::setReplaceMode(bool on)
{
  if(on) label->setText(tr("Replace:"));
  else   label->setText(tr("Find:"));
  label->adjustSize();
  dummy->setMinimumSize(label->size());
  
  with->setVisible(on);
  text->setVisible(on);
  
  allCheck->setVisible(!on);
  
  findButton->setVisible(!on);
  findButton->setDefault(!on);
  replaceButton->setVisible(on);
  replaceButton->setDefault(on);
  replaceAllButton->setVisible(on);
  spacer->setVisible(on);
  
  replaceDialog = on;
  
  if(on)
  {
    setWindowTitle(tr("Replace"));
    setFixedSize(sizeHint());
  }
  else
  {
    setWindowTitle(tr("Find"));
    setFixedSize(findSize);
  }
}
#endif // FINDDIALOG_REPLACE

void FindDialog::findClicked()
{
  if(!edit->text().isEmpty())
  {
    emit find(edit->text());
  }
  accept();
}

#ifdef FINDDIALOG_REPLACE
void FindDialog::replaceClicked()
{
  if(!edit->text().isEmpty())
  {
    replaceAll = false;
    emit replace(edit->text(), text->text());
  }
  accept();
}

void FindDialog::replaceAllClicked()
{
  if(!edit->text().isEmpty())
  {
    replaceAll = true;
    emit replace(edit->text(), text->text());
  }
  accept();
}
#endif

//--------------------------------------------------------------------

#ifdef FINDDIALOG_RESULTS
#include <QTableWidget>
#include <QTextCursor>
#include <QHeaderView>
#include <QTextBlock>
#include <QTextDocument>

FindResults::FindResults(QWidget *parent):
  QDialog(parent, Qt::WindowSystemMenuHint|Qt::WindowTitleHint)
{
  setWindowTitle(tr("Search results"));
  
  table = new QTableWidget(this);
  table->setColumnCount(3);
  table->setHorizontalHeaderLabels(QStringList() <<
                                   tr("File")    <<
                                   tr("Line")    <<
                                   tr("Text"));
  table->horizontalHeader()->setStretchLastSection(true);
  table->verticalHeader()->setVisible(false);
  
  QPalette pal = table->palette();
  pal.setColor(QPalette::Inactive, QPalette::Highlight, pal.highlight().color());
  pal.setColor(QPalette::Inactive, QPalette::HighlightedText, pal.highlightedText().color());
  table->setPalette(pal);
  
  table->setShowGrid(false);
  
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  
  table->setFocusPolicy(Qt::NoFocus);
  
  QVBoxLayout * l = new QVBoxLayout(this);
  l->addWidget(table);
  
  connect(table,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(cellClicked(int,int)),
#else
          &QTableWidget::cellClicked,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(cellClicked(int,int)));
#else
          &FindResults::cellClicked);
#endif
  
  list = new QList<QTextCursor>();
  
  setFocusPolicy(Qt::NoFocus);
}

FindResults::~FindResults()
{
  delete list;
}

void FindResults::closeEvent(QCloseEvent *event)
{
  Q_UNUSED(event);
  
  this->hide();
  clear();
}

void FindResults::clear()
{
  table->clearContents();
  list->clear();
}

void FindResults::insert(const QTextCursor& c)
{
  QString file = c.document()->metaInformation(QTextDocument::DocumentTitle);
  QRegExp re("[^/]*$");
  int index = re.indexIn(file);
  QString name = file;
  if(index != -1) name = re.cap();
  QTableWidgetItem *f = new QTableWidgetItem(name);
  QTableWidgetItem *l = new QTableWidgetItem(QString::number(c.blockNumber()+1));
  
  QString text = c.block().text();
  QTableWidgetItem *t = new QTableWidgetItem(text);
  
  int r = list->size();
  
  table->setRowCount(r+1);
  table->setItem(r,0,f);
  table->setItem(r,1,l);
  table->setItem(r,2,t);
  
  list->append(c);
}

void FindResults::setVisible(bool visible)
{
  QDialog::setVisible(visible);
  
  emit visibilityChanged(visible);
}

void FindResults::cellClicked(int row, int column)
{
  Q_UNUSED(column);
  
  emit itemClicked(list->at(row));
}
#endif // FINDDIALOG_RESULTS

