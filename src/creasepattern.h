#ifndef CREASEPATTERN_H
#define CREASEPATTERN_H

#include <vector>
#include <vertex.h>
#include <crease.h>
#include <cmath>
#include <QString>
#include "rowreduce.h"
using namespace std;

class CreasePattern
{
public:
    CreasePattern();
    void AddVertex(double x, double y);
    void AddVertex(double x, double y, int name);
    void AddCrease(Vertex a, Vertex b);
    void AddBrace(Vertex a, Vertex b);
    void AddCrease(int a, int b, int name);
    void AddDeletedLoops(vector<vector<QString> > lps);
    void DeleteVertex(int name);
    void DeleteCrease(int name);
    void DeleteLoops(vector<Crease> candidate);
    void clearEverything();
    void FindVertexCreases(Vertex vertex, vector<Crease> &c);
    void FindVertexLinks(Vertex vertex, vector<Crease> &c);
    int RightOrLeft(Crease start, bool left, double &angle);
    double FindAngle(Crease a, Crease b);
    void FindWall(Crease start, vector<Crease> &path, bool left);
    void FindLoops();
    bool LoopExists(vector<Crease> path, bool is_deleted);
    void GetVertices(vector<Vertex> &v);
    void GetCreases(vector<Crease> &c);
    void GetLoops(vector<vector<Crease> > &l);
    void GetDeletedLoops(vector<vector<Crease> > &l);
    int GetLocation(Crease c);
    void ClearLoops();
    void ClearDeletedLoops();
    void ToggleVertex(int location);
    void ToggleCrease(int location);
    void ToggleCreaseCut(int location);
    void ToggleLoop(int location);
    bool CreasesContain(vector<Crease> set, Crease possible);
    bool LoopsContain(Crease possible);
    bool IsLoopPicked(int loc);
    int howManyDOFs();
    void BracePanels();
    void GenerateCoeffMatrix();
    int howManySSs();

private:
    vector<Vertex> vertices; //Each QPoint represent a vertex
    vector<Crease> creases; //Each pair of QPoints represent a crease
    vector<Crease> forceCreases; //Stores the links used to calculate self-stress
    vector<vector<Crease> > loops; //Each inner vector stores the creases surrounding individual panels
    vector<vector<Crease> > vcreases; //Each inner vector stores the creases surounding invidual vertices
    vector<bool> picked_loops; //Stores the picked status of each panel (making it red in the gui).
    vector<vector<Crease> > deleted_loops; //Stores all the delted loops
    vector<vector<double> > coeffmatrix; //Stores the coefficients of the truss matrix "A"
};

#endif // CREASEPATTERN_H
