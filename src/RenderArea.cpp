#include "renderarea.h"
#include <QPainter>
#include <cmath>

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    scale = 1.0;
    zoomOffset.setX(0);
    zoomOffset.setY(0);
    point_radius = 5;
    origin.load("C:/Users/Alden/Desktop/School/ME 570/PainterPrototype/origin.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::ClickFocus);
}

RenderArea::RenderArea(Window *parent)
{
    scale = 1.0;
    zoomOffset.setX(10); //'zoomOffset' initialized to this value to allow the origin to be seen
    zoomOffset.setY(-10);
    point_radius = 5;
    origin.load("C:/Users/Alden/Desktop/School/ME 570/PainterPrototype/origin.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::ClickFocus);
    topWindow = parent;
    last_grid_size = 50;
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(400, 200);
}

//Sets the pen choice where 'which' determines whether 'pen1' or 'pen2' is set
void RenderArea::setPen(const QPen &pen, int which)
{
    if(which == 1)
        this->blackPen = pen;
    else if(which == 2)
        this->redPen = pen;
    update();
}

void RenderArea::setBrush(const QBrush &brush, int which)
{
    if(which == 1)
        this->brush1 = brush;
    else if(which == 2)
        this->brush2 = brush;
    update();
}

void RenderArea::addPoint(double first, double second)
{
    pattern.AddVertex(first, second); //The duplication logic occurs inside this function
    topWindow->refreshTotals();
    update();
}

void RenderArea::addCrease(int firstName, int secondName)
{
    vector<Vertex> verts;
    vector<Crease> crs;
    pattern.GetVertices(verts);
    pattern.GetCreases(crs);
    int order1(0), order2(0); //Stores the location of the desired vertices
    bool first_Exists(false), second_Exists(false), duplicate(false); //Whether or not they exist

    for(int i = 0; i < (int)verts.size(); i++) //Does the first vertex exist already?
    {
        if(verts[i].GetName() == firstName) order1 = i;
        first_Exists = true;
    }
    for(int i = 0; i < (int)verts.size(); i++) //Does the second vertex exist already?
    {
        if(verts[i].GetName() == secondName) order2 = i;
        second_Exists = true;
    }
    for(int i = 0; i < (int)crs.size(); i++) //Does the crease already exist, backwards or forwards?
    {
        //Creates the crease from the vertices if they exist
        if((crs[i].GetStartName() == firstName && crs[i].GetEndName() == secondName) ||
                (crs[i].GetStartName() == secondName && crs[i].GetEndName() == firstName))
            duplicate = true;
    }

    if((first_Exists && second_Exists) && (firstName != secondName) && !duplicate) //If not, add
        pattern.AddCrease(verts[order1], verts[order2]);

    pattern.ClearLoops();
    pattern.FindLoops(); //Find loops again in case something weird happened
    topWindow->refreshTotals();
    update();
}

void RenderArea::getLoops()
{
    pattern.FindLoops();
    update();
}

int RenderArea::numberOfVertices()
{
    vector<Vertex> v;
    pattern.GetVertices(v);
    int number = v.size();
    return number;
}

int RenderArea::numberOfEdges()
{
    vector<Crease> c;
    pattern.GetCreases(c);
    return c.size();
}

int RenderArea::numberOfPanels()
{
    vector<vector<Crease> > l;
    pattern.GetLoops(l);
    return l.size();
}

int RenderArea::numberOfDOFs()
{
    return pattern.howManyDOFs();
}

int RenderArea::numberOfSSs()
{
    return pattern.howManySSs();
}

void RenderArea::clearAll()
{
    pattern.clearEverything();
    scale = 1.0;
    zoomOffset.setX(10);
    zoomOffset.setY(-10);
}

void RenderArea::restorePanels()
{
    pattern.ClearDeletedLoops();
    pattern.ClearLoops();
    pattern.FindLoops();
}

//Gets all the data stored in the crease pattern for saving purposes
void RenderArea::getData(vector<QString> &elements)
{
    vector<Crease> crs;
    vector<Vertex> vrts;
    vector<vector<Crease> > lps;
    pattern.GetCreases(crs);
    pattern.GetVertices(vrts);
    pattern.GetDeletedLoops(lps);

    for(int i = 0; i < (int)vrts.size(); i++) //Gets all the vertex data
    {
        QString val;
        elements.push_back(val.setNum(vrts[i].GetName()));
        elements.push_back(val.setNum(vrts[i].GetX()));
        elements.push_back(val.setNum(vrts[i].GetY()));
    }

    elements.push_back(tr("end")); //This separates the vertex from the crease data

    for(int i = 0; i < (int)crs.size(); i++) //Gets all the crease data
    {
        QString val;
        elements.push_back(val.setNum(crs[i].GetName()));
        elements.push_back(val.setNum(crs[i].GetStartName()));
        elements.push_back(val.setNum(crs[i].GetEndName()));
        elements.push_back(val.setNum(crs[i].isCut()));
    }

    elements.push_back(tr("end")); //This separates the crease data from the deleted
                                   //loops data
    for(int i = 0; i < (int)lps.size(); i++)
    {
        for(int j = 0; j < (int)lps[i].size(); j++)
        {
            QString val;
            elements.push_back(val.setNum(lps[i][j].GetStartName()));
            elements.push_back(val.setNum(lps[i][j].GetEndName()));
            elements.push_back(val.setNum(lps[i][j].GetName()));
        }
        elements.push_back(tr("end"));
    }
}

//Takes a vector of String data and converts it to crease pattern data in the program
void RenderArea::loadData(vector<QString> elements)
{
    vector<QString> vs;
    vector<QString> cs;
    vector<vector<QString> > lps;
    int i = 0;
    while(i < (int)elements.size() && elements[i] != tr("end")) //Adds vertex data up to the separator
    {
        vs.push_back(elements[i]);
        i++;
    }

    i++; //Skips the separator
    while(i < (int)elements.size() && elements[i] != tr("end")) //Adds crease data up to the end
    {
        cs.push_back(elements[i]);
        i++;
    }

    i++; //Skips the second separator
    while(i < (int)elements.size())
    {
        vector<QString> lp;
        while(elements[i] != tr("end"))
        {
            lp.push_back(elements[i]);
            i++;
        }
        lps.push_back(lp);
        i++;
    }

    for(int i = 0; i < (int)vs.size(); i = i + 3)
        pattern.AddVertex(vs[i+1].toDouble(), vs[i+2].toDouble(),
                vs[i].toInt());
    for(int i = 0; i < (int)cs.size(); i = i + 4)
    {
        pattern.AddCrease(cs[i+1].toInt(), cs[i+2].toInt(), cs[i].toInt());
        if(cs[i+3].toInt())
            pattern.ToggleCreaseCut((int)(i/4));
    }
    pattern.AddDeletedLoops(lps);

    pattern.ClearLoops();
    pattern.FindLoops();
    topWindow->refreshTotals();
    update();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    //The procedure used to draw zoom items takes place in two different unit scales:
    //Viewing Window(VW) and Scales Viewing Window (SVW). VW units are constant in size
    //and only depend on the size of the viwing window in the application. SVW units are
    //smaller or bigger depending on the 'scale'. The drawing procedure is to 1) translate
    //the upper-left corner with vector 'zoomOffset' and then scale the drawing window using
    // 'scale'. The procedures are reversed to prepare for the next cycle of drawing.
    //All calculations have to be done so that the origin is in the lower left of the
    //viewing in the correct screen location.
    painter.translate(zoomOffset);
    painter.scale(scale,scale);

    //Sets the green gradient for the vertices
    QLinearGradient greenGradient(-10/scale, -10/scale, 10/scale, 10/scale);
    greenGradient.setColorAt(0.0, Qt::white);
    greenGradient.setColorAt(0.2, Qt::green);
    greenGradient.setColorAt(1.0, Qt::black);
    setBrush(greenGradient, 1);

    //Sets the red gradient for the verties
    QLinearGradient redGradient(-10/scale, -10/scale, 10/scale, 10/scale);
    redGradient.setColorAt(0.0, Qt::white);
    redGradient.setColorAt(0.2, Qt::red);
    redGradient.setColorAt(1.0, Qt::black);
    setBrush(redGradient, 2);

    setPen(QPen(Qt::black, 1/scale, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin), 1);
    setPen(QPen(Qt::red, 2/scale, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin), 2);

    vector<Vertex> p;
    vector<Crease> c;
    vector<vector<Crease> > lp;

    pattern.GetVertices(p);
    pattern.GetCreases(c);
    pattern.GetLoops(lp);

    //This section resizes the grid and origin as zooming occurs
    int grid = last_grid_size;
    if(width()/(scale*grid) > 10)
        last_grid_size = last_grid_size*2;
    else if(width()/(scale*grid) < 5)
        last_grid_size = last_grid_size/2;

    QPoint square[4] = {QPoint(0,0), QPoint(grid,0), QPoint(grid,grid), QPoint(0,grid)};

    //This section draws the grid pattern accross the entire visible screen
    painter.setPen(QPen(QColor(0, 20, 255, 100), 0.5/scale, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    for(int i = grid*(int)(-zoomOffset.x()/(grid*scale))-grid;
        i < width()/scale - zoomOffset.x()/scale; i+=grid)
    {
        int bottom = -grid*(int)((height() - (zoomOffset.y() + height()*scale))/(scale*grid));
        int top = (height()*scale + zoomOffset.y())/scale + grid;
        for(int j = bottom; j < top; j+=grid)
        {
            painter.translate(QPoint(i, height() - j));
            painter.drawPolygon(square, 4);
            painter.translate(-QPoint(i, height() - j));
        }
    }

    //This code draws the origin
    painter.drawPixmap(-0.5*grid, -0.5*grid + height(),
                       grid, grid, origin);

    if(lp.size() > 0) //This section draws all the loops
    {
        for(int i = 0; i < (int)lp.size(); i++)
        {
            int num_sides = (int)lp[i].size();
            QPoint points[num_sides];
            for(int j = 0; j < num_sides; j++)
            {
                points[j] = QPoint(lp[i][j].GetStart().GetX(),
                                   height() - lp[i][j].GetStart().GetY());
            }
            if(pattern.IsLoopPicked(i))
                painter.setBrush(Qt::red);
            else
                painter.setBrush(Qt::green);
            painter.drawPolygon(points, num_sides);
        }
    }

    if(c.size() > 0) //Here all the creases are drawn
    {
        blackPen.setWidth(2/scale);
        redPen.setWidth(3/scale);
        for(int i = 0; i < (int)c.size(); i++)
        {
            QPoint first((int)(c[i].GetStart()).GetX(), height() - (int)(c[i].GetStart()).GetY());
            QPoint second((int)(c[i].GetEnd()).GetX(), height() - (int)(c[i].GetEnd()).GetY());

            if(c[i].isCut())
            {
                redPen.setStyle(Qt::DashLine);
                blackPen.setStyle(Qt::DashLine);
            }
            if(c[i].isPicked())
            {
                painter.setPen(redPen);
                painter.drawLine(first, second);
            }
            else
            {
                painter.setPen(blackPen);
                painter.drawLine(first, second);
            }
            blackPen.setStyle(Qt::SolidLine);
            redPen.setStyle(Qt::SolidLine);
        }
    }

    if(p.size() > 0) //Here all the vertices are drawn
    {
        blackPen.setWidth(1/scale);
        redPen.setWidth(2/scale);
        painter.setPen(blackPen);
        for(int i = 0; i < (int)p.size(); i++)
        {
            QString a;
            a.setNum(p[i].GetName());
            painter.translate(QPoint((int)p[i].GetX(), height() - (int)p[i].GetY()));

            if(p[i].isPicked())
            {
                painter.setBrush(brush2);
                painter.drawEllipse(-point_radius/scale, -point_radius/scale,
                                    2*point_radius/scale, 2*point_radius/scale);
            }
            else
            {
                painter.setBrush(brush1);
                painter.drawEllipse(-point_radius/scale, -point_radius/scale,
                                    2*point_radius/scale, 2*point_radius/scale);
            }
            painter.setFont(QFont("Arial", 12/scale));
            painter.drawText(2*point_radius/scale, 3*point_radius/scale, a);
            painter.translate(QPoint(-(int)p[i].GetX(), -height() + (int)p[i].GetY()));
        }
    }

    painter.scale(1/scale,1/scale);
    painter.translate(QPoint(-zoomOffset.x(), -zoomOffset.y()));

    painter.setFont(QFont("Arial", 10));
    QString wide;
    painter.drawText(QRect(width() - 55, height() - 20, 50, 20), Qt::AlignRight,
                     wide.setNum((width() - zoomOffset.x())/scale, 'g', 3));
    painter.drawText(QRect(5, 5, 55, 20), Qt::AlignLeft,
                     wide.setNum((zoomOffset.y() + height()*scale)/scale, 'g', 3));

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}


void RenderArea::mousePressEvent(QMouseEvent *event)
{
    vector<Vertex> v;
    vector<Crease> c;
    vector<vector<Crease> > lps;
    pattern.GetVertices(v);
    pattern.GetCreases(c);
    pattern.GetLoops(lps);
    bool is_a_pick = true, was_v_toggle = false, was_c_toggle(false);

    if (event->button() == Qt::LeftButton) //If a selection, then toggle the picked stati appropriately
    {
        QPoint p = event->pos();

        for(int i = 0; i < (int)v.size(); i++) //Toggles the vertex if it was selected
        {
            double x = (p.x()-zoomOffset.x())/scale; //The correct transformations are applied
            double y = (p.y()-zoomOffset.y())/scale;
            //Finds the distance between the click and the stored point
            double distance = sqrt((x - v[i].GetX())*(x - v[i].GetX()) +
                                   (y - height() + v[i].GetY())*(y - height() + v[i].GetY()));
            if(distance < 2*point_radius/scale && !topWindow->isPointPlot() && !topWindow->isCreaseCut())
            {
                pattern.ToggleVertex(i);
                is_a_pick = !v[i].isPicked();
                was_v_toggle = true;
            }
        }

        for(int i = 0; i < (int)c.size(); i++)
        {
            //The distance between a point and a line determined by two points is given by a
            //formula using the following points transformed into the same coordinate system.
            double x0 = (p.x()-zoomOffset.x())/scale;
            double y0 = (p.y()-zoomOffset.y())/scale;
            double x1 = (c[i].GetStart()).GetX();
            double y1 = height() - (c[i].GetStart()).GetY();
            double x2 = (c[i].GetEnd()).GetX();
            double y2 = height() - (c[i].GetEnd()).GetY();
            vector<Crease> d;
            pattern.GetCreases(d);
            bool bounded_in_x(false), bounded_in_y(false);

            //These if-statments ensure that colinear creases are not selected.
            if((x0 < fmax(x1,x2)+point_radius/scale) && (x0 > fmin(x1,x2)-point_radius/scale))
                bounded_in_x = true;
            if((y0 < fmax(y1,y2)+point_radius/scale) && (y0 > fmin(y1,y2)-point_radius/scale))
                bounded_in_y = true;

            //This formula calculates the distance between a point and the crease in question
            double distance = abs((x2 - x1)*(y1 - y0) - (x1 - x0)*(y2 - y1))/
                    sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

            if((distance < 2*point_radius/scale) && bounded_in_x && bounded_in_y &&
                    !topWindow->isCreasePlot())
            {
                if(!topWindow->isCreaseCut())
                {
                    if(!was_v_toggle) //This section ensures that weird unselect-select problems don't ripple
                    {                 //through the pattern if an internal crease or vertex and crease are selected.
                        pattern.ToggleCrease(i);
                        was_c_toggle = true;
                    }
                    else if(is_a_pick)
                    {
                        if(!c[i].isPicked())
                        {
                            pattern.ToggleCrease(i);
                            was_c_toggle = true;
                        }
                    }
                    else
                    {
                        if(c[i].isPicked())
                        {
                            pattern.ToggleCrease(i);
                            was_c_toggle = true;
                        }
                    }
                }
                else
                {
                    pattern.ToggleCreaseCut(i);
                }
            }
        }

        //Determines which loops are picked.
        for(int i = 0; i < (int)lps.size(); i++)
        {
            double angle(0);
            double x = (p.x()-zoomOffset.x())/scale; //Finds click coordinates
            double y = (height()*scale - p.y()+zoomOffset.y())/scale;
            Vertex picked_p(x, y, 0); //Creates temporary vertex at the click
            for(int j = 0; j < (int)lps[i].size(); j++)
            {
                Crease a(lps[i][j].GetStart(), picked_p,0);
                Crease b(picked_p, lps[i][j].GetEnd(),0);
                double delta = pattern.FindAngle(a, b); //Finds the ange created by the crease start, the click
                if(delta > 3.14159)                     //and the crease end.
                    delta = delta - 2*3.14159; //Transforms the (0, 2pi) angle to a (-pi, pi) angle
                angle = angle + delta; //Adds that angle to the other angles calculated for the polygon
            }

            //'delta' is the 'external angle' at each corner of the polygon. For any polygon, the sum
            //of the external angles is 2pi if you travel ccw around the shape (-2pi otherwise). We only
            //are interested in the ccw cases.
            if(angle > 1.9*3.14159 && angle < 2.1*3.14159)
            {
                if(!was_v_toggle && !was_c_toggle && topWindow->isNumberPlot())
                    pattern.ToggleLoop(i); //Adds just the ccw case
            }
        }

        //Adds a point if click-point addition is enabled
        if(topWindow->isPointPlot())
            addPoint((p.x()-zoomOffset.x())/scale,
                     (height()*scale-(p.y()-zoomOffset.y()))/scale);

        //Adds a crease if crease addition by clicking is enabled
        if(topWindow->isCreasePlot())
        {
            vector<Vertex> pairs;
            vector<int> locs;
            pattern.GetVertices(v);
            for(int i = 0; i < (int)v.size(); i++)
            {
                if(v[i].isPicked())
                {
                    pairs.push_back(v[i]); //Gathers all the picked vertices
                    locs.push_back(i);
                }
            }

            //As soon as there are two picked vertices, unpick them and create a crease
            if(pairs.size() == 2)
            {
                addCrease(pairs[0].GetName(), pairs[1].GetName());
                pattern.ToggleVertex(locs[0]);
                pattern.ToggleVertex(locs[1]);
            }
        }

        update();
    }
}

//This function deletes all selected vertices and creases
void RenderArea::keyPressEvent(QKeyEvent *event)
{
    vector<Vertex> v;
    vector<Crease> c;
    vector<vector<Crease> > lps;
    pattern.GetVertices(v);
    pattern.GetCreases(c);
    pattern.GetLoops(lps);

    if((event->key() == Qt::Key_Backspace) || (event->key() == Qt::Key_Delete))
    {
        for(int i = 0; i < (int)v.size(); i++)
        {
            if(v[i].isPicked() && !topWindow->isCreasePlot())
            {
                pattern.DeleteVertex(v[i].GetName());
            }
        }

        for(int i = 0; i < (int)c.size(); i++)
            if(c[i].isPicked()) pattern.DeleteCrease(c[i].GetName());

        for(int i = 0; i < (int)lps.size(); i++)
            if(pattern.IsLoopPicked(i)) pattern.DeleteLoops(lps[i]);
    }
    pattern.ClearLoops();
    pattern.FindLoops();
    topWindow->refreshTotals();
    update();
}

/*//This function handles scaling
void RenderArea::wheelEvent(QWheelEvent * event)
{
    double diff = event->angleDelta().y()/1440.0; //1440 is an arbitrary scaling factor to dampen zooming
    //if(abs(diff) > 0.05)
      //  diff = 0.05*diff/abs(diff);
    QPoint delta = -diff/scale*(event->position() - zoomOffset);
    zoomOffset = zoomOffset + delta;
    scale = scale + diff;
    if(scale < 0.1) //0.1 is an arbitary maximum zoom-out value
        scale = 0.1;
    update();
}*/
