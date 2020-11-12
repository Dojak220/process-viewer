#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"

#include <time.h>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(refreshTable()));

    pid_selected = new QTableWidgetItem();

    ui->ledit_pid->setText("");

    tableConfiguration();
    refreshTable();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::tableConfiguration() {
    int n_columns = 12;

    ui->tw_processes->setColumnCount(n_columns); //PID, USER, PRI, NI, VIRT, RES, SHR, S, CPU, MEM, TIME+, Command

    // Eu não tenho o SHR, eu chamo o TRS no lugar dele! No htop ele mostra o SHR!
    QStringList table_headers = {"PID", "USER", "PRI", "NI", "VIRT", "RES", "TRS", "S", "CPU(%)", "MEM(%)", "TIME+", "COMMAND"};
    ui->tw_processes->setHorizontalHeaderLabels(table_headers);
    ui->tw_processes->verticalHeader()->setVisible(false);
    ui->tw_processes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_processes->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tw_processes->setColumnWidth(0, 50);   // PID
    ui->tw_processes->setColumnWidth(1, 100);   // USER
    ui->tw_processes->setColumnWidth(2, 40);   // PRI
    ui->tw_processes->setColumnWidth(3, 35);   // NI
    ui->tw_processes->setColumnWidth(4, 90);   // VIRT
    ui->tw_processes->setColumnWidth(5, 70);   // RES
    ui->tw_processes->setColumnWidth(6, 70);   // TRS
    ui->tw_processes->setColumnWidth(7, 25);   // S
    ui->tw_processes->setColumnWidth(8, 65);   // CPU(%)
    ui->tw_processes->setColumnWidth(9, 70);   // MEM(%)
    ui->tw_processes->setColumnWidth(10, 70);   // TIME+
    ui->tw_processes->setColumnWidth(11, 479);   // COMMAND
}

void MainWindow::refreshTable() {
/*
    int row = ui->tw_processes->currentRow();
    cout << "pid_row: " << row << "\n";
    if (row >= 0) {
        pid_selected = ui->tw_processes->item(row, 0);

        cout << pid_selected->text().toUtf8().constData() << "\n";
    }

    fillTable();

    if (pid_selected != nullptr) {
        ui->tw_processes->selectRow(pid_selected->row());
    }
*/

    fillTable();
    timer->start();
}

void MainWindow::fillTable() {
    ui->tw_processes->setRowCount(0); // Elimina todas as linhas, limpando automaticamente os dados delas.

    ifstream processes_list;
    string process_data;

    // Variáveis referentes às colunas da tabela
    string pid, user, pri, nice, vsz, rss, trs, state, cpu, mem, time, command;

    // Criação do arquivo com os dados instantâneos dos processos
    if (filter_text == "") {
        sprintf(bash_command, "ps -eo pid,user,pri,nice,vsz,rss,trs,state,pcpu,pmem,time,command --sort=-pcpu > ps--eo.log");
        system(bash_command);
    } else {
        QByteArray ba = filter_text.toUtf8();
        const char *c_str2 = ba.data();
        sprintf(bash_command, "ps -eo pid,user,pri,nice,vsz,rss,trs,state,pcpu,pmem,time,command --sort=-pcpu | grep '%s' > ps--eo.log", c_str2);
        system(bash_command);
    }

    processes_list.open ("ps--eo.log");
    if (processes_list.is_open()) {
        int line = 0;

        getline(processes_list, process_data); // Isso faz com que a primeira linha seja pulada
        while ( getline (processes_list,process_data) ) {
            stringstream ss(process_data);
            ss >> pid >> user >> pri >> nice >> vsz >> rss >> trs >> state >> cpu >> mem >> time;
            getline(ss, command);

            ui->tw_processes->insertRow(line);
            ui->tw_processes->setItem(line, 0, new QTableWidgetItem(QString::fromStdString(pid)));
            ui->tw_processes->setItem(line, 1, new QTableWidgetItem(QString::fromStdString(user)));
            ui->tw_processes->setItem(line, 2, new QTableWidgetItem(QString::fromStdString(pri)));
            ui->tw_processes->setItem(line, 3, new QTableWidgetItem(QString::fromStdString(nice)));
            ui->tw_processes->setItem(line, 4, new QTableWidgetItem(QString::fromStdString(vsz)));
            ui->tw_processes->setItem(line, 5, new QTableWidgetItem(QString::fromStdString(rss)));
            ui->tw_processes->setItem(line, 6, new QTableWidgetItem(QString::fromStdString(trs)));
            ui->tw_processes->setItem(line, 7, new QTableWidgetItem(QString::fromStdString(state)));
            ui->tw_processes->setItem(line, 8, new QTableWidgetItem(QString::fromStdString(cpu)));
            ui->tw_processes->setItem(line, 9, new QTableWidgetItem(QString::fromStdString(mem)));
            ui->tw_processes->setItem(line, 10, new QTableWidgetItem(QString::fromStdString(time)));
            ui->tw_processes->setItem(line, 11, new QTableWidgetItem(QString::fromStdString(command)));

            line++;
        }
        processes_list.close();
    }
    else cout << "Unable to open file";
    return;
}

void MainWindow::on_btn_matar_clicked()
{
    pid = ui->ledit_pid->text();
    if (pid == "") {
        QMessageBox::information(this, tr("Sem PID"), tr("Digite um valor para PID."));
        return;
    }
    kill((pid_t)pid.toInt(), SIGKILL);
}
void MainWindow::on_btn_suspender_clicked()
{
    pid = ui->ledit_pid->text();
    if (pid == "") {
        QMessageBox::information(this, tr("Sem PID"), tr("Digite um valor para PID."));
        return;
    }
    kill((pid_t)pid.toInt(), SIGSTOP);
}
void MainWindow::on_btn_continuar_clicked()
{
    pid = ui->ledit_pid->text();
    if (pid == "") {
        QMessageBox::information(this, tr("Sem PID"), tr("Digite um valor para PID."));
        return;
    }
    kill((pid_t)pid.toInt(), SIGCONT);
}
void MainWindow::on_ledit_filter_textChanged(const QString &arg1)
{
    filter_text = arg1;
}
void MainWindow::on_btn_prio_clicked()
{
        pid = ui->ledit_pid->text();
        ni = ui->ledit_prio->text();
        if (pid == "") {
            QMessageBox::information(this, tr("Sem PID"), tr("Digite um valor para PID."));
            return;
        }
        if (ni == "") {
            QMessageBox::information(this, tr("Sem NI"), tr("Digite um valor para NI."));
            return;
        }

        sprintf(bash_command, "renice %d -p %d", ni.toInt(), pid.toInt());
        system(bash_command);
}
void MainWindow::on_btn_cpu_clicked()
{
    pid = ui->ledit_pid->text();
    cpu = ui->ledit_cpu->text();
    if (pid == "") {
        QMessageBox::information(this, tr("Sem PID"), tr("Digite um valor para PID."));
        return;
    }
    if (cpu == "") {
        QMessageBox::information(this, tr("Sem CPU"), tr("Digite um valor para CPU."));
        return;
    }
    printf("mundando cpu");
    sprintf(bash_command, "taskset -cp %d %d", cpu.toInt(), pid.toInt());
    system(bash_command);
}
