# scrot

[![Language grade: C/C++][lgtm-badge]][lgtm-cpp]

A nice and straightforward screen capture utility implementing the dynamic
loaders of Imlib2.

This utility works in X11-based environments only - it does not work inside
Wayland or XWayland sessions.

## Dependencies

Fedora:

    $ sudo dnf install meson imlib2-devel libX11-devel libXfixes-devel

Debian, Ubuntu:

    $ sudo apt install meson libimlib2-dev libx11-dev libxfixes-dev

## Build instructions

    $ meson setup build
    $ ninja -C build
    $ ninja -C build install

## License

This software is covered by [MIT-feh] license. All changes authored by
Patryk Obara are dual-licensed using MIT-feh and normal [MIT] license.

[MIT-feh]:    https://spdx.org/licenses/MIT-feh.html
[MIT]:        https://opensource.org/licenses/MIT
[lgtm-cpp]:   https://lgtm.com/projects/g/dreamer/scrot/context:cpp
[lgtm-badge]: https://img.shields.io/lgtm/grade/cpp/g/dreamer/scrot.svg
