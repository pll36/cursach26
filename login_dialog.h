#pragma once
#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    LoginDialog(QWidget* parent = nullptr);

private slots:
    void onOk();

private:
    void switchMode(bool first);

    bool firstRun;
    int attempts;
    QLabel* hint;
    QLabel* message;
    QLabel* repeatLabel;
    QLineEdit* loginEdit;
    QLineEdit* passEdit;
    QLineEdit* repeatEdit;
    QPushButton* okButton;
};
