#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QCheckBox;

class FindDialog : public QDialog
{
    Q_OBJECT
    
    QLabel    *label;
    QLabel    *dummy;
    QLineEdit *edit;
#ifdef FINDDIALOG_REPLACE
    QLabel    *with;
    QLineEdit *text;
#endif
    
    QCheckBox *backCheck;
#ifdef FINDDIALOG_ALL
    QCheckBox *allCheck;
#endif
    QCheckBox *caseCheck;
    QCheckBox *wholeCheck;
    QCheckBox *regexCheck;
    
    QPushButton *findButton;
#ifdef FINDDIALOG_REPLACE
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QLabel      *spacer;
#endif
    QPushButton *closeButton;
    
#ifdef FINDDIALOG_REPLACE
    bool replaceAll;
    bool replaceDialog;
    
    QSize findSize;
#endif
    
public:
    FindDialog(QWidget *parent = 0);
    
    bool isBackward();
#ifdef FINDDIALOG_ALL
    bool isAll();
#endif
    bool isCaseSensitive();
    bool isWholeWords();
    bool isRegex();
    
#ifdef FINDDIALOG_REPLACE
    bool isReplace();
    bool isReplaceAll();
#endif
    
signals:
    void find(const QString &str);
#ifdef FINDDIALOG_REPLACE
    void replace(const QString &str, const QString &with);
#endif
    
#ifdef FINDDIALOG_REPLACE
public slots:
    void setReplaceMode(bool on);
#endif
    
private slots:
    void findClicked();
#ifdef FINDDIALOG_REPLACE
    void replaceClicked();
    void replaceAllClicked();
#endif
};

//--------------------------------------------------------------------

#ifdef FINDDIALOG_RESULTS
class QTableWidget;
class QTextCursor;

class FindResults : public QDialog
{
    Q_OBJECT
    
    QTableWidget       *table;
    QList<QTextCursor> *list;
    
public:
    FindResults(QWidget *parent = 0);
   ~FindResults();
    
protected:
    void closeEvent(QCloseEvent *event);
    
signals:
    void itemClicked(const QTextCursor& c);
    void visibilityChanged(bool visible);
    
public slots:
    void clear();
    void insert(const QTextCursor& c);
    void setVisible(bool visible);
    
private slots:
    void cellClicked(int row, int column);
};
#endif // FINDDIALOG_RESULTS

#endif // FINDDIALOG_H
