#ifndef CREASE_H
#define CREASE_H

#include <vertex.h>
#include <vector>
#include <cmath>

class Crease
{
public:
    Crease();
    Crease(Vertex m, Vertex n, int name);
    void SetStart(Vertex m);
    void SetEnd(Vertex m);
    Vertex GetStart();
    Vertex GetEnd();
    int GetStartName();
    int GetEndName();
    int GetName();
    double GetCosine();
    double GetSine();
    void TogglePick();
    void ToggleCut();
    bool isPicked();
    bool isCut();

private:
    int m_Name; //Stores the vertex name used to uniquely identify each crease instantiation
    Vertex a, b; //Stores the vertices defining the endpoints of the crease
    bool picked; //Determines whether or not the crease is selected in the gui (making it red).
    bool cut;
};

#endif // CREASE_H
