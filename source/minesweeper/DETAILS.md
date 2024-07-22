# Minesweeper/2024 - Details

## Technical tidbits

Performance may be suboptimal on lower-end graphics cards, likely due to the use of arrays.

While arrays are not ideal for shader performance, they were chosen for their simplicity and ease of implementation, which is beneficial for a 4KiB executable.

Mid-tier and higher-end graphics cards appear to handle uniform array usage more efficiently.

For instance, on my work laptop with an integrated AMD Radeon GPU, utilization reaches 90%. In contrast, my desktop with a mid-tier graphics card maintains only 2% GPU usage.

During shader development, I observed that modifying the array within shader execution significantly increased GPU usage on NVIDIA cards as well.

A potential optimization could involve rewriting the code to use textures for communicating game state, rather than arrays.