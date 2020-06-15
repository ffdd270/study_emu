# Gameboy Register and Flags

# 레지스터

```wasm
16bit   Hi  Lo   Name/Function
AF    | A | - | Accumulator & Flags ( 계산 & Flag )
BC    | B | C | BC ( 비트가 B, C이지 BC에 별 다른 의미는 없습니다. 아래도 동일.  )
DE    | D | E | DE
HL    | H | L | HL
SP    | - | - | Stack Pointer
PC    | - | - | Program Counter/Pointer
```

보시다 싶이, 대부분 레지스터들은 16비트나 두 개의 별도 8비트 레지스터로 접근할 수 있습니다. 

# Flag 레지스터 ( AF 레지스터의 하위 8비트 )

```wasm
Bit | Name | Set | Clr | Exp.
7   |  zf  |  Z  | NZ  | Zero Flag 
6   |   n  |  -  |  -  | Add/Sub-Flag(BCD)
5   |   h  |  -  |  -  | Half Carry Flag(BCD ) 
4   |  cy  |  C  | NC  | Carry Flag 
3-0 |   -  |  -  |  -  | Not Used( 항상 0 )
```

이 플래그들은 가장 최근 연산에 영향을 받은 값들을 가지고 있습니다. 

## Zero Flag ( Z )

이 비트는 만약 연산 결과가 0이면 1로 지정됩니다. 조건 Jump들에 사용됩니다. ( if 0? jmp 3100 )

## Carry Flag ( C, Cy )

덧셈의 결과가 0xFF나 0xFFFF보다 크거나, 뺄샘의 결과나 비교의 결과가 0보다 작아졌을 때 설정되는 비트입니다. ( Z80이나 80x86 CPU들과 비슷하지만, 65XX나 ARM CPU들과는 다릅니다. ) 그리고 rotate/shfit 연산이 1비트 넘어가게 되어도 설정됩니다. ( 8비트 연산인데 연산결과가 9비트라던가. ) 조건 Jump, 그리고 ADC, SBC, RL, RLA, 기타 등등 명령어를 위해 사용됩니다. 

## BCD Flags ( N, H )

이 플래그는 (드물게) DAA 명령에만 사용됩니다. N 이전 명령이 덧셈인지 뺄셈인지를 나타내고 H는 결과의 하위 4 비트에 대한 캐리를 나타내고 DAA에 대해서도 C 플래그는 상위 8 비트에 대한 캐리를 나타내야합니다 . 두 개의 BCD 번호를 더하거나 빼면 DAA는 결과를 BCD 형식으로 변환합니다. BCD 숫자의 범위는 00h에서 FFh가 아닌 00h에서 99h입니다. C 및 H 플래그에는 각 숫자에 대한 캐리 아웃이 포함되어야하므로 DAA는 16 비트 연산 (4 자리) 또는 INC / DEC 연산 (C 플래그에 영향을 미치지 않음)에는 사용할 수 없습니다. ( 구글 번역 )