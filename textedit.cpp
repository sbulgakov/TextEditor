#include "textedit.h"

#include <QPlainTextEdit>

TextEdit::TextEdit(QWidget *parent) : QWidget(parent)
{
  edit = new QPlainTextEdit(this);
  edit->setLineWrapMode(QPlainTextEdit::NoWrap);
}

TextEdit::~TextEdit()
{

}

void TextEdit::resizeEvent(QResizeEvent *event)
{
  edit->resize(event->size());
}
