#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QWidget>

class QPlainTextEdit;

class TextEdit : public QWidget
{
    Q_OBJECT
    
    QPlainTextEdit *edit;
    
public:
    TextEdit(QWidget *parent = 0);
   ~TextEdit();
};

#endif // TEXTEDIT_H