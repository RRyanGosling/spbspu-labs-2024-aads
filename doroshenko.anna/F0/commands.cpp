#include "commands.hpp"
#include <iostream>
#include <functional>
#include <algorithm>
#include <fstream>
#include <vector>
#include "List.hpp"

using dictionary = BST< std::string, std::string >;

void doroshenko::help(std::istream& input, std::ostream& output)
{
  if (input.peek() != '\n')
  {
    throw std::exception();
  }
  output << "1) help - output of available commands\n";
  output << "2) create <name> - create a dictionary named name\n";
  output << "3) remove <name> - delete a dictionary named name\n";
  output << "4) print <name> - output the contents of the dictionary named name\n";
  output << "5) sort <frequency> <name> - sort the dictionary by frequency\n";
  output << "6) sort <alphabet> <name> - sort the dictionary by alphabet\n";
  output << "7) delete <key> - delete the element\n";
  output << "8) find <key> - output the frequency of an element\n";
  output << "9) top <name> - output of the tree most common words\n";
  output << "10) open <filename> - open a file to create dictionary from its contents\n";
  output << "11) write <filename> - open a file to write a dictionary\n";
  output << "12) stop - close the dictionary to edit\n";
  output << "13) save - shut down a file, exit edit module, save data\n";
}

void doroshenko::createDict(BST< std::string, dictionary >& dicts, std::istream& input, std::ostream& output)
{
  std::string dictName;
  input >> dictName;
  if (!input)
  {
    return;
  }
  dicts.insert(dictName, dictionary());
  dictionary currentDict = dicts.at(dictName).second;
  using namespace std::placeholders;
  BST< std::string, std::function< void(dictionary&, std::istream&, std::ostream&) > > cmdDict;
  cmdDict.insert("delete", std::bind(doroshenko::deleteKey, _1, _2, _3));
  cmdDict.insert("find", std::bind(doroshenko::findKey, _1, _2, _3));
  cmdDict.insert("open", std::bind(doroshenko::openFile, _1, _2, _3));
  std::string cmdType;
  while (input >> cmdType && cmdType != "stop")
  {
    try
    {
      if(cmdDict.find(cmdType) != cmdDict.cend())
      {
        cmdDict.at(cmdType).second(currentDict, input, output);
        dicts.erase(dictName);
        dicts.insert(dictName, currentDict);
      }
      else
      {
        warningDict(output);
        input.clear();
        input.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
      }
    }
    catch (const std::out_of_range& e)
    {
      warningDict(output);
      input.clear();
      input.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
  }
}

void doroshenko::deleteKey(dictionary& dict, std::istream& input, std::ostream& output)
{
  std::string key;
  input >> key;
  if (dict.find(key) == dict.cend())
  {
    warningElem(output);
    return;
  }
  dict.erase(key);
}

void doroshenko::findKey(dictionary& dict, std::istream& input, std::ostream& output)
{
  std::string key;
  input >> key;
  if (dict.find(key) == dict.cend() || dict.find(key)->second.empty())
  {
    warningElem(output);
    return;
  }
  output << dict.find(key)->second << "\n";
}

void doroshenko::openFile(dictionary& dict, std::istream& input, std::ostream& output)
{
  std::string word;
  List< std::string > words;
  std::string filename;
  input >> filename;
  std::ifstream in(filename);
  if (!in)
  {
    warningFile(output);
    return;
  }
  while (in >> word)
  {
    std::string lowercaseWord = "";
    for (char c : word)
    {
      lowercaseWord += std::tolower(c);
    }
    std::string result = "";
    for (char c : lowercaseWord)
    {
      if (std::isalnum(c))
      {
        result += c;
      }
    }
    words.pushBack(result);
  }
  for (List< std::string >::Iterator it = words.begin(); it != List< std::string >::ConstIterator(); it++)
  {
    size_t frequency = 0;
    for (List< std::string >::Iterator sIt = words.begin(); sIt != List< std::string >::ConstIterator(); sIt++)
    {
      if (*sIt == *it)
      {
        frequency++;
      }
    }
    if (dict.find(*it) == dict.cend())
    {
      dict.insert(*it, std::to_string(frequency));
    }
  }
}

void doroshenko::writeToFile(const BST< std::string, dictionary >& dicts, std::istream& in, std::ostream& out)
{
  std::string filename;
  in >> filename;
  std::ofstream output;
  output.open(filename, std::ios::app);
  if (!output)
  {
    warningFile(out);
    return;
  }
  std::string dictName;
  in >> dictName;
  if (dicts.find(dictName) == dicts.cend())
  {
    warningDict(out);
    return;
  }
  dictionary dict = dicts.find(dictName)->second;
  output << "\n" << dictName << "\n";
  for (auto it = dict.begin(); it != dict.end(); it++)
  {
    output << it->first << ": " << it->second << "\n";
  }
  output.close();
}

void doroshenko::removeDict(BST< std::string, dictionary >& dicts, std::istream& in, std::ostream& out)
{
  std::string dictName;
  in >> dictName;
  if (dicts.find(dictName) == dicts.cend())
  {
    warningDict(out);
    return;
  }
  dicts.erase(dictName);
}

void doroshenko::printDict(const BST< std::string, dictionary >& dicts, std::istream& in, std::ostream& out)
{
  std::string dictName;
  in >> dictName;
  if (dicts.find(dictName) == dicts.cend())
  {
    warningDict(out);
    return;
  }
  dictionary dictToPrint = dicts.find(dictName)->second;
  out << dictName << "\n";
  if (dictToPrint.isEmpty())
  {
    return;
  }
  for (auto it = dictToPrint.begin(); it != dictToPrint.end(); it++)
  {
    out << it->first << ": " << it->second << "\n";
  }
}

void doroshenko::sortDict(BST< std::string, dictionary >& dicts, std::istream& input, std::ostream& output)
{
  using namespace std::placeholders;
  using func = std::function< void(BST< std::string, dictionary >&, std::istream&, std::ostream&) >;
  BST< std::string, func > cmdSort;
  cmdSort.insert("frequency", std::bind(doroshenko::sortByFrequency, _1, _2, _3));
  cmdSort.insert("alphabet", std::bind(doroshenko::sortByAlphabet, _1, _2, _3));
  std::string sortType;
  input >> sortType;
  try
  {
    if (dicts.isEmpty())
    {
      warningDict(output);
    }
    else
    {
      cmdSort.at(sortType).second(dicts, input, output);
    }
  }
  catch (const std::out_of_range& e)
  {
    warningDict(output);
    std::cin.clear();
    std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
  }
}

void doroshenko::sortByFrequency(BST< std::string, dictionary >& dicts, std::istream& in, std::ostream& out)
{
  std::string dictName;
  in >> dictName;
  if (dicts.find(dictName) == dicts.cend())
  {
    warningDict(out);
    return;
  }
  dictionary dictToSort = dicts.find(dictName)->second;
  if (dictToSort.isEmpty() || std::isdigit(dictToSort.begin()->first[0]))
  {
    return;
  }
  out << dictName << "\n";
  BST< std::string, std::string > temp;
  for (auto it = dictToSort.begin(); it != dictToSort.end(); it++)
  {
    temp.insert(it->first, it->second);
  }
  size_t i = 0;
  while (i != temp.getSize())
  {
    auto maxIter = temp.begin();
    for (auto iter = temp.begin(); iter != temp.end(); ++iter)
    {
      if (std::stoll(iter->second) > std::stoll(maxIter->second) && std::stoll(iter->second) != 0)
      {
        maxIter = iter;
      }
    }
    out << maxIter->first << ": " << maxIter->second << "\n";
    temp.at(maxIter->first).second = "0";
    i++;
  }
}

void doroshenko::sortByAlphabet(BST< std::string, dictionary >& dicts, std::istream& in, std::ostream& out)
{
  std::string dictName;
  in >> dictName;
  if (dicts.find(dictName) == dicts.cend())
  {
    warningDict(out);
    return;
  }
  dictionary dictToSort = dicts.find(dictName)->second;
  if (dictToSort.isEmpty() || !std::isdigit(dictToSort.begin()->first[0]))
  {
    out << "The dictionary is already sorted\n";
    return;
  }
  BST< std::string, std::string > temp;
  for (auto it = dictToSort.begin(); it != dictToSort.end(); it++)
  {
    temp.insert(it->second, it->first);
  }
  out << dictName << "\n";
  for (auto it = temp.begin(); it != temp.end(); it++)
  {
    out << it->first << ": " << it->second << "\n";
  }
}

void doroshenko::printTop(BST< std::string, dictionary >& dicts, std::istream& in, std::ostream& out)
{
  std::string dictName;
  in >> dictName;
  if (dicts.find(dictName) == dicts.cend())
  {
    warningDict(out);
    return;
  }
  dictionary dictToSort = dicts.find(dictName)->second;
  BST< std::string, std::string > temp;
  for (auto it = dictToSort.begin(); it != dictToSort.end(); it++)
  {
    temp.insert(it->first, it->second);
  }
  size_t i = 0;
  while (i < 3)
  {
    auto maxIter = temp.begin();
    for (auto iter = temp.begin(); iter != temp.end(); ++iter)
    {
      if (std::stoll(iter->second) > std::stoll(maxIter->second) && std::stoll(iter->second) != 0)
      {
        maxIter = iter;
      }
    }
    out << maxIter->first << ": " << maxIter->second << "\n";
    temp.at(maxIter->first).second = "0";
    i++;
  }
}

void doroshenko::warningDict(std::ostream& output)
{
  output << "Dictionary does not exist\n";
}

void doroshenko::warningElem(std::ostream& output)
{
  output << "Element does not exist\n";
}

void doroshenko::warningFile(std::ostream& output)
{
  output << "File does not exist\n";
}

void doroshenko::warningInvCom(std::ostream& output)
{
  output << "<INVALID COMMAND>\n";
}
