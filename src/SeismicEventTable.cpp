#include "SeismicEventTable.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDebug>

SeismicEventTable::SeismicEventTable(QWidget *parent) 
    : QWidget(parent), m_model(nullptr) {
    setupUI();
    setupDatabase();
}

SeismicEventTable::~SeismicEventTable() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

void SeismicEventTable::setupUI() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Toolbar
    auto *toolbarLayout = new QHBoxLayout();
    
    m_searchBox = new QLineEdit();
    m_searchBox->setPlaceholderText("Search by magnitude, location...");
    connect(m_searchBox, &QLineEdit::textChanged, this, &SeismicEventTable::onSearchTextChanged);
    
    m_refreshButton = new QPushButton("Refresh");
    connect(m_refreshButton, &QPushButton::clicked, this, &SeismicEventTable::onRefreshClicked);
    
    m_addButton = new QPushButton("Add Event");
    connect(m_addButton, &QPushButton::clicked, this, &SeismicEventTable::onAddClicked);
    
    m_deleteButton = new QPushButton("Delete");
    m_deleteButton->setEnabled(false);
    connect(m_deleteButton, &QPushButton::clicked, this, &SeismicEventTable::onDeleteClicked);
    
    toolbarLayout->addWidget(m_searchBox, 1);
    toolbarLayout->addWidget(m_refreshButton);
    toolbarLayout->addWidget(m_addButton);
    toolbarLayout->addWidget(m_deleteButton);
    
    mainLayout->addLayout(toolbarLayout);

    // Table View
    m_tableView = new QTableView();
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSortingEnabled(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    
    connect(m_tableView, &QTableView::clicked, this, &SeismicEventTable::onRowClicked);
    
    mainLayout->addWidget(m_tableView);

    // Status bar
    m_statusLabel = new QLabel("Status: Not connected");
    mainLayout->addWidget(m_statusLabel);
}

void SeismicEventTable::setupDatabase() {
    m_database = QSqlDatabase::addDatabase("QPSQL", "tsunami_connection");
    m_database.setHostName("localhost");
    m_database.setDatabaseName("tsunami_data");
    m_database.setUserName("farhan");
    m_database.setPassword("farhan"); // Isi password jika ada
    
    if (connectToDatabase()) {
        setupTableView();
        loadData();
    }
}

bool SeismicEventTable::connectToDatabase() {
    if (!m_database.open()) {
        QString error = m_database.lastError().text();
        m_statusLabel->setText("Status: Connection failed - " + error);
        QMessageBox::critical(this, "Database Error", 
            "Cannot connect to database:\n" + error);
        return false;
    }
    
    m_statusLabel->setText("Status: Connected to tsunami_data");
    return true;
}

void SeismicEventTable::setupTableView() {
    m_model = new QSqlTableModel(this, m_database);
    m_model->setTable("sumber_tsunami");
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // Set header labels
    m_model->setHeaderData(0, Qt::Horizontal, "ID");
    m_model->setHeaderData(1, Qt::Horizontal, "Latitude");
    m_model->setHeaderData(2, Qt::Horizontal, "Longitude");
    m_model->setHeaderData(3, Qt::Horizontal, "Magnitude");
    m_model->setHeaderData(4, Qt::Horizontal, "Strike");
    m_model->setHeaderData(5, Qt::Horizontal, "Dip");
    m_model->setHeaderData(6, Qt::Horizontal, "Slip");
    m_model->setHeaderData(7, Qt::Horizontal, "Origin Time");
    
    m_tableView->setModel(m_model);
    
    // Hide geometry column if exists
    int geomCol = m_model->record().indexOf("geom_source");
    if (geomCol != -1) {
        m_tableView->hideColumn(geomCol);
    }
    
    // Set column widths
    m_tableView->setColumnWidth(0, 60);   // ID
    m_tableView->setColumnWidth(1, 100);  // Lat
    m_tableView->setColumnWidth(2, 100);  // Lon
    m_tableView->setColumnWidth(3, 80);   // Magnitude
    m_tableView->setColumnWidth(4, 70);   // Strike
    m_tableView->setColumnWidth(5, 70);   // Dip
    m_tableView->setColumnWidth(6, 70);   // Slip
}

void SeismicEventTable::loadData() {
    if (m_model) {
        if (!m_model->select()) {
            QMessageBox::warning(this, "Error", 
                "Failed to load data: " + m_model->lastError().text());
        } else {
            m_statusLabel->setText(QString("Status: %1 events loaded")
                .arg(m_model->rowCount()));
        }
    }
}

void SeismicEventTable::refreshTable() {
    loadData();
}

void SeismicEventTable::applyFilter(const QString &filter) {
    if (m_model) {
        m_model->setFilter(filter);
        m_model->select();
        m_statusLabel->setText(QString("Status: %1 events found")
            .arg(m_model->rowCount()));
    }
}

void SeismicEventTable::onRefreshClicked() {
    refreshTable();
}

void SeismicEventTable::onAddClicked() {
    // TODO: Implement dialog untuk menambahkan event baru
    QMessageBox::information(this, "Add Event", 
        "Dialog untuk menambahkan event akan ditambahkan");
}

void SeismicEventTable::onDeleteClicked() {
    QModelIndex index = m_tableView->currentIndex();
    if (!index.isValid()) return;
    
    int ret = QMessageBox::question(this, "Delete Event",
        "Are you sure you want to delete this event?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        m_model->removeRow(index.row());
        if (m_model->submitAll()) {
            m_statusLabel->setText("Status: Event deleted");
            loadData();
        } else {
            QMessageBox::warning(this, "Error", 
                "Failed to delete: " + m_model->lastError().text());
            m_model->revertAll();
        }
    }
}

void SeismicEventTable::onSearchTextChanged(const QString &text) {
    if (text.isEmpty()) {
        applyFilter("");
    } else {
        // Search in magnitude or coordinates
        QString filter = QString(
            "CAST(magnitudo AS TEXT) LIKE '%%1%' OR "
            "CAST(latitude AS TEXT) LIKE '%%1%' OR "
            "CAST(longitude AS TEXT) LIKE '%%1%'"
        ).arg(text);
        applyFilter(filter);
    }
}

void SeismicEventTable::onRowClicked(const QModelIndex &index) {
    if (!index.isValid()) return;
    
    m_deleteButton->setEnabled(true);
    
    // Emit signal dengan data event
    int row = index.row();
    int id = m_model->data(m_model->index(row, 0)).toInt();
    double lat = m_model->data(m_model->index(row, 1)).toDouble();
    double lon = m_model->data(m_model->index(row, 2)).toDouble();
    double mag = m_model->data(m_model->index(row, 3)).toDouble();
    
    emit eventSelected(id, lat, lon, mag);
}
