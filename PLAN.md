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

## Display

Fullscreen on multiple displays (select display / adapter?).

To get Borderless on AMD Radeon... Simply make the window 1 pixel higher, and reduce the client size in WM_NCCALCSIZE by one pixel. :)

To get Borderless on Intel HD... Make the client size one pixel wider than the window size!

https://www.khronos.org/opengl/wiki/Swap_Interval

Check WGL_EXT_swap_control WGL_EXT_swap_control_tear in BOTH WGL and GL extensions.

Call wglSwapIntervalEXT if swap control available, -1 for adaptive vsync if swap_control_tear is available, 1 for regular vsync otherwise.

## Refactor

Move rendering into WM_PAINT, use callback.

Handle exceptions directly inside the window message procedure.

Need a System and a Window class.

Add callback for what needs to be done on exceptions, part of System.

Main message loop runs on System, not as part of the window. All windows run in the main thread. Use the libsev win32 main loop. System should not depend on the event lib, just compose them.

Window procedure is part of the Window, though.

Try to pop up two independent GL windows.

Try to share GL resources between GL contexts.
