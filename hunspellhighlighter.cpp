#include "hunspellhighlighter.h"

#define HUNSPELL_STATIC
#include "hunspell/hunspell.hxx"

#include <QTextCodec>

#include <QDir>
#include <QFileInfo>
#include <QLocale>

HunspellHighlighter::HunspellHighlighter(QTextDocument *document)
  : QSyntaxHighlighter(document),
  hunspell(0), codec(0)
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
    else
      data->clear();
    
    QTextCharFormat fmt;
    fmt.setUnderlineColor(QColor("red"));
    fmt.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    
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
