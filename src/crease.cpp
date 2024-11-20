#include "crease.h"

Crease::Crease()
{
    picked = false;
    cut = false;
}

Crease::Crease(Vertex m, Vertex n, int name)
{
    a = m; //Sets the starting vertex
    b = n; //Sets the end vertex. All creases are also treated like vectors pointing from start to end.
    m_Name = name; //An number uniquely identifying each crease drawn
    picked = false; //Sets the picked status to be 'false' by default
    cut = false; //Sets the 'cut' status to be 'false' by default
}

void Crease::SetStart(Vertex m)
{
    a = m;
}

void Crease::SetEnd(Vertex m)
{
    b = m;
}

Vertex Crease::GetStart()
{
    return a;
}

Vertex Crease::GetEnd()
{
    return b;
}

int Crease::GetStartName()
{
    return a.GetName();
}

int Crease::GetEndName()
{
    return b.GetName();
}

int Crease::GetName()
{
    return m_Name;
}

double Crease::GetCosine()
{
    double dy = b.GetY() - a.GetY();
    double dx = b.GetX() - a.GetX();
    double cosine = dx/pow(dx*dx + dy*dy,0.5);
    return cosine;
}

double Crease::GetSine()
{
    double dy = b.GetY() - a.GetY();
    double dx = b.GetX() - a.GetX();
    double sine = dy/pow(dx*dx + dy*dy,0.5);
    return sine;
}

//Allows calling functions to switch the picked status of a crease
void Crease::TogglePick()
{
    if(picked)
        picked = false;
    else
        picked = true;
}

void Crease::ToggleCut()
{
    if(cut)
        cut = false;
    else
        cut = true;
}

bool Crease::isPicked()
{
    return picked;
}

bool Crease::isCut()
{
    return cut;
}

