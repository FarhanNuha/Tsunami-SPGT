#!/bin/bash

echo "Installing dependencies for Tsunami Monitoring System..."
echo ""

# Update package list
echo "Updating package list..."
sudo apt-get update

# Install PostgreSQL and PostGIS
echo ""
echo "Installing PostgreSQL and PostGIS..."
sudo apt-get install -y postgresql postgresql-contrib postgis

# Install Qt6 SQL PostgreSQL driver
echo ""
echo "Installing Qt6 PostgreSQL driver..."
sudo apt-get install -y libqt6sql6-psql

# Check if driver is installed
echo ""
echo "Checking installed Qt6 SQL drivers..."
if dpkg -l | grep -q libqt6sql6-psql; then
    echo "✓ Qt6 PostgreSQL driver installed successfully"
else
    echo "✗ Failed to install Qt6 PostgreSQL driver"
fi

# Start PostgreSQL service
echo ""
echo "Starting PostgreSQL service..."
sudo systemctl start postgresql
sudo systemctl enable postgresql

echo ""
echo "All dependencies installed!"
echo ""
echo "Next steps:"
echo "1. Run: ./setup_database.sh to create database and tables"
echo "2. Build: mkdir build && cd build && cmake .. && make"
echo "3. Run: ./bismillah"
