on run argv
    set volumeName to item 1 of argv
    
    tell application "Finder"
        tell disk volumeName
            open
            set current view of container window to icon view
            set toolbar visible of container window to false
            set statusbar visible of container window to false
            set the bounds of container window to {400, 100, 920, 540}
            set theViewOptions to the icon view options of container window
            set arrangement of theViewOptions to not arranged
            set icon size of theViewOptions to 128
            
            -- Set background image
            set background picture of theViewOptions to file ".background:background.png"
            
            -- Wait a moment for the disk to be ready
            delay 2
            
            -- Position icons (Firefox-style layout)
            try
                set position of item "dave.app" of container window to {200, 300}
            end try
            try  
                set position of item "Applications" of container window to {520, 300}
            end try
            
            -- Update and close
            update without registering applications
            delay 1
            close
        end tell
    end tell
end run