#include "student_dialog.h"
#include "common.h"
#include "gui_util.h"
#include "ops.h"
#include "storage.h"
#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

using namespace std;

StudentDialog::StudentDialog(QWidget* parent, const Student* existing)
    : QDialog(parent), editMode(existing != nullptr) {
    setWindowTitle(editMode ? U8("Изменение записи") : U8("Добавление записи"));
    setMinimumWidth(460);

    if (existing) student = *existing;
    else clearStudent(student);

    keyEdit   = new QLineEdit;
    fioEdit   = new QLineEdit;
    groupEdit = new QLineEdit;
    gpaEdit   = new QLineEdit;
    socialBox = new QCheckBox(U8("есть право на социальную стипендию"));

    // Подсказки формата перед вводом (ТЗ 4.4.5)
    keyEdit->setPlaceholderText(U8("до 11 латинских букв и цифр, например 1001"));
    fioEdit->setPlaceholderText(U8("до 95 байт (кириллица: 2 байта на букву)"));
    groupEdit->setPlaceholderText(U8("например АБ-11"));
    gpaEdit->setPlaceholderText(U8("от 2.00 до 5.00, например 4.57"));

    if (existing) {
        keyEdit->setText(QString::fromUtf8(existing->zachetka));
        keyEdit->setReadOnly(true);
        fioEdit->setText(QString::fromUtf8(existing->fio));
        groupEdit->setText(QString::fromUtf8(existing->group));
        gpaEdit->setText(QString("%1.%2").arg(existing->gpa100 / 100)
                             .arg(existing->gpa100 % 100, 2, 10, QChar('0')));
        socialBox->setChecked(existing->social != 0);
    }

    QFormLayout* form = new QFormLayout;
    form->addRow(U8("Номер зачётки:"), keyEdit);
    form->addRow(U8("ФИО:"), fioEdit);
    form->addRow(U8("Группа:"), groupEdit);
    form->addRow(U8("Средний балл:"), gpaEdit);
    form->addRow(QString(), socialBox);

    QPushButton* okButton = new QPushButton(U8("Сохранить"));
    QPushButton* cancelButton = new QPushButton(U8("Отмена"));
    okButton->setDefault(true);
    QHBoxLayout* buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(okButton);
    buttons->addWidget(cancelButton);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addLayout(buttons);

    connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void StudentDialog::onOk() {
    string errorText;
    string key   = toStd(keyEdit->text().trimmed());
    string fio   = toStd(fioEdit->text().trimmed());
    string group = toStd(groupEdit->text().trimmed());
    int32_t gpa = 0;

    if (!editMode && !validateKey(key, errorText)) {
        showWarning(this, fromStd(errorText));
        keyEdit->setFocus();
        return;
    }
    if (!validateText(fio, FIO_LEN, "ФИО", errorText)) {
        showWarning(this, fromStd(errorText));
        fioEdit->setFocus();
        return;
    }
    if (!validateText(group, GROUP_LEN, "Группа", errorText)) {
        showWarning(this, fromStd(errorText));
        groupEdit->setFocus();
        return;
    }
    if (!parseGpa(toStd(gpaEdit->text().trimmed()), gpa)) {
        showWarning(this, U8("Средний балл: введите число от 2.00 до 5.00 (не более 2 знаков после точки)."));
        gpaEdit->setFocus();
        return;
    }
    if (!editMode) {
        try {
            if (fileExists(FILE_STUDENTS) && findByKey(FILE_STUDENTS, key) >= 0) {
                showWarning(this, U8("Запись с таким номером зачётки уже существует."));
                keyEdit->setFocus();
                return;
            }
        } catch (const FileError& error) {
            showError(this, fromStd(error.what()) + U8(". Операция не выполнена."));
            return;
        }
        setText(student.zachetka, KEY_LEN, key);
    }
    setText(student.fio, FIO_LEN, fio);
    setText(student.group, GROUP_LEN, group);
    student.gpa100 = gpa;
    student.social = socialBox->isChecked() ? 1 : 0;
    calcStipend(student);
    accept();
}
