#ifndef TRAY_H
#define TRAY_H

#include <QWidget>
#include<QCloseEvent>
QT_BEGIN_NAMESPACE
namespace Ui { class Tray; }
QT_END_NAMESPACE

class Tray : public QWidget
{
    Q_OBJECT

public:
    Tray(QWidget *parent = nullptr);
    ~Tray();

private slots:

private:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Tray *ui;

};
#endif // TRAY_H
