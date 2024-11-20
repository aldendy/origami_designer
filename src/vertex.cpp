#include "vertex.h"

Vertex::Vertex()
{
    m_x = 0.0; //Sets the default configuration of the vertex
    m_y = 0.0;
    vName = 0;
    picked = false;
}

Vertex::Vertex(double x, double y, int name)
{
    m_x = x; //Sets the position of the vertex
    m_y = y;
    vName = name; //Sets the number uniquely identifying each drawn vertex
    picked = false; //The default picked status is 'false'.
}

void Vertex::SetX(double x)
{
    m_x = x;
}

void Vertex::SetY(double y)
{
    m_y = y;
}

void Vertex::SetName(int name)
{
    vName = name;
}

double Vertex::GetX()
{
    return m_x;
}

double Vertex::GetY()
{
    return m_y;
}

int Vertex::GetName()
{
    return vName;
}

//Allows calling functions to toggle the picked status of the vertex
void Vertex::TogglePick()
{
    if(picked)
        picked = false;
    else
        picked = true;
}

bool Vertex::isPicked()
{
    return picked;
}

