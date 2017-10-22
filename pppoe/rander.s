extrn shitcpy
extrn rotl
extrn rotr
extrn getrandnum
extrn dh


getrandnum:
	rnd r0
	re


rotl:
	rl r0, r0, r1
	re
rotr:
	rr r0, r0, r1
	re


dh:
	xr r2, r2, r2 ; 0 r5
	xr r4, r4, r4 ; 0 r3
	xr r6, r6, r6 ; 0 r6
	xr r7, r7, r7 ; 0 r6

	ad r5, r1, r7

	ml r3, 859
	mh r3, 93100 ; move high 0x5aeb35b

	ori r7, r7, 1

	; state , r2:3 has 0x5aeb35b 
	; r4:5 has 2
	; r6:r7 is result
doLoop:
	ani r1, r0, 1
	be noChange
	mum r6, r6, r4
	mdm r6, r6, r2
noChange:
	mum r4, r4, r4
	mdm r4, r4, r2
	dvi r0, r0, 2
	bn doLoop
	xr r0, r0, r0
	or r0, r0, r7
	re

shitcpy:
	dmt r0, r1, r2
	re


;    result := 1
;    base := base mod modulus
 ;   while exponent > 0
  ;      if (exponent mod 2 == 1):
   ;        result := (result * base) mod modulus
    ;    exponent := exponent >> 1
     ;   base := (base * base) mod modulus