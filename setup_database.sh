#!/bin/bash

# Setup PostgreSQL database untuk tsunami monitoring system
# User: farhan, Password: farhan, Database: tsunami_data

echo "Setting up tsunami_data database..."
echo ""

# Check if PostgreSQL is running
if ! sudo systemctl is-active --quiet postgresql; then
    echo "Starting PostgreSQL service..."
    sudo systemctl start postgresql
fi

# Check if PostGIS is installed
if ! dpkg -l | grep -q postgis; then
    echo "PostGIS not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y postgresql-14-postgis-3 postgis
fi

# Create database if not exists
sudo -u postgres psql -c "SELECT 1 FROM pg_database WHERE datname = 'tsunami_data'" | grep -q 1 || \
sudo -u postgres psql -c "CREATE DATABASE tsunami_data;"

# Grant privileges to user farhan
sudo -u postgres psql -c "CREATE USER farhan WITH PASSWORD 'farhan';" 2>/dev/null || true
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE tsunami_data TO farhan;"

# Enable PostGIS extension
sudo -u postgres psql -d tsunami_data -c "CREATE EXTENSION IF NOT EXISTS postgis;"

# Create table
PGPASSWORD=farhan psql -h localhost -U farhan -d tsunami_data << EOF
-- Drop table if exists for clean setup
DROP TABLE IF EXISTS sumber_tsunami CASCADE;

# Create table
CREATE TABLE sumber_tsunami (
    id SERIAL PRIMARY KEY,
    event_id VARCHAR(20) UNIQUE NOT NULL,
    origintime TIMESTAMP WITH TIME ZONE NOT NULL,
    magnitudo REAL NOT NULL,
    latitude DOUBLE PRECISION NOT NULL,
    longitude DOUBLE PRECISION NOT NULL,
    strike INTEGER,
    dip INTEGER,
    slip INTEGER
);

-- Add geometry column
SELECT AddGeometryColumn('sumber_tsunami', 'geom_source', 4326, 'POINT', 2);

-- Create spatial index
CREATE INDEX idx_sumber_tsunami_geom ON sumber_tsunami USING GIST(geom_source);

-- Insert sample data dengan event_id
INSERT INTO sumber_tsunami (event_id, origintime, magnitudo, latitude, longitude, strike, dip, slip, geom_source)
VALUES 
    ('ujicoba0001', '2024-12-17 08:30:00+07', 7.5, -6.2088, 106.8456, 90, 45, 90, ST_SetSRID(ST_MakePoint(106.8456, -6.2088), 4326)),
    ('ujicoba0002', '2024-12-17 14:15:00+07', 6.8, -7.7956, 110.3695, 85, 40, 85, ST_SetSRID(ST_MakePoint(110.3695, -7.7956), 4326)),
    ('ujicoba0003', '2024-12-17 10:45:00+07', 7.2, -8.3405, 115.0920, 95, 50, 95, ST_SetSRID(ST_MakePoint(115.0920, -8.3405), 4326)),
    ('ujicoba0004', '2024-12-17 03:20:00+07', 8.1, 3.5952, 98.6722, 100, 35, 80, ST_SetSRID(ST_MakePoint(98.6722, 3.5952), 4326)),
    ('ujicoba0005', '2024-12-17 19:05:00+07', 6.5, -0.9471, 119.8707, 88, 42, 92, ST_SetSRID(ST_MakePoint(119.8707, -0.9471), 4326));

-- Grant permissions
GRANT ALL PRIVILEGES ON TABLE sumber_tsunami TO farhan;
GRANT USAGE, SELECT ON SEQUENCE sumber_tsunami_id_seq TO farhan;

EOF

echo "Database setup completed!"
echo ""
echo "Database: tsunami_data"
echo "User: farhan"
echo "Password: farhan"
echo "Table: sumber_tsunami"
echo ""
echo "Sample data inserted: 5 records"
echo ""
echo "You can now run the Qt application to view the data."
