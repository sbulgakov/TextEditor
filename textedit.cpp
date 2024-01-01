#include "textedit.h"

#include <QPlainTextEdit>

TextEdit::TextEdit(QWidget *parent) : QWidget(parent)
{
  edit = new QPlainTextEdit(this);
}

TextEdit::~TextEdit()
{

}
