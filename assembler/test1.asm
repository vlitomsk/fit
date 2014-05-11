
;some comment foo bar


NOP
NOP

lab1 : NOP ; this is comment

; comment 2

SUB ra,   rb
ADD     rc, rd
inc ra
lab2: dec rb
cmp rc, rd
jMp 0x56
jl lab2
JG lab1
je 095
MOV ra, 0x12


NOP
