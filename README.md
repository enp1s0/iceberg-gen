# iceberg-gen

![iceberg-wallpaper-sample](./docs/sample.png)

This program generates a [iceberg](https://github.com/cocopon/iceberg.vim) wallpaper.

## Build
### Requirements
- C++11 or later
- libpng

### Build command
```bash
cd generator
make
```

## Usage

### Example

```
./generator/iceberg-gen.out 3840 2160 sample.png ms
```

### Arguments
- 1st : output image width
- 2nd : output image height
- 3rd : output image file name
- 4th : options. `m` means `moon` and `s` means `stars`
