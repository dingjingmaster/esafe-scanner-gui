//
// Created by dingjing on 22-12-1.
//

#include "scanner-result-widget-menu.h"

#include "utils/configure.h"

#include <QDebug>
#include <QMessageBox>
#include <QPushButton>

ScannerResultWidgetMenu::ScannerResultWidgetMenu(QWidget *parent)
    : QMenu (nullptr, parent), mIndex(QModelIndex())
{
    connect (addAction (QIcon(":/data/menu-del.png"), "删除"), &QAction::triggered, this, [=] (bool) {
        QMessageBox* box = new QMessageBox(this);
        box->setText ("是否确定删除？");
        box->setWindowTitle("提示");

        QPushButton* apply = new QPushButton(box);
        QPushButton* cancel = new QPushButton(box);

        apply->setText ("确定");
        cancel->setText ("取消");

        box->addButton (apply, QMessageBox::AcceptRole);
        box->addButton (cancel, QMessageBox::RejectRole);

        box->connect (apply, &QPushButton::clicked, this, [=] () {
            Q_EMIT deleteItem (mIndex);
        });
        box->exec();
        box->deleteLater();
    });

#if 0
    connect (addAction (QIcon(":/data/menu-dsm.png"), "授权加密"), &QAction::triggered, this, [=] (bool) {
        QMessageBox* box = new QMessageBox(this);
        box->setText (Configure::getInstance()->dsmPrompt());
        box->setWindowTitle("提示");

        QPushButton* apply = new QPushButton(box);
        QPushButton* cancel = new QPushButton(box);

        apply->setText ("确定");
        cancel->setText ("取消");

        box->addButton (apply, QMessageBox::AcceptRole);
        box->addButton (cancel, QMessageBox::RejectRole);

        box->connect (apply, &QPushButton::clicked, this, [=] () {
            Q_EMIT makeDSMItem(mIndex);
        });
        box->exec();
        box->deleteLater();
    });
#endif

    connect (addAction (QIcon(":/data/menu-misreport.png"), "例外文件"), &QAction::triggered, this, [=] (bool) {
        QMessageBox* box = new QMessageBox(this);
        box->setText (Configure::getInstance()->misreportPrompt());
        box->setWindowTitle("提示");

        QPushButton* apply = new QPushButton(box);
        QPushButton* cancel = new QPushButton(box);

        apply->setText ("确定");
        cancel->setText ("取消");

        box->addButton (apply, QMessageBox::AcceptRole);
        box->addButton (cancel, QMessageBox::RejectRole);

        box->connect (apply, &QPushButton::clicked, this, [=] () {
            Q_EMIT misReportItem(mIndex);
        });
        box->exec();
        box->deleteLater();
    });

}

void ScannerResultWidgetMenu::setItem(QModelIndex &idx)
{
    mIndex = idx;
}
