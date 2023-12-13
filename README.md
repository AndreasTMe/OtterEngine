# Otter Engine

## Description

Otter Engine is (or at least supposed to become) a game engine written in C++. It is a personal project that I started
in October 2023, after I got inspired by projects like the [Hazel Engine](https://github.com/TheCherno/Hazel) and
the [Kohi Engine](https://github.com/travisvroman/kohi), so shoutout to the Cherno and Travis Vroman for their work and
for being a source of inspiration.

I am currently working on the engine in my free time, so it is not very advanced yet. I am also trying to experience
the platform-specific weirdness first-hand, and this is one of the reasons I am not using any libraries for window
management, math, etc. The third party libraries I am using at the moment are:

- [Vulkan](https://www.khronos.org/vulkan/) for graphics
- [STB Image](https://github.com/nothings/stb) for image/texture loading (_quick & easy choice, will probably be
  replaced by custom code_)
- [Google Test](https://github.com/google/googletest) for unit testing

## Current state

The engine is currently able to create a window and a Vulkan instance, and to render images on the screen, but the
graphics pipelines are not very advanced yet. I have been mostly focused on the engine architecture in order to have a
solid base to build on.

The engine has:

- A memory system with a free list allocator, responsible for capturing memory during the engine's startup and
  releasing it when the engine shuts down
- An event system, that can register event listeners and dispatch events by using delegates (C# style) and a circular
  queue
- A logging system, which can be used to log messages to the console
- A window system (Windows only for now), which can create a window and handle events
- A Vulkan renderer, which can create a Vulkan instance and render images on the screen
- Custom collections that use the memory system above (trying my best to allocate memory in contiguous blocks)
    - Span/ReadOnlySpan (fixed size, stack allocated data)
    - Array/ReadOnlyArray (fixed size, heap allocated data)
    - List (dynamic size, heap allocated data)
    - Stack (dynamic size, stack allocated data)
    - Queue (circular, dynamic size, heap allocated data)
    - Deque (dynamic size, heap allocated data)
    - HashSet (dynamic size, prime number size, heap allocated data)
    - Dictionary (dynamic size, prime number size, heap allocated data)
- Custom math classes
    - Vector2D, Vector3D, Vector4D
    - Quaternion
    - Matrix2x2, Matrix3x3, Matrix4x4

## Future plans

- [ ] An entity-component-system architecture (a naive implementation for now)
- [ ] Build a simple 2D game using the engine

Honestly, this is it for now. This will probably be expanded as I go, with things like (in priority order):

- Sprite rendering (more robust and tidier)
- Text rendering
- A 2D physics system
- A sound system
- A UI system (probably using Dear ImGui)
- A scripting system (definitely C#)
- A 3D renderer, with meshes, materials, lighting, shadows, etc.
- A 3D physics system
- ...???

Probably more, but hey, I'm just getting started, so there is no timetable yet. My goal is to have a solid foundation
for a game engine, and to learn as much as I can in the process.

That's all for now, thanks for reading!