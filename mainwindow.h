#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class TextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    TextEdit *textEdit;
    
public:
    MainWindow(QWidget *parent = 0);
   ~MainWindow();
};

#endif // MAINWINDOW_H
