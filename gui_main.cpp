#include <QApplication>
#include "login_dialog.h"
#include "main_window.h"

using namespace std;

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setApplicationName("ISNS-EP");

    // Перед работой с данными нужна успешная проверка учётных данных (ТЗ 4.2.9)
    LoginDialog login;
    if (login.exec() != QDialog::Accepted) return 0;

    MainWindow window;
    window.show();
    return app.exec();
}
