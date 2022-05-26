#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_syncButton_clicked();

    void on_diffButton_clicked();

    void on_incButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
