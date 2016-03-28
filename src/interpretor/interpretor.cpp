#include "interpretor.h"

Interpretor::Interpretor(size_t memorySize, bool defaultInstructions)
{
    setMemorySize(memorySize);

    if(defaultInstructions)
        this->createDefaultInstructions();

    reset(true, true);
}

void Interpretor::reset(bool keepMemorySize, bool keepInstrtructions)
{
    if(keepMemorySize)
    {
        for(auto& m : mMemory)
            m = 0;
    }
    else
        mMemory.clear();

    if(!keepInstrtructions)
        mInstructions.clear();

    mCurrentInstructionPtr = 0;
    mCurrentMemoryPtr = 0;

    mProgram = "";

    mInput = "";
    mOutput = "";
}

void Interpretor::setProgram(const std::string& code, bool reset)
{
    if(reset)
        this->reset();

    mProgram = code;
}

std::string Interpretor::getProgram() const
{
    return mProgram;
}

void Interpretor::setMemorySize(size_t size)
{
    mMemory.resize(size);
}

size_t Interpretor::getMemorySize() const
{
    return mMemory.size();
}

bool Interpretor::step()
{
    try
    {
        mInstructions.at(mProgram[mCurrentInstructionPtr++])();
    }
    catch(...)
    {
        return false;
    }

    return true;
}

size_t Interpretor::execute(size_t maxStep)
{
    size_t i = 0;
    for(;(i < maxStep || maxStep == -1u) && mCurrentInstructionPtr < mProgram.size() && step(); i++);
    return (i == maxStep || mCurrentInstructionPtr == mProgram.size()) ? i : -1;
}

void Interpretor::setInput(const std::string& input)
{
    mInput = input;
}

std::string Interpretor::getOutput() const
{
    return mOutput;
}

void Interpretor::resetOutput(size_t InstructionCountToRemove)
{
    if(InstructionCountToRemove == -1u)
        mOutput.clear();
    else
        mOutput.erase(0, std::min(mOutput.size(), InstructionCountToRemove));
}

void Interpretor::clearInstructions()
{
    mInstructions.clear();
}

void Interpretor::setInstruction(Instruction instruction, const std::function<void(void)>& func, bool overwriteExisting)
{
    if(overwriteExisting)
        removeInstruction(instruction);

    mInstructions.insert({instruction, func});
}

void Interpretor::removeInstruction(Instruction instruction)
{
    mInstructions.erase(instruction);
}

bool Interpretor::instructionExist(Instruction instruction) const
{
    return mInstructions.find(instruction) != mInstructions.end();
}

std::vector<Interpretor::Instruction> Interpretor::getInstructions() const
{
    std::vector<Instruction> instructions;
    for(const auto& pair : mInstructions)
        instructions.push_back(pair.first);

    return instructions;
}

Interpretor::Instruction Interpretor::getCurrentInstruction() const
{
    return mProgram[mCurrentInstructionPtr];
}

Interpretor::MemoryType Interpretor::getCurrentMemory() const
{
    return mMemory.at(mCurrentMemoryPtr);
}

Interpretor::MemoryType& Interpretor::getCurrentMemory()
{
    return mMemory.at(mCurrentMemoryPtr);
}

void Interpretor::createDefaultInstructions()
{
    setInstruction('+', [this](){getCurrentMemory()++;});
    setInstruction('-', [this](){getCurrentMemory()--;});

    setInstruction('<', [this](){mCurrentMemoryPtr = mCurrentMemoryPtr == 0 ? mMemory.size()-1 : mCurrentMemoryPtr-1;});
    setInstruction('>', [this](){mCurrentMemoryPtr = mCurrentMemoryPtr == mMemory.size()-1 ? 0 : mCurrentMemoryPtr+1;});

    setInstruction('[', [this]()
    {
        if(!getCurrentMemory())
        {
            int deep = 1;
            while(deep > 0 && mCurrentInstructionPtr <= mMemory.size())
            {
                if(getCurrentInstruction() == '[')
                    deep++;
                else if(getCurrentInstruction() == ']')
                    deep--;

//                mCurrentInstructionPtr += (bool)deep; //only increment if we did nto found yet
                mCurrentInstructionPtr++;
            }

//            mCurrentInstructionPtr--; //oups, we got too far
        }
    });

    setInstruction(']', [this]()
    {
        if(getCurrentMemory())
        {
            ----mCurrentInstructionPtr; //we are currently pointing to the instructing AFTER ']' so we decrement once to get to the instruction and a second time to get to the precedent
            int deep = 1;
            while(deep > 0 && mCurrentInstructionPtr != -1)
            {
                if(getCurrentInstruction() == ']')
                    deep++;
                else if(getCurrentInstruction() == '[')
                    deep--;

//                mCurrentInstructionPtr -= (bool)deep; //only increment if we did nto found yet
                mCurrentInstructionPtr--;
            }

            mCurrentInstructionPtr++; //oups, we got too far
        }
    });

    setInstruction('.', [this](){mOutput.push_back(getCurrentMemory());});
//    setInstruction('^', [this](){mOutput += std::to_string(getCurrentMemory());});
//    setInstruction(',', [this](){mOutput.push_back(getCurrentMemory());});
}
