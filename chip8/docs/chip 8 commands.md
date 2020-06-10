# CHIP - 8 명령어 시트

코드 섹션은 임의로 분리해놓았습니다. 맞지 않거나 이상한 부분이 있으면 Issue 발행 부탁드립니다. 

# BIT

## 0x8XY1 : OR Vx, Vy

## 0x8XY2 :  AND Vx, Vy

## 0x8XY3 : XOR Vx, Vy

## 0x8XY6 : SHR Vx, Vy

## 0x8XYE : SHL Vx, Vy

# CODE

## 0x00E0: CLS

## 0x00EE: RTN

## 0x1NNN : JP NNN

## 0x2NNN : call NNN

## 0xANNN : LD I, NNN

## 0xBNNN : JP V0, NNN.

## 0xCXKK : RND Vx, kk

## 0xDXYN: Vx, Vy. niddle

## 0xFX29 : Set F, V