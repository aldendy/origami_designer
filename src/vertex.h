#ifndef POINT_H
#define POINT_H


class Vertex
{
public:
    Vertex();
    Vertex(double x, double y, int name);
    void SetX(double x);
    void SetY(double y);
    void SetName(int name);
    double GetX();
    double GetY();
    int GetName();
    void TogglePick();
    bool isPicked();

private:
    double m_x, m_y; //Contains the x- and y-coordinates of the vertex (measured from bottom left up and right).
    int vName; //Stores the vertex name used to uniquely identify each plotted vertex
    bool picked; //Holds the vertex's picked status (green or red in the gui)
};

#endif // POINT_H
