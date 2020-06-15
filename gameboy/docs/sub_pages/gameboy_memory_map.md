# Gameboy Memory Map

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

아스키 HEX digit 값입니다, 낮은 자리수 ( nibble, 반-바이트 )  가 라이센스 코드입니다. 이 두 바이트는 최신 게임(SGB 이후)에만 사용됩니다. ( 일반적으로는 0x014B가 0x33이 아니라면 0x00이 들어있습니다. )

0x0146

GB/SGB 구분자 ( 00 = GameBoy, 03 = Super GameBoy 함수들) ( 만약 0x03이 아니라면, 슈퍼 게임 보이의 함수들이 작동하지 않습니다. )

0x0147 

카트리지 타입 

- 0 : ROM ONLY
- 1 : ROM+MBC1
- 2 : ROM+MBC1+RAM
- 3 : ROM+MBC1+RAM+BATT
- 5 : ROM+MBC2
- 6 : ROM+MBC2+BATTERY
- 8 : ROM+RAM
- 9 : ROM+RAM+BATTERY
- B : ROM+MMMD1
- C : ROM+MMMD1+SRAM
- D : ROM+MMMD1+SRAM+BATT
- F : ROM+MBC3+TIMER+BATT
- 10 : ROM+MBC3+TIMER+RAM+BATT
- 11 : ROM+MBC3
- 12 : ROM+MBC3+RAM
- 13 : ROM+MBC3+RAM+BATT
- 19 : ROM+MBC5
- 1A : ROM+MBC5+RAM
- 1B : ROM+BMC+RAM+BATT
- 1C : ROM+MBC5+RUMBLE
- 1D : ROM+MBC5+RUMBLE+SRAM
- 1D : ROM+MBC5+RUMBLE+SRAM + BATT
- 1F : Pocket Camera
- FD : Bandai TAMA5
- FE : HUDSON HuC - 3
- FF : HUDSON HuC - 1

0x0148 

롬 크기 

- 0 - 256Kbit = 32Kbyte = 2banks;
- 1 - 512Kbit = 64Kbyte = 4banks;
- 2 - 1Mbit = 128Kbyte = 8banks;
- 3 - 2Mbit = 256Kbyte = 16banks;
- 4 - 4Mbit = 512Kbyte = 32banks;
- 5 - 8Mbit = 1Mbyte = 64banks;
- 6 - 16Mbit = 2Mbyte = 128banks;
- 0x52 - 9Mbit = 1.1Kbyte = 72banks;
- 0x53 - 10Mbit = 1.2Kbyte = 80banks;
- 0x54 - 12Mbit = 1.5Kbyte = 96banks;

0x0149 ( RAM SIZE )

렘 크기

- 0 - 없음
- 1 - 16kBit   = 2kB     = 1 bank
- 2 - 64kBit   = 8kB     = 1 bank
- 3 - 256kBit = 32kB   = 4 bank
- 4 - 1MBit   = 128kB  =16 bank

0x014A

- 0 : 일본
- 1 : 일본 아님

0x014B

라이센스 코드 ( 구 버전 , SGB 이전. ) 

- 33 - 0x0144/0145 라이센스 코드 확인.
- 79 - Accolade
- A4 - Konami

( 0x33이 아니라면 슈퍼 게임보이의 기능이 작동하지 않습니다. ) 

0x014C

ROM 버전 마스크 ( 주로 0x00 )

0x014D

보완 검사용 체크섬. 0x0134~0x014C 범위의 확인 값입니다. [https://gbdev.gg8.se/wiki/articles/The_Cartridge_Header#0148_-_ROM_Size](https://gbdev.gg8.se/wiki/articles/The_Cartridge_Header#0148_-_ROM_Size) 계산 방법은 해당 사이트를 참고해주세요. ( 만약 이 값이 맞지 않는다면 **게임보이에서 구동 실패합니다**.  )

0x014E-0x014F

ROM 전체에 대한 16비트 체크섬 값입니다. ( 상위 비트 우선 ) 카트리지의 모든 바이트를 더해 생성됩니다. (이 두 체크섬 바이트를 제외하고.) 게임 보이는 이 체크섬을 확인하지 않습니다. 

# 카트리지 타입

해당 정의는 카트리지의 0x147 주소에 존재합니다.

- ROM ONLY ( 롬만)

    32kB ( 256kb ) ROM이며, 0000~7FFF 에 할당되어 있습니다.  

- MBC1( Memory Bank Controller 1 )

    MBC1는 두개의 다른 메모리 모드를 가지고 있습니다 : 16Mbit ROM/ 8Kbyte RAM이나 4Mibt ROM/32Kbyte RAM. 

    MBC1은 기본적으로 16Mbit ROM / 8KByte RAM 모드로 실행됩니다. 6000~7FFF 영역에 값을 쓸 때 ( XXXXXXXS - X는 신경쓰지 않아도 되는 값, S는 메모리 모델 선택 ) 사용할 메모리 모델을 선택하게 될 것입니다. 4 / 32 모드에서는 S는 1로 설정합니다.

    0x2000-0x3FFF 영역에 값을 쓰면 ( XXXBBBBB - X = 신경쓰지 않아도 되는 값, B = 뱅크 선택 비트 ) 0x4000~0x7FFF에 적절한 롬 뱅크를 선택됩니다.  값이 0 이나 1일 경우 같은 일을 하고, ROM bank 1을 가리키게 됩니다.

    ROM bank 0은 0x4000-0x7FFF에 접근 가능하지 않고, 0x0000~0x3FFF에서만 읽을 수만 있습니.다. 

    만약 메모리 모델이 4/32일 경우.

    0x4000~0x5FFF 영역에 값을 쓰면 ( XXXXXXBB - X = 신경 안 써도 됨. B = bank select bit. ) 0xA000~0xC000에서 적절한 RAM 뱅크가 선택됩니다.  RAM 뱅크를 읽거나 쓰기 전 0000~1FFF  영역에 XXXX1010 값을 써서 ( wrtiing ) RAM 뱅크를 활성화 해야 합니다. 램 뱅크를 비활성화 시키려면 XXXX1010값을 제외한 아무값이나 0x0000~0x1FFF 영역에 쓰면 됩니다.  RAM 뱅크를 비활성화시키는 것은 게임보이가 꺼지는 동안 RAM 뱅크를 잘못된 값을 쓰는 것을 보호하기 위함일 수 있습니다. ( 참고: 닌텐도는 램 뱅크를 활성화 값을 0x0A로, 비활성 값으로는 0x00을 제안했습니다. ) 

    만약 메모리 모델이 16/8 일 경우엔 : 

    0x4000~0x5FFF 영역에 값을 쓰면 ( XXXXXXBB - X = 신경 안 써도 됨. B = Bank Select Bits) 두개의 가장 앞에 있는 ROM 주소 라인이 설정된다. 

    참고 : Super Smart Card는 항상 RAM 뱅크가 활성화 되어 있기 때문에 이 연산이 필요하지 않습니다. Super Smart Card와 게임보이 양쪽에서 잘 동작해야 할 때 이 연산을 넣으시면 됩니다.

- MBC2( Memory Bank Controller 2 ):

    이 메모리 컨트롤러는 MBC1과 아래 예외를 제외하고는 MBC1과 유사합니다.

    MBC2는 ROM 사이즈가 최대 2Mbit입니다. 

    0x2000~0x3FFF 영역에 값을 쓰는 건 ( XXXXBBBB - X는 상관 없는 값, B는 Bank Select Bits.) 0x4000~0x7FFF에 있는 적합한 ROM Bank를 선택할 것입니다. 

    RAM 스위칭은 제공되지 않습니다. MBC1과 다르게 추가 램은 MBC2 자체가 가지고 있는 512 x 4 bits 램을 사용할 것이며. 전원이 꺼져 있는데에도 세이브데이터를 유지하기 위해 추가 배터리가 필요할 것입니다.

    상위 주소 바이트의 최하위 비트는 카트리지에 있는 램을 활성화 / 비활성화 하기 위하여 0이어야 합니다. 예를 들어 카트리지의 RAM을 활성화하기 위해 다음 주소를 사용할 수 있습니다. 

    0000-00FF, 0200-02FF, 0400-04FF, ... 1E00- 1EFF. 

    MBC2의 램 활성화 / 비활성화에 권장되는 주소 범위는 0000-00FF가 권장됩니다. 

    상위 주소의 최하위 비트는 ROM bank를 선택하기 위해선 1이어야 합니다. 예를 들어, ROM Bank를 선택하기 위해서는 다음과 같은 주소들을 사용할 수 있습니다. 

    2100-21FF, 2300-23FF, 2500-25FF .. 3F00-3FFF.

    MBC2 롬 뱅크를 선택하기 위해 사용되는 주소는  2100-21FF가 권장됩니다.  

- MBC3(Memory Bank Controller 3):

    이 컨트롤러는 MBC1과 비슷하며, 16mbits의 롬을 4000-5FFF 영역에 값을 쓰지 않고 접근할 수 있습니다. ( expect it. )

    2000-3FFF 영역에 값을 씀( XBBBBBBB  -  X - 아무 값. B = Bank Select Bit.) 으로써 ROM 뱅크 4000-7FFF 영역에 접근을 제어할 수 있습니다.

    그리고, 이 MBC는 다른 MBC에서는 볼 수 없는, Real Time Clock을 위해 ( RTC ) 내장되어 있는 배터리를 가지고 있습니다. 일부 MBC3 카트리지는 이걸 지원하지 않지만 ( 와리오 랜드 2 비-컬러 버전 ) , 일부는 지원합니다. ( 하버스트 문 / 일어 버전 )\

- MBC5( Memory Bank Controller 5):

    이 컨트롤러 게임보이 컬러에서 두배속으로 돌아가도록 보장된 첫 MBC이지만, 다른 MBC들도 GBC 두배속 모드에서 잘 돌아갑니다. 

    MBC5는 MBC3와 비슷하지만 ( RTC는 없음 ) ROM 64mibts과 1mbit RAM만큼 접근할 수 있습니다. 9bit의 롬 선택 비트중 상위 8비트는 2000-2FFF 영역에 기록하고,  푀하위 비트는 3000-3FFF 영역이 기록됩니다.

    만약 카트리지에 RAM이 존재한다면, 4000-5FFF 영역에 값을 쓰는 것은 ( XXXXBBBB - X는 상관 없음, B =  뱅크 선택 비트 ) A000-BEFFF에 있는 렘 Bank를 접근하는 걸 선택하게 됩니다. 램 사이즈는 64kbit, 256kbit와 1mbit가 있습니다.

    그리고, MBC5는 ROM 뱅크 선택 비트에 0x000을 기록하므로써 ROM Bank 0 가 4000-7FFF 범위에 나타나게 할 수 있는 첫 MBC입니다. 

- Rumble Carts ( 럼블(진동) 카트리지  ) :

    진동 카트리지는 MBC5 Memory Bank Controller를 사용합니다. 럼블 카트리지는 오직 256kibt RAM까지만 가질 수 있으며. 비 - 럼블  카트리지에서 사용하는 1mbit 램에 가장 높은 RAM 주소 라인은 럼블 카트리지가 모터를 끄고 킬 때 사용합니다.

     만약 카트리지가 램을 가지고 있다면, 4000-5FFF 영역에 값을 쓰는 것은 ( XXXXMBBB = X = 신경 안 써도 됨. M = 모터, B = Bank Select Bit ) A000-BFFF 영역에 있는 RAM 뱅크의 접근에 대해 선택할 수 있습니다.

    램 사이즈는 64kbit거나 256kbit입니다. 진동 모터를 켜기 위해서는 M =1,  끄기 위해서는 M = 0을 설정하면 됩니다.

- HuC1( 메모리 뱅크 / 적외선 컨트롤러 ) :

    Hudson Soft가 만든 MBC1과 매우 비슷한 컨트롤러입니다. 주된 차이점은 적외선 LED 입출력을 지원하는 것입니다. 

    일본에서 만든 카트리지인 "Fighting Phoenix"가 이 칩을 탑재했다고 알려져있습니다.