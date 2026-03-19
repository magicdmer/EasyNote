#ifndef NOTEWIDGET_H
#define NOTEWIDGET_H

#include <QWidget>
#include <QFont>
#include <QTextDocument>
#include "codeeditor.h"
#include <QTimer>

namespace Ui {
class NoteWidget;
}

class NoteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NoteWidget(QWidget *parent = nullptr);
    NoteWidget(QWidget *parent,QString noteName,QString fileName,QFont font);
    ~NoteWidget();

public:
    bool setFile(QString& fileName);
    bool rename(QString& newName);
    bool find(QString& text,QTextDocument::FindFlags flags);
    bool replace(QString& findText, QString& replaceText, QTextDocument::FindFlags flags, bool replaceAll = false);
    bool save(QString& filePath);
    void setTextFont(QFont& font);
    void setTabWidth(int width);
    bool isEmpty();
    void deletefile();
    bool load();
    bool save();
    void dealBackTab();

public slots:
    void sltTextChanged();
    void sltFilterEntries();

private:
    Ui::NoteWidget *ui;
    QString m_filePath;
    CodeEditor* m_textEdit;
    QString m_noteName;
    QTimer* m_typingTimer;
    bool m_textChanged;
    QString m_filterText;
};

#endif // NOTEWIDGET_H
