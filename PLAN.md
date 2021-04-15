# Plan

We're making a game! #NoEngine

Here's the plan:

1. Open a window
2. Draw a triangle
3. ???
4. Profit!

This file will be kept up-to-date.

## Sound

For sound, OpenAL "will do". :(

## Input

For controls, better to use XInput directly.

## Event loop

Event loop should post messages into the windows message loop.

Game loop itself is still periodic, so it's easier to call it as-is.

## Display

Handle window resizes.

Support fullscreen.

https://devblogs.microsoft.com/oldnewthing/20080104-00/?p=23923
https://www.gamasutra.com/blogs/MasonRemaley/20210222/377715/Fullscreen_Exclusive_Is_A_Lie_sort_of.php
https://devblogs.microsoft.com/directx/demystifying-full-screen-optimizations/

WS_POPUP, DPI Awareness, and hide cursor, to go fullscreen?

Fullscreen on multiple displays.

Restore windowed after fullscreen with previous size.

Keep rendering while resizing.

Borderless https://www.opengl.org/pipeline/article/vol003_7/

To get Borderless... Simply make the window 1 pixel higher, and reduce the client size in WM_NCCALCSIZE by one pixel. :)

EXT_swap_control_tear or WGL_EXT_swap_control

https://www.khronos.org/opengl/wiki/Swap_Interval