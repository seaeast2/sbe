#pragma once

namespace gclang
{
	class Inst;
	class Phi;

	class BasicBlock
	{
	private:
		std::string name_;
		int block_index_;

		//std::vector<Inst*> allocas_;
		std::list<Inst*> insts_;
		std::list<Phi*> phis_;
		std::vector<BasicBlock*> preds_; // ������. �� BB�� ������ ��� ����
		std::vector<BasicBlock*> succs_; // ������. �� BB���� ������ ��� ����
									
		// for liveness analysis
		/*std::unordered_set<Inst*> ins_;
		std::unordered_set<Inst*> outs_;
		std::unordered_set<Inst*> defs_;
		std::unordered_set<Inst*> uses_;*/

		// loop related
		BasicBlock* loop_start_{ nullptr };
		BasicBlock* loop_end_{ nullptr };

	public:
		BasicBlock(); // ���� �� �Ͻ��� ���� ����
		BasicBlock(const std::string& name);
		~BasicBlock();

		void SetName(std::string& name);
		std::string& GetName();

		void SetBlockIndex(int block_index);
		int GetBlockIndex();

		void AddInst(Inst* inst);
		std::list<Inst*>& GetInsts();
		void AddAlloca(Inst* inst);
		
		void AddPhi(Phi* phi);
		std::list<Phi*>& GetPhis();
		
		void AddPred(BasicBlock* pred);
		std::vector<BasicBlock*>& GetPreds();

		void AddSucc(BasicBlock* succ);
		std::vector<BasicBlock*>& GetSuccs();

		
		std::string PrintLabel();

		virtual std::string toString();
	};
}