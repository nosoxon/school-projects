# style.m4 - circuit_macros overrides for diagram
# 20220224 Oliver Emery

define(`ic_tiny',`iflatex(`\hbox{\tiny `$1'}',ifsvg(`svg_small(`$1')',`$1'))')

define(`lg_pintxt',
 `"ifxfig(`$1',`ifsvg(`svg_small(`$1',75)',`sp_{\ttfamily\scriptsize `$1'}sp_')')"')

define(`lg_pin',`ifelse(`$1',,,`move to $1')
  linethick_(0.5)
  define(`dna_',`substr(`$4',1)')define(`m4lE',)define(`m4lch',0)dnl
  define(`m4ld',`ifelse(`$4',,e,`substr(`$4',0,1)')')dnl
  define(`m4lph',`ifelse(m4ld,n,0,m4ld,w,-1,m4ld,s,0,1)')dnl
  define(`m4lpv',`ifelse(m4ld,n,1,m4ld,w,0,m4ld,s,-1,0)')dnl
  define(`m4lpl',`ifelse(`$6',,`lg_plen',(`$6')/L_unit)')dnl
  sc_draw(`dna_',E,`define(`m4lE',1)dnl
    { line from rsvec_(lp_xy(0,N_rad)) \
      to rsvec_(lp_xy(-N_diam*sqrt(3)/2,0)) then to rsvec_(lp_xy(0,-N_rad)) }')
  ifelse(`$2',,,
   `{ move ifelse(m4ld,w,`right', m4ld,n,`down', m4ld,s,`up',`left') 0.02; dnl
      lg_pintxt(`$2') ifelse(m4ld,w,`ljust_', m4ld,n,`below_',
      m4ld,s,`above_',`rjust_') at Here dnl
      ifxfig(`+(lp_xy(-0.72bp__,0))') dnl
      ifelse(m4lE,1,`+svec_(lp_xy(-N_diam*sqrt(3)/2,0))') }')
  sc_draw(`dna_',N,`define(`m4lch',N_diam*L_unit)
    { NOT_circle \
        at rsvec_(lp_xy(N_rad,0)) }')
  sc_draw(`dna_',L,`define(`m4lch',N_rad*2.5*L_unit)
    {line from rsvec_(lp_xy(0,
      ifelse(m4ld,w,-,m4ld,s,-)N_rad*3/2)) to rsvec_(lp_xy(N_rad*2.5,0)) \
      then to Here }')
  sc_draw(`dna_',M,`define(`m4lch',N_rad*2.5*L_unit)
    { line to rsvec_(lp_xy(N_rad*2.5,
      ifelse(m4ld,w,-,m4ld,s,-)N_rad*3/2)) then to rsvec_(lp_xy(N_rad*2.5,0)) \
        then to Here}')
  {ifelse(`$3',,,`$3':) line to rsvec_(lp_xy(m4lpl,0)) chop m4lch chop 0 dnl
   ifinstr(dna_,I,` <- wid linethick*5.6bp__ ht linethick*7.2bp__ ')dnl
   ifinstr(dna_,O,` -> wid linethick*5.6bp__ ht linethick*7.2bp__ ')
   ifelse(`$5',,,`move to last line.c; lg_pintxt(`$5') dnl
     ifelse(m4ld,n,`rjust_', m4ld,w,`above_',m4ld,e,`above_',`rjust_')')
   }
   linethick_(2.0) ')
