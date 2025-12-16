#ifndef SEISMICEVENTTABLE_H
#define SEISMICEVENTTABLE_H

#include <QWidget>
#include <QTableView>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class SeismicEventTable : public QWidget {
    Q_OBJECT

public:
    explicit SeismicEventTable(QWidget *parent = nullptr);
    ~SeismicEventTable();

    bool connectToDatabase();
    void loadData();
    void refreshTable();

private:
    void setupUI();
    void setupDatabase();
    void setupTableView();
    void applyFilter(const QString &filter);

    QTableView *m_tableView;
    QSqlTableModel *m_model;
    QSqlDatabase m_database;
    
    QPushButton *m_refreshButton;
    QPushButton *m_addButton;
    QPushButton *m_deleteButton;
    QLineEdit *m_searchBox;
    QLabel *m_statusLabel;

private slots:
    void onRefreshClicked();
    void onAddClicked();
    void onDeleteClicked();
    void onSearchTextChanged(const QString &text);
    void onRowClicked(const QModelIndex &index);

signals:
    void eventSelected(int id, double lat, double lon, double magnitude);
};

#endif // SEISMICEVENTTABLE_H
