#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QWidget>

class QPlainTextEdit;

class LineNumbers;
class FindDialog;
#ifdef FINDDIALOG_RESULTS
class FindResults;
#endif
#ifdef TEXTEDIT_MENU
class QMenu;
class QAction;
#endif
class Highlighter;
class QTextCursor;

class TextEdit : public QWidget
{
    Q_OBJECT
    
    QPlainTextEdit *edit;
    LineNumbers *lineNumbers;
    int pos;
    int blockCount;
    
    FindDialog  *findDialog;
#ifdef FINDDIALOG_RESULTS
    FindResults *findResults;
#endif
    
#ifdef TEXTEDIT_MENU
    QMenu   *editMenu;
    QAction *editActUndo;
    QAction *editActRedo;
    QAction *editActCut;
    QAction *editActCopy;
    QAction *editActPaste;
    QAction *editActDelete;
    QAction *editActSelectAll;
    
    bool undo;
    bool redo;
#endif
    
    Highlighter *highlighter;
    
    QWidget *screen;
    
public:
    TextEdit(QWidget *parent = 0);
   ~TextEdit();
    
    int lineNumbersWidth() const;
    
#ifdef TEXTEDIT_MENU
    QMenu* createStandardContextMenu();
    void   updateStandardContextMenu(QMenu *menu);
#endif
    
public slots:
    bool Open(const QString& fileName);
    bool Save(const QString& fileName);
    
    bool Find(const QString& str);
    void showFindDialog();
#ifdef FINDDIALOG_RESULTS
    FindResults* getFindResults();
#endif
    
#ifdef FINDDIALOG_REPLACE
    bool Replace(const QString& str, const QString& with);
    void showReplaceDialog();
#endif
    
    void setTextCursor(const QTextCursor &cursor);
    
private slots:
    void updateLineNumbersWidth(int newBlockCount);
    void updateLineNumbers(const QRect &rect, int dy);
    void updateLineNumbers(int val);
    void updateLineNumbers();
    
    void highlightCurrentLine();
    
#ifdef TEXTEDIT_MENU
    void editMenuRequested(const QPoint& pos);
    void editDelete();
    void editCanUndo(bool yes);
    void editCanRedo(bool yes);
#endif

#ifdef HAVE_HUNSPELL
    void correctWord();
#endif

protected:
    void resizeEvent(QResizeEvent *event);
    
#ifdef TEXTEDIT_MENU
private:
    void createMenu(QMenu *menu);
#endif
};

//--------------------------------------------------------------------

#include <QTextBlockUserData>

class TextBlockUserData: public QTextBlockUserData
{
    TextBlockUserData *next;
    ushort             type;
    
public:
    TextBlockUserData();
    TextBlockUserData(ushort type);
   ~TextBlockUserData();
    
    void append(TextBlockUserData* data);
    TextBlockUserData* find(ushort type);
};

//--------------------------------------------------------------------

class LineNumbers: public QWidget
{
    TextEdit *edit;
    QStringList numbers;
    int scroll;
    
public:
    LineNumbers(TextEdit *parent = 0);
    virtual void  setNumbers(const QStringList& numbers);
    virtual const QStringList& getNumbers() const;
    
    virtual int   getScroll() const;
    virtual void  setScroll(int scroll);
    
    virtual int   getLeftMargin();
    virtual int   getRightMargin();
    
protected:
    void paintEvent(QPaintEvent *event);
};

//--------------------------------------------------------------------

class EditScreen: public QWidget
{
    QPlainTextEdit *edit;
    
    bool spaces;
    bool tabulators;
    
public:
    EditScreen(TextEdit *parent, QPlainTextEdit *component);
    
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // TEXTEDIT_H
