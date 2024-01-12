#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class TextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    TextEdit *textEdit;
    QString   fileName;
    QString   filePath;
    
    QMenu   *fileMenu;
    QAction *fileActNew;
    QAction *fileActOpen;
    QAction *fileActSave;
    QAction *fileActSaveAs;
    QAction *fileActExit;
    
    QMenu   *editMenu;
    QAction *editActFind;
    
    QMenu   *helpMenu;
    QAction *helpActAbout;
    
public:
    MainWindow(QWidget *parent = 0);
   ~MainWindow();
    
private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void editFind();
    void helpAbout();
};

#endif // MAINWINDOW_H
