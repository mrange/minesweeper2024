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

#version 430

// ----------------------------------------------------------------------------
// Required prelude

const int
  CELLS       = 12
, HCELLS      = CELLS/2
, STATE_SIZE  = 2
;

const float
  STATE_SLEEP = 1./8
;

// The result of the shader
out vec4 fcol;
// Set by draw_game
uniform vec4[146] state;  // 146 is CELLS*CELLS + STATE_SIZE

// ----------------------------------------------------------------------------
// The Shader
//  The entrypoint is: void main()
//  gl_FragCoord is the input fragment position
//  fcol is the output fragment color

const float
    BORDER_DIM      = .825
  , CELL_DIM        = BORDER_DIM/HCELLS
  , PI              = acos(-1)
  , TAU             = 2*PI
  , tcw             = BORDER_DIM/6
  , fz              = .25
  , tz              = .05
  , cz              = CELL_DIM
  , tr              = .75
  , textChars[12]   = float[](
        5   // S
      , 12  // C
      , 0   // O
      , 17  // R
      , 14  // E
      , 19  // BLANK
      , 16  // L
      , 14  // E
      , 15  // F
      , 18  // T
      , 19  // BLANK
      , 19  // BLANK
      )
  ;

vec2
    ddim        = vec2(.75, .075)
  , bstates[7]  = vec2[](
      vec2(0  ,0)
    , vec2(.5 ,1)
    , vec2(4  ,2)
    , vec2(10 ,1)
    , vec2(10 ,2)
    , vec2(4  ,1)
    , vec2(10 ,2)
    )
  ;

int ddigits[20] = int[](
    0x7D // 0, O
  , 0x50 // 1
  , 0x4F // 2
  , 0x57 // 3
  , 0x72 // 4
  , 0x37 // 5, S
  , 0x3F // 2
  , 0x51 // 7
  , 0x7F // 8
  , 0x77 // 9
  , 0x7B // A
  , 0x3E // B
  , 0x2D // C
  , 0x5E // D
  , 0x2F // E
  , 0x2B // F
  , 0x2C // L
  , 0xFB // R
  , 0x29 // T
  , 0x00 // BLANK
  );
/*
int ddigits[20] = int[](
    125 // 0, O   (0x7D)
  , 80  // 1      (0x50)
  , 79  // 2      (0x4F)
  , 87  // 3      (0x57)
  , 114 // 4      (0x72)
  , 55  // 5, S   (0x37)
  , 63  // 6      (0x3F)
  , 81  // 7      (0x51)
  , 127 // 8      (0x7F)
  , 119 // 9      (0x77)
  , 123 // A      (0x7B)
  , 62  // B      (0x3E)
  , 45  // C      (0x2D)
  , 94  // D      (0x5E)
  , 47  // E      (0x2F)
  , 43  // F      (0x2B)
  , 44  // L      (0x2C)
  , 251 // R      (0xFB)
  , 41  // T      (0x29)
  , 0   // BLANK  (0x00)
);
*/


vec3 palette(float a) {
  return 1+sin(vec3(-4,3,1)/2+a);
}

float segmentx(vec2 p, vec2 dim) {
  p.x = abs(p.x);
  float o = max(dim.x-dim.y, 0)/2;
  return (p.x < o ? abs(p.y) : length(p-vec2(o, 0)))-dim.y;
}

vec3 digit(vec2 p, vec3 acol, vec3 icol, float aa, float n) {
  vec2
      ap = abs(p)
    , p0 = p
    , p2 = p
    , p1 = ap.yx-.5
    , sp = sign(p)
    ;

  if (ap.x > (.6+ddim.y)||ap.y > (1.1+ddim.y)) return vec3(0);

  p0.y -= 1;
  float n0 = round(p0.y);
  p0.y -= n0;

  p2.y  = ap.y-.5;
  p2    = abs(p2);

  // Praying bit shift operations aren't TOO slow
  vec3 scol =
      ((ddigits[int(floor(n))] & (1 << int(dot(vec2(1, -1)/sqrt(2), p2) > 0 ? (3+(sp.x+1)/2 + sp.y+1) : -n0))) == 0)
    ? icol
    : acol
    ;

  return scol*smoothstep(aa, -aa, min(segmentx(p0, ddim), segmentx(p1, ddim)));
}

// License: Unknown, author: Unknown, found: don't remember
float hash(vec2 co) {
  return fract(sin(dot(co.xy ,vec2(12.9898,58.233))) * 13758.5453);
}

void main() {
  vec2
      res = state[0].xy
    , p   = (2*gl_FragCoord.xy-res)/res.yy
    , ap  = abs(p)
    , tcp = p
    , ts  = vec2(tr*tcw, tcw)
    , cp
    , np
    , tnp
    , cp4
    ;

  float
      atm = state[0].z/2
    , bs  = state[1].z
    , gtm = state[0].w
    , rem = state[1].w
    , aa  = sqrt(2) / res.y
    , caa = aa/cz
    , taa = aa/tz
    , faa = aa/(fz*cz)
    , sty = sign(tcp.y)
    , fi
    ;

  vec3
      col = vec3(0)
    , p3  = vec3(p, 0)
    , ro  = vec3(0,0,atm)
    , rd  = normalize(vec3(p,2))
    ;

  cp = p/cz-.5;

  tcp.x -= p.y/5-tcw*tr/2;
  tcp.y = abs(tcp.y)-.9;

  tcp /= ts;

  tnp = round(tcp);
  np  = round(cp);

  tcp -= tnp;
  cp -= np;

  cp4 = cp*cp*cp*cp;

  tcp *= ts;
  tcp.y *= sty;

  np += HCELLS;

  tcp /= tz;

  fi = np.x+np.y*CELLS+STATE_SIZE;

  for (int i = 1; i < 9; ++i) {
    float tw = -(ro.x-6*sqrt(i))/abs(rd).x;

    vec3 wp = ro+rd*tw;

    vec2
        wp2 = wp.yz*2E-2
      , wn2 = round(wp2)
      , wc2 = wp2 - wn2
      ;

    if (hash(wn2+i+.5*sign(rd.x)) < .5)
      wc2 = vec2(wc2.y, -wc2.x);

    float
        fo = smoothstep(-.7, 1., sin(wp.z/9+atm+i+sign(rd.x)))
      , wd  = abs(min(length(wc2+.5)-.5, length(wc2-.5)-.5))-.025
      ;

    col += palette(5E-2*tw+atm)*exp(-3E-3*tw*tw)*25E-4/max(abs(wd), 3E-3*fo)*fo;
  }

  col *= .8+.3*sin(length(p)*res.y*TAU/8);

  if (tnp.y == 0 && abs(tnp.x-.5) < 6) {
    float
      v = sty < 0 ? rem : bs
    , d = tnp.x > 0 ? mod(v*pow(10, tnp.x-6), 10) : textChars[int(tnp.x+5+3*(1-sty))];
    vec3
        acol = palette(2.5+1.5*sty+.4*tcp.y+(tnp.x < 1 ? 0:3))
      , icol = acol*.075
      ;
    col += digit(tcp, acol, icol, taa, d);
  }

  if (max(ap.x, ap.y) < BORDER_DIM) {
    vec4 c = state[int(fi)];

    float
        cts = c.z
      , mts = c.w
      , d1  = pow(dot(cp4, cp4), 1./8)-0.45
      , mfo = smoothstep(mts+1./2, mts+1./8, gtm)
      , sfo = smoothstep(cts, cts+STATE_SLEEP, gtm)
      ;

    vec3 ccol  = tanh(8*col)/8;

    for (int i = 0; i < 2; ++i) {
      float
        cs    = i == 0?c.y:c.x
      , m     = i == 0?1-sfo:sfo
      , gd    = abs(length(cp)-mfo/9)
      ;
      vec2 bstate = bstates[int(cs)];
      for (float yy = 0; yy < bstate.y; ++yy) {
        gd = min(abs(gd-.1), gd);
      }
      vec3 scol =(.2+palette(2-cs))*(bstate.x*5E-3/max(gd, 3E-3));


      if (cs < 1) {
        vec2 fcp = cp/fz;
        fcp.x += -fcp.y/8;
        vec3
            acol = palette(cs/2-fcp.y/2)
          , icol = acol/20
          ;
        if (cs < 0) {
  //        ccol += acol*1E-2/max(length(fcp), 5E-1);
          ccol += digit(fcp, acol, icol, faa, -cs);
        }
      } else {
        ccol   = mix(ccol, scol, m*smoothstep(caa, -caa, d1));
      }
    }

    col = mix(col, ccol, smoothstep(caa, -caa, d1));
    col = mix(col, mix(palette(3.+p.y)/4,vec3(1), mfo), smoothstep(caa, -caa, abs(d1)-1./80));
  }


  fcol = vec4(sqrt(tanh(col)), 1);
}
