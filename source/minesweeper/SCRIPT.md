# Video script

Hi. My name is Mårten Rånge and together with Virgill I wrote this version of Minesweeper.

I use an AI voice not because I am shy but because I don't want to spend hours edit out my hums and pauses if I did a proper recording. I am not media trained.

Anyway, the inspiration for this 4KiB game is obviously the old Microsoft Windows game Minesweeper but what triggered me to attempt a 4KiB game was the 256 byte C64 Tetris game by Proton which I felt was a remarkable achievement.

I have created a few 4KiB intros before so thought it should be possible to create a 4KiB game but with OpenGL graphics and music by Virgill (Sointu player by Pestis).

I enjoyed playing Minesweeper back in the day but I mostly enjoyed the small puzzles that weren't too complicated so I wanted to change the rules so that the boards are rather small but there is a time factor that encourages keeping the pace up.

There are 16 bombs on a 12x12 board which is usually quite simple to solve but tricky situation can occur. Of course if you are unlucky you can end up up in an ambigious situation but it is often possible to solve using the board and the info about how many non-bomb squares haven't been revealed on the bottom of the screen.

When starting the game you get 1000 points and then it ticks down, you get to carry over the remaining points when finishing a board so it's good to be quick and precise. When hitting a bomb it's over and you get the final score.

When starting the game I realized the x86 code would be bigger than the 4KiB intros I have done up to this point but I felt sure I would be able to do manual optimization with assembler. Well, turned out that it was a folly, I did manage to shave some bytes of by using assembler code but I learnt that I am not particular skilled in x86 assembler and I found more space optimization rewriting the OpenGL shader. A better x86 programmer can probably shave of significantly more bytes.

I rely on uniform arrays to send the game state to the shader and that is probably the reason that on low-tier graphics card the GPU load is quite high. Arrays are bad for for performance but good for space. Given some more time I could probably rewrite to using a texture to pass the state instead. On mid-tier NVIDIA graphics cards it seems it is able to optimize the array access.

Big thanks to Virgill and Pestis for the music and music player. Big thanks to the authors of Crinkler and Shader minimizer that are tools I use to minimize the shaders and the final exe size. Big props to Proton for his amazing 256 byte Tetris game on C64.

The minesweeper game source is available on github for those that are interested.
