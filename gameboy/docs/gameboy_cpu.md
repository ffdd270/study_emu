# Gameboy

# 주요 스펙

- 8비트 CPU
- 8K 메인 렘
- 8K 비디오 렘
- 해상도 160x144 ( 20x18 타일 )
- 최대 스프라이트 : 40
- 라인당 최대 스프라이트 : 10
- 최대 스프라이트 크기 : 8x16
- 최소 스프라이트 크기 : 8x8
- 클럭 속도 : 4.194304 Mhz

1 머신 사이클 = 4 클락 사이클.  즉.. 

머신 사이클 : 1.05Mhz  = NOP 1 사이클.

클락 사이클 : 4.19Mhz  = NOP 4 사이클

# 프로세서

게임 보이 CPU는 Intel 8080과 비슷함. 8080의 교환 명령어를 제외하곤 모든 명령어가 포함되어있음. Z80 프로세서와는 Intel 8080보다 더 비슷함. Z80과 비교하자면, 몇몇 명령어가 추가되었음. 

아래는 추가된 명령어. 

```wasm
ADD SP, nnn          ;nn = signed byte
LD**I** (HL), A          ;(Load and Increment) Write A to (HL) and **I**ncrement HL (A에 HL을 긁고, HL을 증가시킨다.)
LD**D** (HL), A          ;(Load and Decrement) Write A to (HL) and **D**ecrement HL (A에 HL 저장후 HL을 감소시킨다.)
LD**I** A, (HL)          ;Write (HL) to A and increment HL
LD**D** A, (HL)          ;Write (HL) to A and decrement HL
LD  A, ($FF00+nn)    ;(LoaD)
LD  A, ($FF00+C)
LD  ($FF00+nn), A  
LD  ($FF00+C), A
LD  (nnnn), SP
LD  HL, SP+nn        ;nn = singed byte
STOP                 ;Stop Processor & screen until button press ( 버튼 입력까지 프로세서 스크린 중지. )

SWAP r               ;Swap high & low nibble of r ( R의 높고 & 낮은 비트 상호 교환 ) 
```

삭제된 명령어들. 

- IX나 IY 레지스터를 사용하는 모든 명령어들 .
- 모든 IN / OUT 명령들.
- 모든 교환 명령들.
- ED를 접두사로 쓰는 모든 명령들 ( RETI 로 리맵되었음. )
- 패리티 / 부호 / 오버플로우에 대한 모든 조건부 JUMP / CALL / RET( 리턴 ) 명령어들 .

# 메모리 맵

Interrupt Enable Register ( 인터럽트 활성 레지스터 )

0xFFFF 

---

Internal RAM ( 내부 램 )

0xFF80

---

Empty But unusable for I/O ( 비어있지만 사용하지 않는 I/O를 위한 곳 )

0xFF4C 

---

I/O ports  (  I/O 포트들  )

0xFF00

---

Empty But unusable for I/O ( 비어있지만 사용하지 않는 I/O를 위한 곳 )

0xFEA0

---

Sprite Attrib Memory ( 스프라이트 속성 메모리 )

0xFE00

---

Echo of 8kb Internal RAM ( ??? )

0xE000

---

8kB Internal RAM ( 8kb 내부 램 )

0xC000

---

8kB Switchable RAM bank ( 8kb 교환가능한 RAM 뱅크)

0xA000

---

8kB Video RAM ( 8kB 비디오 램 )

0x8000

---

16kB switchable ROM bank ( 16kb 교환 가능한 ROM 뱅크 )

0x4000 (32kB 카트리지)

---

16kB ROM bank #0 

0x0000 (32kB 카트리지)

---