#ifndef DATABASEVIEW_H
#define DATABASEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QPushButton>
#include <QLabel>
#include <QItemSelection>

class DatabaseView : public QWidget {
    Q_OBJECT

public:
    explicit DatabaseView(QWidget *parent = nullptr);
    ~DatabaseView();
    
    bool connectToDatabase();
    void loadData();
    void updateDateRange();
    int getSelectedEventId() const;

signals:
    void eventSelected(int eventId, double lat, double lon, double magnitude, const QString &eventInfo);

private slots:
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onSelectEvent();

private:
    void setupUI();
    void setupDatabase();
    
    QTableView *m_tableView;
    QSqlTableModel *m_model;
    QSqlDatabase m_db;
    
    QPushButton *m_btnSelect;
    QLabel *m_statusLabel;
    QLabel *m_dateRangeLabel;
    
    bool m_isConnected;
    int m_selectedEventId;
};

#endif // DATABASEVIEW_H
