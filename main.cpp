#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <iomanip>

using mystring = std::string;
using pieces = std::vector<mystring*>;
using strings = std::vector<pieces>;

using deduplicator = std::set<mystring>;

void split(pieces& results, mystring const& original, char separator);

template<typename T>
struct Comparator {
private:
    std::vector<int> _pie;
public:
    Comparator<T>(std::vector<int>& pie):_pie(pie){}
    bool operator()(const pieces& first, const pieces& second);
    bool operator()(const std::vector<mystring>& first, const std::vector<mystring>& second);
};

struct GroupBy {
private:
    std::vector<int> _pie;
    std::vector<int> _distinct;
public:
    GroupBy(std::vector<int>& pie, std::vector<int>& distinct):_pie(pie), _distinct(distinct){}
    void groupby(strings& source, std::vector<std::vector<mystring>>& destination);
    void distinct(std::vector<std::set<mystring>>& sets, std::vector<mystring>& value);
};

deduplicator normalizer;

int main()
{
    using namespace std;

    fstream input("/home/andrei/Desktop/oldies/desktop4/waa_android.csv");
    mystring line;
    strings arrayString;
    vector<std::vector<mystring>> groupbyDestination;

    vector<int> sortColumns{0,1,2,3};
    vector<int> groupByColumns{0,1,2,3};
    vector<int> distinctColumns{8,9,10};

    pieces piece;

    while(getline(input, line))
    {
        split(piece, line, ',');

        arrayString.emplace_back(piece);
        piece.clear();
    }

    input.close();

    cout << "lines read : " << arrayString.size() << endl;

    sort(arrayString.begin(), arrayString.end(), Comparator<mystring>(sortColumns));

    int count = 0;
    for(auto it = arrayString.begin(); it != arrayString.end(); ++it) {
        for(size_t st = 0; st < (*it).size() - 1; ++st)
        {
            cout << setw(16) << *(*it)[st] << ";";
        }
        cout << setw(16) << *(*it)[(*it).size() - 1];
        cout << endl;
        count++;
        if (count == 100) {
            break;
        }
    }

    cout << "normalizer size : " << normalizer.size() << endl;

    GroupBy group(groupByColumns, distinctColumns);
    group.groupby(arrayString, groupbyDestination);

    cout << "groupby size : " << groupbyDestination.size() << endl;

//    sort(groupbyDestination.begin(), groupbyDestination.end(), Comparator<int>(distinctColumns));
//    vector<int> floats{5};
//    sort(groupbyDestination.begin(), groupbyDestination.end(), Comparator<mystring>(floats));
//    vector<int> floatss{4};
//    sort(groupbyDestination.begin(), groupbyDestination.end(), Comparator<int>(floatss));

    for(auto it = groupbyDestination.begin(); it != groupbyDestination.end(); ++it) {
        for(size_t st = 0; st < (*it).size() - 1; ++st)
        {
            cout << setw(16) << (*it)[st] << ";";
        }
        cout << setw(16) << (*it)[(*it).size() - 1];
        cout << endl;
    }

    return 0;
}

void split(pieces& results, mystring const& original, char separator)
{
    mystring::const_iterator start = original.begin();
    mystring::const_iterator end = original.end();
    mystring::const_iterator next = find(start, end, separator);
    while (next != end) {

        mystring str(start, next);
        mystring* toPush = nullptr;
        deduplicator::iterator val = normalizer.find(str);
        if (val == normalizer.end()) {
            normalizer.insert(str);
            toPush = const_cast<mystring*>(&*normalizer.find(str));
        } else {
            toPush = const_cast<mystring*>(&*val);
        }

        results.emplace_back(toPush);
        start = next + 1;
        next = find(start, end, separator);
    }

    mystring str(start, next);
    mystring* toPush = nullptr;
    deduplicator::iterator val = normalizer.find(str);
    if (val == normalizer.end()) {
        normalizer.insert(str);
        toPush = const_cast<mystring*>(&*normalizer.find(str));
    } else {
        toPush = const_cast<mystring*>(&*val);
    }

    results.emplace_back(toPush);
}

template<typename T>
bool Comparator<T>::operator()(const pieces& first, const pieces& second)
{
    bool _comp = false;

    for(int _pos : _pie) {
        if (*first[_pos] != *second[_pos]) {
            _comp = (*first[_pos] < *second[_pos]);
            return _comp;
        }
    }

    return _comp;
}

template<typename T>
bool Comparator<T>::operator()(const std::vector<mystring>& first, const std::vector<mystring>& second)
{
    bool _comp = false;

    for(int _pos : _pie) {
        if (first[_pos] != second[_pos]) {
            _comp = (first[_pos] < second[_pos]);
            return _comp;
        }
    }

    return _comp;
}

template<>
bool Comparator<int>::operator()(const std::vector<mystring>& first, const std::vector<mystring>& second)
{
    bool _comp = false;

    for(int _pos : _pie)
    {
        int fv = stoi(first[_pos]);
        int sv = stoi(second[_pos]);

        if (fv != sv) {
            _comp = (fv < sv);
            return _comp;
        }
    }


    return _comp;
}

template<>
bool Comparator<float>::operator()(const std::vector<mystring>& first, const std::vector<mystring>& second)
{
    bool _comp = false;

    for(int _pos : _pie)
    {
        float fv = stof(first[_pos]);
        float sv = stof(second[_pos]);

        if (fv != sv) {
            _comp = (fv < sv);
            return _comp;
        }
    }


    return _comp;
}

void GroupBy::groupby(strings &source, std::vector<std::vector<mystring>> &destination)
{
    std::vector<std::set<mystring>> sets(13, std::set<mystring>());
    std::vector<mystring> value;

    for(auto it = source.begin(); it != source.end(); ++it) {
        std::transform(std::begin(*it), std::end(*it), std::back_inserter(value), [](mystring* item){return *item;});

        size_t size = destination.size();
        std::vector<mystring>& temp = destination[size-1];

        if (size == 0) {
            destination.emplace_back(value);
        } else {
            size = destination.size();
            temp = destination[size-1];

            bool _comp = false;

            for(int _pos : _pie) {
                if (value[_pos] != temp[_pos]) {
                    _comp = false;
                    break;
                } else {
                    _comp = true;
                }
            }

            if (_comp == false)
            {
                for(int _dist : _distinct) {
                    size_t count = sets[_dist].size();
                    sets[_dist].clear();
                    temp[_dist] = std::to_string(count);
                }

                destination.emplace_back(value);
            }
            this->distinct(sets, value);
        }
        value.clear();
    }

    size_t size = destination.size();
    std::vector<mystring>& temp = destination[size-1];
    for(int _dist : _distinct) {
        size_t count = sets[_dist].size();
        sets[_dist].clear();
        temp[_dist] = std::to_string(count);
    }
}

void GroupBy::distinct(std::vector<std::set<mystring>>& sets, std::vector<mystring>& value)
{
    for(int _dist : _distinct)
    {
        std::set<mystring>* current = &sets[_dist];
        current->insert(value[_dist]);
    }
}
