# Minesweeper

## Build

Should build fine with Visual 2022 with the C++ "workload" installed.

## Commands

### Generate SPIR-V human form

Requires `layout(location=0)` on uniforms.

```bash
glslangValidator.exe shader.frag -G -H
```

