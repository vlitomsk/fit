#include <functional>
#include <iostream>
#include <vector>
#include <cstdint>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "json11.hpp"

using namespace std;
using namespace json11;

typedef enum {LOG_0 = 0, LOG_1 = 1, LOG_HIZ = 666} llevel_t;
typedef enum {IN, OUT, BIN, MUX, ZERO, ONE} btype;

class LogicBlock {
public:
    LogicBlock(size_t inCount, size_t outCount) {
        inputs.resize(inCount);
        outputs.resize(outCount);
        fill(inputs.begin(), inputs.end(), nullptr);
    }

    LogicBlock(const LogicBlock& lb);
    ~LogicBlock() { }
    /* :-[[ */
    void attachInput(size_t inNumber, LogicBlock* blOut) {
        inputs.at(inNumber) = blOut;
    }

    void detachInput(size_t inNumber) {
        inputs.at(inNumber) = nullptr;
    }

    llevel_t output(size_t outNumber) const {
        return outputs.at(outNumber);
    }

    llevel_t output(void) const {
        return outputs.at(0);
    }

    size_t inputCount(void) const {
        return inputs.size();
    }

    size_t outputCount(void) const {
        return outputs.size();
    }

    bool levelDefined(vector<LogicBlock*>::const_iterator start, vector<LogicBlock*>::const_iterator end) {
        return count_if(start, end, [](const LogicBlock* x) { if (x == nullptr) return 1; return static_cast<int>(x->output() == LOG_HIZ); }) == 0;
    }

    bool levelDefined(vector<LogicBlock*>::const_iterator it) {
        if (*it != nullptr) 
            return (*it)->output() != LOG_HIZ;
        return false;
    }

    // unsafe method
    size_t getValue(vector<LogicBlock*>::const_iterator lsb, vector<LogicBlock*>::const_iterator msb) {
        size_t power = 0;
        return accumulate(lsb, msb, 0,
            [&](size_t acc, const LogicBlock* x) { return acc + ((x->output() == LOG_1 ? 1 : 0) << (power++)); });       
    }

    void whoami(void) {
        cout << "I'm ";
        if (tag == IN) 
            cout << "input";
        if (tag == OUT)
            cout << "output";
        if (tag == BIN)
            cout << "bingate";
        if (tag == ONE)
            cout << "const ONE";
        if (tag == ZERO)
            cout << "const ZERO";
        if (tag == MUX)
            cout << "mux";
        cout << endl;
    }
    virtual void propagate(void) = 0;
//private:
    int dfsMark;
    btype tag;
    vector<llevel_t> outputs;
    vector<LogicBlock*> inputs;
};

// Singletons LogicOne, LogicZero
class LogicOne : public LogicBlock {
public:
    LogicOne() : LogicBlock(0, 1) {
        tag = ONE;
        outputs[0] = LOG_1;
    }
//public:
    void propagate(void) { outputs[0] = LOG_1; }
};

class LogicZero : public LogicBlock {
public:
    LogicZero() : LogicBlock(0, 1) {
        tag = ZERO;
        outputs[0] = LOG_0;
    }
//public:
    void propagate(void) { outputs[0] = LOG_0; }
};

// used for inputs & outputs
class LogicBuffer : public LogicBlock {
public:
    LogicBuffer() : LogicBlock(1, 1) { tag = OUT; }
    ~LogicBuffer();

    void propagate(void) {
        outputs[0] = inputs[0]->output();
    }
};

typedef enum {GATE_AND = 0, GATE_OR = 1, GATE_XOR = 2} gate_t;
typedef std::function<llevel_t(llevel_t, llevel_t)> binaryGateFn_t;

llevel_t notFunction(llevel_t a) {
    if (a == LOG_0)
        return LOG_1;
    if (a == LOG_1)
        return LOG_0;
    return LOG_HIZ;
}

llevel_t andFunction(llevel_t a, llevel_t b) {
    if (a == LOG_HIZ || b == LOG_HIZ) 
        return LOG_HIZ;
    if (a == LOG_1 && b == LOG_1)
        return LOG_1;
    return LOG_0;
}

// lazy evaluation :-)
llevel_t orFunction(llevel_t a, llevel_t b) {
    if (a == LOG_1 || b == LOG_1)
        return LOG_1;
    if (a == LOG_HIZ || b == LOG_HIZ)
        return LOG_HIZ;
    return LOG_0;
}

llevel_t xorFunction(llevel_t a, llevel_t b) {
    if ((a == LOG_1 && b == LOG_1) || (a == LOG_0 && b == LOG_0))
        return LOG_0;
    if (a == LOG_HIZ || b == LOG_HIZ)
        return LOG_HIZ;
    return LOG_1;
}

static binaryGateFn_t fnDet[3] = { andFunction, orFunction, xorFunction };
binaryGateFn_t swBinGateFn(gate_t gateType) {
    return fnDet[gateType];
}

class BinaryGate : public LogicBlock {
public:
    BinaryGate(binaryGateFn_t fn) : LogicBlock(2, 1), gateFn(fn) { tag= BIN; };
    BinaryGate(gate_t gateType) : LogicBlock(2, 1), gateFn(swBinGateFn(gateType)) { }
    BinaryGate(const BinaryGate& bg) : LogicBlock(bg), gateFn(bg.gateFn) { };
    ~BinaryGate();

//public:
    void propagate(void) {
        outputs[0] = gateFn(inputs[0]->output(), inputs[1]->output());
        //outputs[0] = LOG_1;
    }
private:
    binaryGateFn_t gateFn;
};

class Mux8 : public LogicBlock{
public:
    Mux8() : LogicBlock(11,1) { }
    Mux8(const Mux8& mx) : LogicBlock(mx) { tag = MUX;}
    ~Mux8();
//public:
    void propagate(void) {
        outputs[0] = LOG_HIZ;
        if (levelDefined(inputs.cbegin() + 8, inputs.cend())) {
            size_t idx = getValue(inputs.cbegin() + 8, inputs.cend());
            if (levelDefined(inputs.cbegin() + idx)) {
                outputs[0] = notFunction(inputs[idx]->output());
            }
        } 
    }
};

class JSONCircuit : public LogicBlock {
public:
    JSONCircuit(const string& path, const string& circName) : LogicBlock(0, 0),
     namedInputsCount(0), namedOutputsCount(0), blockCount(0) {
        // manually resize vectors here
        ifstream jsonFile(path);
        stringstream ss;
        ss << jsonFile.rdbuf();
        string err;
        auto circuits = Json::parse(ss.str(), err);
        if (err.length() > 0) {
            cerr << "JSON parse error: " << err << endl;
            throw 1;
        }
        try {
            auto circuit = circuits[circName].array_items();
            loadItems(circuit);
            setLinks(circuit);
        } catch (std::exception& e) {
            cerr << "Your file is corrupted (can't find some fields)" << endl;
//            throw 2;
        }
    }

    JSONCircuit(const JSONCircuit& jc);
    ~JSONCircuit() {
        for (auto b : blocks)
            delete b;
    }

    void attachInput(const string& inName, LogicBlock* blOut) {
        blocks.at(vertIdx.at(inName))->attachInput(0, blOut);
//        LogicBlock::attachInput(vertIdx.at(inName), blOut);
    }

    void detachInput(const string& inName) {
        blocks.at(vertIdx.at(inName))->detachInput(0);
    }

    llevel_t output(const string& outName) {
        return blocks.at(vertIdx.at(outName))->output();;
    }

//public:
    void dfs(LogicBlock* lb) {
        if (lb == nullptr)
            return;
        if (lb->dfsMark == dfsMark)
            return;
        for (auto l : lb->inputs) {
            dfs(l);
        }
        lb->propagate();
        lb->dfsMark = dfsMark;
    }

    void propagate(void) {
        // bfs here
        dfsMark = rand();
        size_t outputIdx = 0;
        for (auto logOutput : logOutputs) {
            dfs(logOutput);
//            outputs[outputIdx++] = logOutput->output();
        }
    }
private:
    int dfsMark;
    unordered_map<string, size_t> vertIdx;
    vector<LogicBuffer*> logOutputs;
    vector<LogicBlock*> blocks;
    Json circuit;
    size_t namedInputsCount, namedOutputsCount, blockCount;

    LogicBlock* parseBlockType(const string& type) const {
        if (type == "and")
            return new BinaryGate(GATE_AND);
        if (type == "or")
            return new BinaryGate(GATE_OR);
        if (type == "xor")
            return new BinaryGate(GATE_XOR);
        if (type == "multiplexer")
            return new Mux8();
        cerr << "Can't recognize block type: " << type << endl;
        throw std::out_of_range("Can't recognize block type");
    }
    
    void loadItems(const Json::array& jsonCircuit) { // may throw out_of_range exception
        for (auto jsonBlock : jsonCircuit) {
            string id = jsonBlock["id"].string_value();
            string type = jsonBlock["type"].string_value();
            if (type == "in") {
                vertIdx[id] = namedInputsCount++; // индекс в inputs
                auto li = new LogicBuffer();
                inputs.push_back(li);
                blocks.push_back(li);
            } else if (type == "out") {
                vertIdx[id] = namedOutputsCount++; 
                auto li = new LogicBuffer();
                logOutputs.push_back(li);
                blocks.push_back(li); 
            } else {
                blocks.push_back(parseBlockType(type));
            }
                vertIdx[id] = blockCount;
            blockCount++;
        }
        outputs.resize(namedOutputsCount);
        fill(outputs.begin(), outputs.end(), LOG_HIZ);
    }

    void setLinks(const Json::array& jsonCircuit) {
        size_t blockIdx = 0;
        for (auto jsonBlock : jsonCircuit) {
            string id = jsonBlock["id"].string_value();
            size_t inputNo = 0;
            Json::array blInputs = jsonBlock["inputs"].array_items();
            for (auto inputName : blInputs) {
                string sval = inputName.string_value();
                if (sval.length() != 0) {
                    blocks.at(blockIdx)->attachInput(inputNo, blocks.at(vertIdx.at(sval)));
                } else {
//                    blocks.at(blockIdx)->detachInput(inputNo);                
                }
                inputNo++;
            }
            blockIdx++;
        }
    }
};

static LogicZero l_0_;
static LogicOne l_1_;
static LogicZero* l0 = &l_0_;
static LogicOne* l1 = &l_1_;

LogicBlock* lvl(unsigned x) {
    if (x & 1)
        return l1;
    return l0;
}

void aluOper(JSONCircuit& jc, const string& filename, unsigned op) {
    ofstream out(filename);
    jc.attachInput("Op2", l0);
    jc.attachInput("Op1", lvl(op >> 1));
    jc.attachInput("Op0", lvl(op >> 0));
    out << "A0\tA1\tB0\tB1\tC.in\t|Out0\tOut1\tC.out" << endl;
    for (int i = 0; i < 32; ++i) {
        unsigned a = i & 3;
        unsigned b = (i >> 2) & 3;
        unsigned carryIn = (i >> 4) & 1;
        jc.attachInput("A0", lvl(a));
        jc.attachInput("A1", lvl(a >> 1));
        jc.attachInput("B0", lvl(b));
        jc.attachInput("B1", lvl(b >> 1));
        jc.attachInput("CarryIn", lvl(carryIn));

        jc.propagate();
        out << (a & 1) << "\t" << (a >> 1) << "\t";
        out << (b & 1) << "\t" << (b >> 1) << "\t";
        out << carryIn << "\t";

        out << "|" << jc.output("Out0") << "\t" << jc.output("Out1") << "\t" << jc.output("CarryOut");
        out << endl;
    }
    out.close();
}

void doLaba(void) {
    JSONCircuit jc("alu.json", "ALU");
    aluOper(jc, "xnor.txt", 0);
    aluOper(jc, "nor.txt", 1);
    aluOper(jc, "nand.txt", 2);
}

int main() {
    cout << "Doing LABA...\n";
    doLaba();
    cout << "Done!\n";

    return 0;
}

