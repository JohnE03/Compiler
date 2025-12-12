//
// Created by kiro3 on 12/11/2025.
//

#ifndef COMPILER_MAINWINDOW_H
#define COMPILER_MAINWINDOW_H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QLabel>
#include <string>
using namespace std;

// These two functions come from your parser/visualizer logic
int tree(string& text);
string scan(string& codeSnippet);

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Left panel (Extract Tokens)
    void browseExtractTokensFile();  // Open a file
    void saveExtractTokensFile();    // Run scanner and save output

    // Right panel (Parse Tree)
    void browseParseTreeFile();      // Open tokens file
    void generateParseTree();        // Run parser + visualizer

private:
    // Extract Tokens widgets
    QTextEdit *extractTokensTextEdit;
    QPushButton *browseExtractButton;
    QPushButton *saveExtractButton;

    // Parse Tree widgets
    QTextEdit *parseTreeTextEdit;
    QPushButton *browseParseButton;
    QPushButton *generateParseButton;

    // Stores last saved token file path (optional)
    QString lastExtractTokensOutput;
};

#endif // MAINWINDOW_H

#endif //COMPILER_MAINWINDOW_H
