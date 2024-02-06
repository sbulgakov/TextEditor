#include "textedit.h"

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QTextBlock>
#include <QMessageBox>
#include <QTextStream>
#include <QPainter>

#include "finddialog.h"

#ifdef TEXTEDIT_MENU
#include <QMenu>
#endif

#ifdef HAVE_SETTINGS
#include "settings.h"
#endif

#ifdef HAVE_HUNSPELL
#include "hunspellhighlighter.h"
#endif

TextEdit::TextEdit(QWidget *parent) : QWidget(parent),
  pos(0), blockCount(0),
  findDialog(0)
#ifdef FINDDIALOG_RESULTS
  , findResults(0)
#endif
#ifdef TEXTEDIT_MENU
  , undo(false), redo(false)
#endif
  , highlighter(0)
{
#ifdef HAVE_SETTINGS
  QFont fon = this->font();
  fon.setPointSize(Settings::instance()->value("Editor/fontSize").toInt());
  fon.setFamily(Settings::instance()->value("Editor/fontFamily").toString());
  this->setFont(fon);
#endif
  edit = new QPlainTextEdit(this);
  edit->setLineWrapMode(QPlainTextEdit::NoWrap);
  
  screen = new EditScreen(this, edit);
  screen->setAttribute(Qt::WA_TransparentForMouseEvents, true);
  
  lineNumbers = new LineNumbers(this);
  lineNumbers->setNumbers(QStringList() << "1");
#ifdef HAVE_SETTINGS
  lineNumbers->setVisible(
    Settings::instance()->value("Editor/showLineNumbers").toBool());
#endif
  
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
  
#ifdef TEXTEDIT_MENU
  createMenu(0);
  
  edit->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(customContextMenuRequested(const QPoint&)),
#else
          &QPlainTextEdit::customContextMenuRequested,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(editMenuRequested(const QPoint&)));
#else
          &TextEdit::editMenuRequested);
#endif
  connect(edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(undoAvailable(bool)),
#else
          &QPlainTextEdit::undoAvailable,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(editCanUndo(bool)));
#else
          &TextEdit::editCanUndo);
#endif
  connect(edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SIGNAL(redoAvailable(bool)),
#else
          &QPlainTextEdit::redoAvailable,
#endif
          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
          SLOT(editCanRedo(bool)));
#else
          &TextEdit::editCanRedo);
#endif
#endif // TEXTEDIT_MENU
  
  highlightCurrentLine();
  
#ifdef HAVE_HUNSPELL
  highlighter = new HunspellHighlighter(edit->document());
#endif
}

TextEdit::~TextEdit()
{

}

int TextEdit::lineNumbersWidth() const
{
  if(lineNumbers->isVisible() == false) return 0;
  
  int max = 0;
  int num = qMax(1,edit->blockCount());
  
  while(num)
  {
    max +=  1;
    num /= 10;
  }
  
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  int wid =     fontMetrics().width(QLatin1Char('9')) * max;
#else
  int wid =     fontMetrics().horizontalAdvance(QLatin1Char('9')) * max;
#endif
  
  int margins = lineNumbers->getLeftMargin() + 
                lineNumbers->getRightMargin();
  
  return wid + margins;
}

#ifdef TEXTEDIT_MENU
QMenu* TextEdit::createStandardContextMenu()
{
  QMenu *menu = new QMenu();
  
  createMenu(menu);
  
  return menu;
}

void TextEdit::updateStandardContextMenu(QMenu *menu)
{
  QList<QAction*> acts = menu->actions();
  
  bool read   = edit->isReadOnly();
  bool select = edit->textCursor().hasSelection();
  bool paste  = edit->canPaste();
  bool empty  = edit->document()->isEmpty();
  
  acts.at(0)->setEnabled(!read && undo);   //editActUndo
  acts.at(1)->setEnabled(!read && redo);   //editActRedo
  //separator
  acts.at(3)->setEnabled(!read && select); //editActCut
  acts.at(4)->setEnabled(select);          //editActCopy
  acts.at(5)->setEnabled(!read && paste);  //editActPaste
  acts.at(6)->setEnabled(!read && select); //editActDelete
  acts.at(7)->setEnabled(!empty);          //editActSelectAll
}
#endif

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

#ifdef TEXTEDIT_MENU
void TextEdit::editMenuRequested(const QPoint& pos)
{
  updateStandardContextMenu(editMenu);
  
  QMenu *menu = editMenu;
  
#ifdef HAVE_HUNSPELL
  HunspellHighlighter *hunspellHighlighter = 
    qobject_cast<HunspellHighlighter*>(highlighter);
  if(hunspellHighlighter)
  {
    QTextCursor cur = edit->textCursor();
    TextBlockUserData *userData = 
      static_cast<TextBlockUserData*>(edit->textCursor().block().userData());
    HunspellBlockData *data = 0;
    if(userData)       data = static_cast<HunspellBlockData*>(userData->find(1));
    if(data && !cur.hasSelection())
    {
      const QStringList& words = data->words();
      
      cur.select(QTextCursor::WordUnderCursor);
      QString word = cur.selectedText();
      
      if(words.indexOf(word) != -1)
      {
        QStringList suggestions = hunspellHighlighter->suggestions(word);
        
        menu = new QMenu();
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
        foreach (const QString& sugg, suggestions) {
#else
        for(const QString& sugg : suggestions) {
#endif
          menu->addAction(sugg,
                          this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
                          SLOT(correctWord()));
#else
                          &TextEdit::correctWord);
#endif
        }
        menu->addSeparator();
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
        foreach (QAction *action, editMenu->actions()) {
#else
        for(QAction *action : editMenu->actions()) {
#endif
          menu->insertAction(0, action);
        }
        menu->deleteLater();
      }
    }
  }
#endif
  
  QPoint p = mapToGlobal(pos);
  p.setX(p.x()+lineNumbers->width());
  menu->exec(p);
}

void TextEdit::editDelete()
{
  edit->textCursor().insertText("");
}

void TextEdit::editCanUndo(bool yes)
{
  undo = yes;
}

void TextEdit::editCanRedo(bool yes)
{
  redo = yes;
}
#endif // TEXTEDIT_MENU

#ifdef HAVE_HUNSPELL
void TextEdit::correctWord()
{
  QAction *word = qobject_cast<QAction*>(sender());
  
  if(word)
  {
    QTextCursor cur = edit->textCursor();
    
    cur.select(QTextCursor::WordUnderCursor);
    cur.insertText(word->text());
  }
}
#endif

void TextEdit::resizeEvent(QResizeEvent *event)
{
  int wid = lineNumbersWidth();
  QSize size = event->size();
  
  size.setWidth(size.width()-wid);
  edit->resize(size);
  edit->move(wid,0);
  screen->resize(size);
  screen->move(wid,0);
  lineNumbers->resize(wid,size.height());
}

#ifdef TEXTEDIT_MENU
void TextEdit::createMenu(QMenu *menu)
{
  if(menu == 0)
  {
    editMenu = new QMenu(this);
    menu = editMenu;
  }
  
  menu->addAction(
#ifdef HAVE_ICONS
    QIcon(":/images/undo.png"),
#endif
    tr("&Undo"),
    edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(undo()),
#else
    &QPlainTextEdit::undo,
#endif
    QKeySequence::Undo
    );
  
  menu->addAction(
#ifdef HAVE_ICONS
    QIcon(":/images/redo.png"),
#endif
    tr("&Redo"),
    edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(redo()),
#else
    &QPlainTextEdit::redo,
#endif
    QKeySequence::Redo
    );
  
  menu->addSeparator();
  
  menu->addAction(
#ifdef HAVE_ICONS
    QIcon(":/images/cut.png"),
#endif
    tr("Cu&t"),
    edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(cut()),
#else
    &QPlainTextEdit::cut,
#endif
    QKeySequence::Cut
    );
  
  menu->addAction(
#ifdef HAVE_ICONS
    QIcon(":/images/copy.png"),
#endif
    tr("&Copy"),
    edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(copy()),
#else
    &QPlainTextEdit::copy,
#endif
    QKeySequence::Copy
    );
  
  menu->addAction(
#ifdef HAVE_ICONS
    QIcon(":/images/paste.png"),
#endif
    tr("&Paste"),
    edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(paste()),
#else
    &QPlainTextEdit::paste,
#endif
    QKeySequence::Paste
    );
  
  menu->addAction(
#ifdef HAVE_ICONS
    QIcon(":/images/delete.png"),
#endif
    tr("Delete"),
    this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(editDelete())
#else
    &TextEdit::editDelete
#endif
    );
  
  menu->addAction(
#ifdef HAVE_ICONS
    QIcon(":/images/selectall.png"),
#endif
    tr("Select All"),
    edit,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    SLOT(selectAll()),
#else
    &QPlainTextEdit::selectAll,
#endif
    QKeySequence::SelectAll
    );
  
  if(menu == editMenu)
  {
    QList<QAction*> acts = menu->actions();
    
    editActUndo      = acts.at(0);
    editActRedo      = acts.at(1);
    //separator      = acts.at(2);
    editActCut       = acts.at(3);
    editActCopy      = acts.at(4);
    editActPaste     = acts.at(5);
    editActDelete    = acts.at(6);
    editActSelectAll = acts.at(7);
  }
}
#endif // TEXTEDIT_MENU

//--------------------------------------------------------------------

TextBlockUserData::TextBlockUserData()
  : next(0), type(0)
{

}

TextBlockUserData::TextBlockUserData(ushort type)
  : next(0), type(type)
{

}

TextBlockUserData::~TextBlockUserData()
{
  TextBlockUserData *cur = next;
  
  while(cur)
  {
    TextBlockUserData *tmp = cur;
    cur = cur->next;
    delete tmp;
  }
}

void TextBlockUserData::append(TextBlockUserData* data)
{
  TextBlockUserData *cur = next;
  
  while(cur->next)
  {
    cur = cur->next;
  }
  cur->next = data;
}

TextBlockUserData* TextBlockUserData::find(ushort type)
{
  TextBlockUserData *ret = 0;
  TextBlockUserData *cur = this;
  
  while(cur)
  {
    if(cur->type == type)
    {
      ret = cur;
      break;
    }
    cur = cur->next;
  }
  
  return ret;
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

const QStringList& LineNumbers::getNumbers() const
{
  return numbers;
}

int LineNumbers::getScroll() const
{
  return scroll;
}

void LineNumbers::setScroll(int scroll)
{
  this->scroll = scroll;
}

int LineNumbers::getLeftMargin()
{
  return 3;
}

int LineNumbers::getRightMargin()
{
  return 3;
}

void LineNumbers::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  
  QPainter p(this);
  
  int rt  = getRightMargin();
  int wd  = width() - rt;
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

//--------------------------------------------------------------------

EditScreen::EditScreen(TextEdit *parent, QPlainTextEdit *component)
  : QWidget(parent), edit(component)
  , spaces(false), tabulators(true)
{
#ifdef HAVE_SETTINGS
  spaces     = Settings::instance()->value("Editor/showSpaces").toBool();
  tabulators = Settings::instance()->value("Editor/showTabulators").toBool();
#endif
}

void EditScreen::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)
  
  if(!(spaces||tabulators)) return;
  
  QPainter p(this);
  
  QTextBlock b = edit->document()->findBlockByLineNumber(0);
  
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  int spacewidth = edit->fontMetrics().width(QLatin1Char(' '));
#else
  int spacewidth = edit->fontMetrics().horizontalAdvance(QLatin1Char(' '));
#endif
  int tabwidth   = edit->tabStopWidth();
  
  int w;
  
  while(b.isValid())
  {
    QString t = b.text();
    QTextCursor c = edit->textCursor();
    
    bool hasselection = c.hasSelection();
    int  selectionbeg = -1;
    int  selectionend = -1;
    if(hasselection)
    {
      selectionbeg = c.selectionStart();
      selectionend = c.selectionEnd();
    }
    
    c.setPosition(b.position());
    for(int i=0; i<t.size(); ++i)
    {
      if(t.at(i) == ' ' && spaces)
      {
        if(hasselection && c.position() < selectionend && c.position() >= selectionbeg)
             p.setPen(edit->palette().highlightedText().color());
        else p.setPen(edit->palette().text().color());
        
        w = spacewidth;
        QRect r = edit->cursorRect(c);
        p.drawLine(r.left()+2, r.bottom()-4, r.left()+2,   r.bottom());
        p.drawLine(r.left()+2, r.bottom(),   r.left()+w-1, r.bottom());
        p.drawLine(r.left()+w, r.bottom(),   r.left()+w,   r.bottom()-4);
      }
      else if(t.at(i) == '\t' && tabulators)
      {
        if(hasselection && c.position() < selectionend && c.position() >= selectionbeg)
             p.setPen(edit->palette().highlightedText().color());
        else p.setPen(edit->palette().text().color());
        
        QRect r = edit->cursorRect(c);
        QTextCursor n(c);
        
        if(n.movePosition(QTextCursor::Right))
             w = edit->cursorRect(n).left() - r.left();
        else w = tabwidth;
        
        p.drawLine(r.left()+w-4, r.top()+r.height()/1.75-4, r.left()+w, r.top()+r.height()/1.75);
        p.drawLine(r.left()+2,   r.top()+r.height()/1.75,   r.left()+w, r.top()+r.height()/1.75);
        p.drawLine(r.left()+w-4, r.top()+r.height()/1.75+4, r.left()+w, r.top()+r.height()/1.75);
      }
      c.movePosition(QTextCursor::Right);
    }
    b = b.next();
  }
}
