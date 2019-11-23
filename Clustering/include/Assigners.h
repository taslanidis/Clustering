#include <string>
#include <vector>
#include "Library.h"

using namespace std;

template <class Point>
class Assigner {
protected:
    int K;
    int Grids;
    int L;
    int k;
public:
    Assigner(){}
    virtual vector<int>** assign(vector<vector<Point>>*, vector<vector<Point>*>) {return NULL;}
    virtual string get_name() {}
    virtual int get_K() {return K;}
};

template <class Point>
class Lloyd_assignment : public Assigner<Point> {
private:
    string name = "Lloyd's Assignment";
public:
    Lloyd_assignment(int K, int Grids, int L, int k){this->K = K; this->Grids = Grids; this->L = L; this->k = k;}
    vector<int>** assign(vector<vector<Point>>*, vector<vector<Point>*>);
    string get_name();
};

template <class Point>
class Inverse_assignment : public Assigner<Point> {
private:
    string name = "Inverse Assignment";
public:
    Inverse_assignment(int K, int Grids, int L, int k){this->K = K; this->Grids = Grids; this->L = L; this->k = k;}
    vector<int>** assign(vector<vector<Point>>*, vector<vector<Point>*>);
    string get_name();
};