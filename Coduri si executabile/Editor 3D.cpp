#include <graphics.h>
#include <math.h>
#include <iostream>
using namespace std;

#define ESC 27
const double phi = (sqrt(5) + 1.0) / 2.0;
const double invPhi = 1.0 / phi;

const int w = 900, h = 600, w2 = w/2, h2 = h/2;
const int FRAME_TIME = 30;

enum planuri{XY, XZ, YZ};

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
class Dodecaedru{
private:
    punct3D centru = {0,0,0}; double latura = 2;

    int nrVarfuri = 20, nrMuchii = 30;
    punct3D varfuri[20] = {
        {1,1,1},{-1,1,1},{1,-1,1},{1,1,-1},
        {1,-1,-1},{-1,1,-1},{-1,-1,1},{-1,-1,-1},
        {0,phi,invPhi},{0,-phi,invPhi},{0,phi,-invPhi},{0,-phi,-invPhi},
        {invPhi,0,phi},{-invPhi,0,phi},{invPhi,0,-phi},{-invPhi,0,-phi},
        {phi,invPhi,0},{-phi,invPhi,0},{phi,-invPhi,0},{-phi,-invPhi,0}
    };
    bool eVarfSelectat[20] = {false}; int nrVfSelectate = 0;
    pereche muchii[300] = {
        {8,10},{9,11},{12,13},{14,15},{16,18},{17,19},
        {0,8},{0,12},{0,16},{1,8},{1,13},{1,17},
        {2,9},{2,12},{2,18},{3,10},{3,14},{3,16},
        {4,11},{4,14},{4,18},{5,10},{5,15},{5,17},
        {6,9},{6,13},{6,19},{7,11},{7,15},{7,19}
    };
    punct2D proiectii[20];

public:
    Dodecaedru(){}
    Dodecaedru(double Latura){ setLatura(Latura); }
    Dodecaedru(punct3D Centru){ setPozitia(Centru); }
    Dodecaedru(double Latura, punct3D Centru){
        setLatura(Latura); setPozitia(Centru);
    }

    int getNrVarfuri(){ return nrVarfuri; }
    punct2D getProiectie(int i){ return proiectii[i]; }

    punct3D calcCentruSelect(){
        punct3D centruSelect = centru;
        if(nrVfSelectate && nrVfSelectate < nrVarfuri){
            centruSelect = {0,0,0};
            for(int i = 0; i < nrVarfuri; i++){
                if(eVarfSelectat[i]){
                    centruSelect = centruSelect + varfuri[i];
                }
            }
            centruSelect = centruSelect * (1.0 / nrVfSelectate);
        }
        return centruSelect;
    }
    punct3D calcCentru(){
        punct3D cen = {0,0,0};
        for(int i = 0; i < nrVarfuri; i++){
            cen = cen + varfuri[i];
        }
        cen = cen * (1.0 / nrVarfuri);
        return cen;
    }

    void draw(punct3D &obs, double zoom){

        for(int i = 0; i < nrVarfuri; i++){
            proiectii[i] = varfuri[i].proiecteaza(obs);
            dotToScreen(proiectii[i], zoom);
        }

        setcolor(WHITE);
        for(int i = 0; i < nrMuchii; i++){
            line(proiectii[muchii[i].a].x,
                 proiectii[muchii[i].a].y,
                 proiectii[muchii[i].b].x,
                 proiectii[muchii[i].b].y);
        }

        setcolor(COLOR(0, 255, 128));
        setfillstyle(SOLID_FILL, COLOR(255, 128, 0));
        for(int i = 0; i < nrVarfuri; i++){
            if(eVarfSelectat[i]){
                fillellipse(proiectii[i].x, proiectii[i].y,
                            10, 10);
            }
        }
    }

    void roteste(double unghi, int plan){
        punct3D centruSelect = calcCentruSelect();
        for(int i = 0; i < nrVarfuri; i++){
            if(!nrVfSelectate || eVarfSelectat[i]){
                varfuri[i] = varfuri[i] - centruSelect;
                varfuri[i].roteste(unghi, plan);
                varfuri[i] = varfuri[i] + centruSelect;
            }
        }
    }
    void mareste(double marire){
        punct3D centruSelect = calcCentruSelect();
        for(int i = 0; i < nrVarfuri; i++){
            if(!nrVfSelectate || eVarfSelectat[i]){
                varfuri[i] = varfuri[i] - centruSelect;
                varfuri[i] = varfuri[i] * marire;
                varfuri[i] = varfuri[i] + centruSelect;
            }
        }
    }
    void deplaseaza(punct3D &deplasare){
        for(int i = 0; i < nrVarfuri; i++){
            if(!nrVfSelectate || eVarfSelectat[i]){
                varfuri[i] = varfuri[i] + deplasare;
            }
        }
        if(!nrVfSelectate || nrVfSelectate == nrVarfuri){
            centru = centru + deplasare;
        }
        else{
            centru = calcCentru();
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

    void selecteaza(int varf){
        if(!eVarfSelectat[varf]){
            eVarfSelectat[varf] = true; nrVfSelectate++; return;
        }
        eVarfSelectat[varf] = false; nrVfSelectate--;
    }
    void adMuchie(){
        if(nrVfSelectate != 2){ return; }
        int x = -1, y;
        for(int i = 0; i < nrVarfuri; i++){
            if(eVarfSelectat[i]){
                if(x < 0){ x = i; continue; }
                y = i; break;
            }
        }
        pereche per = {x, y};
        muchii[nrMuchii] = per; nrMuchii++;
    }
};

enum stari{ ST_ROTIRE, ST_DEPLASARE, ST_MARIRE, ST_SELECTIE };
const int NR_STARI = 4;
char numeStari[][18] = {"Rotire", "Deplasare", "Redimensionare", "Selectare varfuri"};

int xMov, yMov, xClk = -1, yClk;
char key;

int main(){
    cout<<"Exista 4 moduri de editare: \n\n";
    cout<<"1) Rotire\n2) Deplasare\n3) Redimensionare\n";
    cout<<"4) Selectare varfuri\n\n";
    cout<<"Pentru a schimba modul de editare, apasati tasta spatiu.\n\n";
    cout<<"Pentru primele 2, deplasati mouse-ul pe fereastra in timp ce tineti apasat click stanga, sau folositi tastele 'W', 'S', 'A', 'D', 'Q' si 'E'.\n";
    cout<<"Pentru al 3-lea, deplasati mouse-ul pe fereastra in timp ce tineti apasat click stanga (deplasare spre sus => marire; deplasare spre jos => micsorare)\n";
    cout<<"Pentru al 4-lea, folositi mouse-ul pentru a selecta varfurile (click stanga), apoi puteti selecta 2 varfuri si apasa tasta 'm' pentru a adauga o muchie intre ele (daca nu exista deja), sau puteti schimba modul de editare, iar modificarile (rotiri, deplasari, redimensionari) se vor aplica numai varfului/varfurilor selectate, raportate la centrul lor de greutate.\n\n";
    cout<<"Mod editare curent: Rotire\n";

    initwindow(w, h, "Editor 3D", 300, 200);

    punct3D obs = {0,0,3}; double zoom = 60;
    int stare = ST_ROTIRE;

    Dodecaedru c(2, {0,0,-2});
    cleardevice();
    c.draw(obs, zoom);

    double ajustare = 0.02;
    while(true){
        if(ismouseclick(WM_MOUSEMOVE)){
            getmouseclick(WM_MOUSEMOVE, xMov, yMov);
            if(xClk >= 0 && stare != ST_SELECTIE){
                int dx = xMov - xClk;
                int dy = yMov - yClk;

                if(stare == ST_ROTIRE){
                    c.roteste(-ajustare*dx, XZ);
                    c.roteste(ajustare*dy, YZ);
                }
                else if(stare == ST_DEPLASARE){
                    punct3D depl = {ajustare*dx, -ajustare*dy};
                    c.deplaseaza(depl);
                }
                else{
                    c.mareste(1 - ajustare*dy);
                }

                cleardevice();
                c.draw(obs, zoom);

                xClk = xMov; yClk = yMov;
            }
            continue;
        }
        else if(ismouseclick(WM_LBUTTONDOWN)){
            getmouseclick(WM_LBUTTONDOWN, xClk, yClk);
            if(stare == ST_SELECTIE){
                double distMin; int varfMin;
                for(int i = 0; i < c.getNrVarfuri(); i++){
                    double dx = xClk - c.getProiectie(i).x;
                    double dy = yClk - c.getProiectie(i).y;
                    double dist = dx*dx + dy*dy;

                    if(!i || dist < distMin){
                        distMin = dist; varfMin = i;
                    }
                }
                if(distMin <= 100.0){
                    c.selecteaza(varfMin);

                    cleardevice();
                    c.draw(obs, zoom);
                }
            }
            continue;
        }
        else if(ismouseclick(WM_LBUTTONUP)){
            xClk = -1; clearmouseclick(WM_LBUTTONUP);
            continue;
        }


        else if(kbhit()){
            key = getch();
        }
        else{ continue; }

        if(key == 'm'){
            c.adMuchie();

            cleardevice();
            c.draw(obs, zoom);
        }
        else if(key == ESC){ break; }
        else if(key == ' '){
            stare++; stare %= NR_STARI;
            cout<<"Mod editare curent: ";
            cout<<numeStari[stare];
            cout<<"\n"; continue;
        }

        if(stare == ST_ROTIRE){
            if(key == 'w'){
                c.roteste(-ajustare, YZ);
            }
            else if(key == 's'){
                c.roteste(ajustare, YZ);
            }
            else if(key == 'a'){
                c.roteste(ajustare, XZ);
            }
            else if(key == 'd'){
                c.roteste(-ajustare, XZ);
            }
            else if(key == 'q'){
                c.roteste(ajustare, XY);
            }
            else if(key == 'e'){
                c.roteste(-ajustare, XY);
            }
            else{ continue; }
        }
        else if(stare == ST_DEPLASARE){
            punct3D depl;
            if(key == 'w'){
                depl = {0, ajustare, 0};
            }
            else if(key == 's'){
                depl = {0, -ajustare, 0};
            }
            else if(key == 'a'){
                depl = {-ajustare, 0, 0};
            }
            else if(key == 'd'){
                depl = {ajustare, 0, 0};
            }
            else if(key == 'q'){
                depl = {0, 0, ajustare};
            }
            else if(key == 'e'){
                depl = {0, 0, -ajustare};
            }
            else{ continue; }

            c.deplaseaza(depl);
        }
        else{
            if(key == '+'){
                c.mareste(ajustare+1);
            }
            else if(key == '-'){
                c.mareste(1-ajustare);
            }
            else{ continue; }
        }

        cleardevice();
        c.draw(obs, zoom);
    }

    //getch();
    closegraph();
    return 0;
}
