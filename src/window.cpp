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
#include "QDebug"
#include <iostream>

//initialize everything
CWindow::CWindow() : QMainWindow(), OpenedFile("")
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
    box.setText("Contact at benquach16@yahoo.com");

    box.exec();
}

//TODO : make this code faster
QColor CWindow::convertToRGB(const string& hex)
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
string CWindow::convertToHex(const QColor& color)
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

unsigned char CWindow::convertToHex(int r, int g, int b)
{
return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

void CWindow::OpenedWithFile(const string &file)
{
    OpenedFile = QString::fromAscii(file.data(), file.size());
    DisplayFile();
}

void CWindow::DisplayFile()
{
    if(OpenedFile!="")
    {
        if(!OpenedFile.isNull())
        {
            this->setWindowTitle(OpenedFile);
            //read
            fstream file(OpenedFile.toStdString().c_str(),ios::in | ios::binary);

            benHeaderFile bhf;
            benHeaderInfo bhi;

            //file.seekg(0,ios::cur);
            int width, height;
            file.read((char*)&width, sizeof(int));
            file.read((char*)&height, sizeof(int));
            qDebug() << width ;
            qDebug() << height ;

            label->resize(width,height);
            label->setGeometry(QRect(0,20,width,height));
            QPixmap pix = QPixmap(width, height);
            p = new QPainter(&pix);

            p->setViewport(0,0,width, height);
            //base coat
            p->fillRect(QRect(0,0,width, height), QColor(255,255,255,255));


            for(unsigned i = 0; i < height; i++)
            {
                for(unsigned j = 0; j < width; j++)
                {
                    string color;
                    file.read((char*)&color, sizeof(string));
                    qDebug() << color.c_str();
                    p->setPen(convertToRGB(color));
                    p->drawPoint(j, i);
                }
            }
            label->setPixmap(pix);
        }
    }
}

//opens a .ben file
void CWindow::OpenFile()
{
    OpenedFile = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Ben Image Files (*.ben*)"));
    //if something was actually loaded
    DisplayFile();
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
    int height, width;
    QSize rect = pix->size();
    width = rect.width();
    height = rect.height();



    QString SavedFile = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Ben Image Files (*.ben*)"));
    //SavedFile+=".ben";          //gotta have that file extension
    QFileInfo file(SavedFile);

    //ensure that there is a .ben suffix
    if(file.suffix().isEmpty())
    {
        SavedFile+=".ben";
    }
    ofstream savefile;
    savefile.open(SavedFile.toStdString().c_str());

    benHeaderFile bhf;
    benHeaderInfo bhi;
    char type;
    savefile.write((char*)&width, sizeof(int));
    savefile.write((char*)&height, sizeof(int));

    for(unsigned i=0; i<height; i++)
    {
        //for each x value in each y value
        for(unsigned j =0; j < width; j++)
        {
            QColor tmp_color = pix->toImage().pixel(j,i);
            string value = tmp_color.name().toStdString();
            //int t = value.find("#");
            value.erase(0,1);
            savefile.write((char*)&value, sizeof(string));
            //savefile << value;
        }
    }
}
