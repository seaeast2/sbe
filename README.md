# SBE (Simple Backend)

간단한 C++ 기반 컴파일러 백엔드

## 개요

SBE는 교육 목적으로 설계된 간단한 컴파일러 백엔드입니다. 중간 표현(IR)을 x86-64 어셈블리 코드로 변환하는 기능을 제공합니다.

## 주요 기능

- **IR (Intermediate Representation)**: 간단한 중간 표현 정의
  - 기본 산술 연산: ADD, SUB, MUL, DIV
  - 로드/스토어 연산
  - 함수 정의 및 반환
  
- **Code Generation**: x86-64 어셈블리 코드 생성
  - Intel 문법 지원
  - 스택 기반 레지스터 할당
  - 함수 프롤로그/에필로그 자동 생성

## 빌드 방법

### 요구 사항
- CMake 3.10 이상
- C++17 호환 컴파일러 (GCC, Clang 등)

### 빌드 단계

```bash
# 저장소 클론
git clone https://github.com/seaeast2/sbe.git
cd sbe

# 빌드 디렉토리 생성 및 빌드
mkdir build
cd build
cmake ..
make
```

## 실행

### 예제 프로그램 실행
```bash
./sbe_example
```

### 테스트 실행
```bash
./sbe_test
# 또는
make test
```

## 사용 예제

```cpp
#include "ir.h"
#include "codegen.h"

using namespace sbe;

int main() {
    // IR 모듈 생성
    IRModule module;
    IRBuilder builder(module);
    
    // 함수 정의: int calculate() { return (10 + 20) * 3; }
    builder.startFunction("calculate");
    builder.emitLoad("t1", 10);        // t1 = 10
    builder.emitLoad("t2", 20);        // t2 = 20
    builder.emitAdd("t3", "t1", "t2"); // t3 = t1 + t2
    builder.emitLoad("t4", 3);         // t4 = 3
    builder.emitMul("t5", "t3", "t4"); // t5 = t3 * t4
    builder.emitReturn("t5");          // return t5
    builder.endFunction();
    
    // 어셈블리 코드 생성
    CodeGenerator codegen;
    std::string assembly = codegen.generate(module);
    
    std::cout << assembly << std::endl;
    return 0;
}
```

## 아키텍처

### 컴포넌트 구조

```
sbe/
├── include/          # 헤더 파일
│   ├── ir.h         # IR 정의 및 빌더
│   └── codegen.h    # 코드 생성기
├── src/             # 구현 파일
│   ├── ir.cpp       # IR 구현
│   └── codegen.cpp  # 코드 생성 구현
├── examples/        # 예제 프로그램
│   └── simple_example.cpp
└── tests/           # 테스트
    └── test_main.cpp
```

### IR 명령어

- **LOAD**: 상수 값을 변수에 로드
- **STORE**: 변수 간 값 복사
- **ADD**: 덧셈 연산
- **SUB**: 뺄셈 연산
- **MUL**: 곱셈 연산
- **DIV**: 나눗셈 연산
- **RET**: 함수에서 반환

## 생성된 어셈블리 예제

입력 IR:
```
t1 = 10
t2 = 20
t3 = t1 + t2
return t3
```

출력 어셈블리:
```asm
.intel_syntax noprefix
.text

.globl calculate
calculate:
    push rbp
    mov rbp, rsp
    sub rsp, 64
    mov DWORD PTR [rbp-4], 10
    mov DWORD PTR [rbp-8], 20
    mov eax, DWORD PTR [rbp-4]
    add eax, DWORD PTR [rbp-8]
    mov DWORD PTR [rbp-12], eax
    mov eax, DWORD PTR [rbp-12]
    mov rsp, rbp
    pop rbp
    ret
```

## 향후 개선 사항

- [ ] 더 효율적인 레지스터 할당
- [ ] 최적화 패스 추가
- [ ] 제어 흐름 지원 (if, while 등)
- [ ] 다양한 데이터 타입 지원
- [ ] 더 많은 아키텍처 지원

## 라이선스

MIT License

## 기여

이슈 및 풀 리퀘스트는 언제든 환영합니다!
