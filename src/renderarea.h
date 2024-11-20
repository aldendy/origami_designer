#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMouseEvent>
#include <creasepattern.h>
#include <QKeyEvent>
#include "window.h"

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);
    RenderArea(Window* parent);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void setPen(const QPen &pen, int which);
    void setBrush(const QBrush &brush, int which);
    void addPoint(double first, double second);
    void addCrease(int firstName, int secondName);
    void getLoops();
    int numberOfVertices();
    int numberOfEdges();
    int numberOfPanels();
    int numberOfDOFs();
    int numberOfSSs();
    void clearAll();
    void restorePanels();
    void getData(vector<QString> &elements);
    void loadData(vector<QString> elements);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QPen blackPen, redPen; //Stores the black and red pens, respectively, used to indicate selection of creases
    QBrush brush1, brush2; //Stores the red and green brushes used to show selection of vertices.
    CreasePattern pattern; //Contains all the data specific to the crease pattern design
    double scale; //scale > 1.0 when zoomed in. Changes the size of the object
                  //while keeping the upper left corner stationary.
    QPoint zoomOffset; //is a vector from the top-left corner of the window
                       //to the top-left corner of the drawing window. Measured in non-scaled units.
    int point_radius; //Sets initial relative proportions of different objects.
    int last_grid_size; //Stores the last grid size for resizing purposes
    QPixmap origin; //Contains the image used to draw the grid origin
    Window* topWindow; //Stores a pointer to the 'window' object to provide access to certain functions

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent * event);
    //void wheelEvent(QWheelEvent *event);
};

#endif // RENDERAREA_H
