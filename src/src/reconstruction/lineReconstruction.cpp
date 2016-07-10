#include "lineReconstruction.h"
#include "src/vect/vect_catmull.h"
using namespace reco;


//Determinar si un punto final es valido a partir de sus vecindades
int reco::isValid(int j, int i, Mat& src1, int x, int y)
{
    uchar pk =  src1.at<uchar>(j,i);
    if(pk==0 && (j>y && i>x) && (j<src1.rows -y && i<src1.cols-x))
    {

         uchar p1 = src1.at<uchar>(j-1,i)== 255   ? 0:1;
         uchar p2 = src1.at<uchar>(j-1,i+1)== 255   ? 0:1;
         uchar p3 = src1.at<uchar>(j,i+1)== 255   ? 0:1;
         uchar p4 = src1.at<uchar>(j+1,i+1)== 255   ? 0:1;
         uchar p5 = src1.at<uchar>(j+1,i)== 255   ? 0:1;
         uchar p6 = src1.at<uchar>(j+1,i-1)== 255   ? 0:1;
         uchar p7 = src1.at<uchar>(j,i-1)== 255   ? 0:1;
         uchar p8 = src1.at<uchar>(j-1,i-1)== 255   ? 0:1;

         int val = getCode(p1,p2,p3,p4,p5,p6,p7,p8);
         return val;
    }

    return -1;
}


//Funcion de distancia
double reco::dist(int x0, int x1, int y0, int y1) {
     double x = (x0-x1)*(x0-x1);
     double y = (y1-y0)*(y1-y0);
     return sqrt(x+y);
}


//Algoritmo principal
int reco::execLineReconstruction(Mat& src, int h)
{
    /*PARAMETROS*/
    int limit = 20;
    int limitX = 15;
    int limitY = 15;
    int inc = 5;
    int Max = 200;
    int hInit = h;

    int lineCount = 0;

    Mat src1 = src.clone();

    //Variables de control
    int nPoints = 0;
    int j, i, v, t, endVal,up, down, left, right,d1,d2,xd,yd,endValInner,dirCandidate;
    Point p;
    double dirPoint,max,distan;


    //Recorrer todos los pixeles de la imagen
    for(j=1;j<(src.rows);j++)
    {
        for (i=1;i<(src.cols);i++)
        {
           //DETERMINO SI ES UN PUNTO DE FIN
           endVal = isValid(j,i, src1, limitX, limitY);
           if(isEndPoint(endVal))
                {
                    //CALCULO LIMITES DE LA MASCARA
                    up = j-(h/2);
                    down = j+(h/2);
                    if(up < 0)
                    {
                        up = 0;
                    }
                    if(down>src.rows)
                    {
                        down = src.rows;
                    }

                    left = i-(h/2);
                    right = i+(h/2);
                    if(left < 0)
                    {
                        left = 0;
                    }
                    if(right>src.cols)
                    {
                        right = src.cols;
                    }

                    Point p = nextPoint(i,j,limit, src1);
                    dirPoint = dirEndPoint(i, j, 5, src1);
                    d1 = (int ) changeDir(round(dirPoint));

                    xd = -1;
                    yd = -1;
                    max = INFINITY;


                    //Busqueda en los limites de la mascara
                    for(v=up; v<down;  v++)
                    {
                        for(t=left; t<right;  t++)
                        {
                            //Mat src4 = src1.clone();

                            //Determinio si es un pixel negro
                            endValInner = isValid(v,t, src1, limitX, limitY);
                            if(endValInner>=0)
                            {
                            	//Determino si es un punto final
                                if(isEndPoint(endValInner))
                                {
                                    if(v!=j || t!=i)
                                    {
                                        if(p.x != t && p.y != v)
                                        {
                                           // cout<<v<<endl;
                                           // cout<<t<<endl;
                                        	//Calculo de direcciones
                                            dirCandidate = dirEndPoint(t, v, 5, src1);
                                            

                                            //Cambiar dirección
                                            d2 = (int ) changeDir(round(dirCandidate));
                                           // cout<<"Candidate"<<d2<<endl;

                                            //Verificar si se cumplen condiciones de direccion
                                            if(deletePoints(d1, d2))
                                            {

                                            			//Se calcula distancia en coordenada Y
                                                        if(abs(j-v)< max)
                                                        {
                                                        	//Se actualiza el maximo valor
                                                            max = abs(j-v);
                                                            yd = v;
                                                            xd = t;
                                                        }

                                            }
                                        }
                                    }
                                }
                            }


                        }

                    }

                    //Si se cumple la condición, se pinta la linea entre los puntos.
                    if(xd != -1 && yd != -1)
                    {

                        Point pi = Point(i,j);
                        Point pj = Point(xd,yd);
                        paintLine(pi,pj,src1);

                        lineCount++;

                    }

                }




        }
    }


    for(j=1;j<(src.rows);j++)
    {
        h = hInit;
        for (i=1;i<(src.cols);i++)
        {



           endVal = isValid(j,i, src1, limitX, limitY);
           if(isEndPoint(endVal))
                {
                    //CALCULO LIMITES DE LA MASCARA
                    up = j-(h/2);
                    down = j+(h/2);
                    if(up < 0)
                    {
                        up = 0;
                    }
                    if(down>src.rows)
                    {
                        down = src.rows;
                    }

                    left = i-(h/2);
                    right = i+(h/2);
                    if(left < 0)
                    {
                        left = 0;
                    }
                    if(right>src.cols)
                    {
                        right = src.cols;
                    }

                    /*****************************************/

                    p = nextPoint(i,j,h, src1);
                    dirPoint = dirEndPoint(i, j, 10, src1);
                    //int d1 = (int ) changeDir(round(dirPoint));
                    switch((int)dirPoint)
                    {
                        case 0:
                            up = j;
                            down = j+h;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-(h/2);
                            right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 1:
                             up = j;
                            down = j+h;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-(h/2);
                            right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 2:
                            up = j-(h/2);
                            down = j+(h/2);
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-h;
                            right = i;
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 3:
                            up = j-h;
                            down = j;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-(h/2);
                            right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 4:
                             up = j-h;
                             down = j;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                             left = i-(h/2);
                             right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 5:
                            up = j-h;
                            down = j;
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                            left = i-(h/2);
                            right = i+(h/2);
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 6:
                             up = j-(h/2);
                             down = j+(h/2);
                            if(up < 0)
                            {
                                up = 0;
                            }
                            if(down>src.rows)
                            {
                                down = src.rows;
                            }

                             left = i;
                             right = i+h;
                            if(left < 0)
                            {
                                left = 0;
                            }
                            if(right>src.cols)
                            {
                                right = src.cols;
                            }
                        break;

                        case 7:
                           up = j;
                           down = j+h;
                           if(up < 0)
                           {
                               up = 0;
                           }
                           if(down>src.rows)
                           {
                               down = src.rows;
                           }

                           left = i-(h/2);
                           right = i+(h/2);
                           if(left < 0)
                           {
                               left = 0;
                           }
                           if(right>src.cols)
                           {
                               right = src.cols;
                           }
                        break;

                    }

                    xd = -1;
                    yd = -1;
                    max = INFINITY;

                    for(v=up; v<down;  v++)
                    {
                        for(t=left; t<right;  t++)
                        {

                            endValInner = isValid(v,t, src1, limitX, limitY);
                            if(endValInner>=0)
                            {
                                if(isEndPoint(endValInner))
                                {
                                    if(v!=j || t!=i)
                                    {
                                        if(p.x != t && p.y != v)
                                        {

                                            dirCandidate = dirEndPoint(t, v, 10, src1);
                                            if(dirCandidate != dirPoint)
                                            {
                                                distan = dist(i,t,j,v);
                                                if(distan< max)
                                                {
                                                    max = distan;
                                                    yd = v;
                                                    xd = t;
                                                }
                                            }
                                        }
                                    }
                                }
                            }


                        }

                    }

                    if(xd != -1 && yd != -1)
                    {
                        Point pi = Point(i,j);
                        Point pj = Point(xd,yd);
                        paintLine(pi,pj,src1);
                        lineCount++;

                    }
                    else
                    {
                        if(h<= Max)
                        {
                            i = i-1;
                            h = h+inc;
                        }

                    }
                }




        }
    }

    src.release();
    src = src1;
    imwrite("c:/img/hello.png",src);

    return lineCount;
}


//Pintar linea
void reco::paintLine(Point p1, Point p2, Mat src)
{
    int dis = dist(p1.x, p2.x, p1.y, p2.y);



    //Obteniendo pixel 5 de la linea
    Point p11 = findPoint(p1.x,p1.y,10, src);

    //Obteniendo pixel 5 de la linea
    Point p22 = findPoint(p2.x,p2.y,10, src);

    vector<Point2f> l;
    vector<Point2f> res;
    Point2f v1, v2, v3, v4;
    v2 = Point2f(p1.x, p1.y);
    v1 = Point2f(p11.x, p11.y);

    v3 = Point2f(p2.x, p2.y);
    v4 = Point2f(p22.x, p22.y);

    l.push_back(v1);
    l.push_back(v2);
    l.push_back(v3);
    l.push_back(v4);
    cout<<dis<<endl;
    catmull::interpolar(l, res, dis/8);
    //cout<<"RESS"<<endl;
    Point p;
/*
    int first = 0;

    if(p1.x > p2.x && p1.y > p2.y)
    {
        first = 1;
    }
    else
    {
        first = 2;
    }

    int limitx1, limitx2, limity1, limity2;

    if(p1.x>p11.x)
    {
        limitx2 = p1.x;
        limitx1 = p11.x;
    }
    else
    {
        limitx1 = p1.x;
        limitx2 = p11.x;
    }

    if(p1.y >p11.y)
    {
        limity2 = p1.y;
        limity1 = p11.y;
    }
    else
    {
        limitx1 = p1.y;
        limitx2 = p11.y;
    }*/
    int m = 0;
    Point aux = Point(-1,-1);
    for(vector<Point2f>::iterator it = res.begin(); it != res.end(); ++it) {
        if(res.size()>6)
        {
            p = *it;
            cout<<"p"<<p.x<<"|"<<p.y<<endl;
            cout<<"aux"<<aux.x<<"|"<<aux.y<<endl;
            cout<<res.size()<<endl;
            if(m>2 && m<res.size()-2)
            {
                if(aux.x!= -1 && aux.y != -1)
                {
                    line(src,Point(p.x, p.y),aux,CV_RGB(255,0,0),1,8);
                    aux.x = p.x;
                    aux.y = p.y;
                }
                else
                {
                    line(src,Point(p.x, p.y),p1,CV_RGB(255,0,0),1,8);
                    aux.x = p.x;
                    aux.y = p.y;
                }
            }
            else
            {
                if(m==res.size()-2)
                {
                    line(src,aux,p2,CV_RGB(255,0,0),1,8);
                }
            }

            m++;
        }

        else
        {
            line(src,p1,p2,CV_RGB(255,0,0),1,8);
        }

    }

}

//Determinar si un punto es punto final
bool reco::isEndPoint(int x)
{
    return x==128 || x==2 || x==8 || x==32 || x==1 || x == 4|| x==16 || x== 32 || x==64;
}

//Obtener codificiación
int reco::getCode(uchar p1, uchar p2, uchar p3, uchar p4, uchar p5, uchar p6, uchar p7, uchar p8)
{
    return (p8*64)+(p1*128)+(p2*1)+(p3*2)+ (p4*4) + (p5*8)+ (p6*16)+ (p7*32);
}


//Determinar punto siguiente en los pixeles.
Point reco::nextPoint(int x, int y, int limit, Mat dst)
{
    int paint[limit][2];

    for(int i=0; i<limit; i++)
    {
       int val = getDir(x,y, dst);
       //int a = i;
       //stringstream ss;
      // ss << a;
       //string str = ss.str();
       dst.at<uchar>(y,x) = 255;

       paint[i][0] = y;
       paint[i][1] = x;

       int x0 = x;
       int y0 = y;

       switch(val)
       {
           case 1:
               y = y-1;
               x = x+1;
           break;
           case 2:
               x = x+1;
           break;
           case 4:
               y = y+1;
               x = x+1;
           break;
           case 8:
               y=y+1;
           break;
           case 16:
               y=y+1;
               x=x-1;
           break;
           case 32:
               x= x-1;
           break;
           case 64:
               y = y-1;
               x = x-1;
           break;
           case 128:
               y = y - 1;
           break;
       }
       dst.at<uchar>(y0,x0) = 0;
       int dir =getDir(x,y, dst);
       if(isEndPoint(dir))
       {
          for(int k = 0; k<=i; k++)
          {
              dst.at<uchar>(paint[k][0],paint[k][1]) = 0;
          }
          return Point(x,y);
       }
       dst.at<uchar>(y0,x0) = 255;

    }


    for(int k = 0; k<limit; k++)
    {
        dst.at<uchar>(paint[k][0],paint[k][1]) = 0;
    }
    return Point(-1,-1);
}

//Determinar direccion de un punto
double reco::dirEndPoint(int x, int y, int limit, Mat dst)
{
    //double dir = 0.0;
    int cont = 0, i, val;
    int paint[limit][2];
    double points[10];

    for(i=0; i<limit; i++)
    {
        //int vaj = dst.at<uchar>(y,x);
        if(dst.at<uchar>(y,x) == 0)
        {
            val = getDir(x,y, dst);
            if(isEndPoint(val))
            {
                //cout<<val<<endl;
                //dir += getChainDir(val);
                points[i] = getChainDir(val);
                //int a = i;
                //stringstream ss;
               // ss << a;
               // string str = ss.str();
                dst.at<uchar>(y,x) = 255;
                //int x0 = x;
                //int y0 = y;
                paint[i][0] = y;
                paint[i][1] = x;


                switch(val)
                {
                    case 1:
                        y = y-1;
                        x = x+1;
                    break;
                    case 2:
                        x = x+1;
                    break;
                    case 4:
                        y = y+1;
                        x = x+1;
                    break;
                    case 8:
                        y=y+1;
                    break;
                    case 16:
                        y=y+1;
                        x=x-1;
                    break;
                    case 32:
                        x= x-1;
                    break;
                    case 64:
                        y = y-1;
                        x = x-1;
                    break;
                    case 128:
                        y = y - 1;
                    break;
                }

                cont++;
            }
            else
            {
                break;
            }

        }


    }

    int k;
    double p1 = points[0], p2;
    //double acum = 0.0;


    for(k=0; k<cont; k++)
    {
        p2 = points[k];
        if(abs(p1-p2)<=180)
        {
            p1 = (p1+p2)/2;
        }
        else
        {
            p1 = (p1+p2)/2;
            p1 = p1+180.0;
        }
        dst.at<uchar>(paint[k][0],paint[k][1]) = 0;
    }

     return getCodeDir(p1);
}


//Obtener codigo a partir de direccion
int reco::getCodeDir(double d)
{
    if(d> 337.5 && d<= 22.5)
    {
        return 2;
    }
    if(d>22.5 && d<= 67.5)
    {
        return 1;
    }
    if(d>67.5 && d <= 112.5)
    {
        return 0;
    }
    if(d> 112.5 && d <= 157.5)
    {
        return 7;
    }
    if(d> 157.5 && d <= 202.5)
    {
        return 6;
    }
    if(d> 202.5 && d<= 247.5)
    {
        return 5;
    }
    if((d > 247.5) && (d <= 292.5))
    {
        return 4;
    }
    if(d>292.5 && d<=337.5)
    {
        return 3;
    }

    return 0;
}

//Obtener dirección
int reco::getDir(int x, int y, Mat dst)
{
       uchar p1 = dst.at<uchar>(y-1,x)== 255   ? 0:1;

       uchar p2 = dst.at<uchar>(y-1,x+1)== 255   ? 0:1;

       uchar p3 = dst.at<uchar>(y,x+1)== 255   ? 0:1;

       uchar p4 = dst.at<uchar>(y+1,x+1)== 255   ? 0:1;

       uchar p5 = dst.at<uchar>(y+1,x)== 255   ? 0:1;

       uchar p6 = dst.at<uchar>(y+1,x-1)== 255   ? 0:1;

       uchar p7 = dst.at<uchar>(y,x-1)== 255   ? 0:1;

       uchar p8 = dst.at<uchar>(y-1,x-1)== 255   ? 0:1;

       int val = getCode(p1,p2,p3,p4,p5,p6,p7,p8);

       return val;
}

//Obtener dirección a partir de codigo
double reco::getChainDir(int dir)
{
    switch(dir)
    {
        case 1:
            return 45.0;
        case 2:
            return 0.0;
        case 4:
            return 315.0;
        case 8:
          return 270.0;
        case 16:
            return 224.0;
        case 32:
            return 180.0;
        case 64:
            return 135.0;
        case 128:
            return 90.0;
    }

    return 0.0;
}

//Invertir direccion
int reco::changeDir(int val)
{
    switch(val)
    {
        case 0:
            return 7;
        case 6:
            return 5;
        case 4:
            return 3;
        case 2:
            return 1;
        default:
            return val;
    }

}

//Determinar condiciones de direccion
bool reco::deletePoints(int d1, int d2)
{
    switch(d1)
    {
        case 1:
            if(d2 == 5 || d2 == 3 || d2 == 7)
            {
                return true;
            }
        break;
        case 3:
            if(d2 == 7 || d2 == 5 || d2 == 1)
            {
                return true;
            }
        break;
        case 5:
            if(d2 == 7 || d2 == 3 || d2 == 1)
            {
                return true;
            }
        break;
        case 7:
            if(d2 == 5 || d2 == 3 || d2 == 1)
            {
                return true;
            }
        break;


    }

    return false;
}


char* reco::parsePoint(int x, int y) {
    char *a = "x";
    char *b = "y";
    stringstream sstm;
    sstm << a <<x<< b<<y;
    string ans = sstm.str();
    char * ans2 = (char *) ans.c_str();
    return ans2;
}

Point reco::findPoint(int x, int y, int limit, Mat dst)
{
    int paint[limit][2];

    for(int i=0; i<limit; i++)
    {
       int val = getDir(x,y, dst);
       //int a = i;
       //stringstream ss;
      // ss << a;
       //string str = ss.str();
       dst.at<uchar>(y,x) = 255;

       paint[i][0] = y;
       paint[i][1] = x;

       int x0 = x;
       int y0 = y;

       switch(val)
       {
           case 1:
               y = y-1;
               x = x+1;
           break;
           case 2:
               x = x+1;
           break;
           case 4:
               y = y+1;
               x = x+1;
           break;
           case 8:
               y=y+1;
           break;
           case 16:
               y=y+1;
               x=x-1;
           break;
           case 32:
               x= x-1;
           break;
           case 64:
               y = y-1;
               x = x-1;
           break;
           case 128:
               y = y - 1;
           break;
       }
       dst.at<uchar>(y0,x0) = 0;
       int dir =getDir(x,y, dst);
       if(isEndPoint(dir))
       {
          for(int k = 0; k<=i; k++)
          {
              dst.at<uchar>(paint[k][0],paint[k][1]) = 0;
          }
          return Point(x,y);
       }
       dst.at<uchar>(y0,x0) = 255;

    }


    for(int k = 0; k<limit; k++)
    {
        dst.at<uchar>(paint[k][0],paint[k][1]) = 0;
    }
    return Point(x,y);
}
