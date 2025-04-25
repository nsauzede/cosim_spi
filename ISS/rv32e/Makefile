
RVQEMU:=$(shell hash -r; which qemu-system-riscv32)
RVGDB:=riscv32-elf-gdb
CP:=cp -f

# Important: must specify cpu-num=0 on top of addr=0x0, to override default start_addr==0x1000
RVQOPT:= -S -s -machine darkriscv -bios none -device loader,file=darksocv.bin,addr=0x0,cpu-num=0
RVQOPT+=-nographic
ifdef MEM
RVQOPT+=-m $(MEM)
endif

ifndef APPLICATION
APPLICATION:=spidemo
#APPLICATION:=primes
endif

DROPT:=
ifdef APPLICATION
DROPT+=APPLICATION=$(APPLICATION)
endif
DROPT+=NOBANNER=1

RVGOPT:=-nx -ex "set confirm off" -ex "target remote :1234" -ex "set confirm on"
RVGOPT+=-ex "b main" -ex 'disp/i$$pc'

.PHONY: all clean sim
all:
	echo "DROPT=$(DROPT)"
	(cd darkriscv/src ; $(MAKE) $@ $(DROPT))
clean:
	(cd darkriscv/src ; $(MAKE) $@ $(DROPT))
	$(RM) darksocv.bin

sim: all
	$(CP) darkriscv/src/darksocv.bin .
	-killall -9 `basename $(RVQEMU)`
	echo "RVQOPT=$(RVQOPT)"
	$(RVQEMU) $(RVQOPT) &

debug_:
	-killall -9 `basename $(RVGDB)`
	$(RVGDB) $(RVGOPT) darkriscv/src/darksocv.o
	-killall -9 `basename $(RVQEMU)`
	-killall -9 `basename $(RVGDB)`

debug: sim debug_

# in qemu console:
# (qemu) info registers
# ...
#  pc       00001000
# (qemu) xp/8i 0x1000
# 0x00001000:  00000513          mv                      a0,zero
# 0x00001004:  f1402573          csrrs                   a0,mhartid,zero
# 0x00001008:  00050463          beqz                    a0,8                    # 0x1010
# 0x0000100c:  0000006f          j                       0                       # 0x100c
# 0x00001010:  400005b7          lui                     a1,262144
# 0x00001014:  00058503          lb                      a0,0(a1)
# 0x00001018:  04050663          beqz                    a0,76                   # 0x1064
# 0x0000101c:  07500513          addi                    a0,zero,117


# in gdb:
# (gdb) x/8i $pc
# => 0x1000 <getchar+20>: li      a0,0
#    0x1004 <getchar+24>: csrr    a0,mhartid
#    0x1008 <putchar>:    beqz    a0,0x1010 <putchar+8>
#    0x100c <putchar+4>:  j       0x100c <putchar+4>
#    0x1010 <putchar+8>:  lui     a1,0x40000
#    0x1014 <putchar+12>: lb      a0,0(a1)
#    0x1018 <putchar+16>: beqz    a0,0x1064 <gets+32>
#    0x101c <putchar+20>: li      a0,117
# (gdb) disp/i $pc
# 1: x/i $pc
# => 0x1000 <getchar+20>: li      a0,0
# (gdb) si
# 0x00001004 in getchar ()
# 1: x/i $pc
# => 0x1004 <getchar+24>: csrr    a0,mhartid
