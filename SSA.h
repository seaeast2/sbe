#pragma once

namespace gclang
{
    class Inst;
    class Phi;
    class BasicBlock;
    
    //class Inst
    //{
    //public:
    //    std::vector<Inst*> Uses; // 보통 1~3개
    //    std::vector<Inst*> Defs; // 보통 1개
    //};

    // SSA 의사코드
    //class BasicBlock
    //{
    //public:
    //    // CFG 상에서
    //    std::vector<BasicBlock*> pred; // 이 노드로 들어오는 모든 노드들의 집합
    //    std::vector<BasicBlock*> succ; // 이 노드 밖으로 나가는 모든 노드들의 집합

    //    // 지배자 tree 와 상관 없이 자식으로 등록된 노드
    //    std::vector<BasicBlock*> children;

    //    std::vector<Inst*> Stmts; // 현 BB의 모든 statment들의 집합
    //    std::vector<Inst*> Aorig; // def 집합
    //    std::unordered_map<Inst*, Phi*> Phis; // phi 집합

    //    // vector 의 index 계산
    //    int getIndex(vector<BasicBlock*> v, BasicBlock* K)
    //    {
    //        auto it = find(v.begin(), v.end(), K);

    //        // If element was found
    //        if (it != v.end())
    //        {

    //            // calculating the index
    //            // of K
    //            int index = it - v.begin();
    //            //cout << index << endl;
    //            return index;
    //        }
    //        else {
    //            // If the element is not
    //            // present in the vector
    //            //cout << "-1" << endl;
    //            return -1;
    //        }
    //    }
    //};


    class SSA
    {
    public:
        int N;
        // <node, dfnum> : node 로 해당 dfnum 얻기
        std::unordered_map<BasicBlock*, int> dfnum;
        // <dfnum, node> : dfnum 으로 해당 node 얻기
        std::unordered_map<int, BasicBlock*> vertex;
        // <child, parent> : 자식 node 로 부모 node 참조. 부모가 여러개일 경우는??
        std::unordered_map<BasicBlock*, BasicBlock*> parent;

        // <semidominator, node> : 준지배자 숲. 준지배자에게 지배받는 모든 노드들의 집합. (노드중 준지배자가 같은 노드들의 집합.)
        std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> bucket;
        // <node, semidominator> : 특정 노드의 준지배자 얻기
        std::unordered_map<BasicBlock*, BasicBlock*> semi;
        // 신장 숲에서 v 위의 모든 노드.
        std::unordered_map<BasicBlock*, BasicBlock*> ancestor;

        std::unordered_map<BasicBlock*, BasicBlock*> samedom;
        // <node, semidominator> : 준지배자가 ancestor[v]에서 v까지 건너뛴 경로에서 가장 낮은 dfnum을 갖는 노드(v는 포함하지만 ancestor[v]는 제외).
        std::unordered_map<BasicBlock*, BasicBlock*> best;

        //= Modern Compiler Implementation의 SSA 알고리즘 구현 =============
        // 함수 1개에 들어 있는 모든 BB 들의 집합
        std::vector<BasicBlock*>* BBs;
        // <node, idom> : node 에 대한 idom 노드를 반환. 아래 children 의 역참조.
        std::unordered_map<BasicBlock*, BasicBlock*> idom;
        // <idom, idom's child> : 직접 지배자가 n 인 노드의 집합.
        std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> children;
        // 노드 n의 지배 경계 집합
        std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> DF;
        // 변수 a 에대한 phi 함수를 가져야 하는 노드들의 집합
        std::unordered_map<BasicBlock*, std::unordered_set<Inst*>> Aphi;
        // 변수 a가 정의된 BB 들의 집합
        std::unordered_map<Inst*, std::vector<BasicBlock*>> defsites;
        // 변수 a 의 재정의 횟수
        std::unordered_map<Inst*, int> Count;
        // 변수 a의 stack
        std::unordered_map<Inst*, std::stack<int>> Stack;


    public:
        SSA();
        ~SSA();

        void SetBBs(std::vector<BasicBlock*>* bbs);

        // 지배 경계를 계산
        void computeDF(BasicBlock* n);
        // dominator가 dominated 를 지배하는지 여부를 반환
        bool IsDomitanted(BasicBlock* dominator, BasicBlock* dominated);
        // Phi 함수 삽입
        void PlacePhiFunctions();
        // 변수 이름 변경
        void Initialization();

        void Rename(BasicBlock* n);

        // LENGAUER-TARJAN ALGORITHM 위한 선행 Dfs 함수
        void Dfs(BasicBlock* p, BasicBlock* n);
        // LENGAUER-TARJAN ALGORITHM 
        void Dominators(BasicBlock* r); // r 은 root basicblock

        void Link1(BasicBlock* p, BasicBlock* n);
        void Link2(BasicBlock* p, BasicBlock* n);
        // v의 가장 작은 dfnum 을 갖는 준지배자를 반환 (일반 버전)
        BasicBlock* AncestorWithLowestSemi1(BasicBlock* v);
        // v의 가장 작은 dfnum 을 갖는 준지배자를 반환 (경로 압축 버전)
        BasicBlock* AncestorWithLowestSemi2(BasicBlock* v);

    };
}
