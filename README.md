# rndl

A WS2812B LED project focused on rendering.

## Build

This library is built using ESP-IDF on an esp32-wroom-32s. I am using a few
WS2812B in my setup.

Once you have ESP-IDF on your system and have wired up your panels, build and
flash using these commands.

```
make
make flash
```

## API Usage

1. Create your driver and configuration in static memory
2. Create your surface in static memory
3. Draw using the surface primitives
4. Render with `surface->render(surface)`

## Design

The graphics engine is designed in layers. At each layer N, only the
immediate N-1 layer may be accessed. In this coordinate system,
origin in the upper left corner of the surface. X and Y increase
right and down, respectively.

```
Application
    ↓
Graphics API
    ↓
Driver API
    ↓
Hardware
```

The application uses the Graphics API to draw pixels. At this layer,
only Graphics objects may be used.

- Pixel: a single point with color and amplitude
- Line: a pair of points
- Surface: the state of a collection of pixels in a 2D space

The Graphics API maintains an internal buffer of its current 2D state.
The Driver API provides primitives for encoding pixels into a linear
index. The Graphics API is responsible for pushing this buffer to the driver

The Driver API is responsible for encoding and transmitting data to
the target Hardware. Data is encoded into a wire format and then
sent to the Hardware over an interface.
