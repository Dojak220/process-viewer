#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <signal.h> //kill()
#include <QTimer>
#include <QTableWidgetItem>
#include <QDir>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void tableConfiguration();

    void on_btn_matar_clicked();

    void on_btn_suspender_clicked();

    void on_btn_continuar_clicked();

    void on_btn_m_prio_clicked();

    void on_btn_l_prio_clicked();

    void refreshTable();

    void fillTable();

    void on_ledit_filter_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QTableWidgetItem *pid_selected;
    QString filter_text = "";
    char bash_command[105] = "";
};
#endif // MAINWINDOW_H
