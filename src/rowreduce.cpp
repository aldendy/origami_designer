#include "rowreduce.h"

int rowreduce(vector<vector<double> > *matrix)
{
    int rank = 0;

    for (int col = 0; col < (int)(*matrix)[0].size(); col++)
    {
        int pivot = GetPivot(col, rank, matrix);

        if (pivot != -1) //if a pivot exists, do this. otherwise, do nothing.
        {
            SwitchRows(rank, pivot, matrix);

            for (int i = (rank + 1); i < (int)matrix->size(); i++)
            {
                SubtractRows(rank, i, col, matrix);
            }
            rank++;
        }
    }

    return rank;
}

//This function returns the first non-zero row that can serve as a pivot. This is done in preparation
// for row switching.
int GetPivot(int col, int rank, vector<vector<double> > *matrix)
{
    for (int i = rank; i < (int)matrix->size(); i++)
    {
        if (((*matrix)[i][col] > 0.000001) || ((*matrix)[i][col] < -0.000001))
            return i;
    }
    return -1; //this is the error condition where no non-zero items were found
}

//This fucntion switches the row 'first' with the row 'second'. No operation is performed when the rows are the same.
//It is assumed that the matrix has sufficient rows.
void SwitchRows(int first, int second, vector<vector<double> > *matrix)
{
    if (first != second)
    {
        vector<double> other = (*matrix)[first];
        (*matrix)[first] = (*matrix)[second];
        (*matrix)[second] = other;
    }
}

//This function uses the 'first' row to cancel out the 'second'. The 'first' row is assumed to contain the pivot.
void SubtractRows(int first, int second, int col, vector<vector<double> > *matrix)
{
    double factor = (*matrix)[second][col] / (*matrix)[first][col];

    for (int i = 0; i < (int)(*matrix)[second].size(); i++)
    {
        (*matrix)[second][i] = (*matrix)[second][i] - factor*(*matrix)[first][i];
    }
}
