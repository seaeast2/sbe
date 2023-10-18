#ifndef _LIVENESS_H_
#define _LIVENESS_H_

// liveness 분석
class Liveness
{
    // in[n] : 노드 n 으로 들어오는 변수 집합
    // out[n] : 노드 n 에서 나가는 변수 집합
    // use[n] : 노드 n 에서 사용되는 변수 집합
    // def[n] : 노드 n 에서 정의되는 변수 집합
public:
    Liveness();
    ~Liveness();

    void Run();
};
#endif