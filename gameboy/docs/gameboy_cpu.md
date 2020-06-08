# Gameboy CPU Manual

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

Echo of 8kb Internal RAM ( 8kB 내부 램의 Echo. ( Echo 서버할 때 그 Echo )  )

0xE000

---

8kB Internal RAM ( 8kb 내부 램 )

0xC000

---

8kB Switchable RAM bank ( 8kb 전환 가능한 RAM 뱅크)

0xA000

---

8kB Video RAM ( 8kB 비디오 램 )

0x8000

---

16kB switchable ROM bank ( 16kb 전환 가능한 ROM 뱅크 )

0x4000 (32kB 카트리지)

---

16kB ROM bank #0 

0x0000 (32kB 카트리지)

---

## Echo of 8kB Internal RAM

주소 E000-FE00는 C000-DE00과 동일하게 내부 RAM를 미러링합니다. (즉, E000에 바이트 주소를 쓰면 C000과 E000에 써지고, C000에 바이트를 쓰면 C000과 E000에 써집니다.)

## User I/O

메모리 맵에는 전환 가능한 RAM 뱅크 영역을 제외하고 입력 포트를 구현하기위한 빈 공간이 없습니다 (RAM 뱅크가 항상 활성화되어 있기 때문에 Super Smart Card가 사용할 순  없습니다.). 출력 전용 포트는 A000-FDFF 사이에서 구현할 수 있습니다. RAM 영역에서 구현되는 경우 다른 용도로 사용되지 않게 주의하며 사용해야 합니다. (0xFE00 이상은 CPU가 이 위치에 대해 external / WR을 생성하지 않기 때문에 사용할 수 없습니다.)

MBC1, ROM 4Mibt 이하 및 RAM 8Kbyte 이하 ( 혹은 램이 없는 ) 카트가 있을 경우, MBC1의 핀 6, 7을 디지털 출력 핀으로 사용하여 원하는 목적에 따라 사용할 수 있습니다. 이 핀들을 사용하려면 먼저 0x6000에 01을 기록하여 MBC1을 4MitROM/32Kbyte RAM 모드로 설정해야 합니다. 0x4000에 기록한 최하위 비트 2개가 이 핀에 출력됩니다. 

## Reserved Memory Locations ( 예약 메모리 위치 )

0x0000

Restart $00 Address ( RST $00 calls this address ) 

0x0008

Restart $08 Address ( RST $08 calls this address ) 

0x0010

Restart $10 Address ( RST $10 calls this address ) 

0x0018

Restart $18 Address ( RST $18 calls this address ) 

0x0020

Restart $20 Address ( RST $20 calls this address ) 

0x0028

Restart $28 Address ( RST $28 calls this address ) 

0x0030

Restart $30 Address ( RST $30 calls this address ) 

0x0038

Restart $38 Address ( RST $38 calls this address ) 

0x0040

Restart $40 Address ( RST $40 calls this address ) 

0x0048

Restart $48 Address ( RST $48 calls this address ) 

0x0050

Restart $50 Address ( RST $50 calls this address ) 

0x0058

Restart $58 Address ( RST $58 calls this address ) 

0x0060

Restart $60 Address ( RST $60 calls this address ) 

내부 정보 영역은 각 카트리지의 0x0100-0x014에 있습니다. 아래는 다음 값들입니다.

0x0100-0x103

카트리지의 시작 지점입니다. 일반적으로 NOP와 JP 명령이 있지만 항상 그런 건 아닙니다.

0x0104-0x0133

```wasm
CE ED 66 66 CC 0D 00 0B 03 73 00 83 00 0C 00 0D        
00 08 11 1F 88 89 00 0E DC CC 6E E6 DD DD D9 99        
BB BB 67 63 6E 0E EC CC DD DC 99 9F BB B9 33 3E
```

움직이는 닌텐도 그래픽입니다. **변경시 프로그램이 작동하지 않습니다!** 

0x0134-0x142

대문자 아스키 코드로 된 게임의 제목입니다. 만약 16자보다 작다면, 남은 바이트는 00으로 채워집니다. 

0x0143

0 = Color GB, $00 or other = not Color GB

0x0144

아스키 HEX digit 값입니다, 높은 자리수 ( nibble, 반-바이트)  가 라이센스 코드입니다.이 두 바이트는 최신 게임(SGB 이후)에만 사용됩니다. 

0x0145

아스키 HEX digit 값입니다, 낮은 자리수 ( nibble, 반-바이트 )  가 라이센스 코드입니다. 이 두 바이트는 최신 게임(SGB 이후)에만 사용됩니다. ( 일반적으로는 $014B가 $33이 아니라면 $00이 들어있습니다. )

0x0146

GB/SGB 구분자 ( 00 = GameBoy, 03 = Super GameBoy 함수들) ( 만약 $03이 아니라면, 슈퍼 게임 보이의 함수들이 작동하지 않습니다. )

0x0147

Restart $00 Address ( RST $00 calls this address ) 

0x0148

Restart $00 Address ( RST $00 calls this address ) 

0x0149

Restart $00 Address ( RST $00 calls this address ) 

0x014A

Restart $00 Address ( RST $00 calls this address ) 

0x014B

Restart $00 Address ( RST $00 calls this address ) 

0x014C

Restart $00 Address ( RST $00 calls this address ) 

0x014D

Restart $00 Address ( RST $00 calls this address ) 

0x014E-0x014F

Restart $00 Address ( RST $00 calls this address )