#include "BinaryTreeII.hpp"
#include <iostream>
#include "fstream"
#include <vector>
#include <climits>
#include <string>
#include "BinarySearchTree.hpp"

void hohlova::readFile(const std::string& path, BinarySearchTree<int, std::string>& bst) {

    if (path.empty())
        throw std::runtime_error("Path is empty\n");

    std::ifstream file(path);

    if (!file.is_open())
        throw std::runtime_error("File not open\n");


    std::string line;
    while (!file.eof()) {
        std::getline(file, line);
        ReadDataSet(line, bst);
    }

    file.close();
}

void hohlova::print(Dictionary& bst, const std::string& str)
{
    BinarySearchTree< int, std::string >* findBst = nullptr;
    findBst = bst.get(str);
    if (findBst == nullptr)
    {
        warningInvCom(std::cout);
        return;
    }
    else if (findBst->empty())
    {
        warningEmpty(std::cout);
        return;
    }
    auto it = findBst->begin();
    std::cout << str << ' ';
    for (; it != findBst->end();)
    {
        std::cout << (*it).first << ' ' << (*it).second;
        if (++it != findBst->end())
        {
            std::cout << ' ';
        }
    }
    std::cout << '\n';
}
int hohlova::ParseNum(const std::string& num) {
    int result = 0;
    result = std::stoi(num);
    return result;
}
void hohlova::ReadDataSet(const std::string& dataSet, BinarySearchTree<int, std::string>& bst) {

    size_t pos = 0;
    std::string word;
    word.reserve(10);
    std::vector<std::string > stringList;
    stringList.reserve(10);

    while (pos <= dataSet.size()) {
        if (std::isspace(dataSet[pos]) || pos == dataSet.size()) {
            stringList.push_back(word);
            word = "";
        }
        else {
            word += dataSet[pos];
        }
        ++pos;
    }

    size_t i = 0;

    while (i < stringList.size() - 1)
    {
        bst.push(ParseNum(stringList[i]), stringList[i + 1]);
        i += 2;
    }
}

int sum(int a, int b) {
    if (b > 0 && a > INT_MAX - b) {
        throw std::runtime_error("Sum overflow");
    }
    else if (b < 0 && a < INT_MIN - b) {
        throw std::runtime_error("Sum underflow");
    }
    return a + b;
}

void hohlova::warningEmpty(std::ostream& out)
{
    out << "<EMPTY>\n";
}

void hohlova::warningInvCom(std::ostream& out)
{
    out << "<INVALID COMMAND>\n";
}
