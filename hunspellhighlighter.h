#ifndef HUNSPELLHIGHLIGHTER_H
#define HUNSPELLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class Hunspell;

class HunspellHighlighter: public QSyntaxHighlighter
{
    Q_OBJECT
    
    Hunspell   *hunspell;
    QTextCodec *codec;
    QStringList dicts;
    
public:
    HunspellHighlighter(QTextDocument *document);
   ~HunspellHighlighter();
    
    QStringList suggestions(const QString& word);
    
    const QStringList& dictionaries() const;
    bool setDictionary(const QString& name);
    
protected:
    void highlightBlock(const QString &text);
};

//--------------------------------------------------------------------

#include "textedit.h"

class HunspellBlockData: public TextBlockUserData
{
    QStringList m_words;
    
public:
    HunspellBlockData();
    
    const QStringList& words() const;
    void insert(const QString& word);
    void clear();
};

#endif // HUNSPELLHIGHLIGHTER_H
