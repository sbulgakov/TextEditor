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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QObject>

class QTextDocument;
#include <QTextBlock>
#include <QTextLayout>
class QTextBlockUserData;
class QTextCharFormat;

class Highlighter: public QObject
{
    Q_OBJECT
    
    QTextDocument *doc;
    QTextBlock     block;
    QList<QTextLayout::FormatRange> blockFormats;
    bool ignoreSignals;
    
public:
    Highlighter(QObject *parent = 0);
    Highlighter(QTextDocument *document);
    virtual ~Highlighter();
    
    QTextDocument* document() const;
    void setDocument(QTextDocument *doc);
    
public slots:
    void rehighlight();
    void rehighlightBlock(const QTextBlock& block);
    
protected:
    QTextBlock currentBlock() const;
    int currentBlockState() const;
    QTextBlockUserData* currentBlockUserData() const;
    QTextCharFormat format(int position) const;
    
    virtual void highlightBlock(const QString& text) = 0;
    
    int previousBlockState() const;
    void setCurrentBlockState(int newState);
    void setCurrentBlockUserData(QTextBlockUserData *data);
    void setFormat(int start, int count, const QTextCharFormat& format);
    void setFormat(int start, int count, const QColor& color);
    void setFormat(int start, int count, const QFont& font);
    
private slots:
    void documentChanged(int position, int charsRemoved, int charsAdded);
    void documentDestroyed();
    
private:
    void processChanges(int position, int charsRemoved, int charsAdded);
    void processBlock(const QTextBlock& block);
    
    void applyChanges();
};

#endif // HIGHLIGHTER_H
