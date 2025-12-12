//
// Created by kiro3 on 12/11/2025.
//
#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "../Compiler Logic/Scanner.h"
#include "../Compiler Logic/Parser.h"
#include "visualizer.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    // Create central widget for the window
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // MAIN horizontal layout – splits window into LEFT and RIGHT panels
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // ===========================
    //  LEFT SIDE (Extract Tokens)
    // ===========================
    QVBoxLayout *extractTokensLayout = new QVBoxLayout();

    extractTokensLayout->addWidget(new QLabel("Extract Tokens", this));

    extractTokensTextEdit = new QTextEdit(this);
    extractTokensLayout->addWidget(extractTokensTextEdit);

    browseExtractButton = new QPushButton("Browse", this);
    extractTokensLayout->addWidget(browseExtractButton);
    connect(browseExtractButton, &QPushButton::clicked,
            this, &MainWindow::browseExtractTokensFile);

    saveExtractButton = new QPushButton("Extract Tokens and Save", this);
    extractTokensLayout->addWidget(saveExtractButton);
    connect(saveExtractButton, &QPushButton::clicked,
            this, &MainWindow::saveExtractTokensFile);

    // ==========================
    //  RIGHT SIDE (Parse Tree)
    // ==========================
    QVBoxLayout *parseTreeLayout = new QVBoxLayout();

    parseTreeLayout->addWidget(new QLabel("Parse Tree", this));

    parseTreeTextEdit = new QTextEdit(this);
    parseTreeLayout->addWidget(parseTreeTextEdit);

    browseParseButton = new QPushButton("Browse", this);
    parseTreeLayout->addWidget(browseParseButton);
    connect(browseParseButton, &QPushButton::clicked,
            this, &MainWindow::browseParseTreeFile);

    generateParseButton = new QPushButton("Generate Parse Tree", this);
    parseTreeLayout->addWidget(generateParseButton);
    connect(generateParseButton, &QPushButton::clicked,
            this, &MainWindow::generateParseTree);

    // Add both vertical layouts to the MAIN horizontal layout
    mainLayout->addLayout(extractTokensLayout);
    mainLayout->addLayout(parseTreeLayout);

    // Window setup
    setWindowTitle("Token Extractor and Parse Tree Viewer");
    resize(1200, 600);

    // Dark theme styling
    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #1e1e1e;
        }
        QTextEdit {
            background-color: #252526;
            color: #ffffff;
            border: 1px solid #3c3c3c;
            font-size: 14px;
        }
        QLabel {
            color: #ffffff;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton {
            background-color: #0078d4;
            color: white;
            border: none;
            padding: 6px 10px;
            border-radius: 4px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #1a9aff;
        }
        QPushButton:pressed {
            background-color: #005a9e;
        }
    )");
}

MainWindow::~MainWindow() {}
void MainWindow::browseExtractTokensFile() {
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Open File for Extract Tokens"),
            "",
            tr("Text Files (*.txt);;All Files (*)")
    );

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            extractTokensTextEdit->setPlainText(in.readAll());
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Unable to open the file for Extract Tokens"));
        }
    }
}
void MainWindow::saveExtractTokensFile() {

    // Get the text from the Extract Tokens editor
    QString tokensText = extractTokensTextEdit->toPlainText();

    if (tokensText.isEmpty()) {
        QMessageBox::information(this, tr("No Input"),
                                 tr("The Extract Tokens editor is empty."));
        return;
    }

    string codeSnippet = tokensText.toStdString();
    string x;

    try {
        // Run scanner on user code
        x = scan(codeSnippet);

        QString outputText = QString::fromStdString(x);

        // Ask user where to save the token output
        QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save Extract Tokens File"),
                "",
                tr("Text Files (*.txt);;All Files (*)")
        );

        if (!fileName.isEmpty()) {
            QFile file(fileName);

            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << outputText;
                file.close();

                lastExtractTokensOutput = fileName;

                // Load saved token file into Parse Tree editor
                QFile savedFile(fileName);

                if (savedFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&savedFile);
                    parseTreeTextEdit->setPlainText(in.readAll());
                    savedFile.close();
                }
            } else {
                QMessageBox::warning(this, tr("Error"),
                                     tr("Unable to save the file."));
            }
        }
    }
    catch (const std::exception& e) {
        QMessageBox::information(this, tr("Error"),
                                 tr("An error occurred:\n%1").arg(e.what()));

        return;
    }

}
void MainWindow::browseParseTreeFile() {
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Open File for Parse Tree"),
            "",
            tr("Text Files (*.txt);;All Files (*)")
    );

    if (!fileName.isEmpty()) {
        QFile file(fileName);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            parseTreeTextEdit->setPlainText(in.readAll());
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Unable to open the file for Parse Tree"));
        }
    }
}
void MainWindow::generateParseTree() {
    QString inputText = parseTreeTextEdit->toPlainText();

    if (inputText.isEmpty()) {
        QMessageBox::information(this, tr("Generate Parse Tree"),
                                 tr("No input provided for Parse Tree generation."));
        return;
    }

    // Call the tree function with inputText
    string s= inputText.toStdString();
    int result = tree( s);

    if (result == 0) {  // 0 means success
        QMessageBox::information(this, tr("Parse Tree"),
                                 tr("Parse Tree generated successfully."));
    } else {
        QMessageBox::warning(this, tr("Parse Tree"),
                             tr("Failed to generate Parse Tree."));
    }
}

string scan(string& codeSnippet) {
    return scanner(codeSnippet);
}


int tree(string &text) {
   vector<TokenRecord> tokens = readTokensFromString(text);
    Parser parser;
    Node* root = parser.parse(tokens);
    int visResult = visualizeTree(root);

    return visResult; // Return 0 for success
}
