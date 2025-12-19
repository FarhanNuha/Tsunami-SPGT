#include "DatabaseView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDateTime>
#include <QDate>
#include <QDebug>

DatabaseView::DatabaseView(QWidget *parent) 
    : QWidget(parent)
    , m_model(nullptr)
    , m_isConnected(false)
{
    setupUI();
    setupDatabase();
}

DatabaseView::~DatabaseView() {
    if (m_db.isOpen()) {
        m_db.close();
    }
    QSqlDatabase::removeDatabase("tsunami_connection");
}

void DatabaseView::setupUI() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // Table view
    m_tableView = new QTableView();
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSortingEnabled(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    mainLayout->addWidget(m_tableView);
    
    // Date range filter section
    auto *dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("Event Date Range:"));
    
    m_startDateEdit = new QDateEdit();
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat("dd MMM yyyy");
    m_startDateEdit->setDate(QDate::currentDate().addDays(-1));
    dateLayout->addWidget(m_startDateEdit);
    
    dateLayout->addWidget(new QLabel("to"));
    
    m_endDateEdit = new QDateEdit();
    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("dd MMM yyyy");
    m_endDateEdit->setDate(QDate::currentDate());
    dateLayout->addWidget(m_endDateEdit);
    
    m_btnFilter = new QPushButton("Apply Filter");
    dateLayout->addWidget(m_btnFilter);
    dateLayout->addStretch();
    
    mainLayout->addLayout(dateLayout);
    
    // Bottom toolbar
    auto *bottomLayout = new QHBoxLayout();
    
    m_statusLabel = new QLabel("Not connected");
    m_statusLabel->setStyleSheet("padding: 5px; background-color: #44444E; border: 1px solid #715A5A;");
    bottomLayout->addWidget(m_statusLabel, 1);
    
    m_btnSelect = new QPushButton("Select Event");
    m_btnSelect->setEnabled(false);
    m_btnSelect->setMinimumWidth(120);
    bottomLayout->addWidget(m_btnSelect);
    
    mainLayout->addLayout(bottomLayout);
    
    // Connect signals
    connect(m_btnSelect, &QPushButton::clicked, this, &DatabaseView::onSelectEvent);
    connect(m_btnFilter, &QPushButton::clicked, this, &DatabaseView::onDateRangeChanged);
    connect(m_tableView, &QTableView::doubleClicked, this, &DatabaseView::onTableDoubleClicked);
}

void DatabaseView::setupDatabase() {
    if (connectToDatabase()) {
        loadData();
    }
}

bool DatabaseView::connectToDatabase() {
    QStringList drivers = QSqlDatabase::drivers();
    qDebug() << "Available SQL drivers:" << drivers;
    
    if (!drivers.contains("QPSQL")) {
        QString errorMsg = "PostgreSQL driver (QPSQL) not available.\n"
                          "Install: sudo apt-get install libqt6sql6-psql";
        m_statusLabel->setText("Driver not found");
        m_statusLabel->setStyleSheet("padding: 5px; background-color: #8B0000; color: white;");
        QMessageBox::critical(this, "Database Error", errorMsg);
        return false;
    }
    
    m_db = QSqlDatabase::addDatabase("QPSQL", "tsunami_connection");
    m_db.setHostName("localhost");
    m_db.setDatabaseName("tsunami_data");
    m_db.setUserName("farhan");
    m_db.setPassword("farhan");
    
    if (!m_db.open()) {
        QString errorMsg = QString("Connection failed: %1\n\n"
                                  "Make sure PostgreSQL is running and database is created.\n"
                                  "Run: ./setup_database.sh")
                          .arg(m_db.lastError().text());
        m_statusLabel->setText("Connection failed");
        m_statusLabel->setStyleSheet("padding: 5px; background-color: #8B0000; color: white;");
        QMessageBox::warning(this, "Database Error", errorMsg);
        return false;
    }
    
    m_isConnected = true;
    m_statusLabel->setText("Connected to tsunami_data");
    m_statusLabel->setStyleSheet("padding: 5px; background-color: #006400; color: white;");
    return true;
}

void DatabaseView::loadData() {
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();
    loadDataWithDateFilter(startDate, endDate);
}

void DatabaseView::loadDataWithDateFilter(const QDate &startDate, const QDate &endDate) {
    if (!m_isConnected) {
        QMessageBox::warning(this, "Database Error", "Not connected to database");
        return;
    }
    
    m_model = new QSqlQueryModel(this);
    
    QString queryStr = QString(
        "SELECT event_id, origintime, magnitudo, latitude, longitude, depth_km, strike, dip, slip "
        "FROM sumber_tsunami "
        "WHERE origintime BETWEEN '%1 00:00:00' AND '%2 23:59:59' "
        "ORDER BY origintime DESC"
    ).arg(startDate.toString("yyyy-MM-dd"))
     .arg(endDate.toString("yyyy-MM-dd"));
    
    m_model->setQuery(queryStr, m_db);
    
    if (m_model->lastError().isValid()) {
        m_statusLabel->setText("Query error: " + m_model->lastError().text());
        m_statusLabel->setStyleSheet("padding: 5px; background-color: #8B0000; color: white;");
        return;
    }
    
    m_model->setHeaderData(0, Qt::Horizontal, "Event ID");
    m_model->setHeaderData(1, Qt::Horizontal, "Origin Time (UTC)");
    m_model->setHeaderData(2, Qt::Horizontal, "Magnitude");
    m_model->setHeaderData(3, Qt::Horizontal, "Latitude");
    m_model->setHeaderData(4, Qt::Horizontal, "Longitude");
    m_model->setHeaderData(5, Qt::Horizontal, "Depth (km)");
    m_model->setHeaderData(6, Qt::Horizontal, "Strike");
    m_model->setHeaderData(7, Qt::Horizontal, "Dip");
    m_model->setHeaderData(8, Qt::Horizontal, "Slip");
    
    m_tableView->setModel(m_model);
    m_tableView->resizeColumnsToContents();
    m_tableView->horizontalHeader()->setSortIndicatorShown(true);
    m_tableView->horizontalHeader()->setSectionsClickable(true);
    
    // Connect selection model after setting model
    connect(m_tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &DatabaseView::onSelectionChanged);
    
    int rowCount = m_model->rowCount();
    m_statusLabel->setText(QString("Loaded %1 events from %2 to %3")
                          .arg(rowCount)
                          .arg(startDate.toString("dd MMM yyyy"))
                          .arg(endDate.toString("dd MMM yyyy")));
    m_statusLabel->setStyleSheet("padding: 5px; background-color: #006400; color: white;");
}

void DatabaseView::onDateRangeChanged() {
    loadData();
}

void DatabaseView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    Q_UNUSED(deselected);
    m_btnSelect->setEnabled(!selected.isEmpty());
}

void DatabaseView::onTableDoubleClicked(const QModelIndex &index) {
    Q_UNUSED(index);
    onSelectEvent();
}

void DatabaseView::onSelectEvent() {
    QModelIndexList selected = m_tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Select Event", "No event selected");
        return;
    }
    
    int row = selected.first().row();
    
    m_selectedEventId = m_model->data(m_model->index(row, 0)).toString();
    QDateTime originTime = m_model->data(m_model->index(row, 1)).toDateTime();
    double mag = m_model->data(m_model->index(row, 2)).toDouble();
    double lat = m_model->data(m_model->index(row, 3)).toDouble();
    double lon = m_model->data(m_model->index(row, 4)).toDouble();
    int depth = m_model->data(m_model->index(row, 5)).toInt();
    int strike = m_model->data(m_model->index(row, 6)).toInt();
    int dip = m_model->data(m_model->index(row, 7)).toInt();
    int slip = m_model->data(m_model->index(row, 8)).toInt();
    
    QString eventInfo = QString("Event ID: %1\nMagnitude %2 | %3\nLat: %4°, Lon: %5°")
                       .arg(m_selectedEventId)
                       .arg(mag, 0, 'f', 1)
                       .arg(originTime.toString("dd MMM yyyy HH:mm:ss"))
                       .arg(lat, 0, 'f', 4)
                       .arg(lon, 0, 'f', 4);
    
    emit eventSelected(m_selectedEventId, lat, lon, mag, depth, strike, dip, slip, eventInfo);
    
    m_statusLabel->setText(QString("Selected event: %1").arg(m_selectedEventId));
}

QString DatabaseView::getSelectedEventId() const {
    return m_selectedEventId;
}
