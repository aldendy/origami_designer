#ifndef ROWREDUCE
#define ROWREDUCE

#include<vector>

using namespace std;

int rowreduce(vector<vector<double> > *matrix);

int GetPivot(int col, int rank, vector<vector<double> > *matrix);

void SwitchRows(int first, int second, vector<vector<double> > *matrix);

void SubtractRows(int first, int second, int col, vector<vector<double> > *matrix);

#endif // ROWREDUCE

