#include "Life.h"
#include <fstream>
#include <sstream>
#include <istream>
#include <iostream>
#include <cctype>

using namespace std;

Life::Life(const std::string& name, size_t width, size_t height, 
     uint8_t birthFactor, uint8_t surviveFactor)
    : bFactor(birthFactor), sFactor(surviveFactor), w(width), h(height),
      univName(name), aliveCount(0), field(new Bitmap(width * height, true)),
      auxField(new Bitmap(width * height, true)), difference(new std::set<LifeCoord>()),
      auxDifference(new std::set<LifeCoord>())
{
}

void my_getline(ifstream&, std::string&);
void get_word(ifstream&, std::string&);

Life::Life(const std::string& filename)
    : bFactor(0), sFactor(0), aliveCount(0), difference(new std::set<LifeCoord>()),
      auxDifference(new std::set<LifeCoord>()) 
{
    ifstream in;
    in.open(filename.c_str()); // for watcom
    std::string buf;
    my_getline(in, buf);
    if (buf != "#Life 1.06") {
        cout << "Bad version\n";
        throw 1;
    }
    //in >> buf;
    get_word(in, buf);
    if (buf != "#N") {
        cout << "Bad name\n";
        throw 1;
    }
    my_getline(in, univName);
    //in >> buf;
    get_word(in, buf);
    if (buf != "#R") {
        cout << "Bad rules\n";
        throw 1;
    }
    my_getline(in, buf);
    parseFactors(buf);
    //in >> buf;
    get_word(in, buf);
    if (buf != "#S") {
        cout << "Bad sizes\n";
        throw 1;
    }
    in >> w >> h;
    field = new Bitmap(w * h, true);
    //auxField = make_shared<Bitmap>(w * h, true);
    //field->resize(w * h);
    //auxField->resize(w * h);
    int64_t x, y;
    while (in >> x >> y) 
        set(x, y);
    auxField = new Bitmap(*field);
    in.close();
}

Life::~Life() {
    delete field;
    delete auxField;
    delete difference;
    delete auxDifference;
}

size_t Life::getAliveCount(void) {
    return aliveCount;
}

size_t Life::getWidth(void) {
    return w;
}

size_t Life::getHeight(void) {
    return h;
}

const std::set<LifeCoord>& Life::getDifference(void) {
    return *auxDifference;
}

void my_getline(ifstream& is, std::string& buf) {
    char *cbuf = new char[1024];
    is.getline(cbuf, 1024);
    buf = cbuf;
    delete[] cbuf;
}

void get_word(ifstream& is, std::string& buf) {
    buf = "";

    char ch;
    while (1) { // eat spaces
        ch = is.get();
        if (is.eof())
            break;
        if (!isspace(ch)) {
            is.putback(ch);
            break;
        }
    }

    while (1) {
        ch = is.get();
        if (is.eof())
            break;
        if (isspace(ch))
            break;
        buf += ch;
    }
}

void Life::set(int64_t x, int64_t y, bool value) {
    //cout << "Set " << x << " " << y << " = " << value << endl;
    if (get(x, y) != value) {
        addDiffBlock(x, y);
        aliveCount = aliveCount + (value ? 1 : -1);
        field->set(flatIdx(x, y), value);
    }
}

void Life::clear(int64_t x, int64_t y) {
    set(x, y, false);
}

bool Life::get(int64_t x, int64_t y) {
    return field->at(flatIdx(x, y));
}

void Life::addDiffBlock(int64_t i, int64_t j) {
   // std::cout << "Adding diff block " << i << " " << j << std::endl;
    for (int x = -1; x < 2; ++x)
        for (int y = -1; y < 2; ++y) {
       //     std::cout << "Size: " << auxDifference->size() << std::endl;
            auxDifference->insert(LifeCoord(normalizeX(i + x), normalizeY(j + y)));
        }
}

void Life::step(int stCount) {
    if (stCount == 0)
        return;

    uint8_t aliveCount;
    *difference = *auxDifference;

    for (size_t step = 0; step < stCount; ++step) {
        /*std::cout << "DIFFERENCE: ";
        for (std::set<LifeCoord>::const_iterator c = difference->begin(); c!= difference->end(); ++c) {
            std::cout << "(" << c->x << "; " << c->y << ") ";
        }
        std::cout << std::endl;*/

        auxDifference->clear();

        for (std::set<LifeCoord>::const_iterator c = difference->begin(); c != difference->end(); ++c) {
            const int64_t& i = c->x,
                           j = c->y;

            aliveCount = 
                get(i - 1, j - 1) + get(i, j - 1) + get(i + 1, j - 1) +
                get(i - 1, j) +                     get(i + 1, j) +
                get(i - 1, j + 1) + get(i, j + 1) + get(i + 1, j + 1);

            if (get(i, j) && !((1 << aliveCount) & sFactor)) {
                this->aliveCount--;
                addDiffBlock(i, j);
                auxField->set(flatIdx(i, j), false);
            } else if (!get(i, j) && ((1 << aliveCount) & bFactor)) {
                this->aliveCount++;
                addDiffBlock(i, j);
                auxField->set(flatIdx(i, j), true);
            }
        }

        *difference = *auxDifference;
        *field = *auxField;
    }
}

int64_t Life::normalizeX(int64_t x) {
    return (x < 0 ? w + x : (size_t)x % w);
}

int64_t Life::normalizeY(int64_t y) {
    return (y < 0 ? h + y : (size_t)y % h);
}

size_t Life::flatIdx(int64_t x, int64_t y) {
    x = (x < 0 ? w + x : (size_t)x % w);
    y = (y < 0 ? h + y : (size_t)y % h);
    return (size_t)y * w + (size_t)x;
}

void Life::parseFactors(const std::string& factorString) {
    uint8_t* arr[2] = {&bFactor, &sFactor};
    size_t i = 0;
    for (size_t j = 0; j < 2; ++j) {
        for (; i < factorString.length(); ++i) {
            char ch = factorString[i];
            if (ch == '/') {
                ++i;
                break;
            }
            if (ch < '0' || ch > '7')
                continue;
            uint8_t num = ch - '0';
            *(arr[j]) |= (1 << num);
        }
    }
}

std::string toBits(uint8_t x)  {
    // watcom has no stringstream :-(
    char res[9]; 
    char len = 0;
    for (size_t i = 0; i < 8; ++i)
        if (x & (1 << i)) {
            res[len++]= static_cast<char>(i) + '0';
        }
    res[len] = 0;

    return std::string(res);
}

std::string Life::strFactors(void) {
    std::string res;
    res += "B";
    res += toBits(bFactor);
    res += "/S";
    res += toBits(sFactor);
    return res;
}

void Life::dump(const std::string& filename) {
    ofstream out;
    out.open(filename.c_str()); // watcom doesn't support `ostream out(filename);`
    out << "#Life 1.06" << endl;
    out << "#N " << univName.c_str() << endl;
    out << "#R " << strFactors().c_str() << endl;
    out << "#S " << w << " " << h << endl;
    for (size_t i = 0; i < w; ++i) {
        for (size_t j = 0; j < h; ++j) {
            if (get(i, j)) {
                out << i << " " << j << endl;
            }
        }
    }
    out.close();
}

void Life::print(void) {
    for (int i = 0;i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            putchar(get(j, i) ? '1' : '0');
        }
        putchar('\n');
    }
}
