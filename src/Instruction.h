#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

// IR 상 변수를 나타내는 추상 클래스
class Value
{
public:
    Value() {}
    virtual ~Value() {}
};

/*
 IR 명령어
 IR 명령어는 3 종류로 구분된다.
    1. 연산 명령어 : 연산 명령어
        - ex) add, sub, mul
    2. 메모리 명령어 : 메모리 입출력 명령어
        - ex) load, store
    3. 분기 명령어 : jmp, branch 등의 제어 흐름 명령어
        - ex) br, jmp
*/
class IRInst : public Value
{
public:
    enum class eIRInstKind
    {
        OpAdd,
        OpSub,
        OpMul,
        OpDiv,
    };

public:
    IRInst();
    ~IRInst();
};

#endif