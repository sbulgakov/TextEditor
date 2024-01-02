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
    
public slots:
    bool Open(const QString& fileName);
    bool Save(const QString& fileName);
    
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // TEXTEDIT_H
