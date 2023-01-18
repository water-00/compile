#include <algorithm>
#include "LinearScan.h"
#include "MachineCode.h"
#include "LiveVariableAnalysis.h"

LinearScan::LinearScan(MachineUnit *unit)
{
    this->unit = unit;
    for (int i = 4; i < 11; i++)
        regs.push_back(i); // regs放可用寄存器
}

void LinearScan::allocateRegisters()
{
    for (auto &f : unit->getFuncs())
    {
        func = f;
        bool success;
        success = false;
        while (!success)        // repeat until all vregs can be mapped
        {
            computeLiveIntervals();
            success = linearScanRegisterAllocation();
            if (success)        // all vregs can be mapped to real regs
                modifyCode();
            else                // spill vregs that can't be mapped to real regs
                genSpillCode();
        }
    }
}

// def-use chains
void LinearScan::makeDuChains()
{
    LiveVariableAnalysis lva;
    lva.pass(func);
    du_chains.clear();
    int i = 0;
    std::map<MachineOperand, std::set<MachineOperand *>> liveVar;
    for (auto &bb : func->getBlocks())
    {
        liveVar.clear();
        for (auto &t : bb->getLiveOut())
            liveVar[*t].insert(t);
        int no;
        no = i = bb->getInsts().size() + i;
        for (auto inst = bb->getInsts().rbegin(); inst != bb->getInsts().rend(); inst++)
        {
            (*inst)->setNo(no--);
            for (auto &def : (*inst)->getDef())
            {
                if (def->isVReg())
                {
                    auto &uses = liveVar[*def];
                    du_chains[def].insert(uses.begin(), uses.end());
                    auto &kill = lva.getAllUses()[*def];
                    std::set<MachineOperand *> res;
                    set_difference(uses.begin(), uses.end(), kill.begin(), kill.end(), inserter(res, res.end()));
                    liveVar[*def] = res;
                }
            }
            for (auto &use : (*inst)->getUse())
            {
                if (use->isVReg())
                    liveVar[*use].insert(use);
            }
        }
    }
}

void LinearScan::computeLiveIntervals()
{
    makeDuChains();
    intervals.clear();
    for (auto &du_chain : du_chains)
    {
        int t = -1;
        for (auto &use : du_chain.second)
            t = std::max(t, use->getParent()->getNo());
        Interval *interval = new Interval({du_chain.first->getParent()->getNo(), t, false, 0, 0, {du_chain.first}, du_chain.second});
        intervals.push_back(interval);
    }
    for (auto& interval : intervals) {
        auto uses = interval->uses;
        auto begin = interval->start;
        auto end = interval->end;
        for (auto block : func->getBlocks()) {
            auto liveIn = block->getLiveIn();
            auto liveOut = block->getLiveOut();
            bool in = false;
            bool out = false;
            for (auto use : uses)
                if (liveIn.count(use)) {
                    in = true;
                    break;
                }
            for (auto use : uses)
                if (liveOut.count(use)) {
                    out = true;
                    break;
                }
            if (in && out) {
                begin = std::min(begin, (*(block->begin()))->getNo());
                end = std::max(end, (*(block->rbegin()))->getNo());
            } else if (!in && out) {
                for (auto i : block->getInsts())
                    if (i->getDef().size() > 0 &&
                        i->getDef()[0] == *(uses.begin())) {
                        begin = std::min(begin, i->getNo());
                        break;
                    }
                end = std::max(end, (*(block->rbegin()))->getNo());
            } else if (in && !out) {
                begin = std::min(begin, (*(block->begin()))->getNo());
                int temp = 0;
                for (auto use : uses)
                    if (use->getParent()->getParent() == block)
                        temp = std::max(temp, use->getParent()->getNo());
                end = std::max(temp, end);
            }
        }
        interval->start = begin;
        interval->end = end;
    }
    bool change;
    change = true;
    while (change)
    {
        change = false;
        std::vector<Interval *> t(intervals.begin(), intervals.end());
        for (size_t i = 0; i < t.size(); i++)
            for (size_t j = i + 1; j < t.size(); j++)
            {
                Interval *w1 = t[i];
                Interval *w2 = t[j];
                if (**w1->defs.begin() == **w2->defs.begin())
                {
                    std::set<MachineOperand *> temp;
                    set_intersection(w1->uses.begin(), w1->uses.end(), w2->uses.begin(), w2->uses.end(), inserter(temp, temp.end()));
                    if (!temp.empty())
                    {
                        change = true;
                        w1->defs.insert(w2->defs.begin(), w2->defs.end());
                        w1->uses.insert(w2->uses.begin(), w2->uses.end());
                        // w1->start = std::min(w1->start, w2->start);
                        // w1->end = std::max(w1->end, w2->end);
                        auto w1Min = std::min(w1->start, w1->end);
                        auto w1Max = std::max(w1->start, w1->end);
                        auto w2Min = std::min(w2->start, w2->end);
                        auto w2Max = std::max(w2->start, w2->end);
                        w1->start = std::min(w1Min, w2Min);
                        w1->end = std::max(w1Max, w2Max);
                        auto it = std::find(intervals.begin(), intervals.end(), w2);
                        if (it != intervals.end())
                            intervals.erase(it);
                    }
                }
            }
    }
    sort(intervals.begin(), intervals.end(), compareStart);
}

bool LinearScan::linearScanRegisterAllocation()
{
    // Todo
    regs.clear();
    active.clear();
    for (int i = 4; i < 11; i++)
        regs.push_back(i);

    for (auto &interval: intervals) {
        expireOldIntervals(interval);
        if (regs.size() == 0) {
            spillAtInterval(interval);
            return false;
        } else {
            auto it = std::min_element(regs.begin(), regs.end()); // 把regs中标号最小的空闲寄存器分配出去
            interval->rreg = *it;
            regs.erase(it);
            active.push_back(interval);
            sort(active.begin(), active.end(), compareEnd);
        }
    }
    return true;
}

void LinearScan::modifyCode()
{
    for (auto &interval : intervals)
    {
        func->addSavedRegs(interval->rreg);
        for (auto def : interval->defs)
            def->setReg(interval->rreg);
        for (auto use : interval->uses)
            use->setReg(interval->rreg);
    }
}

void LinearScan::genSpillCode()
{
    for(auto &interval:intervals)
    {
        if(!interval->spill)
            continue;
        // TODO
        /* HINT:
         * The vreg should be spilled to memory.
         * 1. insert ldr inst before the use of vreg
         * 2. insert str inst after the def of vreg
         */ 

        // 在栈内分配空间，获取偏移
        // 栈从高地址开始往低地址分配空间，所以偏移量是负数
        interval->disp = -func->AllocSpace(4);
        MachineOperand *offset = new MachineOperand(MachineOperand::IMM, interval->disp);
        MachineOperand *fp = new MachineOperand(MachineOperand::REG, 11); // 在arm架构中r11是帧指针寄存器，指向当前函数的栈帧。在这里用作栈的起始地址

        // 遍历其 USE 指令的列表，在 USE 指令前插入 LoadMInstruction，将其从栈内加载到目前的虚拟寄存器中
        for (auto use: interval->uses) {
            // 先看disp是否超过-255和255，如果是，则需要分为两步
            if (interval->disp > 255 || interval->disp < -255) {
                // 需要先有一个虚拟寄存器保存disp
                MachineOperand *vreg = new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
                // 把disp加载到vreg: ldr vreg, offset
                MachineInstruction *inst1 = new LoadMInstruction(use->getMachineBlock(), vreg, offset);
                use->getParent()->insertBefore(inst1);

                // ldr r_use, [fp, vreg]
                MachineInstruction *inst2 = new LoadMInstruction(use->getMachineBlock(), use, fp, new MachineOperand(*vreg));
                use->getParent()->insertBefore(inst2);
            } else {
                // 如果没超过，在使用前直接从disp处加载即可: ldr r_use, [fp, offset]
                MachineInstruction *inst = new LoadMInstruction(use->getMachineBlock(), use, fp, offset);
                use->getParent()->insertBefore(inst);
            }

        }

        // 遍历其 DEF 指令的列表，在 DEF 指令后插入 StoreMInstruction，将其从目前的虚拟寄存器中存到栈内
        for (auto def: interval->defs) {
            if (interval->disp > 255 || interval->disp < -255) {
                MachineOperand *vreg = new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
                // 在定义指令后生成: ldr r_def, offset
                MachineInstruction *inst1 = new LoadMInstruction(def->getMachineBlock(), vreg, offset);
                def->getParent()->insertBack(inst1);

                // str r_def ... TODO
                MachineInstruction *inst2 = new StoreMInstruction(def->getMachineBlock(), def, fp, new MachineOperand(*vreg));
                inst1->insertBack(inst2);

            } else {
                MachineInstruction *inst = new StoreMInstruction(def->getMachineBlock(), def, fp, offset);
                def->getParent()->insertBack(inst);
            }
        }
    }
}

// 检查在interval开始时有没有区间已经过期
void LinearScan::expireOldIntervals(Interval *interval)
{
    // Todo
    auto j = active.begin();
    while (j != active.end()) {
        if ((*j)->end >= interval->start) {
            return; // active中的end是递增的，所以一个j的end比i的start大就说明j及其以后的区间都没过期
        }
        // j过期了
        regs.push_back((*j)->rreg);
        // (*j)->rreg = -1;
        j = active.erase(find(active.begin(), active.end(), *j));
        sort(regs.begin(), regs.end());
    }
}

void LinearScan::spillAtInterval(Interval *interval)
{
    // Todo
    Interval *spill = active.back();
    if (spill->end > interval->end) {
        // active的最后一项更晚结束，被溢出
        interval->rreg = spill->rreg;
        // spill->rreg = -1;
        spill->spill = true;
        active.push_back(interval);
        sort(active.begin(), active.end(), compareEnd);
    } else {
        interval->spill = true;
    }
}

bool LinearScan::compareStart(Interval *a, Interval *b)
{
    return a->start < b->start;
}

bool LinearScan::compareEnd(Interval *a, Interval *b) {
    return a->end < b->end;
}