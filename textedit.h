#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QWidget>

class QPlainTextEdit;

class LineNumbers;

class TextEdit : public QWidget
{
    Q_OBJECT
    
    QPlainTextEdit *edit;
    LineNumbers *lineNumbers;
    int pos;
    int blockCount;
    
public:
    TextEdit(QWidget *parent = 0);
   ~TextEdit();
    
    int lineNumbersWidth() const;
    
public slots:
    bool Open(const QString& fileName);
    bool Save(const QString& fileName);
    
private slots:
    void updateLineNumbersWidth(int newBlockCount);
    void updateLineNumbers(const QRect &rect, int dy);
    void updateLineNumbers(int val);
    void updateLineNumbers();
    
    void highlightCurrentLine();

protected:
    void resizeEvent(QResizeEvent *event);
};

//--------------------------------------------------------------------

class LineNumbers: public QWidget
{
    TextEdit *edit;
    QStringList numbers;
    int scroll;
    
public:
    LineNumbers(TextEdit *parent = 0);
    void setNumbers(const QStringList& numbers);
    
    inline int  getScroll() const;
    inline void setScroll(int scroll);
    
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // TEXTEDIT_H
