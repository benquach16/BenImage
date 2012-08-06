/////////////////////////////////////////////////////////////////////////////////////
//
//This file is part of benImage
//
//benImage is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////////////


#include "window.h"

//initialize everything
CWindow::CWindow() : QMainWindow()
{
    label = new QLabel(this);
    label->resize(500,300);
    //label->setGeometry(QRect(0,20,500,300));
    //create image display
    QPixmap pix = QPixmap(500,300);

    p = new QPainter(&pix);
    p->fillRect(0,0,500,300,QColor(255,255,255,255));
    p->setViewport(0,20,500,300);
    label->setPixmap(pix);
    label->setBackgroundRole(QPalette::Base);
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    label->setScaledContents(true);

    //default size
    resize(500,300);

    //create menubar
    QMenuBar *menu = new QMenuBar();
    setMenuBar(menu);
    file = menuBar()->addMenu("File");
    open = file->addAction("Open");
    open_bmp = file->addAction("Open .bmp");
    save = file->addAction("Save");
    quit = file->addAction("Exit");

    help = menuBar()->addMenu("Help");
    about = help->addAction("About");



    //create actions

    connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(open, SIGNAL(triggered()), this, SLOT(OpenFile()));

    connect(save, SIGNAL(triggered()), this, SLOT(SaveFile()));

    connect(open_bmp, SIGNAL(triggered()), this, SLOT(OpenBMPFile()));

    connect(about, SIGNAL(triggered()), this, SLOT(OpenAbout()));
}

CWindow::~CWindow()
{

}

//basic messagebox
void CWindow::OpenAbout()
{
    QMessageBox box;
    box.setText("Copyright (c) Benjamin Quach, 2012. Licensed under the GPL. Contact at benquach16@yahoo.com");

    box.exec();
}

//TODO : make this code faster
QColor CWindow::convertToRGB(string hex)
{
    int r;
    int g;
    int b;

    string redStr;
    stringstream red;
    //get first two characters for red
    for(int i=0; i<2; i++)
    {
        redStr+=hex.at(i);
    }
    red << std::hex << redStr;
    red >> r;

    string greenStr;
    stringstream green;
    //next two fore green

    for(int i=2; i<4; i++)
    {
        greenStr+=hex.at(i);
    }
    green << std::hex << greenStr;
    green >> g;

    string blueStr;
    stringstream blue;
    //last for blue
    for(int i=4; i<6; i++)
    {
        blueStr+=hex.at(i);
    }
    blue << std::hex << blueStr;
    blue >> b;

    return QColor(r,g,b,255);
}

//legacy
string CWindow::convertToHex(QColor color)
{
    int r, b, g;
    r = color.red();
    b = color.blue();
    g = color.green();

    std::stringstream stream;


    string return_value;

    stream << std::hex << r;
    stream << std::hex << b;

    stream << std::hex << g;

    return_value = stream.str();

    return return_value;
}

//opens a .ben file
void CWindow::OpenFile()
{
    OpenedFile = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Ben Image Files (*.ben*)"));
    //if something was actually loaded
    if(OpenedFile!="0")
    {
        if(!OpenedFile.isNull())
        {
            this->setWindowTitle(OpenedFile);
            //read as txt
            fstream file;
            file.open(OpenedFile.toStdString().c_str());
            std::vector<std::string> record;     //for one line only
            int picx;
            int picy;

            //read file
            while(file)
            {
                string str;
                getline(file,str);

                std::istringstream sstream(str);

                //get everything between the semicolons
                //and add to record vector
                while(sstream)
                {
                    string s;

                    if(!getline(sstream, s, ';'))
                        break;

                    record.push_back(s);
                }

            }
            //ensure proper .ben file
            if(record[0]=="BENIMAGE")
            {
                record.erase(record.begin());
                if(record.size()>0)
                {
                    //load the rest
                    string x = record[0];
                    string y = record[1];
                    picx = atoi(x.c_str());
                    picy = atoi(y.c_str());

                    label->resize(picx,picy);
                    label->setGeometry(QRect(0,20,picx,picy));
                    //only color information left
                    record.erase(record.begin());
                    record.erase(record.begin());


                    QPixmap pix = QPixmap(picx,picy);
                    p = new QPainter(&pix);

                    p->setViewport(0,0,picx,picy);
                    //base coat
                    p->fillRect(QRect(0,0,picx,picy), QColor(255,255,255,255));
                    //convert hex to rgb

                    //old code
                    //for some reason it displays file at a 45 degree shear
                    /*
                    for(int i=0; i<record.size(); i++)
                    {
                        //get color
                        QColor color = convertToRGB(record[i]);

                        if(x_counter < picx)
                        {
                            p->setPen(color);
                            p->drawPoint(x_counter, y_counter);
                            x_counter +=1;
                        }
                        else
                        {
                            if(y_counter < picy)
                            {
                                x_counter = 0;
                                y_counter+=1;
                            }
                        }
                        */


                    //this works
                    int record_counter = 0;
                    //for each y row
                    for(int i=0; i < picy; i++)
                    {
                        for(int n=0; n< picx; n++)
                        {
                            p->setPen(convertToRGB(record[record_counter]));
                            p->drawPoint(n,i);

                            if(record_counter < record.size())
                                record_counter++;

                        }


                    }

                    label->setPixmap(pix);

                }

            }
            else
            {
                //NOT A PROPER BENIMAGE FILE
                //create error
                QMessageBox box;
                box.setText("Invalid .ben file, please resave the file");
                box.exec();


            }

        }
    }
}

//load bmp
void CWindow::OpenBMPFile()
{
    OpenedFile = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Bitmap Files (*.bmp*)"));
    if(OpenedFile!="0")
    {
        if(!OpenedFile.isNull())
        {
            this->setWindowTitle(OpenedFile);
            QImage bmp;
            bmp.load(OpenedFile);
            QPixmap pix(QPixmap::fromImage(bmp));

            label->setPixmap(pix);
            label->resize(bmp.size());
            label->setGeometry(QRect(0,20,bmp.size().width(),bmp.size().height()));
        }
    }
}


//encode file
void CWindow::SaveFile()
{
    //take the pix and stuffs
    const QPixmap *pix = label->pixmap();
    int picx, picy;
    QSize rect = pix->size();
    picx = rect.width();
    picy = rect.height();

    ofstream savefile;

    QString SavedFile = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Ben Image Files (*.ben*)"));
    //SavedFile+=".ben";          //gotta have that file extension
    QFileInfo file(SavedFile);

    //ensure that there is a .ben suffix
    if(file.suffix().isEmpty())
    {
        SavedFile+=".ben";
    }

    savefile.open(SavedFile.toStdString().c_str());

    //ensure proper file
    savefile << "BENIMAGE;";


    savefile << picx << ";" << picy << ";";

    //save color
    QImage tmp = pix->toImage();        //temp convert to image to get pixels

    /*
    int x_counter = 0;
    int y_counter = 0;

    int total = picx * picy;

    for(int i=0; i < total; i++)
    {
        //save all pixesl
        if(x_counter < picx)
        {

            QColor tmp_color = tmp.pixel(QPoint(x_counter,y_counter));
            string value = tmp_color.name().toStdString();
            int t = value.find("#");
            value.erase(t,t+1);
            savefile << value << ";";
            x_counter+=1;
        }
        else
        {
            if(y_counter < picy)
            {
                x_counter = 0;
                y_counter +=1;
            }
        }
    }
    */

    //this works

    for(int i=0; i<picy; i++)
    {
        //for each x value in each y value
        for(int j =0; j < picx; j++)
        {
            QColor tmp_color = tmp.pixel(j,i);
            string value = tmp_color.name().toStdString();
            //int t = value.find("#");
            value.erase(0,1);

            savefile << value << ";";

        }
    }

}
