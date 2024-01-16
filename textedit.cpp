#include "textedit.h"

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextBlock>
#include <QMessageBox>
#include <QTextStream>
#include <QPainter>

#include "finddialog.h"

TextEdit::TextEdit(QWidget *parent) : QWidget(parent),
  pos(0), blockCount(0),
  findDialog(0)
#ifdef FINDDIALOG_RESULTS
  , findResults(0)
#endif
{
  edit = new QPlainTextEdit(this);
  edit->setLineWrapMode(QPlainTextEdit::NoWrap);
  
  lineNumbers = new LineNumbers(this);
  lineNumbers->setNumbers(QStringList() << "1");
  
  connect(edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(updateRequest(QRect,int)),
#else
          &QPlainTextEdit::updateRequest,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(updateLineNumbers(QRect,int)));
#else
          QOverload<const QRect&,int>::of(&TextEdit::updateLineNumbers));
#endif
  
  connect(edit->verticalScrollBar(),
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(valueChanged(int)),
#else
          &QScrollBar::valueChanged,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(updateLineNumbers(int)));
#else
          QOverload<int>::of(&TextEdit::updateLineNumbers));
#endif
  
  connect(edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(cursorPositionChanged()),
#else
          &QPlainTextEdit::cursorPositionChanged,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(updateLineNumbers()));
#else
          QOverload<>::of(&TextEdit::updateLineNumbers));
#endif
  
  connect(edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(cursorPositionChanged()),
#else
          &QPlainTextEdit::cursorPositionChanged,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(highlightCurrentLine()));
#else
          &TextEdit::highlightCurrentLine);
#endif
  highlightCurrentLine();
}

TextEdit::~TextEdit()
{

}

int TextEdit::lineNumbersWidth() const
{
  int max = 1;
  int num = qMax(1,edit->blockCount());
  
  while(num)
  {
    max +=  1;
    num /= 10;
  }
  
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  int wid = 3 + fontMetrics().width(QLatin1Char('9')) * max;
#else
  int wid = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * max;
#endif
  
  return wid;
}

bool TextEdit::Open(const QString& fileName)
{
  edit->clear();
  
  if( fileName != "" )
  {
    QFile file(fileName);
    QFile::OpenMode mode(QIODevice::ReadOnly);
    if(!file.open(mode))
    {
      QMessageBox::critical(this, tr("Error"),
        tr("Unable to open file."));
      return false;
    }
    
    QTextStream in(&file);
    in.setCodec("UTF-8");
    edit->setPlainText(in.readAll());
    edit->setDocumentTitle(fileName);
    
    file.close();
    
    updateLineNumbersWidth(edit->blockCount());
    lineNumbers->setScroll(0);
    lineNumbers->update();
  }
  return true;
}

bool TextEdit::Save(const QString& fileName)
{
    if( fileName != "" )
    {
      QFile file(fileName);
      QFile::OpenMode mode(QIODevice::WriteOnly);
      if(!file.open(mode))
      {
        QMessageBox::critical(this, tr("Error"),
          tr("Unable to open file."));
        return false;
      }
      
      QTextStream out(&file);
      out.setCodec("UTF-8");
      out << edit->toPlainText();
      edit->setDocumentTitle(fileName);
      
      file.close();
    }
    return true;
}

bool TextEdit::Find(const QString& str)
{
  if(!str.isEmpty())
  {
    QTextDocument::FindFlags flags = 0;
    bool regex = false;
#ifdef FINDDIALOG_ALL
    bool every = false;
#endif
    if(findDialog)
    {
      if(findDialog->isBackward())      flags |= QTextDocument::FindBackward;
      if(findDialog->isCaseSensitive()) flags |= QTextDocument::FindCaseSensitively;
      if(findDialog->isWholeWords())    flags |= QTextDocument::FindWholeWords;
      if(findDialog->isRegex())         regex  = true;
#ifdef FINDDIALOG_ALL
      if(findDialog->isAll())           every  = true;
#endif
    }
    
    QTextCursor  c;
    if(!regex)   c = edit->document()->find(str,  edit->textCursor(), flags);
    else c = edit->document()->find(QRegExp(str), edit->textCursor(), flags);
    
    if (!c.isNull())
    {
      edit->setTextCursor(c);
      
#if (defined FINDDIALOG_RESULTS) && (defined FINDDIALOG_ALL)
      if(findResults && every)
      {
        findResults->show();
        findResults->activateWindow();
        
        do
        {
          findResults->insert(c);
          
          if(!regex)   c = edit->document()->find(str,  c, flags);
          else c = edit->document()->find(QRegExp(str), c, flags);
        }
        while(!c.isNull());
      }
#endif // FINDDIALOG_RESULTS && FINDDIALOG_ALL
      
      return true;
    }
  }
  
  return false;
}

#ifdef FINDDIALOG_REPLACE
bool TextEdit::Replace(const QString& str, const QString& with)
{
  if(!str.isEmpty())
  {
    QTextDocument::FindFlags flags = 0;
    bool regex = false;
    bool every = false;
    if(findDialog)
    {
      if(findDialog->isBackward())      flags |= QTextDocument::FindBackward;
      if(findDialog->isCaseSensitive()) flags |= QTextDocument::FindCaseSensitively;
      if(findDialog->isWholeWords())    flags |= QTextDocument::FindWholeWords;
      if(findDialog->isRegex())         regex  = true;
      if(findDialog->isReplaceAll())    every  = true;
    }
    
    QTextCursor  from = edit->textCursor();
    if(every)    from.setPosition(0);
    
    QTextCursor  c;
    if(!regex)   c = edit->document()->find(str,  from, flags);
    else c = edit->document()->find(QRegExp(str), from, flags);
    
    if (!c.isNull())
    {
      int beg = c.selectionStart();
      int end ;
      
      if(!regex) c.insertText(with);
      else
      {
        QString text = c.selectedText();
        text.replace(QRegExp(str), with);
        c.insertText(text);
      }
      end = c.position();
      
      c.setPosition(beg);
      c.setPosition(end, QTextCursor::KeepAnchor);
      edit->setTextCursor(c);
      
      if(every)
      {
        while(!c.isNull())
        {
          if(!regex)   c = edit->document()->find(str,  c, flags);
          else c = edit->document()->find(QRegExp(str), c, flags);
          
          if(!c.isNull())
          {
            beg = c.selectionStart();
            
            if(!regex) c.insertText(with);
            else
            {
              QString text = c.selectedText();
              text.replace(QRegExp(str), with);
              c.insertText(text);
            }
            end = c.position();
            
            c.setPosition(beg);
            c.setPosition(end, QTextCursor::KeepAnchor);
            edit->setTextCursor(c);
          }
        }
      }
      
      return true;
    }
  }
  
  return false;
}
#endif // FINDDIALOG_REPLACE

void TextEdit::showFindDialog()
{
  if(!findDialog)
  {
    findDialog = new FindDialog(this);
    connect(findDialog,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SIGNAL(find(const QString&)),
#else
            &FindDialog::find,
#endif
            this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SLOT(Find(const QString&)));
#else
            &TextEdit::Find);
#endif
    
#ifdef FINDDIALOG_REPLACE
    connect(findDialog,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SIGNAL(replace(const QString&,const QString&)),
#else
            &FindDialog::replace,
#endif
            this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SLOT(Replace(const QString&,const QString&)));
#else
            &TextEdit::Replace);
#endif
#endif // FINDDIALOG_REPLACE
  }
  findDialog->show();
  findDialog->activateWindow();
  findDialog->setFocus();
  
#ifdef FINDDIALOG_REPLACE
  findDialog->setReplaceMode(false);
#endif
}

#ifdef FINDDIALOG_REPLACE
void TextEdit::showReplaceDialog()
{
  showFindDialog();
  findDialog->setReplaceMode(true);
}
#endif

void TextEdit::setTextCursor(const QTextCursor &cursor)
{
  edit->setCenterOnScroll(true);
  edit->setTextCursor(cursor);
  edit->setCenterOnScroll(false);
  
  edit->show();
  edit->activateWindow();
  edit->setFocus();
}

#ifdef FINDDIALOG_RESULTS
FindResults* TextEdit::getFindResults()
{
  if(!findResults)
  {
    findResults = new FindResults(this);
    connect(findResults,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SIGNAL(itemClicked(const QTextCursor&)),
#else
            &FindResults::itemClicked,
#endif
            this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SLOT(setTextCursor(const QTextCursor&)));
#else
            &TextEdit::setTextCursor);
#endif
  }
  
  return findResults;
}
#endif // FINDDIALOG_RESULTS

void TextEdit::updateLineNumbersWidth(int newBlockCount)
{
  QTextDocument *doc = edit->document();
  int num = 1;
  QStringList numbers;
  QTextBlock block = doc->findBlockByNumber(num);
  
  while(num <= newBlockCount)
  {
    if(block.isVisible()) numbers << QString::number(num);
    block = doc->findBlockByNumber(++num);
  }
  
  lineNumbers->setNumbers(numbers);
  
  int wid = lineNumbersWidth();
  QSize size = edit->size();
  size.setWidth(width()-wid);
  edit->resize(size);
  edit->move(wid,0);
  lineNumbers->resize(wid,size.height());
}

void TextEdit::updateLineNumbers(const QRect &rect, int dy)
{
  Q_UNUSED(rect)
  
  if(dy)
  {
    lineNumbers->setScroll(lineNumbers->getScroll() + dy);
    lineNumbers->update();
  }
  else if(pos != edit->verticalScrollBar()->value())
  {
    updateLineNumbers();
  }
}

void TextEdit::updateLineNumbers(int val)
{
  pos = val;
  
  if(val == 0) lineNumbers->setScroll(3);
  else         lineNumbers->setScroll(-1);
  
  int ht  = fontMetrics().height();
  lineNumbers->setScroll(lineNumbers->getScroll()
                         -3
                         -ht*val
                        );
}

void TextEdit::updateLineNumbers()
{
  if(blockCount != edit->blockCount())
  {
    blockCount = edit->blockCount();
    updateLineNumbersWidth(blockCount);
    lineNumbers->update();
  }
  
  if(pos != edit->verticalScrollBar()->value())
  {
    updateLineNumbers(edit->verticalScrollBar()->value());
  }
}

void TextEdit::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;
  
  if (!edit->isReadOnly())
  {
    QTextEdit::ExtraSelection selection;
    
    QColor lineColor = QColor(Qt::lightGray).lighter(125);
    
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = edit->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }
  
  edit->setExtraSelections(extraSelections);
}

void TextEdit::resizeEvent(QResizeEvent *event)
{
  int wid = lineNumbersWidth();
  QSize size = event->size();
  
  size.setWidth(size.width()-wid);
  edit->resize(size);
  edit->move(wid,0);
  lineNumbers->resize(wid,size.height());
}

//--------------------------------------------------------------------

LineNumbers::LineNumbers(TextEdit *parent)
    : QWidget(parent), edit(parent), scroll(0)
{

}

void LineNumbers::setNumbers(const QStringList& numbers)
{
  this->numbers = numbers;
}

int LineNumbers::getScroll() const
{
  return scroll;
}

void LineNumbers::setScroll(int scroll)
{
  this->scroll = scroll;
}

void LineNumbers::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  
  QPainter p(this);
  
  int wd  = width();
  int ht  = fontMetrics().height();
  int top = 3 + 2
#ifdef Q_OS_WIN
              + 1
#endif
              + scroll;
  
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  foreach (QString num, numbers) {
#else
  for(QString num : numbers) {
#endif
    p.drawText(0,top,wd,ht,Qt::AlignRight,num);
    top += ht;
  }
}
