# Smart Parking Management System

## Overview

Smart Parking Management System is a console-based C++ application designed to manage vehicle parking efficiently. The system allocates parking slots, tracks vehicle entry and exit, maintains parking history, manages VIP and Staff parking zones, and generates reports.

## Features

* Vehicle Parking and Slot Allocation
* Vehicle Removal and Slot Release
* VIP, Staff, and General Parking Zones
* Automatic Slot Assignment using Min Heap
* Vehicle Search by Registration Number
* Parking Statistics and Occupancy Tracking
* Parking History Logging
* CSV Export of Parking Records
* VIP and Staff ID Management
* Entry and Exit Time Tracking
* Parking Duration Calculation

## Parking Zones

| Zone | Category | Capacity  |
| ---- | -------- | --------- |
| A    | VIP      | 100 Slots |
| B    | Staff    | 200 Slots |
| C    | General  | 200 Slots |

Total Capacity: **500 Vehicles**

## Data Structures Used

### Arrays

Used for:

* Parking lot storage
* VIP ID storage
* Staff ID storage

### Min Heap

Used for:

* Fast allocation of the nearest available parking slot
* Efficient slot management

### Stack

Used for:

* Maintaining parking activity history

### Structures

* Vehicle
* Hist
* HStack
* MinHeap

## File Storage

The system creates and maintains the following files:

### history.txt

Stores parking entry and exit records.

### history.csv

Generated report containing parking history in CSV format.

### vipid.txt

Stores registered VIP IDs.

### staffid.txt

Stores registered Staff IDs.

## Menu Options

1. Park Vehicle
2. Remove Vehicle
3. Display Occupied Vehicles
4. Search Vehicle
5. View History
6. View Statistics
7. Export History to CSV
8. Exit

## Technologies Used

* C++
* File Handling
* Structures
* Arrays
* Min Heap
* Stack
* Time Functions
* CSV Export

## Compilation

### Using g++

```bash
g++ parking_management_system.cpp -o parking
```

## Run

### Windows

```bash
parking.exe
```

or

```bash
.\parking.exe
```

## Sample Workflow

1. Initialize System
2. Register VIP/Staff IDs
3. Park Vehicle
4. Allocate Slot Automatically
5. Search Vehicle Details
6. Remove Vehicle
7. Calculate Parking Duration
8. Store History
9. Export Records to CSV

## Future Enhancements

* Graphical User Interface (GUI)
* Database Integration
* QR Code Based Entry
* RFID Vehicle Tracking
* Online Slot Reservation
* Real-Time Parking Dashboard
* Billing and Payment Module

## Author

Developed as a Data Structures and C++ Project demonstrating practical use of:

* Arrays
* Heaps
* Stacks
* File Handling
* Searching
* Resource Management
