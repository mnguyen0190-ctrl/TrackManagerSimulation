# TrackManagerSimulation

This is a Track Manager Simulator for a Racing track. The simulation allows users to manage driver information, record lap times, simulate pit stops, edit track layout information, and manage tournament brackets for drivers.

The program is run through a menu and a series of submenus for editing information.

Option description:

=== Car Racing Simulation Menu ===
Show drivers
   - displays all registered drivers by name and car number
Record lap
   - choose a driver and enter lap time
Show lap history for driver
   - displays lap history in order of entry
Request pit stop
   - adds the selected driver's car number to the pit stop queue
Process next pit stop
   - proccess the pit stop and up the driver's pit stop count
Show pit queue
    - displays all cars waiting in the pit queue in order from first request to last request
Show track info
    - displays distances between turns and the total lap distance
Show tournament bracket
    - displays the built tournament bracket (will need to rebuild bracket on launch)
Driver Edit Menu
    - Add Driver - register a new driver
    - Edit Driver - change name or car number of selected driver
    - Remove Driver - deletes a selected racer
    - List Drivers - view all drivers in order of entry (oldest to newest)
Track Edit Menu
    - Add turn - Adds a turn and asks for a distance from last turn (Last turn -> turn is a straight fixed at 500m)
    - Clear track - deletes all track information
    - Show track layout - Shows the turn information
    - Show turn count & lap distance - displays total turns and total distance of a single lap
Bracket Edit Menu
    - Rebuild bracket - builds a tournamet bracket, pairs 2 drivers in order of oldest to newest driver entry to driver list
        * will have to rebuild bracket every startup of the simulation using this option
    - Set Match winner - select a match and winner of the match to proceed up the bracket



How to build/run:

In terminal(Path should be the folder where your TrackManager.cpp file is):

g++ TrackManager.cpp -o TrackManagerSimulator

start TrackManagerSimulator.exe
