#ifndef DATABASEVIEW_H
#define DATABASEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include <QItemSelection>

class DatabaseView : public QWidget {
    Q_OBJECT

public:
    explicit DatabaseView(QWidget *parent = nullptr);
    ~DatabaseView();
    
    bool connectToDatabase();
    void loadData();
    void loadDataWithDateFilter(const QDate &startDate, const QDate &endDate);
    QString getSelectedEventId() const;

signals:
    void eventSelected(const QString &eventId, double lat, double lon, double magnitude, 
                      int depth, int strike, int dip, int slip, const QString &eventInfo);

private slots:
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onSelectEvent();
    void onDateRangeChanged();
    void onTableDoubleClicked(const QModelIndex &index);

private:
    void setupUI();
    void setupDatabase();
    
    QTableView *m_tableView;
    QSqlQueryModel *m_model;
    QSqlDatabase m_db;
    
    QPushButton *m_btnSelect;
    QLabel *m_statusLabel;
    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;
    QPushButton *m_btnFilter;
    
    bool m_isConnected;
    QString m_selectedEventId;
};

#endif // DATABASEVIEW_H
