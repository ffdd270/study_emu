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

나머지 세부사항은 상세 페이지를 참고.

[Gameboy Special Modes](Gameboy%20CPU%20Manual%20d6b9869341884d8e9ae8c27139e987fb/Gameboy%20Special%20Modes%20636a42864c464c0c83c5cae07b52b43c.md)

[Gameboy Memory Map](Gameboy%20CPU%20Manual%20d6b9869341884d8e9ae8c27139e987fb/Gameboy%20Memory%20Map%20ff25f58287f7424d920863517056e8c8.md)

[Gameboy Register and Flags ](Gameboy%20CPU%20Manual%20d6b9869341884d8e9ae8c27139e987fb/Gameboy%20Register%20and%20Flags%20d3a8a46663a34d4ba3012dde889c2eaf.md)

[Gameboy CPU Commands](Gameboy%20CPU%20Manual%20d6b9869341884d8e9ae8c27139e987fb/Gameboy%20CPU%20Commands%20fe6b698cd41c4f6992f1a78a76044668.md)