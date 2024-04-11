.PS
# Connectors.m4
log_init
cct_init

include(style.m4)

  ifelse(ifpgf(T)`'ifpstricks(T)`'ifpsfrag(T),T,`command "{\footnotesize"')

define(`icSN74HC595',`[ Chip: box wid_ lg_chipwd ht_ 10*lg_pinsep
   lg_pin(Chip.sw_+(0,lg_pinsep),GND,Pin8,w,8)
   lg_pin(Chip.sw_+(0,2*lg_pinsep),lg_bartxt(OE),Pin13,wN,13)
   lg_pin(Chip.sw_+(0,4*lg_pinsep),lg_bartxt(SRCLR),Pin10,wN,10)
   lg_pin(Chip.sw_+(0,5*lg_pinsep),V`'ic_tiny(CC),Pin16,w,16)
   lg_pin(Chip.sw_+(0,7*lg_pinsep),SER,Pin14,w,14)
   lg_pin(Chip.sw_+(0,8*lg_pinsep),RCLK,Pin12,w,12)
   lg_pin(Chip.sw_+(0,9*lg_pinsep),SRCLK,Pin11,w,11)
   lg_pin(Chip.se_+(0,lg_pinsep),Q`'ic_tiny(H\rq),Pin9,e,9)
   lg_pin(Chip.se_+(0,2*lg_pinsep),Q`'ic_tiny(H),Pin7,e,7)
   lg_pin(Chip.se_+(0,3*lg_pinsep),Q`'ic_tiny(G),Pin6,e,6)
   lg_pin(Chip.se_+(0,4*lg_pinsep),Q`'ic_tiny(F),Pin5,e,5)
   lg_pin(Chip.se_+(0,5*lg_pinsep),Q`'ic_tiny(E),Pin4,e,4)
   lg_pin(Chip.se_+(0,6*lg_pinsep),Q`'ic_tiny(D),Pin3,e,3)
   lg_pin(Chip.se_+(0,7*lg_pinsep),Q`'ic_tiny(C),Pin2,e,2)
   lg_pin(Chip.se_+(0,8*lg_pinsep),Q`'ic_tiny(B),Pin1,e,1)
   lg_pin(Chip.se_+(0,9*lg_pinsep),Q`'ic_tiny(A),Pin15,e,15)
    `$1']')

define(`ic3461AS', `[ Chip: box wid_ lg_chipwd ht_ 9*lg_pinsep
    lg_pin(Chip.sw_+(0,lg_pinsep),DP,Pin3,w,3)
    lg_pin(Chip.sw_+(0,2*lg_pinsep),G,Pin5,w,5)
    lg_pin(Chip.sw_+(0,3*lg_pinsep),F,Pin10,w,10)
    lg_pin(Chip.sw_+(0,4*lg_pinsep),E,Pin1,w,1)
    lg_pin(Chip.sw_+(0,5*lg_pinsep),D,Pin2,w,2)
    lg_pin(Chip.sw_+(0,6*lg_pinsep),C,Pin4,w,4)
    lg_pin(Chip.sw_+(0,7*lg_pinsep),B,Pin7,w,7)
    lg_pin(Chip.sw_+(0,8*lg_pinsep),A,Pin11,w,11)
    lg_pin(Chip.se_+(0,5*lg_pinsep),DIG4,Pin6,e,6)
    lg_pin(Chip.se_+(0,6*lg_pinsep),DIG3,Pin8,e,8)
    lg_pin(Chip.se_+(0,7*lg_pinsep),DIG2,Pin9,e,9)
    lg_pin(Chip.se_+(0,8*lg_pinsep),DIG1,Pin12,e,12)
    `$1']')

define(`relevantUNO', `[ Chip: box wid_ lg_chipwd ht_ 9*lg_pinsep
    lg_pin(Chip.sw_+(0,lg_pinsep),PB0,Pin8,w,8)
    lg_pin(Chip.sw_+(0,2*lg_pinsep),PB1,Pin9,w,9)
    lg_pin(Chip.sw_+(0,3*lg_pinsep),PB2,Pin10,w,10)
    lg_pin(Chip.sw_+(0,4*lg_pinsep),PB3,Pin11,w,11)
    lg_pin(Chip.sw_+(0,5*lg_pinsep),PB4,Pin12,w,12)
    lg_pin(Chip.sw_+(0,7*lg_pinsep),GND,Pin2,w,)
    lg_pin(Chip.sw_+(0,8*lg_pinsep),V`'ic_tiny(CC),Pin1,w,)

    lg_pin(Chip.se_+(0,lg_pinsep),PD4,Pin4,e,4)
    lg_pin(Chip.se_+(0,2*lg_pinsep),PD5,Pin5,e,5)
    lg_pin(Chip.se_+(0,3*lg_pinsep),PD6,Pin6,e,6)
    lg_pin(Chip.se_+(0,4*lg_pinsep),PD7,Pin7,e,7)
    `$1']')

   right_

IC1: icSN74HC595("SN74HC595" at Chip.sw-(0,0.05) below ljust)
IC2: ic3461AS("3461AS" at Chip.sw-(0,0.05) below ljust) \
     with .Chip.nw at IC1.Chip.e.x+1,IC1.Chip.n.y

RU: relevantUNO("Arduino UNO" at Chip.sw-(0,0.05) below ljust) \
     with .Chip.sw at IC2.Chip.w.x,IC2.Chip.n.y+0.5


linethick_(0.5)
    resistor(from IC1.Pin15.end to IC2.Pin11.end)
    resistor(from IC1.Pin1.end to IC2.Pin7.end)
    resistor(from IC1.Pin2.end to IC2.Pin4.end)
    resistor(from IC1.Pin3.end to IC2.Pin2.end)
    resistor(from IC1.Pin4.end to IC2.Pin1.end)
    resistor(from IC1.Pin5.end to IC2.Pin10.end)
    resistor(from IC1.Pin6.end to IC2.Pin5.end)
    RR: resistor(from IC1.Pin7.end to IC2.Pin3.end); "\SI{1}{\kilo\ohm}" at RR.centre-(0,0.15)

    line down_ 3*lg_pinsep from IC1.Pin13.end; ground(,T,Groundtype)
    dot(at IC1.Pin8.end)

    line from RU.Pin4.end to IC2.Pin12.end

    line right_ lg_pinsep from RU.Pin5.end
    line to IC2.Pin9.end+(lg_pinsep,0)
    line to IC2.Pin9.end

    line right_ 2*lg_pinsep from RU.Pin6.end
    line to IC2.Pin8.end+(2*lg_pinsep,0)
    line to IC2.Pin8.end

    line right_ 3*lg_pinsep from RU.Pin7.end
    line to IC2.Pin6.end+(3*lg_pinsep,0)
    line to IC2.Pin6.end

    line left_ (RU.Pin8.end.x-IC1.Pin11.end.x) from RU.Pin8.end
    line to IC1.Pin11.end

    line left_ (RU.Pin9.end.x-IC1.Pin12.end.x+lg_pinsep) from RU.Pin9.end
    line to IC1.Pin12.end-(lg_pinsep,0)
    line to IC1.Pin12.end

    line left_ (RU.Pin10.end.x-IC1.Pin14.end.x+2*lg_pinsep) from RU.Pin10.end
    line to IC1.Pin14.end-(2*lg_pinsep,0)
    line to IC1.Pin14.end

    resized(0.5,`bswitch',right_ 0.5 from RU.Pin11.end-(0.5,0))
    resized(0.5,`bswitch',right_ 0.5 from RU.Pin12.end-(0.5,0))
    line left_ 0.5 from RU.Pin2.end
    line up_ 6*lg_pinsep from RU.Pin11.end-(0.5,0); ground(,T,Groundtype,U)
    dot(at RU.Pin12.end-(0.5,0))
    dot(at RU.Pin2.end-(0.5,0))

    line up_ 2*lg_pinsep from RU.Pin1.end; { move up 0.03; "V\hbox{\tiny CC}" above }
    move left 0.1
    line right_ 0.2

    C1: capacitor(from RU.Pin1.end+(-0.5,lg_pinsep) to RU.Pin1.end+(0,lg_pinsep)); "\SI{0.1}{\micro\farad}" at C1.centre-(0,0.18)
    dot(at RU.Pin1.end+(-0.5,lg_pinsep))
    dot(at RU.Pin1.end+(0,lg_pinsep))

    line left_ 2*lg_pinsep from IC1.Pin10.end
    line left_ 2*lg_pinsep from IC1.Pin16.end
    line down_ 6*lg_pinsep; { move down 0.03; "V\hbox{\tiny CC}" below }
    move left 0.1
    line right_ 0.2
    dot(at IC1.Pin10.end-(2*lg_pinsep,0))

    "{\scriptsize B}" at RU.Pin12.end+(-0.37,0.07)
    "{\scriptsize A}" at RU.Pin11.end+(-0.37,0.07)

  ifelse(ifpgf(T)`'ifpstricks(T)`'ifpsfrag(T),T,`command "}"')

.PE