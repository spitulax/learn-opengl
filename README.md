My journey of learning OpenGL with the guide of [LearnOpenGL] by Joey de Vries.
This program contains multiple mini-programs roughly for each chapter of the
book. **This is not a guide on learning OpenGL**.

## Dependency

These libraries/dependncies are required:

- A graphics driver
- `glfw`

## Building & Running

```sh
meson setup build
meson compile -C build
build/learn-opengl <mini-program>
```

## Mini-programs

- `triangle`
- `square`
- `two_triangles`
- `shader_triangle`
- `texture`

## Credits

- [LearnOpenGL] by Joey de Vries

[LearnOpenGL]: https://learnopengl.com/
