# Minesweeper

## Commands

### Generate SPIR-V human form

Requires `layout(location=0)` on uniforms.

```bash
glslangValidator.exe shader.frag -G -H
```

## Current Rules

Certainly! Here's a revised version:

1. **Modified Minesweeper Rules**
2. Each square displays a number indicating the count of adjacent bombs.
3. Revealing a square is done by clicking the left mouse button.
4. Revealing a square containing a bomb ends the game.
5. If a revealed square has no adjacent bombs, adjacent squares are automatically revealed.
6. Marking a square as a possible bomb is done by clicking the right mouse button. This action has no impact on gameplay.
7. The game board is considered complete when all non-bomb squares are revealed.
8. The "Left" indicator at the top displays the number of unrevealed squares remaining.
9. Scoring begins at 1000 points and decreases over a period of 60 seconds.
10. Successfully clearing a board awards a new score equal to the current score plus 1000 times the number of boards cleared.
11. Upon clearing a board, a "locked score" is calculated by halving the new score. If the resulting score is lower than the previous locked score, the previous locked score is retained. The score then decreases from the new score to the locked score over 60 seconds.
12. On game over, your final score is displayed.
13. Pressing the "R" key restarts the game, allowing you to start fresh with a new board.
14. Pressing the "Escape" key quits the game.

## Ideas

### Scoring Ideas

1. Score is constantly ticking down
2. Score down tick speed is depending on how many revealed you have
3. Perhaps score can't tick below some limit? Like last board score?
3. Clearing a board awards points
4. You need to clear boards in a certain time
5. Triggering in a bomb don't end the game but hurts score
6. When you are happy with your score you end the game to collect the score
