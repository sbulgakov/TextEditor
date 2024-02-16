/*

MIT license

Copyright 2024 Stanislav Bulgakov

Permission is hereby granted, free of charge, to any
person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the
Software without restriction, including without
limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice
shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include "highlighter.h"

#include <QTextDocument>

Highlighter::Highlighter(QObject *parent)
  : QObject(parent)
  , doc(0)
  , ignoreSignals(false)
{
  setObjectName("Highlighter");
}

Highlighter::Highlighter(QTextDocument *document)
  : QObject(0)
  , doc(0)
  , ignoreSignals(false)
{
  setObjectName("Highlighter");
  setDocument(document);
}

Highlighter::~Highlighter()
{
  setDocument(0);
}

QTextDocument* Highlighter::document() const
{
  return doc;
}

void Highlighter::setDocument(QTextDocument *document)
{
  if(doc)
  {
    disconnect(doc,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
               SIGNAL(contentsChange(int,int,int)),
#else
               &QTextDocument::contentsChange,
#endif
               this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
               SLOT(documentChanged(int,int,int)));
#else
               &Highlighter::documentChanged);
#endif
    disconnect(doc,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
               SIGNAL(destroyed(QObject*)),
#else
               &QObject::destroyed,
#endif
               this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
               SLOT(documentDestroyed()));
#else
               &Highlighter::documentDestroyed);
#endif
  }
  doc = document;
  
  if(doc)
  {
    connect(doc,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SIGNAL(contentsChange(int,int,int)),
#else
            &QTextDocument::contentsChange,
#endif
            this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SLOT(documentChanged(int,int,int)));
#else
            &Highlighter::documentChanged);
#endif
    connect(doc,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SIGNAL(destroyed(QObject*)),
#else
            &QObject::destroyed,
#endif
            this,
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
            SLOT(documentDestroyed()));
#else
            &Highlighter::documentDestroyed);
#endif
  }
}

void Highlighter::rehighlight()
{
  if(ignoreSignals) return;
  if(doc)
  {
    QTextBlock b = doc->begin();
    ignoreSignals = true;
    while(b.isValid())
    {
      processBlock(b);
      b = b.next();
    }
    ignoreSignals = false;
  }
}

void Highlighter::rehighlightBlock(const QTextBlock& block)
{
  if(!ignoreSignals)
  {
    ignoreSignals = true;
    processBlock(block);
    ignoreSignals = false;
  }
}

QTextBlock Highlighter::currentBlock() const
{
  return block;
}

int Highlighter::previousBlockState() const
{
  if(block.isValid())
  {
    QTextBlock prev = block.previous();
    if(prev.isValid()) return prev.userState();
  }
  return -1;
}

int Highlighter::currentBlockState() const
{
  if(block.isValid()) return block.userState();
  
  return -1;
}

QTextBlockUserData* Highlighter::currentBlockUserData() const
{
  if(block.isValid()) return block.userData();
  
  return 0;
}

void Highlighter::setCurrentBlockState(int newState)
{
  if(block.isValid()) block.setUserState(newState);
}

void Highlighter::setCurrentBlockUserData(QTextBlockUserData *data)
{
  if(block.isValid()) block.setUserData(data);
}

QTextCharFormat Highlighter::format(int position) const
{
  QTextCharFormat format;
  
  QTextDocument *doc = this->doc;
  Highlighter *parent = qobject_cast<Highlighter*>(this->parent());
  while(parent)
  {
    if(parent->objectName().startsWith("Highlighter"))
    {
      doc = parent->doc;
      parent = qobject_cast<Highlighter*>(parent->parent());
    }
    else break;
  }
  if(doc)
  {
    QTextBlock b = doc->findBlock(position);
    if(b.isValid())
    {
      QList<QTextLayout::FormatRange> formats = blockFormats;
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
      formats.append(b.layout()->additionalFormats());
#else
      formats.append(b.layout()->formats().toList());
#endif

      for(int i = 0; i < formats.size(); ++i)
      {
        int beg = formats.at(i).start;
        int end = formats.at(i).start + blockFormats.at(i).length;
        if(beg <= position && end >= position)
        {
          format = blockFormats.at(i).format;
          break;
        }
      }
    }
  }
  
  return format;
}

void Highlighter::setFormat(int start, int count, const QTextCharFormat &format)
{
  QTextLayout::FormatRange f = {start, count, format};
  
  blockFormats.append(f);
}

void Highlighter::setFormat(int start, int count, const QColor& color)
{
  QTextCharFormat format;
  format.setForeground(color);
  
  QTextLayout::FormatRange f = {start, count, format};
  
  blockFormats.append(f);
}

void Highlighter::setFormat(int start, int count, const QFont& font)
{
  QTextCharFormat format;
  format.setFont(font);
  
  QTextLayout::FormatRange f = {start, count, format};
  
  blockFormats.append(f);
}

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
bool operator ==(const QTextLayout::FormatRange& l, const QTextLayout::FormatRange& r)
{
  return  (l.start == r.start)
        &&(l.length == r.length)
        &&(l.format.background() == r.format.background())
        &&(l.format.foreground() == r.format.foreground())
        &&(l.format.fontWeight() == r.format.fontWeight())
        &&(l.format.underlineStyle() == r.format.underlineStyle())
        &&(l.format.font() == r.format.font());
}
#endif

void Highlighter::applyChanges()
{
  QTextLayout *layout = block.layout();
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  QList<QTextLayout::FormatRange> formats = layout->additionalFormats();
#else
  QList<QTextLayout::FormatRange> formats = layout->formats().toList();
#endif
  
  if(formats == blockFormats) return;
  
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
  layout->setAdditionalFormats(blockFormats);
#else
  layout->setFormats(blockFormats.toVector());
#endif
  doc->markContentsDirty(block.position(), block.length());
}

void Highlighter::documentChanged(int position, int charsRemoved, int charsAdded)
{
  if (!ignoreSignals) processChanges(position, charsRemoved, charsAdded);
}

void Highlighter::documentDestroyed()
{
  disconnect(doc, 0, this, 0);
  doc = 0;
}

void Highlighter::processChanges(int position, int charsRemoved, int charsAdded)
{
  ignoreSignals = true;
  
  QTextBlock b = doc->findBlock(position);
  if(b.isValid())
  {
    if(charsRemoved > 0) charsRemoved = 1;
    else                 charsRemoved = 0;
    QTextBlock last = doc->findBlock(position + charsAdded + charsRemoved);
    int end;
    if(last.isValid())   end = last.position() + last.length();
    else                 end = doc->characterCount() - position;
    
    while(b.isValid() && (b.position() < end))
    {
      processBlock(b);
      b = b.next();
    }
  }
  
  ignoreSignals = false;
}

void Highlighter::processBlock(const QTextBlock &b)
{
  block = b;
  
  bool chain = false;
  if(parent())
  {
    if(parent()->objectName().startsWith("Highlighter")) chain = true;
  }
  if(!chain) blockFormats.clear();
  highlightBlock(b.text());
  QList<Highlighter*> children = findChildren<Highlighter*>();
  if(children.size())
  {
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    foreach (Highlighter *child, children) {
#else
    for(Highlighter *child : children) {
#endif
      child->blockFormats = blockFormats;
      child->processBlock(b);
      blockFormats = child->blockFormats;
    }
  }
  if(!chain) applyChanges();
  
  block = QTextBlock();
}
