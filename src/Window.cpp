#include "renderarea.h"
#include "window.h"
#include <QtWidgets>
#include <string>

Window::Window()
{
    renderArea = new RenderArea(this);

    int box_width = 100; //Controls input and output box width

    //Create a text box for inputting the x-coordinate
    xBox = new QLineEdit("0");
    xBox->setAlignment(Qt::AlignRight);
    xBox->setMaxLength(5);
    xBox->setFixedWidth(box_width);

    // Select the font for the text boxes
    QFont font = xBox->font();
    font.setPointSize(font.pointSize() + 4);
    xBox->setFont(font); //Font size used throughout

    // Create a text box for the y-coordinate of the point
    yBox = new QLineEdit("0");
    yBox->setAlignment(Qt::AlignRight);
    yBox->setMaxLength(5);
    yBox->setFixedWidth(box_width);
    yBox->setFont(font);

    // Create a text box for the "A" point to connect with a crease
    aBox = new QLineEdit("0");
    aBox->setAlignment(Qt::AlignRight);
    aBox->setMaxLength(5);
    aBox->setFixedWidth(box_width);
    aBox->setFont(font);

    // Create a text box for the "B" point to connect with a crease
    bBox = new QLineEdit("0");
    bBox->setAlignment(Qt::AlignRight);
    bBox->setMaxLength(5);
    bBox->setFixedWidth(box_width);
    bBox->setFont(font);

    xBoxLabel = new QLabel(tr("x-coordinate:"));
    xBoxLabel->setBuddy(xBox);

    yBoxLabel = new QLabel(tr("&y-coordinate:"));
    yBoxLabel->setBuddy(yBox);

    aBoxLabel = new QLabel(tr("&Point A:"));
    aBoxLabel->setBuddy(yBox);

    bBoxLabel = new QLabel(tr("&Point B:"));
    bBoxLabel->setBuddy(yBox);

    clearButton = new QPushButton(tr("&Clear All"), this);
    clearButton->setFocusPolicy(Qt::NoFocus); //Prevents tabbing from selecting the button

    numbers_radio = new QRadioButton("Plot pnts with (x,y)", this);
    points_radio = new QRadioButton("Plot pnts with cursor", this);
    creases_radio = new QRadioButton("Plot creases", this);
    cut_radio = new QRadioButton("Split creases", this);
    numbers_radio->setFocusPolicy(Qt::NoFocus);
    points_radio->setFocusPolicy(Qt::NoFocus);
    creases_radio->setFocusPolicy(Qt::NoFocus);
    cut_radio->setFocusPolicy(Qt::NoFocus);
    numbers_radio->toggle();

    plot_numbers = true;
    plot_points = false; //Initializes the radio button flags to 'false'
    plot_creases = false;
    cut_creases = false;

    // Setup a box to display the number of vertices
    vertexBox = new QLineEdit("0");
    vertexBox->setAlignment(Qt::AlignRight);
    vertexBox->setReadOnly(true); //Prevents the user from altering the output box contents
    vertexBox->setFont(font);
    vertexBox->setFocusPolicy(Qt::NoFocus);
    vertexBox->setFixedWidth(box_width);

    vertexLabel = new QLabel(tr("Vertices:"));
    vertexLabel->setBuddy(vertexBox);

    // Setup a box to display the number of edges
    edgeBox = new QLineEdit("0");
    edgeBox->setAlignment(Qt::AlignRight);
    edgeBox->setReadOnly(true);
    edgeBox->setFont(font);
    edgeBox->setFocusPolicy(Qt::NoFocus);
    edgeBox->setFixedWidth(box_width);

    edgeLabel = new QLabel(tr("Edges:"));
    edgeLabel->setBuddy(edgeBox);

    // Setup a box to display the number of panels
    panelBox = new QLineEdit("0");
    panelBox->setAlignment(Qt::AlignRight);
    panelBox->setReadOnly(true);
    panelBox->setFont(font);
    panelBox->setFocusPolicy(Qt::NoFocus);
    panelBox->setFixedWidth(box_width);

    panelLabel = new QLabel(tr("Panels:"));
    panelLabel->setBuddy(panelBox);

    // Setup a box to display the number of panels
    DOFBox = new QLineEdit("0");
    DOFBox->setAlignment(Qt::AlignRight);
    DOFBox->setReadOnly(true);
    DOFBox->setFont(font);
    DOFBox->setFocusPolicy(Qt::NoFocus);
    DOFBox->setFixedWidth(box_width);

    DOFLabel = new QLabel(tr("Pattern DOF:"));
    DOFLabel->setBuddy(DOFBox);

    // Setup a box to display the number of panels
    SSBox = new QLineEdit("0");
    SSBox->setAlignment(Qt::AlignRight);
    SSBox->setReadOnly(true);
    SSBox->setFont(font);
    SSBox->setFocusPolicy(Qt::NoFocus);
    SSBox->setFixedWidth(box_width);

    SSLabel = new QLabel(tr("Self Stresses:"));
    SSLabel->setBuddy(SSBox);

    //Sets up the menu bar and its button and action contents
    menuBar = new QMenuBar(this);
    newAct = new QAction(tr("&New"), this);
    saveAct = new QAction(tr("&Save"), this);
    openAct = new QAction(tr("&Open"), this);
    restorePanelsAct = new QAction(tr("&Restore Panels"), this);
    aboutAct = new QAction(tr("&About"), this);
    connect(newAct, SIGNAL(triggered()), this, SLOT(clearSlate()));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(restorePanelsAct, SIGNAL(triggered()), this, SLOT(restorePanels()));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(openAct);
    editMenu = menuBar->addMenu(tr("&Edit"));
    editMenu->addAction(restorePanelsAct);
    helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    //Establishes the top-level layout
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(menuBar, 0, 0, 1, 1);
    mainLayout->addWidget(renderArea, 1, 0, 1, 2);
    mainLayout->setRowStretch(1,1);

    filler = new QWidget;

    //Creates the input layout
    QGridLayout *inputLayout = new QGridLayout;
    inputLayout->setColumnStretch(4, 1);
    inputLayout->setColumnMinimumWidth(4,10);
    inputLayout->addWidget(xBoxLabel,       0,0,1,1);
    inputLayout->addWidget(xBox,            0,1,1,1);
    inputLayout->addWidget(yBoxLabel,       0,2,1,1);
    inputLayout->addWidget(yBox,            0,3,1,1);
    inputLayout->addWidget(aBoxLabel,       1,0,1,1);
    inputLayout->addWidget(aBox,            1,1,1,1);
    inputLayout->addWidget(bBoxLabel,       1,2,1,1);
    inputLayout->addWidget(bBox,            1,3,1,1);
    inputLayout->addWidget(clearButton,     2,0,1,1);
    inputLayout->addWidget(numbers_radio,   2,1,1,1);
    inputLayout->addWidget(points_radio,    2,2,1,1);
    inputLayout->addWidget(creases_radio,   2,3,1,1);
    inputLayout->addWidget(cut_radio,       2,4,1,1);
    inputLayout->addWidget(filler,          0,5,1,3);

    //Organizes the output layout
    QGridLayout *outputLayout = new QGridLayout;
    outputLayout->setVerticalSpacing(0);
    outputLayout->addWidget(vertexLabel,0,0,Qt::AlignRight);
    outputLayout->addWidget(vertexBox,1,0,Qt::AlignRight);
    outputLayout->addWidget(edgeLabel,2,0,Qt::AlignRight);
    outputLayout->addWidget(edgeBox,3,0,Qt::AlignRight);
    outputLayout->addWidget(panelLabel,4,0,Qt::AlignRight);
    outputLayout->addWidget(panelBox,5,0,Qt::AlignRight);
    outputLayout->addWidget(DOFLabel,6,0,Qt::AlignRight);
    outputLayout->addWidget(DOFBox,7,0,Qt::AlignRight);
    outputLayout->addWidget(SSLabel,8,0,Qt::AlignRight);
    outputLayout->addWidget(SSBox,9,0,Qt::AlignRight);

    mainLayout->addLayout(outputLayout,1,2,Qt::AlignBottom);
    mainLayout->addLayout(inputLayout,2,0,1,3,Qt::AlignLeft);
    mainLayout->setColumnStretch(1,1);
    setLayout(mainLayout);

    //Manages all the function-action connections
    connect(xBox, SIGNAL(returnPressed()), this, SLOT(plotPoint()));
    connect(yBox, SIGNAL(returnPressed()), this, SLOT(plotPoint()));
    connect(aBox, SIGNAL(returnPressed()), this, SLOT(plotCrease()));
    connect(bBox, SIGNAL(returnPressed()), this, SLOT(plotCrease()));
    connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearSlate()));
    connect(numbers_radio, SIGNAL(toggled(bool)), this, SLOT(toggleNumberPlot()));
    connect(points_radio, SIGNAL(toggled(bool)), this, SLOT(togglePointPlot()));
    connect(creases_radio, SIGNAL(toggled(bool)), this, SLOT(toggleCreasePlot()));
    connect(cut_radio, SIGNAL(toggled(bool)), this, SLOT(toggleCreaseCut()));
}

void Window::plotPoint()
{
    QString x = xBox->displayText();
    QString y = yBox->displayText();

    if(plot_numbers)
    {
        renderArea->addPoint(x.toDouble(), y.toDouble());
        refreshTotals();
    }
}

void Window::plotCrease()
{
    QString first = aBox->displayText();
    QString second = bBox->displayText();

    if(plot_numbers)
        renderArea->addCrease(first.toInt(), second.toInt());
}

//Refreshes all the values in the output boxes with values obtained from 'renderArea'
void Window::refreshTotals()
{
    QString quantity;
    vertexBox->setText(quantity.setNum(renderArea->numberOfVertices()));
    edgeBox->setText(quantity.setNum(renderArea->numberOfEdges()));
    panelBox->setText(quantity.setNum(renderArea->numberOfPanels()));
    DOFBox->setText(quantity.setNum(renderArea->numberOfDOFs()));
    SSBox->setText((quantity.setNum(renderArea->numberOfSSs()))); // This will need to be updated
    vertexBox->update();
    edgeBox->update();
    panelBox->update();
    DOFBox->update();
}

//Clears all information from the 'renderArea'
void Window::clearSlate()
{
    renderArea->clearAll();
    renderArea->update();
    refreshTotals();
}

//Manages the file-saving process
void Window::saveFile()
{
    bool ok;
    //Gets the desired file name from the user
    QString text = QInputDialog::getText(this, tr("Save File"),
                                         tr("Select a desired file name:"),
                                         QLineEdit::Normal, tr("file name"), &ok);
    if (ok && !text.isEmpty())
    {
        //Gets the directory location selected by the user
        directory = QFileDialog::getExistingDirectory(this,
                                       tr("Find Files"), QDir::currentPath());
        directory.append("/" + text + ".txt"); //Creates the combination path/name variable

        vector<QString> elements;
        output.open((directory.toStdString()).c_str());
        renderArea->getData(elements); //Gets the pattern data

        for(int i = 0; i < (int)elements.size(); i++)
        {
            output << elements[i].toStdString().c_str(); //Writes it to the file
            output << "\n";
        }

        output.close();
    }
}

void Window::openFile()
{
    //Gets the directory and name of the file to open
    directory = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    QDir::currentPath(),
                                                    tr("Text File (*.txt)"));
    if (!directory.isEmpty())
    {
        input.open(directory.toStdString().c_str());
    }

    string line;
    vector<QString> data;
    while(getline(input, line)) //Gets all the data from the file
        data.push_back(QString(line.c_str()));

    clearSlate();
    renderArea->loadData(data); //Loads the data into the 'renderArea' and 'pattern' variables

    input.close();
}

void Window::toggleNumberPlot()
{
    if(plot_numbers)
        plot_numbers = false;
    else
        plot_numbers = true;
}

//These toggle functions are controlled by the radio buttons and manage the plotting and deletion process
void Window::togglePointPlot()
{
    if(plot_points)
        plot_points = false;
    else
        plot_points = true;
}

void Window::toggleCreasePlot()
{
    if(plot_creases)
        plot_creases = false;
    else
        plot_creases = true;
}

void Window::toggleCreaseCut()
{
    if(cut_creases)
        cut_creases = false;
    else
        cut_creases = true;
}

bool Window::isNumberPlot()
{
    return plot_numbers;
}

bool Window::isPointPlot()
{
    return plot_points;
}

bool Window::isCreasePlot()
{
    return plot_creases;
}

bool Window::isCreaseCut()
{
    return cut_creases;
}

void Window::restorePanels()
{
    renderArea->restorePanels();
    refreshTotals();
}

//This button provides basic information on the program. Additional information will be added to facilitate
//program use.
void Window::about()
{
    QMessageBox msgBox;
    msgBox.setText("This program is designed to function as a tool for creating "
                   "origami crease patterns and analyzing their mechanical "
                   "properties. The user interface allows a user to plot a series"
                   " of vertices, connect those vertices with creases and generate"
                   " a crease pattern. Once the crease pattern is drawn, the "
                   "program then calculated the important characteristics of the "
                   "pattern such as its Mobility (dofs). Some key features of the "
                   "program are 'save' and 'open' capability as well as creation"
                   "and deletion of pattern elements.");
    msgBox.exec();
}
