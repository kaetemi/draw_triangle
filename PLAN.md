# Plan

We're making a game! #NoEngine

Here's the plan:

1. Open a window
2. Draw a triangle
3. ???
4. Profit!

This file will be kept up-to-date.

## Open a window

Generate the GL loader code using gl3w.
- https://github.com/skaslev/gl3w

Create window using Win32 API (sorry, Linux!)
Is there even a cross-platform library that can just create a window, and do nothing else?

Here's some alternatives...
- https://www.glfw.org/
- https://github.com/bkaradzic/bgfx (This looks very interesting!)

For sound, OpenAL "will do". :(

For controls, better to use XInput directly.

## Draw a triangle in OpenGL

Draw a triangle, and do it properly. We need to get sRGB and Linear color spaces correct from the go!

## Debug output

Find out how to make fmtlib output to OutputDebugStringW without allocating an std::string.