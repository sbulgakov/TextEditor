#include "textedit.h"

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextBlock>
#include <QMessageBox>
#include <QTextStream>
#include <QPainter>

TextEdit::TextEdit(QWidget *parent) : QWidget(parent),
  pos(0), blockCount(0)
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
          &QPlainTextEdit::highlightCurrentLine);
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
      
      file.close();
    }
    return true;
}

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
