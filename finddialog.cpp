#include "finddialog.h"
#include "ui_finddialog.h"
#include "mainwindow.h"
#include <QTextDocument>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);

    connect(ui->pushButtonFind,SIGNAL(clicked()),this,SLOT(sltPushFindClicked()));
    connect(ui->pushButtonReplace,SIGNAL(clicked()),this,SLOT(sltPushReplaceClicked()));
    connect(ui->pushButtonReplaceAll,SIGNAL(clicked()),this,SLOT(sltPushReplaceAllClicked()));
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::setReplace(bool replace)
{
    if (replace)
    {
        ui->tabWidget->setCurrentIndex(1);
        ui->lineEditFind->setFocus();
    }
    else
    {
        ui->tabWidget->setCurrentIndex(0);
        ui->lineEdit->setFocus();
    }
}

void FindDialog::sltPushFindClicked()
{
    ui->labelError->clear();

    MainWindow* parent = (MainWindow*)this->parent();
    QString text = ui->lineEdit->text();

    QTextDocument::FindFlags flags;
    if (ui->checkBoxCaseSensitive->isChecked())
    {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (ui->radioButtonBackward->isChecked())
    {
        flags |= QTextDocument::FindBackward;
    }

    if (!parent->find(text,flags))
    {
        ui->labelError->setStyleSheet(tr("color:red;"));
        ui->labelError->setText(tr("未找到"));
    }
}

void FindDialog::sltPushReplaceClicked()
{
    ui->labelError->clear();

    MainWindow* parent = (MainWindow*)this->parent();
    QString text_find = ui->lineEditFind->text();
    QString text_replace = ui->lineEditReplace->text();

    QTextDocument::FindFlags flags;
    if (ui->checkBoxCaseSensitive_2->isChecked())
    {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (ui->radioButtonBackward_2->isChecked())
    {
        flags |= QTextDocument::FindBackward;
    }

    if (!parent->replace(text_find, text_replace, flags))
    {
        ui->labelError->setStyleSheet(tr("color:red;"));
        ui->labelError->setText(tr("未找到更多"));
    }
}

void FindDialog::sltPushReplaceAllClicked()
{
    ui->labelError->clear();

    MainWindow* parent = (MainWindow*)this->parent();
    QString text_find = ui->lineEditFind->text();
    QString text_replace = ui->lineEditReplace->text();

    QTextDocument::FindFlags flags;
    if (ui->checkBoxCaseSensitive_2->isChecked())
    {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (ui->radioButtonBackward_2->isChecked())
    {
        flags |= QTextDocument::FindBackward;
    }

    if (!parent->replace(text_find, text_replace, flags, true))
    {
        ui->labelError->setText(tr("未找到"));
    }

    ui->labelError->setStyleSheet(tr("color:black;"));
    ui->labelError->setText(tr("全部替换成功"));
}

