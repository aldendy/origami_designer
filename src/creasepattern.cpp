#include "creasepattern.h"

CreasePattern::CreasePattern()
{

}

//Any vertex added will have a name 'one' greater than the highest current vertex name.
//Deletions will not affect the other vertex names. A numbered vertex is simply lost
//and the name is not reused unless the deleted vertex was the last.
void CreasePattern::AddVertex(double x, double y)
{
    bool duplicate = false;
    for(int i = 0; i < (int)vertices.size(); i++) //This code flags any duplicate vertices
    {
        if(vertices[i].GetX() == x && vertices[i].GetY() == y)
            duplicate = true;
    }
    if(vertices.size() > 0 && !duplicate) //If there are many vertices
        vertices.push_back(Vertex(x, y, vertices[vertices.size()-1].GetName() + 1));
    else if(!duplicate) //If there are no vertices
        vertices.push_back(Vertex(x, y, 0));
}

//This function is used when a saved file is being opened. It gives the calling fucntion control
//over the naming of the vertices to ensure that the opened file has the same naming as the original.
void CreasePattern::AddVertex(double x, double y, int name)
{
    vertices.push_back(Vertex(x, y, name));
}

//Same naming convention will be followed between vertices and creases.
void CreasePattern::AddCrease(Vertex a, Vertex b)
{
    if(creases.size() > 0) //If there are many creases
        creases.push_back(Crease(a, b, creases[creases.size()-1].GetName() + 1));
    else //If this crease is the first to be added
        creases.push_back(Crease(a, b, 0));
}

//This function adds braces to the set of trusses in 'vcreases', not to the pattern
void CreasePattern::AddBrace(Vertex a, Vertex b)
{
    if(forceCreases.size() > 0) //If there are many creases
        forceCreases.push_back(Crease(a, b, forceCreases[forceCreases.size()-1].GetName() + 1));
    else //If this crease is the first to be added
        forceCreases.push_back(Crease(a, b, 0));
}

//This alternate 'addcrease' gives the calling function control over naming for file-open purposes.
void CreasePattern::AddCrease(int a, int b, int name)
{
    Vertex first, second;

    for(int i = 0; i < (int)vertices.size(); i++)
        if(vertices[i].GetName() == a) first = vertices[i];
    for(int i = 0; i < (int)vertices.size(); i++)
        if(vertices[i].GetName() == b) second = vertices[i];

    creases.push_back(Crease(first, second, name));
}

void CreasePattern::AddDeletedLoops(vector<vector<QString> > lps)
{
    for(int i = 0; i < (int)lps.size(); i++)
    {
        vector<Crease> lp;
        for(int j = 0; j < (int)lps[i].size(); j = j + 3)
        {
            Vertex a, b;
            for(int k = 0; k < (int)vertices.size(); k++)
                if(vertices[k].GetName() == lps[i][j].toInt()) a = vertices[k];
            for(int k = 0; k < (int)vertices.size(); k++)
                if(vertices[k].GetName() == lps[i][j+1].toInt()) b = vertices[k];
            lp.push_back(Crease(a, b, lps[i][j+2].toInt()));
        }
        deleted_loops.push_back(lp);
    }
}

//Deleting vertices will not affect the naming of other vertices. The deletion takes place based on name.
void CreasePattern::DeleteVertex(int name)
{
    int order(0); //Stores the position of the vertex to be deleted
    for(int i = 0; i < (int)vertices.size(); i++)
        if(vertices[i].GetName() == name) order = i;
    vertices.erase(vertices.begin() + order);
}

//Deletes a crease with the given unique 'name'. Like the first delete function, it assumes that the
//named vertex exists.
void CreasePattern::DeleteCrease(int name)
{
    int order(0);
    for(int i = 0; i < (int)creases.size(); i++)
        if(creases[i].GetName() == name) order = i;
    creases.erase(creases.begin() + order);
}

void CreasePattern::DeleteLoops(vector<Crease> candidate)
{
    deleted_loops.push_back(candidate);
}

//Clears all the pattern data stored, wiping the slate
void CreasePattern::clearEverything()
{
    vector<Crease> other;
    vector<Vertex> other2;
    creases.swap(other);
    vertices.swap(other2);
    ClearLoops();
}

//Returns a vector of creases 'c' where the creases point from the vertex outward, sometimes
//changing the direction the creases originally pointed.
void CreasePattern::FindVertexCreases(Vertex vertex, vector<Crease> &c)
{
    for(int i = 0; i < (int)creases.size(); i++)
    {
        if((creases[i].GetStartName() == vertex.GetName()) ||
                (creases[i].GetEndName() == vertex.GetName()))
            c.push_back(creases[i]); //Adds creases sharing the common vertex
    }

    for(int i = 0; i < (int)c.size(); i++)
    {
        if(c[i].GetStartName() != vertex.GetName()) //Switches the crease direction if they point inward
        {
            Vertex q = c[i].GetStart();
            Vertex r = c[i].GetEnd();
            c[i].SetStart(r);
            c[i].SetEnd(q);
        }
    }
}

//Similar to 'FindVertexCreases' this function returns a vector of force links. These
//are used in the calculation of self-stresses in the mechanism.
void CreasePattern::FindVertexLinks(Vertex vertex, vector<Crease> &c)
{
    for(int i = 0; i < (int)forceCreases.size(); i++)
    {
        if((forceCreases[i].GetStartName() == vertex.GetName()) ||
                (forceCreases[i].GetEndName() == vertex.GetName()))
            c.push_back(forceCreases[i]); //Adds creases sharing the common vertex
    }

    for(int i = 0; i < (int)c.size(); i++)
    {
        if(c[i].GetStartName() != vertex.GetName()) //Switches the crease direction if they point inward
        {
            Vertex q = c[i].GetStart();
            Vertex r = c[i].GetEnd();
            c[i].SetStart(r);
            c[i].SetEnd(q);
        } //the result is that all the creases returned all share the same start vertex
    }
}

//Returns the name of the left-most crease. The procedure is that, given a crease, find the end.
//Get all the creases attached to the end. Calculate the angles between the first given crease and
//all the other crease. The angle is calculated measuring ccw (0 to 2 pi). The largest angle gives
//the left-most crease. Returns '-1' if there is only one crease and vertex, meaning the loop is broken.
int CreasePattern::RightOrLeft(Crease start, bool left, double &angle)
{
    double best_value; //Largest angle

    if(left) best_value = 0.0; //The variable 'left' sets whether or not the function looks for the
    else best_value = 2*3.14159; //right-most or the left-most crease. This sets the right starting value.

    int best_value_loc = 0;
    vector<Crease> v_Creases; //Stores all the candidate creases
    FindVertexCreases(start.GetEnd(), v_Creases); //The starting location is the crease 'start' endpoint.
    for(int i = 0; i < (int)v_Creases.size(); i++)
    {
        double value = FindAngle(start, v_Creases[i]); //Start is inserted pointing the wrong way, toward
        if(left)                                       //the common vertex instead of away.
        {
            if(value > best_value)
            {
                best_value = value;
                best_value_loc = i;
                angle = value - 3.14159;
            }
        }
        else
        {
            if((value < best_value) && (value > 0))
            {
                best_value = value;
                best_value_loc = i;
                angle = value - 3.14159;
            }
        }
    }

    if(best_value > 0.0) //If there is more than one non-coincident creases
        return v_Creases[best_value_loc].GetName(); //There will always be at least one.
    else
        return -1;
}

//Returns the angle between two crease vectors in Radians.
//The vectors are assumed to both point away from the vertex that they both share.
//Vector a is the starting crease and the angle is measured counter-clockwise
//when viewed from positive z, ccw.
double CreasePattern::FindAngle(Crease a, Crease b)
{
    double q = -(a.GetEnd().GetX() - a.GetStart().GetX()); //The vector 'a' is inverted because it is inserted
    double r = -(a.GetEnd().GetY() - a.GetStart().GetY()); //backwards by the calling function.
    double s = b.GetEnd().GetX() - b.GetStart().GetX();
    double t = b.GetEnd().GetY() - b.GetStart().GetY();
    double sign = (q*t - r*s)/abs(q*t - r*s); //Cross(a, b) or a x b
    if(sign < 0) //This is done to account for situations where the returned angle would be greater than pi.
        return 2*3.14159 - acos((q*s + r*t)/(sqrt(q*q + r*r)*sqrt(s*s + t*t)));
    else
        return acos((q*s + r*t)/(sqrt(q*q + r*r)*sqrt(s*s + t*t)));
}

//This function returns an array of creases, 'path', that contains all the creases outlining a closed loop.
//The procedure is that, given a beginning crease 'start', find the left-most crease at its end point.
//If this exists, make sure that this new crease points away from the first. Switch it if necessary.
//Find the left-most crease of this next crease and so on until a complete, non-intersecting loop is formed.
//If any tests are failed, return nothing. The variable 'left' determines whether or not the left or right
//wall is found. Here it is set to 'left' by default. The right-handed capability is not needed.
void CreasePattern::FindWall(Crease start, vector<Crease> &path, bool left)
{
    bool is_good(true), is_closed(false); //'is_good' is changed if the loop is self-intersects.
                                          //'is_closed is changed if the loop is not closed
    double ext_angle(0), total_angle(0); //'ext_angle' is the external angle (polygons) between
                                         //two side creases. 'total_angle' stores the sum of the
                                         //external angles.
    Crease current = start; //'current' holds the crease used to find the next element of the wall.
    //if(LoopsContain(current))
      //  is_good = false;
    vector<Crease> set; //Holds the potential set of wall or loop creases

    while(is_good && !is_closed) //As long as nothing is broken
    {
        if((RightOrLeft(current, left, ext_angle) == -1) ||
                (CreasesContain(set, current) &&
                 current.GetEndName() != start.GetEndName())) //If the crease dangles or self-intersects
        {
            is_good = false;
        }
        else if(((current.GetName() == start.GetName()) &&
                 (set.size() > 2))) //If it loops around and closes correctly
        {
            is_closed = true;
            if(total_angle > 3.14159*1.95) //For all polygons, the sum of the external angles is plus
                path = set;                //or minus 2 pi, depending on the direction around the shape.
            //This code was added to eliminate any clockwise loops formed from following the left wall
            //around a panel. This would only occur if the starting crease was on the outer edge of the
            //pattern. The result would be an extra panel covering the whole pattern. Except for this
            //eventuality, following the left wall would always allow you to correctly outline all the
            //panels in the crease pattern.
        }
        else
        {
            set.push_back(current); //If good, add the crease to the potential set
            int nombre = RightOrLeft(current, left, ext_angle); //Get the name of the next
            total_angle = total_angle + ext_angle; //Update the 'total_angle'

            for(int j = 0; j < (int)creases.size(); j++)
                if(creases[j].GetName() == nombre) current = creases[j];

            //This section ensures that the creases point in the same direction ccw aroudn the loop
            if(current.GetEndName() == set[set.size()-1].GetEndName())
            {
                current.SetEnd(current.GetStart());
                current.SetStart(set[set.size()-1].GetEnd());
            }
        }
    }
}

//This function finds candidate loops and adds them to 'loops' if they  are correct.
void CreasePattern::FindLoops()
{
    picked_loops.clear();

    for(int i = 0; i < (int)creases.size(); i++)
    {
        //bool are_equivalent(true);
        vector<Crease> pathA, pathB;

        FindWall(creases[i], pathA, true); //'pathA' is found normally
        FindWall(Crease(creases[i].GetEnd(), creases[i].GetStart(), creases[i].GetName()), pathB, true);
        //'pathB' is found by switching the crease direction. This can be important if the creases
        //surrounding a panel all point in the wrong direction.
        if(pathB.size() > 2) //If the alternate direction returned a valid loop, it will be pushed.
            pathA = pathB;

        if(!LoopExists(pathA, false) && !LoopExists(pathA, true)) //Checks to determine that the loop hasn't already been added
        {
            loops.push_back(pathA);
            picked_loops.push_back(false); //The default condition of a loop is 'not picked'
        }
    }
}

//This function checks if a input loop is already contained in the crease pattern.
//The procedure is to look for a specific crease in all the loops. When it is found,
//the function iterates through the specific loop. If it contains all the same creases
//as the candidate loop 'paths', then 'exists' is set to 'true' and returned.
bool CreasePattern::LoopExists(vector<Crease> path, bool is_deleted)
{
    vector<vector<Crease> > pool;
    if(is_deleted)
        pool = deleted_loops;
    else
        pool = loops;

    bool exists(false);
    if(pool.size() < 1 && path.size() > 0)
        return false;
    else if(path.size() < 1)
        return true;

    for(int i = 0; i < (int)pool.size(); i++)
    {
        for(int j = 0; j < (int)pool[i].size(); j++)
        {
            if(pool[i][j].GetName() == path[0].GetName())
            {
                int previous = pool[i][(j-1+pool[i].size())%pool[i].size()].GetName();
                if(previous == path[path.size()-1].GetName())
                {
                    //The only case considered is where both the possible and stored loop
                    //run in the same direction. Otherwise, the loops will never be identical.
                    for(int k = 0; k < (int)pool[i].size(); k++)
                    {
                        int next = pool[i][(k+j+pool[i].size())%pool[i].size()].GetName();
                        if(next != path[k].GetName())
                            exists = false;
                        else
                            exists = true;
                    }
                    if(exists)
                        return true;
                }
            }
        }
    }
    return false;
}

void CreasePattern::GetVertices(vector<Vertex> &v)
{
    v = vertices;
}

void CreasePattern::GetCreases(vector<Crease> &c)
{
    c = creases;
}

void CreasePattern::GetLoops(vector<vector<Crease> > &l)
{
    l = loops;
}

void CreasePattern::GetDeletedLoops(vector<vector<Crease> > &l)
{
    l = deleted_loops;
}

//This function finds the location of a crease in the force-crease set so that it can be correctly placed.
//The order of the crease in the set of 'forceCreases' determines its column order in the 'coeffmatrix'.
int CreasePattern::GetLocation(Crease c)
{
    for(int i = 0; i < (int)forceCreases.size(); i++)
    {
        if(forceCreases[i].GetName()== c.GetName())
            return i;
    }

    //If this code is reached, then the crease was not found and an error has occured.
    return 0;
}

void CreasePattern::ClearLoops()
{
    vector<vector< Crease> > other;
    loops.swap(other);
}

void CreasePattern::ClearDeletedLoops()
{
    deleted_loops.clear();
}

void CreasePattern::ToggleVertex(int location) //Toggles the picked nature of the vertex
{
    vertices[location].TogglePick();
    for(int i = 0; i < (int)creases.size(); i++)
    {
        if(creases[i].GetStart().GetName() == vertices[location].GetName())
            creases[i].SetStart(vertices[location]);
        else if(creases[i].GetEnd().GetName() == vertices[location].GetName())
            creases[i].SetEnd(vertices[location]);
    }
}

void CreasePattern::ToggleCrease(int location) //Toggles the picked nature of a crease
{
    creases[location].TogglePick();
}

void CreasePattern::ToggleCreaseCut(int location)
{
    creases[location].ToggleCut();
}

void CreasePattern::ToggleLoop(int location)
{
    if(picked_loops[location])
        picked_loops[location] = false;
    else
        picked_loops[location] = true;
}

bool CreasePattern::CreasesContain(vector<Crease> set, Crease possible) //Determines if 'set' contains 'possible'
{
    for(int i = 0; i < (int)set.size(); i++)
    {
        if(set[i].GetName() == possible.GetName())
            return true;
    }
    return false;
}

bool CreasePattern::LoopsContain(Crease possible) //Determines if 'loops' contains 'possible'
{
    for(int i = 0; i < (int)loops.size(); i++)
    {
        for(int j = 0; j < (int)loops[i].size(); j++)
        {
            if(loops[i][j].GetName() == possible.GetName())
                return true;
        }
    }
    return false;
}

bool CreasePattern::IsLoopPicked(int loc)
{
    if(picked_loops[loc])
        return true;
    else
        return false;
}

int CreasePattern::howManyDOFs() //Calculates and returns the number of degrees of freedom of the pattern
{
    int num_stiffeners = 0; // number of bars needed to rigidize a panel
    int dof = 0; // number of pattern degrees of freedom

    if(loops.size() > 0)
    {
        for(int i = 0; i < (int)loops.size(); i++)
        { // here, we add stiffeners for all the panels needing rigidity
            num_stiffeners = num_stiffeners + 2*loops[i].size() - 6;
        }
    }

    if((int)loops.size() > 0) //dof > 0 only if there are panels
        dof = 3*vertices.size() - (creases.size() + num_stiffeners) - 6;

    return dof;
}

//Here, we add all the braces to 'vcreases' needed to make all the panels rigid. This is done
//by connecting two adjacent vertices in the panel to every other vertex in the panel. This
//should result in a fully rigid panel (Mobility = 0).
void CreasePattern::BracePanels()
{
    //Clear the 'vcrease' array so that a new set of braces can be found
    vector<Crease> other = creases;
    forceCreases.swap(other);

    //Connect the first two vertices in each loop from 'loops' with all the other vertices
    for(int i = 0; i < (int)loops.size(); i++)
    {
        if((int)loops[i].size() > 3)
        {
            //Connect the first vertex
            for(int j = 2; j < ((int)loops[i].size() - 1); j++)
                AddBrace(loops[i][0].GetStart(), loops[i][j].GetStart());

            //Connect the second vertex
            for(int j = 3; j < (int)loops[i].size(); j++)
                AddBrace(loops[i][1].GetStart(), loops[i][j].GetStart());
        }
    }
}

//This function generates the coefficient matrix A 'coeffmatrix'. Here, it is initialized and
//then populated with the correct values so that self-stresses can be found.
void CreasePattern::GenerateCoeffMatrix()
{
    //This code clears the coefficient matrix
    vector<vector<double> > other2;
    coeffmatrix.swap(other2);

    //Here, we initialize 'coeffmatrix' with zeros to the correct size (2*vertices x num_force_creases)
    vector<double> zeros;
    for(int i = 0; i < (int)forceCreases.size(); i++)
        zeros.push_back(0);

    if((int)zeros.size() > 0)
    {
        for(int i = 0; i < (2*(int)vertices.size()); i++)
            coeffmatrix.push_back(zeros);

        //Next, we assign the correct values or bearings to the correct locations in the matrix
        for(int i = 0; i < (int)vcreases.size(); i++)
        {
            for(int j = 0; j < (int)vcreases[i].size(); j++)
            {
                int location = GetLocation(vcreases[i][j]);
                coeffmatrix[2*i][location] = vcreases[i][j].GetCosine();
                coeffmatrix[2*i + 1][location] = vcreases[i][j].GetSine();
            }
        }
    }
}

//This function calculates the number of self-stresses in the crease pattern. After the panels
//are fully braced, the self-stresses are calculated and returned.
int CreasePattern::howManySSs()
{
    //Here, all the panels are braced with criss-crossing links to give them zero DOFs
    BracePanels();

    //Here, we find all the creases at each vertex and group them by vertex, sorted by vertex
    vcreases.clear();

    for(int i = 0; i < (int)vertices.size(); i++)
    {
        vector<Crease> bunch;
        FindVertexLinks(vertices[i], bunch);
        vcreases.push_back(bunch);
    }

    //Next, we need the coefficient matrix "A" and the function "GetBearing". We construct "A" here
    //by initializing it with zeros to the correct size.
    GenerateCoeffMatrix();

    int numSS = 0; //Stores the number of self-stresses in the matrix
    int rank = 0; //Stores the rank of the coefficient matrix
    int maxRank = 0; //Stores the full-rank value

    if((int)coeffmatrix.size() > 0)
    {
        rank = rowreduce(&coeffmatrix);

        if((int)coeffmatrix.size() >= (int)coeffmatrix[0].size())
            maxRank = coeffmatrix[0].size();
        else
            maxRank = coeffmatrix.size();

        numSS = maxRank - rank;
    }

    return numSS;
}
