#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QWidget>

class TextEdit : public QWidget
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = 0);
   ~TextEdit();
};

#endif // TEXTEDIT_H
