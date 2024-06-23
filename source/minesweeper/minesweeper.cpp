/*
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2024 Mårten Rånge

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include "minesweeper.h"

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

extern "C" {

#ifdef _DEBUG
  void APIENTRY debugCallback(
      GLenum          source
    , GLenum          type
    , GLuint          id
    , GLenum          severity
    , GLsizei         length
    , GLchar const *  message
    , void const *    userParam
    )
  {
    printf(message);
    printf("\n");
  }
  char debugLog[0xFFFF];
#endif

  #pragma code_seg(".lcg_rand_uint32_12")
  uint32_t MS_NOINLINE lcg_rand_uint32_12() {
    lcg_state = (1664525U * lcg_state + 1013904223U);
    uint64_t v = static_cast<uint64_t>(lcg_state)*CELLS;
    return static_cast<uint32_t>(v >> 32);
  }

  #pragma code_seg(".reset_board")
  void MS_NOINLINE reset_board(float time) {
#ifdef NOCRT
    // Well this is awkward
    #define SZ_OF_BOARD 0x2254
    static_assert(SZ_OF_BOARD == sizeof(board), "The sizeof(board) and SZ_OF_BOARD must be the same");
    _asm {
      LEA edi, [game.board]
      XOR eax, eax
      MOV ecx, SZ_OF_BOARD
      REP STOSB
    }
    #undef SZ_OF_BOARD
#else
    memset(&game.board, 0, sizeof(board));
#endif

    auto remaining_bombs = BOMBS_PER_BOARD;
    do {
      auto x = lcg_rand_uint32_12();
      auto y = lcg_rand_uint32_12();
      auto i = CELLS*y+x;
      assert(i >= 0);
      assert(i < CELLS*CELLS);
      auto & cell = game.board.cells[i];
      if (cell.has_bomb) {
        continue;
      }

      cell.has_bomb = true;
    } while (--remaining_bombs > 0);

    auto y = CELLS-1;
    do {
      auto x = CELLS-1;
      do {
        auto i            = x + y*CELLS;
        assert(i >= 0);
        assert(i < CELLS*CELLS);
        auto & cell       = game.board.cells[i];
        cell.prev_state   = cell_state::initial;
        cell.state        = cell.next_state = cell_state::covered_empty;
        cell.changed_time = cell.mouse_time = time-game.start_time;

        auto near_bombs   = 0;
        auto near_cells   = 0;

        for (auto yy = -1; yy <= 1; ++yy) {
          auto near_y     = y+yy;
          for (auto xx = -1; xx <= 1; ++xx) {
            auto near_x   = x+xx;
            auto near_i   = near_y*CELLS+near_x;
            auto hit_mid = xx == 0 && yy == 0;
            if (!hit_mid && near_y >= 0 && near_y < CELLS && near_x >= 0 && near_x < CELLS) {
              assert(near_i >= 0);
              assert(near_i < CELLS*CELLS);
              auto & near_cell            = game.board.cells[near_i];
              cell.near_cells[near_cells] = &near_cell;
              ++near_cells;
              if (near_cell.has_bomb) ++near_bombs;
            }
            assert(near_cells <= 8);
          }
        }
        cell.near_bombs = near_bombs;
      } while(--x >= 0);
    } while (--y >= 0);

    for(;;) {
      auto x = lcg_rand_uint32_12();
      auto y = lcg_rand_uint32_12();

      assert(x >= 0);
      assert(x < CELLS);
      assert(y >= 0);
      assert(y < CELLS);
      auto i = x+CELLS*y;
      assert(i >= 0);
      assert(i < CELLS*CELLS);
      auto & cell = game.board.cells[i];
      if (cell.near_bombs == 0 && !cell.has_bomb) {
        cell.next_state = cell_state::uncovering;
        break;
      }
    }


  }

  #pragma code_seg(".reset_game")
  void MS_INLINE reset_game(float time) {
#ifdef NOCRT
    // Well this is awkward
    #define SZ_OF_GAME 0x2270
    static_assert(SZ_OF_GAME == sizeof(game), "The sizeof(game) and SZ_OF_GAME must be the same");
    _asm {
      LEA edi, [game]
      XOR eax, eax
      MOV ecx, SZ_OF_GAME
      REP STOSB
    }
    #undef SZ_OF_GAME
#else
    memset(&game, 0, sizeof(game));
#endif
    game.start_time = time;
    game.last_score = 1000.F;
    game.game_state = game_state::playing;

    reset_board(time);
  }

  #pragma code_seg(".update_res_and_mouse")
  void MS_INLINE update_res_and_mouse(UINT uMsg, LPARAM lParam) {
          static_assert(STATE__RES_X + 4 == STATE__MOUSE_X, "STATE__RES_X+4");
          static_assert(STATE__RES_Y + 4 == STATE__MOUSE_Y, "STATE__RES_Y+4");
          assert(uMsg == WM_SIZE || uMsg == WM_MOUSEMOVE);
#ifdef APPLY_ASSEMBLER
    __asm {
      mov         eax,[lParam]
      // ecx is x
      movsx       ecx,ax
      // eax is y
      sar         eax,0x10
      mov         edx,[uMsg]

      // Check is less than 0x10
      sub         edx,0x10
      // edx is 0 if so
      sbb         edx,edx
      // but flip its
      not         edx
      // Offset depending on message
      and         edx,0x10
      lea         esi,[state+edx];
      push        eax
      fild        [esp]
      fstp        [esi+4]
      push        ecx
      fild        [esp]
      fstp        [esi]
      test edx, edx
      jnz         wm_mousemove
      // edx is 0
      push        edx
      push        edx
      call        glViewport
      sub         esp,8
  wm_mousemove:
      add         esp,8
    }
#else
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    int o = WM_SIZE == uMsg ? 0 : 4;
    state[STATE__RES_X+o] = static_cast<GLfloat>(x);
    state[STATE__RES_Y+o] = static_cast<GLfloat>(y);
    if (!o)
      glViewport(0, 0, x, y);
#endif
  }

  #pragma code_seg(".update_mouse_buttons")
  void MS_INLINE update_mouse_buttons(UINT uMsg) {
    static_assert(WM_LBUTTONDOWN  == 0x201, "WM_LBUTTONDOWN");
    static_assert(WM_LBUTTONUP    == 0x202, "WM_LBUTTONUP"  );
    static_assert(WM_RBUTTONDOWN  == 0x204, "WM_RBUTTONDOWN");
    static_assert(WM_RBUTTONUP    == 0x205, "WM_RBUTTONUP"  );
    assert(uMsg >= 0x201 && uMsg <= 0x205);
    assert(uMsg != 0x203);
#ifdef APPLY_ASSEMBLER
    __asm {
      mov eax                 , 0x205
      sub eax                 , [uMsg]
      push 3
      pop ecx
      cdq
      div ecx
      mov [mouse_buttons+eax] , dl
    }
#else
    auto i = -(static_cast<int>(uMsg) - 0x205);
    mouse_buttons[i/3] = i%3;
#endif
  }
  #pragma code_seg(".draw_game")
  void MS_NOINLINE draw_game(float game_time) {
    int const size    = sizeof(state)/sizeof(GLfloat);
    auto board_time   = GAME_SPEED*(game_time-game.start_time);

    auto clear_factor = (board_time-game.lock_time)/CLEAR_DEADLINE;
    clear_factor      = clear_factor > 1.F ? 1.F : clear_factor;
    auto board_score  = game.last_score - clear_factor*(game.last_score - game.locked_score);

    assert(game.game_state == game_state::playing || game.game_state == game_state::game_over);
    if (game.game_state == game_state::game_over) {
      board_score = game.last_score;
    }

    // Setup state
    state[STATE__GAME_TIME  ] = GAME_SPEED*game_time;
    state[STATE__BOARD_TIME ] = board_time;
    state[STATE__BOARD_SCORE] = board_score;
    state[STATE__REMAINING  ] = static_cast<GLfloat>((CELLS*CELLS-BOMBS_PER_BOARD) - game.board.uncovered);

    auto mp_y   = (state[STATE__RES_Y]-2.F*state[STATE__MOUSE_Y])/state[STATE__RES_Y];
    auto mp_x   = (2.F*state[STATE__MOUSE_X]-state[STATE__RES_X])/state[STATE__RES_Y];

    auto mcp_x  = mp_x;
    auto mcp_y  = mp_y;

    mcp_x       /= CELL_DIM;
    mcp_y       /= CELL_DIM;

    mcp_x       -= 0.5F;
    mcp_y       -= 0.5F;

#ifdef NOCRT
    float mnp_x;
    float mnp_y;
    _asm {
      MOVSS       xmm0  , mcp_x
      ROUNDSS     xmm0  , xmm0, 0
      MOVSS       mnp_x , xmm0

      MOVSS       xmm0  , mcp_y
      ROUNDSS     xmm0  , xmm0, 0
      MOVSS       mnp_y , xmm0
    }

#else
    auto mnp_x  = roundf(mcp_x);
    auto mnp_y  = roundf(mcp_y);
#endif

    static_assert(CELLS%2 == 0, "Expected cells to be even");

    mnp_x       += CELLS*0.5F;
    mnp_y       += CELLS*0.5F;

#ifdef NOCRT
    float cif = mnp_x+mnp_y*CELLS;
    int ci;
    _asm {
      CVTTSS2SI eax , cif
      MOV       ci  , eax
    }
#else
    auto ci      = static_cast<int>(mnp_x+mnp_y*CELLS);
#endif

    if (mnp_x >= 0 && mnp_x < CELLS && mnp_y >= 0 && mnp_y < CELLS) {
      assert(ci >= 0 && ci < CELLS*CELLS);
      auto & cell = game.board.cells[ci];
      cell.mouse_time = board_time;

      if (cell.state == cell.next_state && game.game_state == game_state::playing) {
        // React on mouse click if state is up to date and we are playing

        if (!mouse_buttons[BTN__LEFT] && mouse_buttons_previous[BTN__LEFT]) {
          // Left button released
          switch (cell.state) {
            case cell_state::uncovered:
              {
                int near_flags = 0;
                auto ncp = cell.near_cells;
                assert(*ncp);
                do {
                    if ((*ncp)->state == cell_state::covered_flag) {
                      ++near_flags;
                    }
                } while(*++ncp);
                if (near_flags == cell.near_bombs) {
                  ncp = cell.near_cells;
                  assert(*ncp);
                  do {
                      switch ((*ncp)->state) {
                        case cell_state::covered_empty:
                          (*ncp)->next_state = cell_state::uncovering;
                          break;
                      }
                  } while(*++ncp);
                }
              }
              break;
            case cell_state::covered_empty:
            case cell_state::covered_flag:
              cell.next_state   = cell_state::uncovering;
              break;
          }
        }

        if (!mouse_buttons[BTN__RIGHT] && mouse_buttons_previous[BTN__RIGHT]) {
          // Right button released
          switch (cell.state) {
            case cell_state::covered_empty:
            case cell_state::covered_flag:
              // Toggle flag tile
              cell.next_state = cell.state == cell_state::covered_empty
                ? cell_state::covered_flag
                : cell_state::covered_empty
                ;
              break;
          }
        }
      }
    }

    if (board_time >= game.next_state_advance) {
      game.next_state_advance = board_time + STATE_SLEEP;

      for (auto & cell : game.board.cells) {
        switch (cell.state) {
          case cell_state::uncovering:
            if (cell.has_bomb) {
              cell.next_state = cell_state::exploding;
              game.game_state = game_state::game_over;
              game.last_score = board_score;
            } else {
              ++game.board.uncovered;
              if (BOMBS_PER_BOARD + game.board.uncovered >= CELLS*CELLS) {
                game.boards_cleared++;
                auto new_score      = board_score+1000.F*game.boards_cleared;
                game.lock_time      = board_time;
                game.locked_score   = new_score*0.5F > game.locked_score ? new_score*0.5F : game.locked_score;
                game.last_score     = new_score;
                game.game_state = game_state::resetting_board;
              }
              cell.next_state = cell_state::uncovered;
              if (cell.near_bombs == 0) {
                auto ncp = cell.near_cells;
                assert(*ncp);
                do {
                  switch((*ncp)->state) {
                    case cell_state::covered_empty:
                    case cell_state::covered_flag:
                      (*ncp)->next_state = cell_state::uncovering;
                      break;
                  }
                } while(*++ncp);
              }
            }
            break;
          case cell_state::exploding:
            cell.next_state = cell_state::exploded;
            auto ncp = cell.near_cells;
            assert(*ncp);
            do {
              switch((*ncp)->state) {
                case cell_state::exploding:
                case cell_state::exploded:
                  break;
                default:
                  (*ncp)->next_state = cell_state::exploding;
                  break;
              }
            } while(*++ncp);
            break;
        }
      }

      for (auto & cell : game.board.cells) {
        if (cell.state != cell.next_state) {
          cell.prev_state   = cell.state;
          cell.state        = cell.next_state;
          cell.changed_time = board_time;
        }
      }
    }

    mouse_buttons_previous[0] = mouse_buttons[0];
    mouse_buttons_previous[1] = mouse_buttons[1];

    //  Jump to first cell
    auto s = state+4*STATE_SIZE;
    // Setup cells
    for (auto & cell : game.board.cells) {
      *s++ = cell.state           != cell_state::uncovered ? static_cast<GLfloat>(cell.state) : static_cast<GLfloat>(-cell.near_bombs);
      *s++ = cell.prev_state      != cell_state::uncovered ? static_cast<GLfloat>(cell.prev_state) : static_cast<GLfloat>(-cell.near_bombs);
      *s++ = cell.changed_time    ;
      *s++ = cell.mouse_time      ;
    }
    assert(s == state + TOTAL_STATE);

    // Use the previously compiled shader program
    ((PFNGLUSEPROGRAMPROC)wglGetProcAddress(nm_glUseProgram))(fragmentShaderProgram);
    // Sets shader parameters
    ((PFNGLUNIFORM4FVPROC)wglGetProcAddress(nm_glUniform4fv))(
        0 // Uniform location
      , size
      , state
      );
    // Draws a rect over the entire window with fragment shader providing the gfx
    glRects(-1, -1, 1, 1);
  }

  #pragma code_seg(".WndProc")
  LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
      // It's time to stop!
      case WM_DESTROY:
      case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
      // Resized the window? No problem!
      case WM_SIZE:
      case WM_MOUSEMOVE:
        update_res_and_mouse(uMsg, lParam);
        break;
#ifndef NO_KEY_TEST
      case WM_KEYDOWN:
      case WM_CHAR:
        // Done on Escape
        if (wParam == VK_ESCAPE) {
          PostQuitMessage(0);
          return 0;
        } else if (wParam == 'R') {
          game.game_state = game_state::resetting_game;
#ifdef _DEBUG
        } else if (wParam == 'B') {
          game.game_state = game_state::resetting_board;
#endif
        }
        break;
#endif
#ifndef NO_SYS_COMMAND
      // To be ignored
      case WM_SYSCOMMAND:
        if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
          return 0;
        break;
#endif
      // Capture mouse buttons
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_RBUTTONDOWN:
      case WM_RBUTTONUP:
        update_mouse_buttons(uMsg);
#ifdef _DEBUG
        printf("button state: %d(%d),%d(%d)\n", mouse_buttons[BTN__LEFT], mouse_buttons_previous[BTN__LEFT], mouse_buttons[BTN__RIGHT], mouse_buttons_previous[BTN__RIGHT]);
#endif
        break;
    }

    // Apply default window message handling
    return(DefWindowProcA(hWnd, uMsg, wParam, lParam));
  }

  #pragma code_seg(".WndProc")
  void CALLBACK waveOutProc(
      HWAVEOUT  hwo
    , UINT      uMsg
    , DWORD_PTR dwInstance
    , DWORD_PTR dwParam1
    , DWORD_PTR dwParam2
    )
  {
    if (uMsg != WOM_DONE) return;
      waveHeader.lpData         = reinterpret_cast<LPSTR>(waveBuffer+SU_RESTART_POS);
      waveHeader.dwBufferLength = (SU_BUFFER_LENGTH-SU_RESTART_POS) * sizeof(SUsample);
      waveHeader.dwFlags        &= ~WHDR_DONE;

      auto waveWriteOk = waveOutWrite(
        hwo
      , &waveHeader
      , sizeof(waveHeader)
      );
      assert(waveWriteOk == MMSYSERR_NOERROR);
  }
}

#pragma code_seg(".main")
#ifdef NAKED_ENTRYPOINT
void entrypoint() {
#else
int __cdecl main() {
#endif
  auto app_start_time = GetTickCount() / 1000.F;

/*
  auto dpiAware = SetProcessDPIAware();
  assert(dpiAware);
*/

  auto hinstance    = GetModuleHandle(0);
  assert(hinstance);

  // Use default arrow cursor
  auto hcursor      = LoadCursor(nullptr, IDC_ARROW);

  // Setups the windows class
  windowClassSpecification.hInstance      = hinstance   ;
  windowClassSpecification.hCursor        = hcursor     ;

  // Registers the windows class
  auto regOk = RegisterClassA(&windowClassSpecification);
  assert(regOk);

  auto dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_POPUP | WS_MAXIMIZE;

  // Create the window using the class we registered
  auto hwnd = CreateWindowExA(
    0                                             // dwExStyle
  , windowClassSpecification.lpszClassName        // lpClassName
  , nullptr                                       // lpWindowName
  , dwStyle                                       // dwStyle
  , 0                                             // nX
  , 0                                             // nY
  , XRES                                          // nWidth
  , YRES                                          // nHeight
  , nullptr                                       // hWndParent
  , nullptr                                       // hMenu
  , nullptr                                       // hInstance
  , nullptr                                       // lpParam
  );
  assert(hwnd);

  auto setTextOk = SetWindowTextA(hwnd, windowClassSpecification.lpszClassName);
  assert(setTextOk);

  // We need the Device Context to do Windows graphics
  auto hdc = GetDC(hwnd);
  assert(hdc);

  // Find a pixel format that is compatible with OpenGL
  auto pixelFormat = ChoosePixelFormat(
    hdc
  , &pixelFormatSpecification
  );
  assert(pixelFormat);

  // Set the pixel format on the Device Context to prepare it for OpenGL
  auto setOk = SetPixelFormat(
    hdc
  , pixelFormat
  , nullptr
  );
  assert(setOk);

  // Create OpenGL Context
  auto hglrc = wglCreateContext(hdc);
  assert(hglrc);

  // Sets the OpenGL context as the current one
  auto makeOk = wglMakeCurrent(hdc, hglrc);
  assert(makeOk);

  // Init our game
  lcg_state = GetTickCount()+0x19740531U;
//    lcg_state = 19740531;

  // Bit of debugging info during debug builds
  //  Don't want to waste bytes on that in Release mode
#ifdef _DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  ((PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback"))(debugCallback, 0);
#endif

  // Compiles the provided fragment shader into a shader program
  fragmentShaderProgram = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress(nm_glCreateShaderProgramv))(GL_FRAGMENT_SHADER, 1, fragmentShaders);

#ifdef _DEBUG
  ((PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog"))(fragmentShaderProgram, sizeof(debugLog), NULL, debugLog);
  printf(debugLog);
  glDisable(GL_DEBUG_OUTPUT);
#endif

#ifdef INIT_MUSIC
  // Now init the music.
  //  The way sointu works is that we call su_render_song which writes samples
  //  to a wave buffer
  //  Then we just ask Windows to play it for us

  // Version v0.3.0 of sointu has an issue in that the EBX register is not restored
  //  So save it with some inline assembler
  //  Fix coming: https://github.com/vsariola/sointu/issues/130
  _asm {
    push ebx
  }
  // Load gmdls sound
  su_load_gmdls();
  // And restore the ebx register
  _asm {
    pop ebx
  }

#ifdef USE_SOUND_THREAD
  // Create the wave buffer in a separate thread so we don't have to wait for it
  auto hthread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)su_render_song, waveBuffer, 0, 0);
  assert(hthread);
#else
  // We don't mind waiting for the sound.
  su_render_song(waveBuffer);
#endif
#endif

  HWAVEOUT waveOut;
  auto waveOpenOk = waveOutOpen(
    &waveOut
  , WAVE_MAPPER
  , &waveFormatSpecification
  , reinterpret_cast<DWORD_PTR>(&waveOutProc)
  , 0
  , CALLBACK_FUNCTION
  );
  assert(waveOpenOk == MMSYSERR_NOERROR);

  waveHeader.lpData         = reinterpret_cast<LPSTR>(waveBuffer);
  waveHeader.dwBufferLength = (SU_BUFFER_LENGTH) * sizeof(SUsample);

  auto wavePrepareOk = waveOutPrepareHeader(
    waveOut
  , &waveHeader
  , sizeof(waveHeader)
  );
  assert(wavePrepareOk == MMSYSERR_NOERROR);

  auto waveWriteOk = waveOutWrite(
    waveOut
  , &waveHeader
  , sizeof(waveHeader)
  );
  assert(waveWriteOk == MMSYSERR_NOERROR);

#ifdef _DEBUG
  auto frame_count  = 0.F;
  auto next_report  = app_start_time+1.F;
#endif

   auto done = false;

  // Loop until done
  while(!done) {
    MSG msg;
    // The classic window message pump
    while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) done = 1;
      // Result intentionally ignored
      TranslateMessage(&msg);
      // Result intentionally ignored
      DispatchMessageA(&msg);
    }

    auto time = (GetTickCount() / 1000.F) - app_start_time;

#ifdef _DEBUG
    ++frame_count;
    if (time >= next_report) {
      auto fps = frame_count/(time);
      printf("FPS:%f\n", fps);
      next_report = 1.F+time;
    }
#endif


    switch (game.game_state) {
      case game_state::resetting_game:
        // Useful for debugging potentially buggy boards
        //lcg_state = 0x1e0d6339;
#ifdef _DEBUG
        printf("Resetting game with seed: 0x%x\n", lcg_state);
#endif
        reset_game(time);
        break;
      case game_state::resetting_board:
        // Useful for debugging potentially buggy boards
        //lcg_state = 0x1e0d6339;
#ifdef _DEBUG
        printf("Resetting board with seed: 0x%x\n", lcg_state);
#endif
        reset_board(time);

        game.game_state     = game_state::playing;
        break;
    }

    // Windows message handling done, let's draw some gfx

    // Draw the game
    draw_game(time);

    // Swap the buffers to present the gfx
    auto swapOk = SwapBuffers(hdc);
    assert(swapOk);
  }

  // We are done, just exit. No need to waste bytes on cleaning
  //  up resources. Windows will do it for us.

#ifdef NAKED_ENTRYPOINT
  ExitProcess(0);
#else
  return 0;
#endif
}

