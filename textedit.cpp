#include "textedit.h"

#include <QPlainTextEdit>
#include <QMessageBox>
#include <QTextStream>

TextEdit::TextEdit(QWidget *parent) : QWidget(parent)
{
  edit = new QPlainTextEdit(this);
  edit->setLineWrapMode(QPlainTextEdit::NoWrap);
}

TextEdit::~TextEdit()
{

}

void TextEdit::Open(const QString& fileName)
{
  edit->clear();
  
  if( fileName != "" )
  {
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
      QMessageBox::critical(this, tr("Error"),
        tr("Unable to open file."));
      return;
    }
    
    QTextStream in(&file);
    edit->setPlainText(in.readAll());
    
    file.close();
  }
}

void TextEdit::Save(const QString& fileName)
{
    if( fileName != "" )
    {
      QFile file(fileName);
      if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
      {
        QMessageBox::critical(this, tr("Error"),
          tr("Unable to open file."));
        return;
      }
      
      QTextStream out(&file);
      out << edit->toPlainText();
      
      file.close();
    }
}

void TextEdit::resizeEvent(QResizeEvent *event)
{
  edit->resize(event->size());
}
