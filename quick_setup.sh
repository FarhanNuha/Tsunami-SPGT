#!/bin/bash
set -e

echo "🌊 Tsunami Monitoring System - Quick Setup"
echo "=========================================="

if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: Jalankan script ini dari root project directory!"
    exit 1
fi

echo "📁 Creating directories..."
mkdir -p resources/theme
mkdir -p resources/maps
mkdir -p scripts
mkdir -p build

if [ ! -f "resources/theme/dark.css" ]; then
    echo "Creating resources/theme/dark.css..."
    cat > resources/theme/dark.css << 'EOF'
/* Dark Theme */
QMainWindow, QWidget { background-color: #2b2b2b; color: #e0e0e0; }
QMenuBar { background-color: #3c3c3c; color: #e0e0e0; }
QPushButton { background-color: #0078d7; color: #ffffff; border: none; padding: 6px 16px; }
QTableView { background-color: #2b2b2b; color: #e0e0e0; }
EOF
fi

if [ ! -f "resources/theme/light.css" ]; then
    echo "Creating resources/theme/light.css..."
    cat > resources/theme/light.css << 'EOF'
/* Light Theme */
QMainWindow, QWidget { background-color: #f5f5f5; color: #333333; }
QMenuBar { background-color: #ffffff; color: #333333; }
QPushButton { background-color: #0078d7; color: #ffffff; border: none; padding: 6px 16px; }
QTableView { background-color: #ffffff; color: #333333; }
EOF
fi

if [ ! -f "resources/theme/dark-transparent.css" ]; then
    echo "Creating resources/theme/dark-transparent.css..."
    cat > resources/theme/dark-transparent.css << 'EOF'
/* Dark Transparent Theme */
QMainWindow, QWidget { background-color: rgba(43, 43, 43, 230); color: #e0e0e0; }
QMenuBar { background-color: rgba(60, 60, 60, 230); color: #e0e0e0; }
QPushButton { background-color: rgba(0, 120, 215, 220); color: #ffffff; border: none; padding: 6px 16px; }
QTableView { background-color: rgba(43, 43, 43, 220); color: #e0e0e0; }
EOF
fi

if [ ! -f "resources/theme/light-transparent.css" ]; then
    echo "Creating resources/theme/light-transparent.css..."
    cat > resources/theme/light-transparent.css << 'EOF'
/* Light Transparent Theme */
QMainWindow, QWidget { background-color: rgba(245, 245, 245, 230); color: #333333; }
QMenuBar { background-color: rgba(255, 255, 255, 230); color: #333333; }
QPushButton { background-color: rgba(0, 120, 215, 220); color: #ffffff; border: none; padding: 6px 16px; }
QTableView { background-color: rgba(255, 255, 255, 220); color: #333333; }
EOF
fi

# Check map tiles
TILE_COUNT=$(ls resources/maps/world*.png 2>/dev/null | wc -l)
echo "🗺️  Found $TILE_COUNT map tiles in resources/maps/"

if [ "$TILE_COUNT" -eq 0 ]; then
    echo ""
    echo "⚠️  WARNING: No map tiles found!"
    echo "   Copy your world*.png files to resources/maps/"
    echo "   Example: cp /path/to/tiles/world*.png resources/maps/"
    echo ""
fi

# Generate app.qrc if tiles exist
if [ "$TILE_COUNT" -gt 0 ]; then
    echo "📝 Generating app.qrc with $TILE_COUNT tiles..."
    
    cat > app.qrc << 'EOF'
<!DOCTYPE RCC>
<RCC version="1.0">
    <qresource prefix="/theme">
        <file>resources/theme/dark.css</file>
        <file>resources/theme/light.css</file>
        <file>resources/theme/dark-transparent.css</file>
        <file>resources/theme/light-transparent.css</file>
    </qresource>
    
    <qresource prefix="/maps">
EOF

    # Add all tiles
    for tile in resources/maps/world*.png; do
        if [ -f "$tile" ]; then
            echo "        <file>$tile</file>" >> app.qrc
        fi
    done

    cat >> app.qrc << 'EOF'
    </qresource>
</RCC>
EOF
    
    echo "app.qrc generated successfully"
else
    # Create minimal qrc without tiles
    cat > app.qrc << 'EOF'
<!DOCTYPE RCC>
<RCC version="1.0">
    <qresource prefix="/theme">
        <file>resources/theme/dark.css</file>
        <file>resources/theme/light.css</file>
        <file>resources/theme/dark-transparent.css</file>
        <file>resources/theme/light-transparent.css</file>
    </qresource>
</RCC>
EOF
    echo "✅ app.qrc generated (without tiles)"
fi

echo ""
echo " Setup complete!"
echo ""
echo "Next steps:"
echo "  1. Copy map tiles: cp /path/to/tiles/*.png resources/maps/"
echo "  2. Re-run this script if you added tiles: ./scripts/quick_setup.sh"
echo "  3. Build: cd build && cmake .. && make -j\$(nproc)"
echo "  4. Run: ./bismillah"
echo ""
