#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include "Function.h"
#include "Type.h"
extern FILE* yyout;
#include "MachineCode.h"
Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}



CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb): Instruction(CMP, insert_bb){
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}


UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    branch = to;
}



void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock *true_branch, BasicBlock *false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb){
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}


void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if(src != nullptr) {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if(!operands.empty())
        operands[0]->removeUse(this);
}


AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
}

AllocaGlobalInstruction::AllocaGlobalInstruction(Operand *dst, Operand *num, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
    this->num = num;
}

AllocaGlobalInstruction::~AllocaGlobalInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

CallInstruction::CallInstruction(Operand *dst, SymbolEntry *se, std::vector<Operand*> params, BasicBlock* insert_bb) : Instruction(CALL, insert_bb), dst(dst) {
    this->func = se;
    operands.push_back(dst); // 放函数返回值的操作数
    if (dst != nullptr) {
        dst->setDef(this);
    }
    for (auto param : params) {
        operands.push_back(param); // 放实参
        param->addUse(this);
    }
}

// CallInstruction::~CallInstruction() {

// }


ZextInstruction::ZextInstruction(Operand* dst, Operand* src, BasicBlock* insert_bb) : Instruction(ZEXT, insert_bb) 
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}


ZextInstruction::~ZextInstruction() {}

XorInstruction::XorInstruction(Operand* dst, Operand* src, BasicBlock* insert_bb) : Instruction(XOR, insert_bb) 
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}


XorInstruction::~XorInstruction() {}

MachineOperand* Instruction::genMachineOperand(Operand* ope)
{
    SymbolEntry* se = ope->getEntry();
    MachineOperand* mope = nullptr;
    if(se->isConstant())
        mope = new MachineOperand(MachineOperand::IMM, dynamic_cast<ConstantSymbolEntry*>(se)->getValue());
    else if(se->isTemporary())
        mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry*>(se)->getLabel());
    else if(se->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);
        if(id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().c_str());
        else if(id_se->isParam()){
            if (id_se->getParamNo() < 4) {
                std::cout << "get here\n\n\n\n\n\n"; // 问题就在这儿，为什么不是一个参数呢
                mope = new MachineOperand(MachineOperand::REG, id_se -> getParamNo());
            }
            else {
                mope = new MachineOperand(MachineOperand::REG, 3);
            }
        }
        else
            exit(0);
    }
    return mope;
}

MachineOperand* Instruction::genMachineReg(int reg) 
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand* Instruction::genMachineVReg() 
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
}

MachineOperand* Instruction::genMachineImm(int val) 
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand* Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder* builder)
{
    std::cout << "in AllocaInstruction::genMachineCode\n";
    /* HINT:
    * Allocate stack space for local variabel
    * Store frame offset in symbol entry */
    auto cur_func = builder->getFunction();
    int size = se->getType()->getSize() / 8; // bits to bytes
    if (size < 0) {
        size = 4;
    }
    int offset = cur_func->AllocSpace(size);
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(-offset);
    std::cout << "out AllocaInstruction::genMachineCode\n";
}
//增 复制的上面这个
void AllocaGlobalInstruction::genMachineCode(AsmBuilder* builder)
{
    std::cout << "in AllocaGlobalInstruction::genMachineCode\n";

    /* HINT:
    * Allocate stack space for local variabel
    * Store frame offset in symbol entry */
   /*
    auto cur_func = builder->getFunction();
    int offset = cur_func->AllocSpace(4);
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(-offset);
    */
   //copy
   auto cur_func = builder->getFunction();
    int size = se->getType()->getSize() / 8;
    if (size < 0) {
        size = 4;
    }
    int offset = cur_func->AllocSpace(size);
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(-offset);
    std::cout << "out AllocaGlobalInstruction::genMachineCode\n";
}
void LoadInstruction::genMachineCode(AsmBuilder* builder)
{
    std::cout << "in LoadInstruction::genMachineCode\n";
    auto cur_block = builder->getBlock();//获取当前block
    MachineInstruction* cur_inst = nullptr;
    // Load global operand
    if(operands[1]->getEntry()->isVariable()
    && dynamic_cast<IdentifierSymbolEntry*>(operands[1]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();//MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
        auto internal_reg2 = new MachineOperand(*internal_reg1);//internal_reg1对应的MachineOperand
        auto src = genMachineOperand(operands[1]);

        // 全局变量加载到寄存器需要两步
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);

        cur_block->InsertInst(cur_inst);

        // example: load r1, [r0]
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);

        cur_block->InsertInst(cur_inst);
    }
    // Load local operand
    else if(operands[1]->getEntry()->isTemporary()
    && operands[1]->getDef()
    && operands[1]->getDef()->isAlloc())//指令类型是ALLOCA
    {
        // example: load r1, [r0, #4]
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineReg(11);// fp register
        auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())->getOffset());//偏移
        cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    // Load operand from temporary variable
    else
    {
        // example: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }

    std::cout << "out LoadInstruction::genMachineCode\n";
}

void StoreInstruction::genMachineCode(AsmBuilder* builder)
{
    std::cout << "in StoreInstruction::genMachineCode\n";
    // TODO 2012679 finish
    auto cur_block = builder->getBlock(); // 获取当前block
    MachineInstruction* cur_inst = nullptr; // 指令
    // store reg, addr
    MachineOperand* src = genMachineOperand(operands[1]);
    // 如果把一个常数存起来
    if (operands[1]->getEntry()->isConstant()){
        std::cout << "immediate\n";
        //将常量加载到temp中，再到src
        MachineOperand* temp = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, temp, src);
        cur_block->InsertInst(cur_inst);
        src = new MachineOperand(*temp);
    }
    // store一个全局变量
    if(operands[0]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry*>(operands[0]->getEntry())->isGlobal()) {
        std::cout << "global operand\n";
        auto dst = genMachineOperand(operands[0]); // 目的
        auto internal_reg1 = genMachineVReg(); // MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
        auto internal_reg2 = new MachineOperand(*internal_reg1); // internal_reg1对应的MachineOperand
        // dst--->internal_reg1
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, dst);
        cur_block->InsertInst(cur_inst);
        // internal_reg2---->src
        cur_inst = new StoreMInstruction(cur_block, src, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // store栈中的临时变量，t0，t1
    else if(operands[0]->getEntry()->isTemporary() && operands[0]->getDef() && operands[0]->getDef()->isAlloc()){
        std::cout << "local operand\n";
        MachineOperand* src1 = genMachineReg(11); // fp register
        // 偏移量
        auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->getOffset());
        cur_inst = new StoreMInstruction(cur_block, src, src1, src2);
        cur_block -> InsertInst(cur_inst);
    }
    // store 临时变量
    else {
        std::cout << "store to pointer\n";
        auto dst = genMachineOperand(operands[0]);
        cur_inst = new StoreMInstruction(cur_block, src, dst);
        cur_block->InsertInst(cur_inst);
    }
    std::cout << "out StoreInstruction::genMachineCode\n";
}

void BinaryInstruction::genMachineCode(AsmBuilder* builder)
{
    std::cout << "in BinaryInstruction::genMachineCode\n";
    // TODO: 
    //copy by GitHub
    // complete other instructions
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    /* HINT:
    * The source operands of ADD instruction in ir code both can be immediate num.
    * However, it's not allowed in assembly code.
    * So you need to insert LOAD/MOV instrucrion to load immediate num into register.
    * As to other instructions, such as MUL, CMP, you need to deal with this situation, too.*/

   MachineInstruction* cur_inst = nullptr;
    if (src1->isImm()) 
    {
        auto temp_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, temp_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*temp_reg);
    }
    if (src2->isImm()) 
    {
        auto temp_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, temp_reg, src2);
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*temp_reg);
    }
    switch (opcode) 
    {
        case ADD:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
            break;
        case SUB:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
            break;
        case AND:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::AND, dst, src1, src2);
            break;
        case OR:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::OR, dst, src1, src2);
            break;
        case MUL:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);
            break;
        case DIV:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
            break;
        case MOD: 
        {
            // dst = src1 % src2 <==>
            // 1. dst = src1 / src2
            // 2. src2 = dst * src2
            // 3. dst = src1 - src2
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
            cur_block->InsertInst(cur_inst);
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, src2, dst, src2);
            cur_block->InsertInst(cur_inst);
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
            break;
        }
        default:
            break;
    }
    cur_block->InsertInst(cur_inst);
    
    std::cout << "out BinaryInstruction::genMachineCode\n";

}

void CmpInstruction::genMachineCode(AsmBuilder* builder)
{
    std::cout << "in CmpInstruction::genMachineCode\n";
    // TODO 2012679 finish
    MachineBlock* cur_block = builder->getBlock();//获取当前block
    MachineInstruction* cur_inst = nullptr;
    MachineOperand * src1 = genMachineOperand(operands[1]);
    MachineOperand * src2 = genMachineOperand(operands[2]);
    //如果是立即数，load
    if (src1->isImm()) {
        MachineOperand* temp1 = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, temp1, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*temp1);
    }
    if (src2->isImm()) {
        MachineOperand* temp2 = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, temp2, src2);
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*temp2);
    }
    //cmp指令
    // 2013287 这里很容易会搞错，MovMInstruction的第四个参数是通过CmpInstruction中的enum得到比较运算符对应的数字
    // 但是在MachineInstruction中是利用这个数字在MachineInstruction中的数字找运算符
    cur_inst = new CmpMInstruction(cur_block, src1, src2, opcode);
    cur_block->InsertInst(cur_inst);
    //E, NE, L, GE, G, LE
    MachineOperand* dst = genMachineOperand(operands[0]);
    MachineOperand* true_Operand = genMachineImm(1);
    MachineOperand* false_Operand = genMachineImm(0);
    // ==
    if (opcode == CmpInstruction::E) {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, true_Operand, E);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, false_Operand, NE);
        cur_block->InsertInst(cur_inst);
    } 
    // !=
    else if (opcode == CmpInstruction::NE) {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, true_Operand, NE);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, false_Operand, E);
        cur_block->InsertInst(cur_inst);
        }
    
    // <
    else if (opcode == CmpInstruction::L) {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, true_Operand, L);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, false_Operand, GE);
        cur_block->InsertInst(cur_inst);
    }
    // <=
    else if (opcode == CmpInstruction::LE) {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, true_Operand, LE);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, false_Operand, G);
        cur_block->InsertInst(cur_inst);
    }
    // >=
    else if (opcode == CmpInstruction::GE) {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, true_Operand, GE);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, false_Operand, L);
        cur_block->InsertInst(cur_inst);
    }
    // >
    else if (opcode == CmpInstruction::G) {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, true_Operand, G);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, false_Operand, LE);
        cur_block->InsertInst(cur_inst);
    }
    std::cout << "out CmpInstruction::genMachineCode\n";

}

void UncondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    std::cout << "in UncondBrInstruction::genMachineCode\n";
    // TODO 2012679 finish
    /*对于 UncondBrInstruction，同学们只需要生成一条无条件跳转指令即可*/
    MachineBlock *cur_block = builder->getBlock();//获取当前block
    std::string temp =".L" + std::to_string(branch->getNo());//.L5
    auto dst = new MachineOperand(temp);
    BranchMInstruction* cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst);

    cur_block->InsertInst(cur_inst);
    std::cout << "out UncondBrInstruction::genMachineCode\n";
}

void CondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    std::cout << "in CondBrInstruction::genMachineCode\n";
    // TODO 2012679 finish
    /*对于CondBrInstruction，首先明确在中间代码中该指令一定位于CmpInstruction之后，
    对CmpInstruction 的翻译比较简单，相信大家都能完成。对 CondBrInstruction，
    同学们首先需要在 AsmBuilder 中添加成员以记录前一条 CmpInstruction 的条件码，
    从而在遇到 CondBrInstruction 时 生成对应的条件跳转指令跳转到 True Branch，
    之后需要生成一条无条件跳转指令跳转到 False Branch。*/
    MachineBlock *cur_block = builder->getBlock();//获取当前block
    MachineInstruction* cur_inst = nullptr;//指令

    //true_branch
    std::string str_true =".L" + std::to_string(true_branch->getNo());//跳转block false_branch
    MachineOperand* dst_true = new MachineOperand(str_true);    
    int cond_no = cur_block->getCmpNo(); //前一条 CmpInstruction 的条件码
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst_true, cond_no);
    cur_block->InsertInst(cur_inst);

    //false_branch
    std::string str_false =  ".L" + std::to_string(false_branch->getNo());//跳转block false_branch
    MachineOperand* dst_false = new MachineOperand(str_false);
    //两条指令的dst是不一样的
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst_false);
    cur_block->InsertInst(cur_inst);
    std::cout << "out CondBrInstruction::genMachineCode\n";

}

void RetInstruction::genMachineCode(AsmBuilder* builder)
{
    std::cout << "in RetInstruction::genMachineCode\n";
    // TODO 2012679 finish
    /* HINT:
    * 1. Generate mov instruction to save return value in r0
    * 2. Restore callee saved registers and sp, fp
    * 3. Generate bx instruction */
   /*对于 RetInstruction 同学们需要考虑的情况比较多。
   首先，当函数有返回值时，我们需要生成 MOV 指令，将返回值保存在 R0 寄存器中；
   其次，我们需要生成 ADD 指令来恢复栈帧，（如果该函数有 Callee saved 寄存器，我们还需要生成 POP 指令恢复这些寄存器），
   生成 POP 指令恢复 FP 寄存器；最后再生成跳转指令来返回到 Caller。*/
   auto cur_block = builder->getBlock();//获取当前block
   MachineInstruction* cur_inst = nullptr;//指令
   //有返回值，mov指令
   if (!operands.empty()) {
        MachineOperand* temp = new MachineOperand(MachineOperand::REG, 0);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, temp, genMachineOperand(operands[0]));
        cur_block->InsertInst(cur_inst);
    }
    //add指令恢复栈帧
    MachineOperand *sp = new MachineOperand(MachineOperand::REG, 13);
    MachineOperand *size =new MachineOperand(MachineOperand::IMM, builder->getFunction()->AllocSpace(0));
    cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,sp, sp, size);
    cur_block->InsertInst(cur_inst);
    /*
    函数返回正确顺序应该是：
    mov r0, r6
	add sp, sp, #12
	pop {r4, r5, r6, fp, lr}
	bx lr
    我的代码输出没有pop
    */
    //pop指令恢复fp寄存器
    /*
    int index = 0;
    long unsigned int i = 1;
    fprintf(yyout, "1:test pop,and the num is %ld\n", operands.size() - 1);
    while(i != operands.size()) 
    {
        if (index == 4)
        {
            break;
        }
        if (genMachineOperand(operands[index+1])->isImm()) 
        {
            cur_block->InsertInst(new LoadMInstruction(cur_block, genMachineReg(index), genMachineOperand(operands[index+1])));
        } 
        else
        {
            cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, genMachineReg(index), genMachineOperand(operands[index+1])));
        }
        index++;
        i++;
    }
    auto operand = genMachineOperand(operands[operands.size() - 1]);
    fprintf(yyout, "2:test pop,and the num is %ld\n", operands.size() - 1);
    //printf("test pop,and the num is %ld",operands.size() - 1);
    for (int i = operands.size() - 1; i > 4; i--) 
    {
        operand = genMachineOperand(operands[i]);
        if (operand->isImm()) 
        {
            cur_inst = new LoadMInstruction(cur_block, genMachineVReg(), operand);
            cur_block->InsertInst(cur_inst);
            operand = genMachineVReg();
        }
        std::vector<MachineOperand*> temp;
        cur_block->InsertInst(new StackMInstruction(cur_block, StackMInstruction::POP, temp, operand));
    }
    */
    //bx lr
    MachineOperand *lr = new MachineOperand(MachineOperand::REG, 14);
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BX, lr);
    cur_block->InsertInst(cur_inst);
    std::cout << "out RetInstruction::genMachineCode\n";
}
//增
void CallInstruction::genMachineCode(AsmBuilder* builder) 
{
    std::cout << "in CallInstruction::genMachineCode\n";
    auto cur_block = builder->getBlock();
    MachineOperand* operand;  
    MachineInstruction* cur_inst;
    int index = 0;
    long unsigned int i = 1;
    // 添加函数实参，注意oparands[0]放返回值
    while(i != operands.size()) {
        if (index == 4) {
            break;
        }
        if (genMachineOperand(operands[index+1])->isImm()) {
            cur_block->InsertInst(new LoadMInstruction(cur_block, genMachineReg(index), genMachineOperand(operands[index+1])));
        } 
        else {
            cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, genMachineReg(index), genMachineOperand(operands[index+1])));
        }
        index++;
        i++;
    }
    // 如果大于4需要生成push指令传递参数
    for (int i = operands.size() - 1; i > 4; i--) {
        operand = genMachineOperand(operands[i]);
        if (operand->isImm()) 
        {
            cur_inst = new LoadMInstruction(cur_block, genMachineVReg(), operand);
            cur_block->InsertInst(cur_inst);
            operand = genMachineVReg();
        }
        std::vector<MachineOperand*> temp;
        cur_block->InsertInst(new StackMInstruction(cur_block, StackMInstruction::PUSH, temp, operand));
    }
    // 之后生成跳转指令进入Callee函数
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, new MachineOperand(func->toStr().c_str()));
    cur_block->InsertInst(cur_inst);
    // 如果之前有压栈传递函数，则现在要恢复SP寄存器
    if (operands.size() > 5) {
        MachineOperand *sp = new MachineOperand(MachineOperand::REG, 13);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,sp, sp, genMachineImm((operands.size() - 5) * 4));
        cur_block->InsertInst(cur_inst);
    }
    // 保存R0寄存器中的函数返回值
    if (dst) {
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, genMachineOperand(dst), new MachineOperand(MachineOperand::REG, 0));
        cur_block->InsertInst(cur_inst);
    }
    std::cout << "out CallInstruction::genMachineCode\n";
}

void ZextInstruction::genMachineCode(AsmBuilder* builder) 
{
    std::cout << "in ZextInstruction::genMachineCode\n";
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    auto cur_inst =new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
    cur_block->InsertInst(cur_inst);
    std::cout << "out ZextInstruction::genMachineCode\n";
}

void XorInstruction::genMachineCode(AsmBuilder* builder) 
{
    std::cout << "in XorInstruction::genMachineCode\n";
    MachineBlock *cur_block = builder->getBlock();
    MovMInstruction *cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, genMachineOperand(operands[0]), genMachineImm(1), MachineInstruction::EQ);
    cur_block->InsertInst(cur_inst);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, genMachineOperand(operands[0]), genMachineImm(0), MachineInstruction::NE);
    cur_block->InsertInst(cur_inst);
    std::cout << "out XorInstruction::genMachineCode\n";
}

