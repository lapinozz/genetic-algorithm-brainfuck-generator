#ifndef INTERPRETOR_H
#define INTERPRETOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include <iostream>

class Interpretor
{
    public:
//        using MemoryType = uint32_t;
        using MemoryType = int8_t;
        using Instruction = char;

        Interpretor(size_t memorySize = 1000, bool defaultInstructions = true);

        void reset(bool keepMemorySize = true, bool keepInstrtructions = true);

        void setProgram(const std::string& code, bool reset = true);
        std::string getProgram() const;

        void setMemorySize(size_t size);
        size_t getMemorySize() const;

        bool step();
        size_t execute(size_t maxStep = -1);

        void setInput(const std::string& input);
        std::string getOutput() const;
        void resetOutput(size_t charCountToRemove = -1);

        void clearInstructions();
        void setInstruction(Instruction instruction, const std::function<void(void)>& func, bool overwriteExisting = true);
        void removeInstruction(Instruction instruction);
        bool instructionExist(Instruction instruction) const;
        std::vector<Instruction> getInstructions() const;

    private:
        void createDefaultInstructions();

        Instruction getCurrentInstruction() const;
        MemoryType getCurrentMemory() const;
        MemoryType& getCurrentMemory();

        std::vector<MemoryType> mMemory;
        std::unordered_map<Instruction, std::function<void(void)>> mInstructions;

        size_t mCurrentInstructionPtr;
        size_t mCurrentMemoryPtr;

        std::string mProgram;

        std::string mInput;
        std::string mOutput;
};

#endif // INTERPRETOR_H
