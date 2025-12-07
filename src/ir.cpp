#include "ir.h"

namespace sbe {

// IRModule implementation
void IRModule::addFunction(const IRFunction& func) {
    functions.push_back(func);
}

IRInstruction IRModule::createLoad(const std::string& dest, int value) {
    return IRInstruction(IROpcode::LOAD, dest, "", "", value);
}

IRInstruction IRModule::createStore(const std::string& dest, const std::string& src) {
    return IRInstruction(IROpcode::STORE, dest, src);
}

IRInstruction IRModule::createBinaryOp(IROpcode op, const std::string& dest,
                                       const std::string& op1, const std::string& op2) {
    return IRInstruction(op, dest, op1, op2);
}

IRInstruction IRModule::createReturn(const std::string& value) {
    return IRInstruction(IROpcode::RET, "", value);
}

// IRBuilder implementation
void IRBuilder::startFunction(const std::string& name) {
    currentFunction = std::make_unique<IRFunction>(name);
}

void IRBuilder::endFunction() {
    if (currentFunction) {
        module.addFunction(*currentFunction);
        currentFunction.reset();
    }
}

void IRBuilder::emitLoad(const std::string& dest, int value) {
    if (currentFunction) {
        currentFunction->addInstruction(IRModule::createLoad(dest, value));
    }
}

void IRBuilder::emitAdd(const std::string& dest, const std::string& op1, const std::string& op2) {
    if (currentFunction) {
        currentFunction->addInstruction(IRModule::createBinaryOp(IROpcode::ADD, dest, op1, op2));
    }
}

void IRBuilder::emitSub(const std::string& dest, const std::string& op1, const std::string& op2) {
    if (currentFunction) {
        currentFunction->addInstruction(IRModule::createBinaryOp(IROpcode::SUB, dest, op1, op2));
    }
}

void IRBuilder::emitMul(const std::string& dest, const std::string& op1, const std::string& op2) {
    if (currentFunction) {
        currentFunction->addInstruction(IRModule::createBinaryOp(IROpcode::MUL, dest, op1, op2));
    }
}

void IRBuilder::emitDiv(const std::string& dest, const std::string& op1, const std::string& op2) {
    if (currentFunction) {
        currentFunction->addInstruction(IRModule::createBinaryOp(IROpcode::DIV, dest, op1, op2));
    }
}

void IRBuilder::emitReturn(const std::string& value) {
    if (currentFunction) {
        currentFunction->addInstruction(IRModule::createReturn(value));
    }
}

} // namespace sbe
