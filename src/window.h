#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMenuBar>
#include <QMainWindow>
#include <fstream>

class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QMenu;
class RenderArea;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

public slots:
    // Display a point or vertex in the render area. Update the calculated results.
    void plotPoint();

    // Display a crease between two vertices in the render area
    void plotCrease();

    // Update the calculated results like number of vertices
    void refreshTotals();

    // Clear the render area of all geometry
    void clearSlate();

    // Save the plotted pattern in a text file
    void saveFile();

    // Open a saved text file containing pattern geometry
    void openFile();

    // Toggle a private boolean controlling the plotting of vertex labels
    void toggleNumberPlot();

    // Toggle a private boolean controlling the plotting of vertices
    void togglePointPlot();

    // Toggle a private boolean controlling the plotting of creases
    void toggleCreasePlot();

    // Toggle a private boolean controlling the plotting of cut (dotted) creases
    void toggleCreaseCut();

    // Returns if number labels should be plotted
    bool isNumberPlot();

    // Returns if points or vertices should be plotted.
    bool isPointPlot();

    // Returns if creases should be plotted.
    bool isCreasePlot();

    // Returns if dotted creases (cut creases) should be plotted.
    bool isCreaseCut();

    // Restores deleted panels
    void restorePanels();

    // Opens a popup window providing information about the program.
    void about();

private:
    //The object used to manage and populate the origami creation window
    RenderArea *renderArea;

    //The labels of the different input boxes
    QLabel *xBoxLabel, *yBoxLabel, *aBoxLabel, *bBoxLabel;

    //The labels of the output boxes
    QLabel *vertexLabel, *edgeLabel, *panelLabel, *DOFLabel, *SSLabel;

    //The input boxes
    QLineEdit *xBox, *yBox, *aBox, *bBox;

    //The output boxes
    QLineEdit *vertexBox, *edgeBox, *panelBox, *DOFBox, *SSBox;

    //The menu buttons inserted into the menu bar
    QMenu *fileMenu, *editMenu, *helpMenu;

    //The menubar containing the menu items
    QMenuBar *menuBar;

    //Button objects that might be used
    QPushButton *clearButton, *pointButton, *creaseButton;

    /*Radio buttons used to select the input mode. Number selection relys on typed inputs.
    'Points_radio' lets a user click to create vertices. 'Creases_radio' allows the user
    to click to create creases.*/
    QRadioButton *numbers_radio, *points_radio, *creases_radio, *cut_radio;

    //Contains the actions stored in the menu bar items
    QAction *newAct, *saveAct, *openAct;

    //Restores all deleted panels
    QAction *restorePanelsAct;

    //More items stored in the menubar items
    QAction *aboutAct;

    //Stores a filler widget used to allow the other widgets in the application to resize correctly
    QWidget *filler;

    //Contains the directory information of files being opened or saved
    QString directory;

    //The 'output' file used to store information
    std::ofstream output;

    //The 'input' file used to read saved information
    std::ifstream input;

    //Flags used to tell the program which radio buttons are selected
    bool plot_numbers, plot_points, plot_creases, cut_creases;
};

#endif // WINDOW_H
