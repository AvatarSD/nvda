## How To

#### Dependencies

    * cmake 3.11
    * gcc 11.4.0+

#### Build & Run

```
rm -rf build ; mkdir build && pushd build && cmake ../ && make && make run ; popd
```

## Description

### queue_circular_buffer

##### Initial Task

Implement a custom adapter for std::vector that mimics the behavior of a circular buffer.

##### Details:

- Create a class that uses std::vector internally but provides methods like push_back, pop_front, and front while maintaining the circular nature of the buffer.

- Implement a method resize that adjusts the internal vector size but keeps the circularity intact.