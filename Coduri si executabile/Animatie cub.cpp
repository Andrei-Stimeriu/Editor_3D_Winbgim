#include <graphics.h>
#include <math.h>

const int w = 900, h = 600, w2 = w/2, h2 = h/2;
const int FRAME_TIME = 30;

struct punct2D{
    double x, y;

    punct2D operator*(punct2D &b){
        return {x*b.x-y*b.y , x*b.y+y*b.x};
    }
};
struct punct3D{
    double x, y, z;

    punct3D operator+(punct3D &b){
        return {x+b.x, y+b.y, z+b.z};
    }
    punct3D operator-(punct3D &b){
        return {x-b.x, y-b.y, z-b.z};
    }
    punct3D operator*(double q){
        return {x*q, y*q, z*q};
    }

    void roteste(double unghi, int plan){
        punct2D rotire = {cos(unghi), sin(unghi)};
        punct2D proj = {x, y};
        if(plan == 1){ proj = {x, z}; }
        else if(plan == 2){ proj = {y, z}; }

        proj = proj * rotire;
        if(plan == 0){ x = proj.x; y = proj.y; }
        else if(plan == 1){ x = proj.x; z = proj.y; }
        else if(plan == 2){ y = proj.x; z = proj.y; }
    }
    punct2D proiecteaza(punct3D &obs){
        ///obs -> pct ({x,y,z}) -> plan (z = constanta)
        double dx = x - obs.x;
        double dy = y - obs.y;
        double dz = z - obs.z;

        double dzPlan = -obs.z - z;

        double raport = dzPlan / dz;
        punct2D rez = {x + dx*raport, y + dy*raport};
        return rez;
    }
};


void dotToScreen(punct2D &pct, double zoom){
    pct.x *= zoom; pct.y *= zoom;
    pct.x += w2; pct.y = -pct.y + h2;
}

struct pereche{ int a, b; };
class Cub{
private:
    punct3D centru = {0,0,0}; double latura = 2;

    const int nrVarfuri = 8, nrMuchii = 12;
    punct3D varfuri[8] = {
        {1,1,1},{-1,1,1},{1,-1,1},{1,1,-1},
        {-1,-1,1},{-1,1,-1},{1,-1,-1},{-1,-1,-1}
    };
    pereche muchii[12] = {
        {0,1},{0,2},{0,3},{1,4},{1,5},{2,4},
        {2,6},{3,5},{3,6},{4,7},{5,7},{6,7}
    };

public:
    Cub(){}
    Cub(double Latura){ setLatura(Latura); }
    Cub(punct3D Centru){ setPozitia(Centru); }
    Cub(double Latura, punct3D Centru){
        setLatura(Latura); setPozitia(Centru);
    }

    void draw(punct3D &obs, double zoom){
        punct2D proiectii[nrVarfuri];
        for(int i = 0; i < nrVarfuri; i++){
            proiectii[i] = varfuri[i].proiecteaza(obs);
            dotToScreen(proiectii[i], zoom);
        }

        for(int i = 0; i < nrMuchii; i++){
            line(proiectii[muchii[i].a].x,
                 proiectii[muchii[i].a].y,
                 proiectii[muchii[i].b].x,
                 proiectii[muchii[i].b].y);
        }
    }

    void roteste(double unghi, int plan){
        for(int i = 0; i < nrVarfuri; i++){
            varfuri[i] = varfuri[i] - centru;
            varfuri[i].roteste(unghi, plan);
            varfuri[i] = varfuri[i] + centru;
        }
    }
    void mareste(double marire){
        for(int i = 0; i < nrVarfuri; i++){
            varfuri[i] = varfuri[i] - centru;
            varfuri[i] = varfuri[i] * marire;
            varfuri[i] = varfuri[i] + centru;
        }
    }
    void deplaseaza(punct3D &deplasare){
        centru = centru + deplasare;
        for(int i = 0; i < nrVarfuri; i++){
            varfuri[i] = varfuri[i] + deplasare;
        }
    }

    void setLatura(double lat){
        double rap = lat / latura;
        latura = lat;
        mareste(rap);
    }
    void setPozitia(punct3D &poz){
        punct3D depl = poz - centru;
        deplaseaza(depl);
    }
};

void action1(Cub &c, punct3D &obs, double zoom){
    double t = 0.0;
    for(int i = 0; i < 157; i++){
        c.roteste(0.1, 1);
        c.roteste(0.05, 2);
        t += 0.06;
        c.setLatura(1.2 + 0.8*sin(t));

        cleardevice();
        c.draw(obs, zoom);

        delay(FRAME_TIME);
    }
}
void action2(Cub &c, punct3D &obs, double zoom, punct3D &depl, int noOfFrames){
    punct3D deplPerFrame = depl * (1.0/noOfFrames);
    for(int i = 0; i < noOfFrames; i++){
        c.roteste(0.1, 1);
        c.roteste(0.05, 2);

        c.deplaseaza(deplPerFrame);

        cleardevice();
        c.draw(obs, zoom);

        delay(FRAME_TIME);
    }
}

void show1(){
    punct3D obs = {0,0,3}; double zoom = 60;

    Cub c(2, {0,0,0});

    action1(c, obs, zoom);
    punct3D depl = {-4,3,-6}; action2(c, obs, zoom, depl, 30);
    action1(c, obs, zoom);
    depl = {4,-3,7}; action2(c, obs, zoom, depl, 30);
    action1(c, obs, zoom);
}
void show2(){
    punct3D obs = {0,0,3}; double zoom = 60;
    Cub c(2, {0,0,-20});
    punct3D depl = {0,0,21}; action2(c, obs, zoom, depl, 60);
}

int main(){
    initwindow(w, h, "Animatie cub", 300, 200);

    show1(); delay(2000);
    show2();

    getch();
    closegraph();
    return 0;
}
