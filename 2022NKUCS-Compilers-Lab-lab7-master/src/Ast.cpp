#include "Ast.h"
#include "SymbolTable.h"
#include "Unit.h"
#include "Instruction.h"
#include "IRBuilder.h"
#include <string>
#include "Type.h"

extern MachineUnit mUnit;//2012679 增 for lab7

extern FILE *yyout;
int Node::counter = 0;
IRBuilder* Node::builder = nullptr;

Node::Node()
{
    seq = counter++;
}

void Node::backPatch(std::vector<Instruction*> &list, BasicBlock*bb)
{
    for(auto &inst:list)
    {
        if(inst->isCond())
            dynamic_cast<CondBrInstruction*>(inst)->setTrueBranch(bb);
        else if(inst->isUncond())
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
    }
}


void Node::backPatchFalse(std::vector<Instruction*> &list, BasicBlock*bb)
{
    for(auto &inst:list)
    {
        if(inst->isCond())
            dynamic_cast<CondBrInstruction*>(inst)->setFalseBranch(bb);
        else if(inst->isUncond())
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
    }
}

std::vector<Instruction*> Node::merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2)
{
    std::vector<Instruction*> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    // std::cout  << "in Ast::genCode(Unit *unit)" << std::endl;
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
    // std::cout  << "out Ast::genCode(Unit *unit)" << std::endl;
}

void UnaryExpr::genCode() {
    // std::cout  << "in UnaryExpr::genCode()" << std::endl;
    // 思路：
    // 1. 对于+ -，生成一个二元表达式，与0做运算，通过运算符转换expr的值即可
    // 2. 对于!，生成icmp，与0比较，把expr转换为比较的结果即可
    BasicBlock *bb = builder->getInsertBB();
    if (op == UNARY_MINUS) {
        expr->genCode();
        Operand *src = expr->getOperand();
        ConstantSymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
        Constant *addr_num_0 = new Constant(se);
        Operand *num = addr_num_0->getOperand();

        new BinaryInstruction(BinaryInstruction::SUB, dst, num, src, bb);
        
    } else if (op == UNARY_PLUS) {
        expr->genCode();
        Operand *src = expr->getOperand();
        ConstantSymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
        Constant *addr_num_0 = new Constant(se);
        Operand *num = addr_num_0->getOperand();

        new BinaryInstruction(BinaryInstruction::ADD, dst, num, src, bb);
    } else if (op == NOT) {
        expr->genCode();
        Operand *src = expr->getOperand(); 
        ConstantSymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
        Constant *addr_num_0 = new Constant(se);
        Operand *num = addr_num_0->getOperand();

        Operand* t = new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
        new CmpInstruction(CmpInstruction::NE, t, src, num, bb);
        src = t;
        new XorInstruction(dst, src, bb);
    }

    // std::cout  << "out UnaryExpr::genCode()" << std::endl;
}

void BinaryExpr::genCode()
{
    // std::cout  << "in BinaryExpr::genCode()" << std::endl;
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == AND)
    {
        BasicBlock *trueBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB. 放expr2的中间代码的位置

        // 设置前驱后继
        trueBB->addPred(bb);
        bb->addSucc(trueBB);

        expr1->genCode();
        backPatch(expr1->trueList(), trueBB); // 如果expr1对了，那我要跳转到trueBB，也就是expr2开始的位置

        builder->setInsertBB(trueBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        true_list = expr2->trueList(); // 如果整个expr1 && expr2正确了，那要跳转的地方就是expr2->trueList()
        false_list = merge(expr1->falseList(), expr2->falseList());

        dst->getType()->setKind(Type::BOOL);
    }
    else if(op == OR)
    {
        BasicBlock *falseBB = new BasicBlock(func); // （如果expr为false）放expr2中间代码的位置

        // 设置前驱后继
        falseBB->addPred(bb);
        bb->addSucc(falseBB);

        expr1->genCode();
        backPatchFalse(expr1->falseList(), falseBB); 
        builder->setInsertBB(falseBB);
        expr2->genCode();
        false_list = expr2->falseList();
        true_list = merge(expr1->trueList(), expr2->trueList());

        dst->getType()->setKind(Type::BOOL);
    }
    else if(op >= LESS && op <= GREATER)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        if (src1->getType()->getSize() == 1) 
        {
            // std::cout << "src1->getType()->getSize() == 1\n";
            Operand* dst = new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(dst, src1, bb);
            src1 = dst;
        }
        if (src2->getType()->getSize() == 1) 
        {
            // std::cout << "src2->getType()->getSize() == 1\n";
            Operand* dst = new Operand(new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(dst, src2, bb);
            src2 = dst;
        }

        int opcode;
        switch (op)
        {
        case LESS:
            opcode = CmpInstruction::L;
            break;
        case LESSEQUAL:
            opcode = CmpInstruction::LE;
            break;
        case EQUAL:
            opcode = CmpInstruction::E;
            break;
        case NOTEQUAL:
            opcode = CmpInstruction::NE;
            break;
        case GREATEREQUAL:
            opcode = CmpInstruction::GE;
            break;
        case GREATER:
            opcode = CmpInstruction::G;
            break;
        default:
            break;
        }
        new CmpInstruction(opcode, dst, src1, src2, bb);
        // std::cout << "op = " << opcode << "\n";
        // 添加到truelist和falselist
        true_list = merge(expr1->trueList(), expr2->trueList());
        false_list = merge(expr1->falseList(), expr2->falseList());

        // 条件跳转
        Instruction *temp = new CondBrInstruction(nullptr, nullptr, dst, bb);
        this->trueList().push_back(temp);
        this->falseList().push_back(temp);

        dst->getType()->setKind(Type::BOOL);
    }
    else if(op >= ADD && op <= DIV)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        case MOD:
            opcode = BinaryInstruction::MOD;
            break;
        case MUL:
            opcode = BinaryInstruction::MUL;
            break;
        case DIV:
            opcode = BinaryInstruction::DIV;
            break;
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
    // std::cout  << "out BinaryExpr::genCode()" << std::endl;
}

void IfStmt::genCode()
{
    // std::cout << "in IfStmt::genCode()\n";
    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    // 前驱后继
    // then_bb -> addPred(builder->getInsertBB());
    // builder -> getInsertBB()->addSucc(then_bb);
    // end_bb -> addPred(then_bb);
    // then_bb -> addSucc(end_bb);
    // end_bb -> addPred(builder->getInsertBB());
    // builder -> getInsertBB()->addSucc(end_bb);

    cond->genCode();

    // TODO: 还需要判断，如果cond不是一个bool类型
    // 那么需要将cond与0比较后再跳转

    if (!cond->getOperand()->getType()->isBool()) {
        // std::cout << "这不是一个bool" << std::endl;
        BasicBlock *bb = builder->getInsertBB();

        // 生成0
        ConstantSymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
        Constant *addr_num_0 = new Constant(se);
        Operand *num = addr_num_0->getOperand();

        // 进行比较，创建一个临时变量存放比较结果
        Operand* t = new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
        Operand *src = cond->getOperand();
        // src = t;
        // 比较的dst为t
        CmpInstruction *inst1 = new CmpInstruction(CmpInstruction::NE, t, src, num, bb);
        cond->trueList().push_back(inst1);
        cond->falseList().push_back(inst1);
        // 根据t跳转
        Instruction *inst2 = new CondBrInstruction(nullptr, nullptr, t, bb);
        cond->trueList().push_back(inst2);
        cond->falseList().push_back(inst2);
    }
    
    backPatch(cond->trueList(), then_bb);
    backPatchFalse(cond->falseList(), end_bb);
    //backPatch(cond->falseList(), end_bb);
    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb); // 在then_bb块的结束，要生成一条跳转到end_bb块的指令

    builder->setInsertBB(end_bb);
    // std::cout << "out IfStmt::genCode()\n";
}

void IfElseStmt::genCode()
{
    // std::cout << "in IfElseStmt::genCode()\n";
    // Todo
    Function *func;
    BasicBlock *then_bb, *else_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);   
    else_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    // 前驱后继
    // then_bb -> addPred(builder->getInsertBB());
    // builder -> getInsertBB()->addSucc(then_bb);
    // else_bb -> addPred(builder->getInsertBB());
    // builder -> getInsertBB()->addSucc(else_bb);
    // end_bb -> addPred(then_bb);
    // then_bb -> addSucc(end_bb);
    // end_bb -> addPred(else_bb);
    // else_bb -> addSucc(end_bb);
    
    cond->genCode();
   
    // TODO: 还需要判断，如果cond不是一个bool类型
    if (!cond->getOperand()->getType()->isBool()) {
        // std::cout << "这不是一个bool" << std::endl;
        BasicBlock *bb = builder->getInsertBB();

        // 生成0
        ConstantSymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
        Constant *addr_num_0 = new Constant(se);
        Operand *num = addr_num_0->getOperand();

        // 进行比较，创建一个临时变量存放比较结果
        Operand* t = new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
        Operand *src = cond->getOperand();
        // src = t;
        // 比较指令的dst为t
        CmpInstruction *inst1 = new CmpInstruction(CmpInstruction::NE, t, src, num, bb);
        cond->trueList().push_back(inst1);
        cond->falseList().push_back(inst1);
        // 根据t跳转
        Instruction *inst2 = new CondBrInstruction(nullptr, nullptr, t, bb);
        cond->trueList().push_back(inst2);
        cond->falseList().push_back(inst2);
    }
    
    backPatch(cond->trueList(), then_bb);
    backPatchFalse(cond->falseList(), else_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    //2012679 增加一个end_bb是否为空的判断 解决测试用例027，AC数+1
    //存在if else 语句结束，函数也结束的情况，那么此时end_bb是空白的，yacc它不能识别空白
    //但是then_bb else_bb却都有个跳到end_bb的指令（此时end_bb不存在，没法跳转） ，导致错误
    // if(!end_bb->empty()){
        // std::cout << "!end_bb\n";
        new UncondBrInstruction(end_bb, then_bb); // 在then_bb块的结束，我要生成一条跳转到end_bb块的指令
    // }
    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    //2012679 增加一个end_bb是否为空的判断
    // if(!end_bb->empty()){
        // std::cout << "!end_bb\n" << end_bb->getNo() << "\n";
        new UncondBrInstruction(end_bb, else_bb); // 在else_bb块的结束，我要生成一条跳转到end_bb块的指令
    // }
    builder->setInsertBB(end_bb);
    // std::cout << "out IfElseStmt::genCode()\n";
}

void WhileStmt::genCode() {
    // std::cout << "in WhileStmt::genCode()\n";
    Function *func;
    BasicBlock *loop_bb, *end_bb, *cond_bb;

    func = builder->getInsertBB()->getParent();
    loop_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);
    cond_bb = new BasicBlock(func);

    // 从当前块结束后要跳到cond_bb块，再一次判断条件
    new UncondBrInstruction(cond_bb, builder->getInsertBB());

    // 前驱后驱
    // cond_bb->addPred(builder->getInsertBB());
    // builder->getInsertBB()->addSucc(cond_bb);
    // loop_bb->addPred(cond_bb);
    // cond_bb->addSucc(loop_bb);
    // end_bb->addPred(loop_bb);
    // loop_bb->addSucc(end_bb);
    // end_bb->addPred(cond_bb);
    // cond_bb->addSucc(end_bb);

    builder->setInsertBB(cond_bb);
    cond->genCode();

    // 如果传入的不是bool类型，同if，把cond拿去和0比较
    if (!cond->getOperand()->getType()->isBool()) {
        // std::cout << "这不是一个bool" << std::endl;
        BasicBlock *bb = cond->builder->getInsertBB();

        // 生成0
        ConstantSymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
        Constant *addr_num_0 = new Constant(se);
        Operand *num = addr_num_0->getOperand();

        // 进行比较，创建一个临时变量存放比较结果
        Operand* t = new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
        Operand *src = cond->getOperand();
        src = t;
        // 比较的dst为t
        CmpInstruction *inst1 = new CmpInstruction(CmpInstruction::NE, t, src, num, bb);
        cond->trueList().push_back(inst1);
        cond->falseList().push_back(inst1);
        // 根据t跳转
        Instruction *inst2 = new CondBrInstruction(nullptr, nullptr, t, bb);
        cond->trueList().push_back(inst2);
        cond->falseList().push_back(inst2);
    }
    
    backPatch(cond->trueList(), loop_bb); // 如果cond为真，进入loop_bb块
    backPatchFalse(cond->falseList(), end_bb); // 如果cond为假，进入end_bb块
    //backPatch(cond->falseList(), end_bb);
    builder->setInsertBB(loop_bb);
    whileStmt->genCode();
    loop_bb = builder->getInsertBB();
    new UncondBrInstruction(cond_bb, loop_bb); // 确定下一轮循环的cond的两个分支, true_branch = cond_bb, false_branch = end_bb

    builder->setInsertBB(end_bb);

    // std::cout << "out WhileStmt::genCode()\n";
}

void Constant::genCode()
{
    // we don't need to generate code.

    // std::cout  << "in Constant::genCode()" << std::endl;

    // std::cout  << "out Constant::genCode()" << std::endl;

}

void Id::genCode()
{
    // std::cout  << "in Id::genCode()" << std::endl;
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
    // std::cout  << "out Id::genCode()" << std::endl;
}

void ConstId::genCode(){
    // std::cout  << "in ConstId::genCode()" << std::endl;

    // std::cout  << "out ConstId::genCode()" << std::endl;
}

void CompoundStmt::genCode()
{
    // std::cout  << "in CompoundStmt::genCode()" << std::endl;
    stmt->genCode();
    // std::cout  << "out CompoundStmt::genCode()" << std::endl;
}

void SeqNode::genCode()
{
    // std::cout  << "in SeqNode::genCode()" << std::endl;
    stmt1->genCode();
    stmt2->genCode();
    // std::cout  << "out SeqNode::genCode()" << std::endl;
}

void DeclStmt::genCode()
{
    // std::cout << "in DeclStmt::genCode()\n";
    for (auto id : ids->idlist) {
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
        if(se->isGlobal())
        {
            // se = 符号表中的变量，addr_se = 该变量的地址，它也在符号表中
            // 像load、store指令操作的就是addr_se & addr而非se
            Operand *addr;
            SymbolEntry *addr_se;
            addr_se = new IdentifierSymbolEntry(*se);
            addr_se->setType(new PointerType(se->getType()));
            addr = new Operand(addr_se);
            se->setAddr(addr);
            // AC数增加1个！
            //这个是没加const的全局变量
            mUnit.insertGlobal(se);//2012679 增 把全局变量插入，用于在汇编代码中输出
            Operand* num; // 全局变量在生成中间代码时需要数值
            // 对某个全局se，需要判断它有没有被初始化
            bool inited = false;
            for (long unsigned int i = 0; i < ids->assignList.size(); i++) {
                if (ids->assignList.at(i)->getLval()->getSymPtr() == se) {
                    // 如果有，则生成对应指令，并设置num
                    ids->assignList.at(i)->genCode();
                    num = ids->assignList.at(i)->getExpr()->getOperand();
                    inited = true;
                    break;
                }
            }
            if (!inited) {
                // 如果没有，则将num置成0
                ConstantSymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
                Constant *addr_num_0 = new Constant(se);
                num = addr_num_0->getOperand();
            }

            // 生成global变量的alloc指令
            new AllocaGlobalInstruction(addr, num, se);
        }
        else if(se->isLocal())
        {
            // 对局部变量来说，在中间代码生成的顺序是：先生成一句int语句中的所有alloca语句，再对其中有赋值操作的变量做store
            // 所以第一次遍历只生成alloca就行了
            Function *func = builder->getInsertBB()->getParent();
            BasicBlock *entry = func->getEntry();
            Instruction *alloca;

            Operand *addr;
            SymbolEntry *addr_se;
            Type *type;
            type = new PointerType(se->getType());
            addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel()); // 生成一个临时变量（临时符号表项）t_x，作为se的地址
            addr = new Operand(addr_se);
            alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
            entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
            se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
        }
    }

    // 第二次遍历再对有赋值语句的局部变量生成store语句
    for (auto id : ids->idlist) {
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
        if (se->isLocal()) {
            // 对DeclStmt中的每个局部变量，检查它是否有赋值语句
            for (long unsigned int i = 0; i < ids->assignList.size(); i++) {
                if (ids->assignList.at(i)->getLval()->getSymPtr() == se) {
                    // 如果有，则生成该赋值指令，并修改se地址为有值的新addr
                    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(ids->assignList.at(i)->getLval()->getSymPtr())->getAddr();
                    se->setAddr(addr);
                    ids->assignList.at(i)->genCode();
                }
            }
        }
    }
    // std::cout << "out DeclStmt::genCode()\n";
}

void ConstDeclStmt::genCode() {
    // std::cout << "in ConstDeclStmt::genCode()\n";
    int index = 0;
    for (auto cid : cids->conidlist) {
        // 对于const变量，方便的是idList和assignList中每一项对应，不用再用循环去通过id找assign了
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(cid->getSymPtr());
        if(se->isGlobal())
        {
            Operand *addr;
            SymbolEntry *addr_se;
            addr_se = new IdentifierSymbolEntry(*se);
            addr_se->setType(new PointerType(se->getType()));
            addr = new Operand(addr_se);
            se->setAddr(addr);
            // !!!!!把const的全局变量给漏掉了
            // AC数增加6个！
            mUnit.insertGlobal(se);//2012679 增 把全局变量插入，用于在汇编代码中输出
            // 直接利用index找到cid对应的assign
            cids->assignList.at(index)->genCode();
            Operand* num = cids->assignList.at(index)->getExpr()->getOperand(); // 全局变量在生成中间代码时需要数值
            // 生成global的alloc指令
            new AllocaGlobalInstruction(addr, num, se);
        }
        else if(se->isLocal())
        {
            Function *func = builder->getInsertBB()->getParent();
            BasicBlock *entry = func->getEntry();
            Instruction *alloca;

            Operand *addr;
            SymbolEntry *addr_se;
            Type *type;
            type = new PointerType(se->getType());
            addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel()); // 生成一个临时变量（临时符号表项）t_x
            addr = new Operand(addr_se);
            alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
            entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
            se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.

            cids->assignList.at(index)->genCode();
            Operand *addr1 = dynamic_cast<IdentifierSymbolEntry*>(cids->assignList.at(index)->getLval()->getSymPtr())->getAddr();
            se->setAddr(addr1); 
            Operand *src = cids->assignList.at(index)->getExpr()->getOperand();
            BasicBlock *t = builder->getInsertBB();
            new StoreInstruction(addr1, src, t);     
        }
        index++;
    }
    // std::cout << "out ConstDeclStmt::genCode()\n";
}

void ReturnStmt::genCode()
{
    // std::cout  << "in ReturnStmt::genCode()" << std::endl;
    // Todo
    BasicBlock *bb = builder->getInsertBB();
    retValue->genCode();
    Operand *src = retValue->getOperand();
    new RetInstruction(src, bb);
    // std::cout  << "out ReturnStmt::genCode()" << std::endl;
}

void AssignStmt::genCode()
{
    // std::cout << "in AssignStmt::genCode()\n";
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymPtr())->getAddr();
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    new StoreInstruction(addr, src, bb);
    // std::cout << "out AssignStmt::genCode()\n";
}

void IdList::genCode() {
    // std::cout << "in IdList::genCode()\n";

    // std::cout << "out IdList::genCode()\n";
}

void ConIdList::genCode() {
    // std::cout << "in ConIdList::genCode()\n";

    // std::cout << "out ConIdList::genCode()\n";
}

void FunctionDef::genCode()
{
    // std::cout  << "\nin FunctionDef::genCode()" << std::endl;
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
    BasicBlock *entry = func->getEntry(); // 入口
    
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);

    if (FPs != nullptr) {
        FPs->genCode();
    }
    if (stmt != nullptr) {
        stmt->genCode();
    }

    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
    */
    // 给每个块里的语句添加前驱后继
    for (auto block = func->begin(); block != func->end(); block++)
    {
        Instruction* i = (*block)->begin();
        Instruction* last = (*block)->rbegin();
        
        while (i != last) {
            if (i->isCond() || i->isUncond()) {
                (*block)->remove(i);
            }
            i = i->getNext();
        }
        
        // 如果最后一条语句是条件跳转语句，那就需要添加前驱后继
        if (last->isCond()) {
            BasicBlock *truebranch = dynamic_cast<CondBrInstruction*>(last)->getTrueBranch();
            BasicBlock *falsebranch = dynamic_cast<CondBrInstruction*>(last)->getFalseBranch();
            (*block)->addSucc(truebranch);
            (*block)->addSucc(falsebranch);
            truebranch->addPred(*block);
            falsebranch->addPred(*block);
        }
        // 如果最后一条语句是非条件跳转语句，添加后继即可
        else if (last->isUncond())
        {
            BasicBlock* dst = dynamic_cast<UncondBrInstruction*>(last)->getBranch();
            (*block)->addSucc(dst);
            // 如果后继目标块为空，则插入返回语句
            dst->addPred(*block);
            if (dst->empty()) {
                if (((FunctionType*)(se->getType()))->getRetType() == TypeSystem::intType) {
                    new RetInstruction(new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), dst);
                }
                else if (((FunctionType*)(se->getType()))->getRetType() == TypeSystem::voidType) {
                    new RetInstruction(nullptr, dst);
                }
            }
        }
        // 如果最后一条语句不是返回语句，添加返回语句
        else if (!last->isRet()) {
            if (((FunctionType*)(se->getType()))->getRetType() == TypeSystem::voidType) {
                new RetInstruction(nullptr, *block);
            }
        }
    }


   
    // std::cout  << "out FunctionDef::genCode()" << std::endl;
}

void FuncFParam::genCode(){ // 函数形参
    // std::cout << "in FuncFParam::genCode()\n";
    // 把值从内存加载到参数变量中
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
    // std::cout << "out FuncFParam::genCode()\n";
}

void FuncFParams::genCode() { // 函数形参列表
    // std::cout << "in FuncFParams::genCode()\n";
    for (long unsigned int i = 0; i < FPs.size(); i++) {

        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(FPs[i]->getSymPtr());
        // std::cout << "kind: " << se->getKind() << std::endl; // 1, VARIABLE
        // std::cout << "scope: " << se->getScope() << std::endl; // 1, PARAM


        Function *func = builder->getInsertBB()->getParent();
        Type *type1 = new PointerType(se->getType());
        // Type *type2 = new IntType(32); // 如果要支持float类型那要改的是这里，src

        SymbolEntry *addr_se1 = new TemporarySymbolEntry(type1, SymbolTable::getLabel());
        // SymbolEntry *addr_se2 = new TemporarySymbolEntry(type2, SymbolTable::getLabel());

        Operand *addr1 = new Operand(addr_se1);
        Operand *addr2 = new Operand(FPs[i]->getSymPtr());


        BasicBlock *entry = func->getEntry();
        AllocaInstruction *alloca = new AllocaInstruction(addr1, se);
        entry->insertFront(alloca);

        // str source, dst dst是第一个参数(应该为Temporary类型)，src是第二个(应该为Variable类型)
        StoreInstruction *store = new StoreInstruction(addr1, addr2, builder->getInsertBB()); // 用于生成store i32 %0, i32* %3, align 4
        entry->insertBack(store);

        se->setAddr(addr1);
        func->getParams().push_back(addr2);

    }
    // std::cout << "out FuncFParams::genCode()\n";
}

void FuncRParams::genCode() { // 函数实参列表
    // do nothing
    // std::cout << "in FuncRParams::genCode()\n";

    // std::cout << "out FuncRParams::genCode()\n";
}

void FunctionCall::genCode() {
    // std::cout << "in FunctionCall::genCode()\n";
    std::vector<Operand*> params;

    if (RPs != nullptr) {
        for (long unsigned int i = 0; i < RPs->Exprs.size(); i++) {
            if (RPs->Exprs[i] != nullptr) {
                RPs->Exprs[i]->genCode();
            }
            params.push_back(RPs->Exprs[i]->getOperand()); // 添加实参
        }
    }

    // 把返回值放到操作数dst中
    BasicBlock *entry = builder->getInsertBB();

    Type *type = new IntType(32); // 用于放返回值的临时变量
    SymbolEntry *addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
    dst = new Operand(addr_se);

    CallInstruction *call = new CallInstruction(dst, symbolEntry, params);
    entry->insertBack(call);

    // std::cout << "out FunctionCall::genCode()\n";
}


void Empty::genCode() {
    // std::cout << "in Empty::genCode()\n";

    // std::cout << "out Empty::genCode()\n";
}

void SingleStmt::genCode() {
    // std::cout << "in SingleStmt::genCode()\n";
    expr->genCode();
    // std::cout << "out SingleStmt::genCode()\n";
}


//2012679 增加getvalue函数
int BinaryExpr::getValue() 
{
    int value = 0;
    switch (op) 
    {
        case ADD:
            value = expr1->getValue() + expr2->getValue();
            break;
        case SUB:
            value = expr1->getValue() - expr2->getValue();
            break;
        case MUL:
            value = expr1->getValue() * expr2->getValue();
            break;
        case DIV:
            if(expr2->getValue())
                value = expr1->getValue() / expr2->getValue();
            break;
        case MOD:
            value = expr1->getValue() % expr2->getValue();
            break;
        case AND:
            value = expr1->getValue() && expr2->getValue();
            break;
        case OR:
            value = expr1->getValue() || expr2->getValue();
            break;
        case LESS:
            value = expr1->getValue() < expr2->getValue();
            break;
        case LESSEQUAL:
            value = expr1->getValue() <= expr2->getValue();
            break;
        case GREATER:
            value = expr1->getValue() > expr2->getValue();
            break;
        case GREATEREQUAL:
            value = expr1->getValue() >= expr2->getValue();
            break;
        case EQUAL:
            value = expr1->getValue() == expr2->getValue();
            break;
        case NOTEQUAL:
            value = expr1->getValue() != expr2->getValue();
            break;
    }
    return value;
}
int UnaryExpr::getValue() 
{
    int value = 0;
    switch (op) {
        case NOT:
            value = !(expr->getValue());
            break;
        case UNARY_MINUS:
            value = -(expr->getValue());
            break;  
        case UNARY_PLUS:
            value = expr->getValue();
            break;
    }
    return value;
}

int Constant::getValue() 
{
    return ((ConstantSymbolEntry*)symbolEntry)->getValue();
}

int Id::getValue() 
{
    return ((IdentifierSymbolEntry*)symbolEntry)->getValue();
}

