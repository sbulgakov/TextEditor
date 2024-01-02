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

void TextEdit::resizeEvent(QResizeEvent *event)
{
  edit->resize(event->size());
}
