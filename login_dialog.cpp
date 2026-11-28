#include "login_dialog.h"
#include "auth.h"
#include "common.h"
#include "gui_util.h"
#include "student.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

using namespace std;

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), firstRun(!usersExist()), attempts(0) {
    setWindowTitle(U8("ИСНС-ЭП — вход"));
    setMinimumWidth(420);

    QLabel* title = new QLabel(U8("<h2>ИС начисления стипендий (ИСНС-ЭП)</h2>"
                                  "с имитацией электронной подписи ведомостей"));
    hint = new QLabel;
    hint->setWordWrap(true);

    loginEdit  = new QLineEdit;
    passEdit   = new QLineEdit;
    repeatEdit = new QLineEdit;
    passEdit->setEchoMode(QLineEdit::Password);
    repeatEdit->setEchoMode(QLineEdit::Password);
    loginEdit->setPlaceholderText(U8("логин"));
    passEdit->setPlaceholderText(U8("пароль"));
    repeatLabel = new QLabel(U8("Повторите пароль:"));

    QFormLayout* form = new QFormLayout;
    form->addRow(U8("Логин:"), loginEdit);
    form->addRow(U8("Пароль:"), passEdit);
    form->addRow(repeatLabel, repeatEdit);

    message = new QLabel;
    message->setStyleSheet("color: #c62828;");
    message->setWordWrap(true);

    okButton = new QPushButton;
    okButton->setDefault(true);
    QPushButton* exitButton = new QPushButton(U8("Выход"));
    QHBoxLayout* buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(okButton);
    buttons->addWidget(exitButton);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->addWidget(title);
    root->addWidget(hint);
    root->addLayout(form);
    root->addWidget(message);
    root->addLayout(buttons);

    connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(exitButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(passEdit, SIGNAL(returnPressed()), this, SLOT(onOk()));
    connect(repeatEdit, SIGNAL(returnPressed()), this, SLOT(onOk()));

    switchMode(firstRun);
}

void LoginDialog::switchMode(bool first) {
    firstRun = first;
    repeatLabel->setVisible(first);
    repeatEdit->setVisible(first);
    okButton->setText(first ? U8("Создать учётную запись") : U8("Войти"));
    hint->setText(first ? U8("Первый запуск: задайте логин и пароль (не короче 4 символов).")
                        : U8("Введите логин и пароль."));
    message->clear();
    repeatEdit->clear();
}

void LoginDialog::onOk() {
    QString login = loginEdit->text().trimmed();
    QString password = passEdit->text();
    try {
        if (firstRun) {
            string errorText;
            if (!validateText(toStd(login), 32, "логин", errorText)) {
                message->setText(fromStd(errorText));
                return;
            }
            if (password.size() < 4) {
                message->setText(U8("Пароль слишком короткий (минимум 4 символа)."));
                return;
            }
            if (password != repeatEdit->text()) {
                message->setText(U8("Пароли не совпадают."));
                return;
            }
            createUser(toStd(login), toStd(password));
            showInfo(this, U8("Учётная запись создана. Теперь выполните вход."));
            passEdit->clear();
            switchMode(false);
            return;
        }
        if (checkLogin(toStd(login), toStd(password))) {
            accept();
            return;
        }
        ++attempts;
        passEdit->clear();
        if (attempts >= 3) {
            showError(this, U8("Три неверные попытки входа. Доступ запрещён."));
            reject();
            return;
        }
        // Сообщение не раскрывает ни хеш, ни служебные данные учётной записи (ТЗ 4.2.9)
        message->setText(U8("Неверный логин или пароль (попытка %1 из 3).").arg(attempts));
    } catch (const FileError& error) {
        showError(this, fromStd(error.what()) + U8(".\nЕсли файл учётной записи повреждён, удалите ") +
                        U8(FILE_USERS) + U8(" и запустите программу снова."));
        if (!firstRun) reject();
    }
}
