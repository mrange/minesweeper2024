# Minesweeper/2024, 2nd place in  Evoke 24 interactive

## Development Notes

### Code and Optimization

The game's loader and logic are primarily written in C++, with some assembler code for byte-saving optimizations. While my initial plan was to rely heavily on assembler for further size reduction, I found that the compiler already does a decent job of minimizing code size. However, I did manage to outsmart it occasionally for additional savings.

### Audio

The music is composed using Sointu, which converts the tracks into assembly code for further size reduction. The soundtrack was composed by Virgill, a prolific artist known for creating compact yet high-quality music.

### Graphics

All graphics are handled in a single fragment shader. The game state is passed to the shader as a uniform array. This approach keeps the graphics code minimal. The shader code is further minified using FsShaderMinifier.

### Compilation and Optimization

I use a specialized linker called Crinkler to compress the code and data, stripping out any unnecessary metadata. This process can sometimes trigger false positives in virus scanners due to the lack of metadata.

Despite the small final size, the Crinkler's thorough optimization process—testing around 30,000 compression variations—results in a build time of about one minute.

### Final Optimization

Initially, the executable was around 5 KiB. Reducing it to under 4 KiB took significant effort, with most gains coming from simplifying the fragment shader. Each optimization step, often requiring hours of work, could yield just a few bytes of savings. The final push to fit everything into 4 KiB involved stripping out several graphical features.

