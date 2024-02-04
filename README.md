# Lighty

Another WS12 LED project as an excuse for creating a graphics engine.

## Design

The graphics engine is designed in layers. At each layer N, only the
immediate N-1 layer may be accessed.

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
