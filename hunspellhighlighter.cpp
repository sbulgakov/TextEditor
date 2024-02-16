#include "hunspellhighlighter.h"

#define HUNSPELL_STATIC
#include "hunspell/hunspell.hxx"

#include <QTextCodec>

#include <QDir>
#include <QFileInfo>
#include <QLocale>

HunspellHighlighter::HunspellHighlighter(QObject *parent)
  : Highlighter(parent),
  hunspell(0), codec(0)
{
  setObjectName("Highlighter:Hunspell");
  init();
}

HunspellHighlighter::HunspellHighlighter(QTextDocument *document)
  : Highlighter(document),
  hunspell(0), codec(0)
{
  setObjectName("Highlighter:Hunspell");
  init();
}

void HunspellHighlighter::init()
{
  if(QFileInfo("hunspell").isDir())
  {
    bool suit = false;
    
    QDir dir("hunspell");
    QStringList files = dir.entryList(QDir::Files, QDir::Name);
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    foreach (const QString& file, files) {
#else
    for(const QString& file : files) {
#endif
      if(file.endsWith(".aff"))
      {
        QString basename(file);
        basename.remove(QRegExp("\\.aff$"));
        
        dicts.append(basename);
        
        if(!suit && QLocale::system().bcp47Name() == QLocale(basename).bcp47Name())
        {
          setDictionary(basename);
          suit = true;
        }
      }
    }
  }
}

HunspellHighlighter::~HunspellHighlighter()
{
  if(hunspell)
  {
    delete hunspell;
    hunspell = 0;
  }
}

bool HunspellHighlighter::spell(const QString& word)
{
  bool correct = false;
  
  if(hunspell)
  {
    correct = hunspell->spell(std::string(codec->fromUnicode(word).constData()));
  }
  QList<HunspellHighlighter*> children = findChildren<HunspellHighlighter*>();
  if(children.size())
  {
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    foreach (HunspellHighlighter *child, children) {
#else
    for(HunspellHighlighter *child : children) {
#endif
      correct |= child->spell(word);
    }
  }
  
  return correct;
}

QStringList HunspellHighlighter::suggestions(const QString& word)
{
  QStringList list;
  
  if(hunspell)
  {
    std::vector<std::string> sugg = 
            hunspell->suggest(std::string(codec->fromUnicode(word).constData()));
    
    for(size_t i = 0; i < sugg.size(); ++i)
    {
      list.append(codec->toUnicode(sugg[i].data(),sugg[i].size()));
    }
  }
  QList<HunspellHighlighter*> children = findChildren<HunspellHighlighter*>();
  if(children.size())
  {
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    foreach (HunspellHighlighter *child, children) {
#else
    for(HunspellHighlighter *child : children) {
#endif
      list.append(child->suggestions(word));
    }
  }
  
  return list;
}

const QStringList& HunspellHighlighter::dictionaries() const
{
  return dicts;
}

bool HunspellHighlighter::setDictionary(const QString& name)
{
  if(hunspell)
  {
    delete hunspell;
    hunspell = 0;
  }
  
  if(dicts.indexOf(name) != -1)
  {
    QString aff("hunspell/" + name + ".aff");
    QString dic("hunspell/" + name + ".dic");
    
    if(QFileInfo(aff).exists() && QFileInfo(dic).exists())
    {
      hunspell = new Hunspell(aff.toStdString().data(),
                              dic.toStdString().data());
      char *enc = hunspell->get_dic_encoding();
      codec = QTextCodec::codecForName(enc);
    }
    
    return true;
  }
  
  return false;
}

void HunspellHighlighter::highlightBlock(const QString &text)
{
  bool chain = false;
  if(parent())
  {
    if(parent()->objectName().startsWith("Highlighter:Hunspell")) chain = true;
  }
  if(hunspell)
  {
    TextBlockUserData *userData = 
      static_cast<TextBlockUserData*>(currentBlockUserData());
    HunspellBlockData *data = 0;
    if(userData)       data = static_cast<HunspellBlockData*>(userData->find(1));
    if(data == 0)
    {
      data = new HunspellBlockData();
      if(userData) userData->append(data);
      else  setCurrentBlockUserData(data);
    }
    else if(!chain)
      data->clear();
    
    QTextCharFormat fmt;
    fmt.setUnderlineColor(QColor("red"));
    fmt.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    
    if(chain)
    {
      fmt.setUnderlineStyle(QTextCharFormat::NoUnderline);
      QStringList words = data->words();
      words.removeDuplicates();
      
      QStringList::Iterator i = words.begin();
      while(i != words.end())
      {
        bool corr = hunspell->spell(std::string(codec->fromUnicode(*i).constData()));
        if(corr)
        {
          QString word(*i);
          i = words.erase(i);
          int len = word.length();
          int pos = 0;
          while((pos = text.indexOf(word, pos)) != -1)
          {
            setFormat(pos, len, fmt);
            pos += len;
          }
          continue;
        }
        ++i;
      }
      data->clear();
      for(int i = 0; i < words.size(); ++i) data->insert(words.at(i));
      
      return;
    }
    
    QRegExp word("(\\w+)");
    
    int pos = 0;
    while((pos = word.indexIn(text, pos)) != -1)
    {
      QString cap = word.cap(1);
      int     len = word.matchedLength();
      
      bool corr = hunspell->spell(std::string(codec->fromUnicode(cap).constData()));
     
      if(!corr)
      {
        setFormat(pos, len, fmt);
        
        data->insert(cap);
      }
      
      pos += len;
    }
  }
}

//--------------------------------------------------------------------

HunspellBlockData::HunspellBlockData()
  : TextBlockUserData(1)
{

}

const QStringList& HunspellBlockData::words() const
{
  return m_words;
}

void HunspellBlockData::insert(const QString& word)
{
  m_words.append(word);
}

void HunspellBlockData::clear()
{
  m_words.clear();
}
