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

# Grant schema privileges
sudo -u postgres psql -d tsunami_data -c "GRANT ALL ON SCHEMA public TO farhan;"

# Create table and insert data
PGPASSWORD=farhan psql -h localhost -U farhan -d tsunami_data << 'EOF'
-- Drop table if exists for clean setup
DROP TABLE IF EXISTS sumber_tsunami CASCADE;

-- Create table with proper column order
CREATE TABLE sumber_tsunami (
    id SERIAL PRIMARY KEY,
    event_id VARCHAR(20) UNIQUE NOT NULL,
    origintime TIMESTAMP WITH TIME ZONE NOT NULL,
    magnitudo REAL NOT NULL,
    latitude DOUBLE PRECISION NOT NULL,
    longitude DOUBLE PRECISION NOT NULL,
    depth_km INTEGER NOT NULL,
    strike INTEGER,
    dip INTEGER,
    slip INTEGER
);

-- Add geometry column
SELECT AddGeometryColumn('public', 'sumber_tsunami', 'geom_source', 4326, 'POINT', 2);

-- Create spatial index
CREATE INDEX idx_sumber_tsunami_geom ON sumber_tsunami USING GIST(geom_source);

-- Insert sample data dengan event_id format ujicoba#### dan depth
INSERT INTO sumber_tsunami (event_id, origintime, magnitudo, latitude, longitude, depth_km, strike, dip, slip, geom_source)
VALUES 
    ('ujicoba0001', '2024-12-18 08:30:00+07', 7.5, -6.2088, 106.8456, 10, 90, 45, 90, ST_SetSRID(ST_MakePoint(106.8456, -6.2088), 4326)),
    ('ujicoba0002', '2024-12-18 14:15:00+07', 6.8, -7.7956, 110.3695, 25, 85, 40, 85, ST_SetSRID(ST_MakePoint(110.3695, -7.7956), 4326)),
    ('ujicoba0003', '2024-12-18 10:45:00+07', 7.2, -8.3405, 115.0920, 15, 95, 50, 95, ST_SetSRID(ST_MakePoint(115.0920, -8.3405), 4326)),
    ('ujicoba0004', '2024-12-18 03:20:00+07', 8.1, 3.5952, 98.6722, 8, 100, 35, 80, ST_SetSRID(ST_MakePoint(98.6722, 3.5952), 4326)),
    ('ujicoba0005', '2024-12-19 19:05:00+07', 6.5, -0.9471, 119.8707, 20, 88, 42, 92, ST_SetSRID(ST_MakePoint(119.8707, -0.9471), 4326));

-- Grant permissions
ALTER TABLE sumber_tsunami OWNER TO farhan;
GRANT ALL PRIVILEGES ON TABLE sumber_tsunami TO farhan;
GRANT USAGE, SELECT ON SEQUENCE sumber_tsunami_id_seq TO farhan;

-- Verify data
SELECT COUNT(*) as total_records FROM sumber_tsunami;
SELECT event_id, origintime, magnitudo FROM sumber_tsunami ORDER BY origintime DESC;

EOF

echo ""
echo "=========================================="
echo "Database setup completed!"
echo "=========================================="
echo ""
echo "Database: tsunami_data"
echo "User: farhan"
echo "Password: farhan"
echo "Table: sumber_tsunami"
echo ""
echo "Sample events with dates 2024-12-18 to 2024-12-19"
echo "(Default filter will show last 1 day of events)"
echo ""
echo "You can now run: ./bismillah"
