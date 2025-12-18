#!/bin/bash

echo " Cek bentar bang ..."

echo ""
echo "Checking theme CSS files..."
for css in resources/theme/*.css; do
    if [ -f "$css" ]; then
        size=$(wc -c < "$css")
        echo "  ✓ $(basename $css): ${size} bytes"
        
        # Check for binary/corrupt data
        if file "$css" | grep -q "text"; then
            echo "    → Valid text file"
        else
            echo "    ⚠️  WARNING: Not recognized as text file!"
        fi
    fi
done

# Check map tiles
echo ""
echo "Checking map tiles..."
TILE_COUNT=$(ls resources/maps/world*.png 2>/dev/null | wc -l)
echo "  Found $TILE_COUNT tiles"

if [ "$TILE_COUNT" -gt 0 ]; then
    # Sample check first 5 tiles
    for tile in $(ls resources/maps/world*.png | head -5); do
        size=$(wc -c < "$tile")
        echo "  ✓ $(basename $tile): ${size} bytes"
    done
    
    if [ "$TILE_COUNT" -gt 5 ]; then
        echo "  ... and $((TILE_COUNT - 5)) more tiles"
    fi
fi

# Check app.qrc
echo ""
echo "Checking app.qrc..."
if [ -f "app.qrc" ]; then
    lines=$(wc -l < app.qrc)
    echo "  ✓ app.qrc: $lines lines"
    
    # Verify XML syntax
    if command -v xmllint &> /dev/null; then
        if xmllint --noout app.qrc 2>&1; then
            echo "    → Valid XML"
        else
            echo "    ⚠️  XML syntax error!"
        fi
    fi
else
    echo "  ❌ app.qrc not found!"
fi

echo ""
echo "Done!"
