#include "notewidget.h"
#include "ui_notewidget.h"
#include "helpfunc.h"
#include <QFile>
#include <QSettings>
#include <QDir>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

NoteWidget::NoteWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteWidget)
{
    ui->setupUi(this);
    m_textEdit = new CodeEditor(this);
    ui->verticalLayout->addWidget(m_textEdit);

    m_textChanged = false;

    m_typingTimer = new QTimer( this );
    m_typingTimer->setSingleShot( true );
    connect(m_typingTimer, SIGNAL(timeout()),this, SLOT(sltFilterEntries()));

    connect(m_textEdit,SIGNAL(textChanged()),this,SLOT(sltTextChanged()));
}

NoteWidget::NoteWidget(QWidget *parent,QString noteName, QString fileName, QFont font) :
    NoteWidget(parent, noteName, QString(), fileName, font)
{
}

NoteWidget::NoteWidget(QWidget *parent,QString noteName,QString groupName, QString fileName, QFont font) :
    QWidget(parent),
    ui(new Ui::NoteWidget)
{
    ui->setupUi(this);

    m_textChanged = false;

    m_textEdit = new CodeEditor(this);
    ui->verticalLayout->addWidget(m_textEdit);

    m_noteName = noteName;
    m_group = groupName;

    setTextFont(font);
    setFile(fileName);

    m_typingTimer = new QTimer( this );
    m_typingTimer->setSingleShot( true );
    connect(m_typingTimer, SIGNAL(timeout()),this, SLOT(sltFilterEntries()));

    connect(m_textEdit,SIGNAL(textChanged()),this,SLOT(sltTextChanged()));
}

NoteWidget::~NoteWidget()
{
    if (!m_textEdit->toPlainText().isEmpty())
    {
        if (m_textChanged)
            save();
    }
    else
    {
        QFile::remove(m_filePath);
    }

    delete ui;
}

void NoteWidget::sltFilterEntries()
{
    if (m_textEdit->toPlainText().isEmpty())
    {
        m_textChanged = false;
        return;
    }

    if (m_filterText.count(" ") == m_filterText.size())
    {
        m_textChanged = false;
        return;
    }

    save();

    m_textChanged = false;
}

void NoteWidget::sltTextChanged()
{
    m_textChanged = true;
    m_filterText = m_textEdit->toPlainText();
    m_typingTimer->start( 10000 );
}

bool NoteWidget::load()
{
    if (m_filePath.isEmpty())
    {
        return false;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    QString fileContent = QString::fromUtf8(file.readAll());
    file.close();

    m_textEdit->setPlainText(fileContent);

    return true;
}

bool NoteWidget::save()
{
    if (m_filePath.isEmpty())
    {
        return false;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QString fileContent = m_textEdit->toPlainText();
    file.write(fileContent.toUtf8());
    file.close();

    return true;
}

bool NoteWidget::setFile(QString &fileName)
{
    setObjectName(m_group.isEmpty() ? fileName : (m_group + QLatin1Char('/') + fileName));
    QDir().mkpath(groupPath(m_noteName, m_group));
    m_filePath = noteFilePath(m_noteName, m_group, fileName);
    if (!QFile::exists(m_filePath))
    {
        return save();
    }
    else
    {
        return load();
    }
}

bool NoteWidget::rename(QString &newName)
{
    QString newFilePath = noteFilePath(m_noteName, m_group, newName);
    if (QFile::exists(m_filePath) && !QFile::rename(m_filePath,newFilePath))
    {
        return false;
    }

    setObjectName(m_group.isEmpty() ? newName : (m_group + QLatin1Char('/') + newName));
    m_filePath = newFilePath;

    return true;
}

bool NoteWidget::find(QString &text, QTextDocument::FindFlags flags)
{
    if(!m_textEdit->find(text,flags))
    {
        if (flags & QTextDocument::FindBackward)
        {
            m_textEdit->moveCursor(QTextCursor::End);
        }
        else
        {
            m_textEdit->moveCursor(QTextCursor::Start);
        }

        if (!m_textEdit->find(text,flags))
        {
            return false;
        }
    }

    QPalette palette = m_textEdit->palette();
    palette.setColor(QPalette::Highlight,palette.color(QPalette::Active,QPalette::Highlight));
    m_textEdit->setPalette(palette);

    return true;
}

bool NoteWidget::replace(QString &findText, QString &replaceText, QTextDocument::FindFlags flags, bool replaceAll)
{
    if (findText.isEmpty())
    {
        return false;
    }

    if (replaceAll)
    {
        bool replaced = false;
        m_textEdit->moveCursor((flags & QTextDocument::FindBackward) ? QTextCursor::End : QTextCursor::Start);
        while(m_textEdit->find(findText, flags))
        {
            replaced = true;
            m_textEdit->textCursor().removeSelectedText();
            if (!replaceText.isEmpty())
                m_textEdit->textCursor().insertText(replaceText);
        }
        m_textEdit->moveCursor(QTextCursor::Start);
        return replaced;
    }
    else
    {
        QTextCursor cursor = m_textEdit->textCursor();
        if(cursor.hasSelection())
        {
            cursor.removeSelectedText();
            if (!replaceText.isEmpty())
                cursor.insertText(replaceText);
        }

        if(!m_textEdit->find(findText,flags))
        {
            if (flags & QTextDocument::FindBackward)
            {
                m_textEdit->moveCursor(QTextCursor::End);
            }
            else
            {
                m_textEdit->moveCursor(QTextCursor::Start);
            }

            if (!m_textEdit->find(findText,flags))
            {
                return false;
            }
        }


        QPalette palette = m_textEdit->palette();
        palette.setColor(QPalette::Highlight,palette.color(QPalette::Active,QPalette::Highlight));
        m_textEdit->setPalette(palette);
    }

    return true;
}

bool NoteWidget::save(QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QString fileContent = m_textEdit->toPlainText();
    file.write(fileContent.toUtf8());
    file.close();

    return true;
}

void NoteWidget::setTextFont(QFont &font)
{
    m_textEdit->setFont(font);

    QSettings setting(settingsFile(),QSettings::IniFormat,this);
    int tabWidth = setting.value("tab_width",4).toInt();
    setTabWidth(tabWidth);
}

void NoteWidget::setTabWidth(int width)
{
    QFontMetrics metrics(m_textEdit->font());
    m_textEdit->setTabStopWidth(width * metrics.width(' '));
}

bool NoteWidget::isEmpty()
{
    return m_textEdit->toPlainText().isEmpty();
}

void NoteWidget::deletefile()
{
    // 必须先处理磁盘文件再 clear, 否则进了回收站也是空文件
    if (!m_textEdit->toPlainText().isEmpty())
    {
        moveToTrash(m_filePath);
    }

    m_textEdit->clear();
}

void NoteWidget::dealBackTab()
{
    m_textEdit->dealBackTab();
}
