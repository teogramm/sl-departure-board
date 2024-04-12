# SL Departure Board

![](doc/demo.gif)

An attempt to replicate the SL departure boards using the U8G2 library.

- **Deviation support**: The display also shows information about deviations and problems, for the authentic SL experience.
- **Filter by direction and mode**: Supports all modes, filtering by mode and/or direction.
- **Hardware agnostic**: Can run on any device supported by [u8g2](https://github.com/olikraus/u8g2) and additionally
  provides a translation layer which enables running it on a Raspberry Pi.

## How to build

The following instructions concern building the application. For information about the hardware see [here](doc/hardware.md).

### Cross-compilation setup

If you are building the program on the system you will be running it on, you can skip these steps.

1. If you are cross-compiling, build the image from the provided Dockerfile e.g. by running
   ```podman build -t linux-arm64 .```
2. Generate the script for cross-compiling by running ```podman run --rm -it linux-arm64 > dockcross```
3. Prepend the commands in the following section with ```./dockcross```

### Building

The program is fully built on CMake and most dependencies are fetched automatically. If you are not using the Docker
image, you might need to install some development libraries (such as ```curl-devel```).

1. Run ```cmake -DCMAKE_BUILD_TYPE=Release -S . -B cmake-cross```
2. Run ```cmake --build cmake-cross --target sl_pi_cpp```
3. The program will be in ```cmake-cross/sl_pi_cpp```

## Usage

Be sure to check that the pins in [main.cpp](main.cpp) correspond to your actual connections. The code by default uses
the 8080 bitbang protocol, which requires a lot of wires. If you want to try another connection mode (like SPI)
you can try editing the [wrapper](include/U8G2_SSD1322_NHD_256X64_F_8080_RPi.h), but no guarantees that it works.

To see the available options run ```sl_pi_cpp -h```.

The program accepts the site ID of the stop to display departures. You can find a list of the site IDs of all stops
by running

```
curl -X 'GET' 'https://transport.integration.sl.se/v1/sites' -H 'Content-Type: application/json' > sites.json
```

and searching through the ```sites.json``` file.

For more information see the [SL API documentation](https://www.trafiklab.se/api/trafiklab-apis/sl/transport).

### Auto-starting
If you want the program to start automatically at boot you can create a systemd user service. Place the following
in ```~/.config/systemd/user/sl-display.service```, replacing the path to the executable and the parameters.

```
[Unit]
Description=Controls the SL display
After=network-online.target
Wants=network-online.target

[Service]
ExecStart=/path/to/sl_pi_cpp -m METRO -d 1 -s 00:30-07:00 9181

[Install]
WantedBy=default.target
```
For the service to start automatically make sure to enable lingering for your user by running
```
sudo loginctl enable-linger <username>
```

You can then enable the service by running ```systemctl --user enable --now sl-display.service```. On the first start
after boot, the display might take some time to be enabled, because the network is not immediately available.


## Credits

Big thanks to [chrisys](https://github.com/chrisys/train-departure-display/tree/main) for the inspiration. His blog
post and code pointed me towards the general direction to implement this.