#pragma once
#include <QString>
#include <QWidget>
#include <string>

#define U8(x) QString::fromUtf8(x)

inline std::string toStd(const QString& text) {
    QByteArray bytes = text.toUtf8();
    return std::string(bytes.constData(), bytes.size());
}
inline QString fromStd(const std::string& text) { return QString::fromUtf8(text.c_str()); }

bool askYesNo(QWidget* parent, const QString& title, const QString& text);
void showInfo(QWidget* parent, const QString& text);
void showWarning(QWidget* parent, const QString& text);
void showError(QWidget* parent, const QString& text);
