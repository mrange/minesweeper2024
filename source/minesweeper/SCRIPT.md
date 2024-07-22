# Video script

## FFMPEG

```bash
ffmpeg.exe -to 135 -i ms2024.mkv -i script.wav -filter_complex "[0:v]fade=t=in:st=0:d=3,fade=t=out:st=132:d=3[v];[0:a]volume=0.15,afade=t=in:st=0:d=3,afade=t=out:st=132:d=3[a0];[1:a]volume=1,afade=t=in:st=0:d=0,afade=t=out:st=132:d=3[a1];[a0][a1]amix=inputs=2:duration=first:dropout_transition=3[a]" -map "[v]" -map "[a]" -c:v libx264 -crf 18 -preset veryslow ms2024.mp4
```

## Script


Hi, I'm Mårten Rånge. Together with Virgill, I created this 4KiB version of Minesweeper.

I'm using an AI voice not because I'm shy, but to avoid editing out my hums and pauses.

The inspiration for this 4KiB game is the old Microsoft Minesweeper. What pushed me to attempt a 4KiB game was  [Serato's](https://demozoo.org/sceners/120814) remarkable [256-byte C64 Tetris game](https://demozoo.org/productions/338030).

I've made a few 4KiB intros before, so I thought creating a 4KiB game with OpenGL graphics and music would be an interesting challenge.

I always enjoyed Minesweeper, especially the small puzzles. So, I changed the rules to make the board small and added a time factor to the game.

There are 16 bombs on a 12x12 board, which means it's usually simple to solve, but tricky boards are possible. You may end up in an ambiguous situation, but it's often possible to solve using the board and the info at the bottom of the screen.

You start the game with 1000 points, which tick down. You carry over the remaining points when finishing a board, so it's good to be quick and precise. If you hit a bomb, the game is over, and you get your final score.

When I started, I realized the x86 code would be bigger than for the other 4KiB intros I had done. I thought I could optimize with assembler, but I only managed to save some bytes with assembler code. I was more successful by rewriting the OpenGL shader. A better x86 programmer could probably save more bytes.

I use uniform arrays to send the game state to the shader, which is probably why the GPU load is high on low-tier graphics cards. Arrays are bad for performance but good for space. With more time, I could rewrite to use a texture instead. Mid-tier NVIDIA cards seem to optimize array access.

Thanks to Virgill for the music. Thanks to Pestis for [Sointu](https://github.com/vsariola/sointu). Thanks to Rune Stubbe and Aske Simon Christensen for [Crinkler](https://github.com/runestubbe/Crinkler). Thanks to Laurent Le Brun for [Shader Minifier](https://github.com/laurentlb/shader-minifier). I use these tools to minimize the shaders and the final exe size. Props to [Serato](https://demozoo.org/sceners/120814) for his amazing [256-byte Tetris game on C64](https://demozoo.org/productions/338030).

The [Minesweeper source code](https://github.com/mrange/minesweeper2024) is available on GitHub for those interested.

