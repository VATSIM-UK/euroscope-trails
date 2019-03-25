#pragma once

// Data about history trails - used to send and recieve values from the dialog.
typedef struct HistoryTrailData {
    bool * fade;
    bool * degrade;
    bool * antiAlias;
    int * type;
    int * length;
    COLORREF * colour;
    int * dotSize;
    int * maxAltitude;
    int * minAltitude;
} HistoryTrailData;
