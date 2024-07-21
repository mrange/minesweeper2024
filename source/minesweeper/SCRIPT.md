# Video script

Hi, I'm Mårten Rånge. Together with Virgill, I created this 4KiB version of Minesweeper.

I'm using an AI voice not because I'm shy, but to avoid editing out my hums and pauses. I'm not media trained.

The inspiration for this 4KiB game is the old Microsoft Minesweeper. What pushed me to attempt a 4KiB game was Proton's remarkable [256-byte C64 Tetris game](https://www.youtube.com/watch?v=H0Uj-dWpjg4).

I've made a few 4KiB intros before, so I thought creating a 4KiB game with OpenGL graphics and music should be possible.

I enjoyed Minesweeper, especially the small puzzles. I changed the rules to make the boards small and added a time factor to keep the pace up.

There are 16 bombs on a 12x12 board, usually simple but sometimes tricky. You may end up in an ambiguous situation, but it's often possible to solve using the board and the info at the bottom of the screen.

You start the game with 1000 points, which tick down. You carry over the remaining points when finishing a board, so it's good to be quick and precise. If you hit a bomb, the game is over, and you get your final score.

When I started, I realized the x86 code would be bigger than for the other 4KiB intros I had done. I thought I could optimize with assembler, but it was a folly. I saved some bytes with assembler code, but found more space by rewriting the OpenGL shader. A better x86 programmer could probably save more bytes.

I use uniform arrays to send the game state to the shader, which is probably why the GPU load is high on low-tier graphics cards. Arrays are bad for performance but good for space. With more time, I could rewrite to use a texture instead. Mid-tier NVIDIA cards seem to optimize array access.

Thanks to Virgill for the music. Thanks to Pestis for [Sointu](https://github.com/vsariola/sointu). Thanks to Rune Stubbe and Aske Simon Christensen for [Crinkler](https://github.com/runestubbe/Crinkler). Thanks to Laurent Le Brun for [Shader Minifier](https://github.com/laurentlb/shader-minifier). I use these tools to minimize the shaders and the final exe size. Props to Proton for his amazing [256-byte Tetris game on C64](https://www.youtube.com/watch?v=H0Uj-dWpjg4).

The [Minesweeper](https://github.com/mrange/minesweeper2024) source code is available on GitHub for those interested.
