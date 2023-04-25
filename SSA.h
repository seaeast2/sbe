#pragma once

namespace gclang
{
    class Inst;
    class Phi;
    class BasicBlock;
    
    //class Inst
    //{
    //public:
    //    std::vector<Inst*> Uses; // ���� 1~3��
    //    std::vector<Inst*> Defs; // ���� 1��
    //};

    // SSA �ǻ��ڵ�
    //class BasicBlock
    //{
    //public:
    //    // CFG �󿡼�
    //    std::vector<BasicBlock*> pred; // �� ���� ������ ��� ������ ����
    //    std::vector<BasicBlock*> succ; // �� ��� ������ ������ ��� ������ ����

    //    // ������ tree �� ��� ���� �ڽ����� ��ϵ� ���
    //    std::vector<BasicBlock*> children;

    //    std::vector<Inst*> Stmts; // �� BB�� ��� statment���� ����
    //    std::vector<Inst*> Aorig; // def ����
    //    std::unordered_map<Inst*, Phi*> Phis; // phi ����

    //    // vector �� index ���
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
        // <node, dfnum> : node �� �ش� dfnum ���
        std::unordered_map<BasicBlock*, int> dfnum;
        // <dfnum, node> : dfnum ���� �ش� node ���
        std::unordered_map<int, BasicBlock*> vertex;
        // <child, parent> : �ڽ� node �� �θ� node ����. �θ� �������� ����??
        std::unordered_map<BasicBlock*, BasicBlock*> parent;

        // <semidominator, node> : �������� ��. �������ڿ��� ����޴� ��� ������ ����. (����� �������ڰ� ���� ������ ����.)
        std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> bucket;
        // <node, semidominator> : Ư�� ����� �������� ���
        std::unordered_map<BasicBlock*, BasicBlock*> semi;
        // ���� ������ v ���� ��� ���.
        std::unordered_map<BasicBlock*, BasicBlock*> ancestor;

        std::unordered_map<BasicBlock*, BasicBlock*> samedom;
        // <node, semidominator> : �������ڰ� ancestor[v]���� v���� �ǳʶ� ��ο��� ���� ���� dfnum�� ���� ���(v�� ���������� ancestor[v]�� ����).
        std::unordered_map<BasicBlock*, BasicBlock*> best;

        //= Modern Compiler Implementation�� SSA �˰��� ���� =============
        // �Լ� 1���� ��� �ִ� ��� BB ���� ����
        std::vector<BasicBlock*>* BBs;
        // <node, idom> : node �� ���� idom ��带 ��ȯ. �Ʒ� children �� ������.
        std::unordered_map<BasicBlock*, BasicBlock*> idom;
        // <idom, idom's child> : ���� �����ڰ� n �� ����� ����.
        std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> children;
        // ��� n�� ���� ��� ����
        std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> DF;
        // ���� a ������ phi �Լ��� ������ �ϴ� ������ ����
        std::unordered_map<BasicBlock*, std::unordered_set<Inst*>> Aphi;
        // ���� a�� ���ǵ� BB ���� ����
        std::unordered_map<Inst*, std::vector<BasicBlock*>> defsites;
        // ���� a �� ������ Ƚ��
        std::unordered_map<Inst*, int> Count;
        // ���� a�� stack
        std::unordered_map<Inst*, std::stack<int>> Stack;


    public:
        SSA();
        ~SSA();

        void SetBBs(std::vector<BasicBlock*>* bbs);

        // ���� ��踦 ���
        void computeDF(BasicBlock* n);
        // dominator�� dominated �� �����ϴ��� ���θ� ��ȯ
        bool IsDomitanted(BasicBlock* dominator, BasicBlock* dominated);
        // Phi �Լ� ����
        void PlacePhiFunctions();
        // ���� �̸� ����
        void Initialization();

        void Rename(BasicBlock* n);

        // LENGAUER-TARJAN ALGORITHM ���� ���� Dfs �Լ�
        void Dfs(BasicBlock* p, BasicBlock* n);
        // LENGAUER-TARJAN ALGORITHM 
        void Dominators(BasicBlock* r); // r �� root basicblock

        void Link1(BasicBlock* p, BasicBlock* n);
        void Link2(BasicBlock* p, BasicBlock* n);
        // v�� ���� ���� dfnum �� ���� �������ڸ� ��ȯ (�Ϲ� ����)
        BasicBlock* AncestorWithLowestSemi1(BasicBlock* v);
        // v�� ���� ���� dfnum �� ���� �������ڸ� ��ȯ (��� ���� ����)
        BasicBlock* AncestorWithLowestSemi2(BasicBlock* v);

    };
}
