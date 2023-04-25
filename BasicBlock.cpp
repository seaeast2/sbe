#include "stdafx.h"
#include <unordered_set>

#include "IR.h"
#include "BasicBlock.h"


using namespace gclang;

static int BASIC_BLOCK_COUNT = 0;

gclang::BasicBlock::BasicBlock()
{
	name_ = "auto_gen_";
	block_index_ = BASIC_BLOCK_COUNT++;
}

gclang::BasicBlock::BasicBlock(const std::string& name)
{
	if (name.empty())
		name_ = "auto_gen_";
	else
		name_ = name;
	block_index_ = BASIC_BLOCK_COUNT++;
}

gclang::BasicBlock::~BasicBlock()
{
}

void gclang::BasicBlock::SetName(std::string& name)
{
	name_ = name;
}

std::string& gclang::BasicBlock::GetName()
{
	return name_;
}

void gclang::BasicBlock::SetBlockIndex(int block_index)
{
	block_index_ = block_index;
}

int gclang::BasicBlock::GetBlockIndex()
{
	return block_index_;
}

void gclang::BasicBlock::AddInst(Inst* inst)
{
	inst->SetBB(this);

	if (inst->GetKind() == EIRKind::eAlloca)
		AddAlloca(inst);
	else
		insts_.push_back(inst);
}

std::list<Inst*>& gclang::BasicBlock::GetInsts()
{
	return insts_;
}

void gclang::BasicBlock::AddAlloca(Inst* inst)
{
	for (auto it = insts_.begin(); it != insts_.end(); ++it)
	{
		if ((*it)->GetKind() != EIRKind::eAlloca)
			insts_.insert(it, inst);
	}
}

void gclang::BasicBlock::AddPhi(Phi* phi)
{
	phis_.push_back(phi);
}

std::list<Phi*>& gclang::BasicBlock::GetPhis()
{
	return phis_;
}

void gclang::BasicBlock::AddPred(BasicBlock* pred)
{
	preds_.push_back(pred);
}

std::vector<BasicBlock*>& gclang::BasicBlock::GetPreds()
{
	return preds_;
}

void gclang::BasicBlock::AddSucc(BasicBlock* succ)
{
	succs_.push_back(succ);
}

std::vector<BasicBlock*>& gclang::BasicBlock::GetSuccs()
{
	return succs_;
}

std::string gclang::BasicBlock::PrintLabel()
{
	return name_;
}

std::string gclang::BasicBlock::toString()
{
	std::ostringstream o;
	/*INDENT_PRINT();
	INDENT_PUSH();
	INDENT_POP();*/

	o << name_ << ":" << endl;

	/*for (auto alloca : allocas_)
		o << INDENT_PRINT() << alloca->toString() << endl;*/

	for (auto inst : insts_)
		o << INDENT_PRINT() << inst->toString() << endl;

	return o.str();
}

