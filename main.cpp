#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include "opencv2/opencv.hpp"
#include <string.h>

using namespace std;
using namespace cv;

struct Spoint {
    int x;
    int y;
};
typedef struct Spoint pnt;

pnt* creerPoint(int x, int y) {
    pnt* newpnt = (pnt*)malloc(sizeof(pnt));
    newpnt->x = x;
    newpnt->y = y;
    return newpnt;
}

void supPoint(pnt* point) {
    free(point);
}

void manual() {
    printf("Manual: ./BezierViewer [width of the window] [height of the window] [path the file]\n");
}

void initImage(Mat image, int w, int h, int r, int g, int b) {
    for(int x=0; x<w; x++) {
        for(int y=0; y<h; y++) {
            Vec3b pixel = image.at<Vec3b>(y, x);
            pixel[0] = b;
            pixel[1] = g;
            pixel[2] = r;
            image.at<Vec3b>(y, x) = pixel;
        }
    }
}

void affichePoint(Mat img, pnt* point, int taille, int r, int g, int b) {
    int lim = taille/2;
    for(int x = -lim; x<lim+1; x++) {
        for(int y = -lim; y<lim+1; y++) {
            Vec3b pixel = img.at<Vec3b>(point->y+y, point->x+x);
            pixel[0] = b;
            pixel[1] = g;
            pixel[2] = r;
            img.at<Vec3b>(point->y+y, point->x+x) = pixel;
        }
    }
}

void afficheListePoint(Mat img, pnt** lst, int taille, int taillepnt, int r, int g, int b) {
    for(int i=0; i<taille; i++) {
        affichePoint(img, lst[i], taillepnt, r, g, b);
    }
}

void tracerLigne(Mat img, pnt* p1, pnt* p2, int r, int g, int b) {
    int precision = max(abs(p1->x-p2->x), abs(p1->y-p2->y))*1.5;
    for(int i=0; i<precision+1; i++) {
        float t = static_cast<float>(i)/precision;
        int x = (1-t)*p1->x + t*p2->x;
        int y = (1-t)*p1->y + t*p2->y;
        pnt* point = creerPoint(x, y);
        affichePoint(img, point, 1, r, g, b);
        supPoint(point);
    }

}

void relierListPoint(Mat img, pnt** lst, int taille, int r, int g, int b) {
    for(int i=0; i<taille-1; i++) {
        tracerLigne(img, lst[i], lst[i+1], r, g, b);
    }
}

pnt* calculBezier(pnt** lst, int taille, float t) {
    pnt* r_pnt;
    pnt*** listsCentres = (pnt***)malloc((taille+1)*sizeof(pnt*));
    listsCentres[taille] = (pnt**)malloc(taille*sizeof(pnt));
    for(int i=0; i<taille; i++) {
        listsCentres[taille][i] = lst[i];
    }
    for(int d=taille-1; d>0; d--) {
        listsCentres[d] = (pnt**)malloc(d*sizeof(pnt));
        for(int i=0; i<d; i++) {
            float x = (1-t)*static_cast<float>(listsCentres[d+1][i]->x) + t*static_cast<float>(listsCentres[d+1][i+1]->x);
            float y = (1-t)*static_cast<float>(listsCentres[d+1][i]->y) + t*static_cast<float>(listsCentres[d+1][i+1]->y);
            pnt* point = creerPoint(static_cast<int>(x), static_cast<int>(y));
            listsCentres[d][i] = point;
        }
        if(d==1) {
            r_pnt = listsCentres[d][0];
        }
    }
    return r_pnt;
}

int main(int argc, char* argv[]) {
    string filePath;
    int w; int h;
    if(argc >= 4) {
        w = atoi(argv[1]);
        h = atoi(argv[2]);
        filePath = argv[3];
    } else {
        manual();
        return 0;
    }

    Mat image(h, w, CV_8UC3, 0.0);
    initImage(image, w, h, 100, 100, 100);

    int nbPoints;
    FILE * file = fopen(filePath.data(), "r");
    fscanf(file, "%d", &nbPoints);
    pnt** listPoints = (pnt**)malloc(nbPoints*sizeof(pnt*));

    for(int i=0; i<nbPoints; i++) {
        int x;
        int y;
        fscanf(file,"%d %d", &x, &y);
        pnt* tempp = creerPoint(x, y);
        listPoints[i] = tempp;
    }
    
    int precision = w*h;
    pnt** listCourbe = (pnt**)malloc((precision+1)*sizeof(pnt));
    for(int i=0; i<precision+1; i++) {
        float t = static_cast<float>(i)/precision;
        listCourbe[i] = calculBezier(listPoints, nbPoints, t);
    }

    afficheListePoint(image, listPoints, nbPoints, 6, 0, 255, 0);
    relierListPoint(image, listPoints, nbPoints, 0, 255, 0);
    afficheListePoint(image, listCourbe, precision, 2, 255, 0, 0);

    while(true) {
        imshow("BezierViewer", image);
        waitKey(1);
    }

    for(int i=0; i<nbPoints; i++) {
        supPoint(listPoints[i]);
    }
    free(listPoints);
    for(int i=0; i<precision+1; i++) {
        supPoint(listCourbe[i]);
    }
    free(listCourbe);

    return 0;
}