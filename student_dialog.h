#pragma once
#include <QDialog>
#include "student.h"

class QCheckBox;
class QLineEdit;

class StudentDialog : public QDialog {
    Q_OBJECT

public:
    StudentDialog(QWidget* parent, const Student* existing);
    Student result() const { return student; }

private slots:
    void onOk();

private:
    Student student;
    bool editMode;
    QLineEdit* keyEdit;
    QLineEdit* fioEdit;
    QLineEdit* groupEdit;
    QLineEdit* gpaEdit;
    QCheckBox* socialBox;
};
