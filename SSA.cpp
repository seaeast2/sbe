#include "stdafx.h"

#include "IRType.h"
#include "IR.h"
#include "BasicBlock.h"
#include "SSA.h"


using namespace gclang;

gclang::SSA::SSA()
{

}

gclang::SSA::~SSA()
{
}

void gclang::SSA::SetBBs(std::vector<BasicBlock*>* bbs)
{
    BBs = bbs;
}

// ���� ��踦 ���
void SSA::computeDF(BasicBlock* n)
{
    std::vector<BasicBlock*> S;

    // �̰����� DFlocal[n]�� ����Ѵ�.
    for (auto y : n->GetSuccs())
    {
        if (idom[y] != n)
            S.push_back(y); // n�� �ļ� ������ S �� ����.
    }

    // idom �� n �� ������ ���� ��ȸ (= ������ Ʈ������ �ڽ� ��ȸ)
    for (auto c : children[n])
    {
        computeDF(c);
        // �̰����� DFup[c] �� ����Ѵ�.
        for (auto w : DF[n])
        {
            if (IsDomitanted(n, w) == false || n == w)
                S.push_back(w);
        }
    }

    DF[n] = S;
}

bool SSA::IsDomitanted(BasicBlock* dominator, BasicBlock* dominated)
{
    if (idom[dominated] == nullptr)
        return false;

    if (idom[dominated] != dominator)
        return IsDomitanted(dominator, idom[dominated]);

    return true;
}

/*
void SSA::PlacePhiFunctions()
{
    // ��� ������ ��ȸ�ϸ鼭 defsites ����
    for (auto n : *BBs)
    {// ��� BB ��ȸ
        for (auto a : n->Aorig) // n���� ���ǵ� ��� ���� ��ȸ
        {
            defsites[a].push_back(n);
        }
    }

    // ��� ������ ��ȸ
    for (auto n : *BBs)
    {
        for (auto a : n->Aorig)
        {
            auto W = defsites[a];
            while (!W.empty())
            {// ���� a�� ���ǵ� ������ ��ȸ
                auto nn = W.back();
                W.pop_back();
                for (auto y : DF[nn])
                { // ���� a�� ���ǵ� ������ ���� ��踦 ��ȸ�Ѵ�.

                    if (Aphi[y].find(a) == Aphi[y].end())
                    { // ������ ��� y �� a �� phi �Լ��� ��ϵǾ� ���� ���� ���

                        // Phis �� second �� PhiInst �̰�, AddPhiArg() �� ��� �Լ��� ������ �ִٰ� �����Ѵ�.
                        y->Phis[a]->AddPhiArg(a);

                        // ������ ��� y �� phi �Լ� �߰� ���� ��Ͽ� a�� ����Ѵ�.
                        Aphi[y].insert(a);

                        if (std::find(y->Aorig.begin(), y->Aorig.end(), a) == y->Aorig.end())
                        {
                            // phi �Լ��� ���� ���ǿ� ������ ȿ���� ���Ƿ�
                            // ���� a�� ��� y ���� ���ǵ��� �ʾ�����, �߰��Ѵ�.
                            W.push_back(y);
                            y->Aorig.push_back(a);
                        }
                    }
                }
            }
        }
    }
}

void SSA::Initialization()
{
    // Rename �� ���� �ʱ�ȭ
    // ��� ������ ��ȸ
    for (auto n : BBs)
    {
        for (auto a : n->Aorig)
        {
            Count[a] = 0;
            while (!Stack[a].empty())
                Stack[a].pop(); // std::stack<> �� clear �Լ��� �������� ����.
            Stack[a].push(0);
        }
    }
}

void SSA::Rename(BasicBlock* n)
{
    for (auto S : n->Stmts)
    {
        // Use �κ� �̸� ����
        if (S->IsPhiFunc() == false)
        { // S�� phi �Լ��� �ƴϸ�
            for (auto x : S->Uses)
            { // S �ȿ��� ��� ��� x ��ȸ
                int i = Stack[x].top();
                Inst* xi = new Inst(x, i); // i �� phi ������ ���� ���� ���� ����
                x->replace(xi); // x �� xi �� ��ü
            }
        }
        // Def �κ� �̸� ����
        for (auto a : S->Defs)
        {
            Count[a] += 1;
            int i = Count[a];
            Stack[a].push(i);

            Inst* ai = new Inst(a, i); // i �� phi ������ ���� ���� ���� ����
            a->replace(ai); // x �� xi �� ��ü
        }
    }

    // ����� phi �Լ����� ��ȸ�ϸ鼭 phi �Լ��� arg �� �̸� ����
    for (auto Y : n->GetSuccs())
    {
        // n �� Y �ȿ����� predecessor index �� ���Ѵ�.
        int j = Y->getIndex(Y->pred, n);

        for (auto phi : Y->Phis)
        {
            auto a = phi.second->GetPhiArg(j); // PhiInst::GetPhiArg(int j) �� j ��° phi arg �� ��ȯ.
            int i = Stack[a].top();
            phi.second->ReplacePhiArg(j, new Inst(a, i)); // PhiInst::ReplacePhiArg(int j, Inst* inst) �� j ���� phi arg �� inst �� ġȯ
        }
    }

    for (auto X : n->children)
    {
        Rename(X);
    }

    for (auto S : n->Stmts)
    {
        for (auto a : S->Defs)
        {
            Stack[a].pop();
        }
    }
}

void SSA::Dfs(BasicBlock* p, BasicBlock* n)
{
    if (dfnum[n] == 0)
    {
        dfnum[n] = N; // defnum ���� ����
        vertex[N] = n; // defnum �� ������ ����
        parent[n] = p;

        N = N + 1;

        for (auto w : n->GetSuccs())
        {
            Dfs(n, w);
        }
    }
}

void SSA::Dominators(BasicBlock* r)
{
    N = 0;
    // �ʱ�ȭ
    // bucket, dfnum, semi, ancestor, idom, samedom �ʱ�ȭ
    Dfs(nullptr, r);

    for (int i = N - 1; i; i--) // Skip over node 0, the root node.
    {
        BasicBlock* n, // ���� ���
            * p, // �θ� ���
            * s; // ��������

        n = vertex[i]; // dfnum �� �ش��ϴ� node ���
        p = parent[n]; // �θ� ��� ���
        s = p; // ��������

        // �������� ������ ���� n�� �������ڸ� ����Ѵ�.
        for (auto v : n->pred)
        {
            BasicBlock* ss; // �������� �ĺ�

            if (dfnum[v] <= dfnum[n])
            { // v �� n �� ������ ������ ��
                ss = v;
            }
            else
            {
                ss = semi[AncestorWithLowestSemi1(v)];
            }

            if (dfnum[ss] < dfnum[s])
                s = ss;
        }

        semi[n] = s; // ���� ����� �������ڸ� ����
        bucket[s].push_back(n); // �������� s�� ���� ����Ǵ� ��� ��� n�� �߰�

        // n�� ������ ����� s -> n �� ������ ����� ������ �̷�����.
        Link1(p, n);

        // ���� p���� v������ ��ΰ� ���� ���� ����Ǿ����Ƿ� 
        // �̷��� ���� ������ ������ ù ��° ���� ������� v�� 
        // �����ڸ� ����ϰų� y�� �����ڰ� �˷��� ������ ����� �����մϴ�.
        for (auto v : bucket[p])
        {// n�� �θ� p�� �������ڷ� ���� ��� ��� v
            BasicBlock* y = AncestorWithLowestSemi1(v);
            if (semi[y] == semi[v])
            { // y �� v�� �������ڰ� ������
                idom[v] = p; // v�� ���� �����ڸ� p�� ����
            }
            else
            {
                samedom[v] = y;
            }
        }

        bucket[p].clear();
    }

    for (int i = 1; i < N - 1; i++)
    {
        BasicBlock* n = vertex[i];
        if (samedom[n] != nullptr)
        {
            idom[n] = idom[samedom[n]];
        }
    }
}

BasicBlock* SSA::AncestorWithLowestSemi1(BasicBlock* v)
{ // ���� ���� O(n)
    BasicBlock* u = v;

    // 
    while (ancestor[v] != nullptr)
    {
        if (dfnum[semi[v]] < dfnum[semi[u]])
        {
            u = v;
        }

        v = ancestor[v];
    }

    return u;
}

void SSA::Link1(BasicBlock* p, BasicBlock* n)
{
    ancestor[n] = p;
}


BasicBlock* SSA::AncestorWithLowestSemi2(BasicBlock* v)
{ // �������� O(log n)
    BasicBlock* a = ancestor[v];
    if (ancestor[a] != nullptr)
    {
        BasicBlock* b = AncestorWithLowestSemi2(a);
        ancestor[v] = ancestor[a];
        if (dfnum[semi[b]] < dfnum[semi[best[v]]])
        {
            best[v] = b;
        }
    }

    return best[v];
}

void SSA::Link2(BasicBlock* p, BasicBlock* n)
{
    ancestor[n] = p;
    best[n] = n;
}
*/
