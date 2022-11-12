See https://sschueller.github.io/posts/vbz-fahrgastinformation/ for details


# VBZ Fahrgastinformation

![Finished Sign](https://sschueller.github.io/posts/vbz-fahrgastinformation/P_20221106_172806.jpg)

This is the code to run the Tram/Bus information display clone similar to what is used at certain station in Zürich Switzerland.

The data is pulled of the official Open Data API located here: https://opentransportdata.swiss


## Required Hardware

You will need an ESP and the matrix display. For exact BOM see: https://sschueller.github.io/posts/vbz-fahrgastinformation/

## Software Setup

1. Open this project in PlatformIO
2. Copy ```include/Config.h.dist``` to ```include/Config.h```
3. Edit ```include/Config.h``` to match your API key, station etc.

For a list of stations download the xlsx file located here: https://opentransportdata.swiss/de/dataset/bav_liste

