#ifndef HUNSPELLHIGHLIGHTER_H
#define HUNSPELLHIGHLIGHTER_H

#include "highlighter.h"

class Hunspell;

class HunspellHighlighter: public Highlighter
{
    Q_OBJECT
    
    Hunspell   *hunspell;
    QTextCodec *codec;
    QStringList dicts;
    
public:
    HunspellHighlighter(QObject *parent = 0);
    HunspellHighlighter(QTextDocument *document);
   ~HunspellHighlighter();
    
    bool spell(const QString& word);
    QStringList suggestions(const QString& word);
    
    const QStringList& dictionaries() const;
    bool setDictionary(const QString& name);
    
protected:
    void highlightBlock(const QString &text);
    
private:
    void init();
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
