#ifndef CODECOUNT_H
#define CODECOUNT_H

#include <QWidget>
#include<QCloseEvent>
namespace Ui {
class CodeCount;
}

class CodeCount : public QWidget
{
    Q_OBJECT
public:
    static CodeCount* getInstance();
    explicit CodeCount(QWidget *parent = nullptr);
    ~CodeCount();
private:
    static QScopedPointer<CodeCount> self;
    Ui::CodeCount  *ui;
    QStringList listFile;
private:
    void initForm();
    bool checkFile(const QString& fileName);
    void countCode(const QString& filePath);
    void countCode(const QStringList& files);
    void countCode(const QString& fileName,
                   int& lineCode,
                   int& lineBlank,
                   int& lineNotes);

signals:

private slots:
    void on_btnOpenFile_clicked();
    void on_btnOpenPath_clicked();
    void on_btnClear_clicked();
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // CODECOUNT_H
