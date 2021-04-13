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

Simply push the game update function into the loop, then run the event loop until flushed.

## Display

Handle window resizes.

Support fullscreen.
https://devblogs.microsoft.com/oldnewthing/20080104-00/?p=23923
https://www.gamasutra.com/blogs/MasonRemaley/20210222/377715/Fullscreen_Exclusive_Is_A_Lie_sort_of.php
https://devblogs.microsoft.com/directx/demystifying-full-screen-optimizations/

WS_POPUP, DPI Awareness, and hide cursor, to go fullscreen?
