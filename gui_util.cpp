#include "gui_util.h"
#include <QMessageBox>
#include <QPushButton>

using namespace std;

static void showBox(QWidget* parent, QMessageBox::Icon icon, const QString& title, const QString& text) {
    QMessageBox box(icon, title, text, QMessageBox::NoButton, parent);
    box.addButton(U8("ОК"), QMessageBox::AcceptRole);
    box.exec();
}

bool askYesNo(QWidget* parent, const QString& title, const QString& text) {
    QMessageBox box(QMessageBox::Question, title, text, QMessageBox::NoButton, parent);
    QPushButton* yes = box.addButton(U8("Да"), QMessageBox::YesRole);
    QPushButton* no  = box.addButton(U8("Нет"), QMessageBox::NoRole);
    box.setDefaultButton(no);
    box.exec();
    return box.clickedButton() == yes;
}

void showInfo(QWidget* parent, const QString& text)    { showBox(parent, QMessageBox::Information, U8("Сообщение"), text); }
void showWarning(QWidget* parent, const QString& text) { showBox(parent, QMessageBox::Warning, U8("Проверьте ввод"), text); }
void showError(QWidget* parent, const QString& text)   { showBox(parent, QMessageBox::Critical, U8("Ошибка"), text); }
