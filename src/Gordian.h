/*NAME: KANGLI CHU COURSE: CSE788-----Assignment 3*/
#ifndef _GORDIAN_H_
#define _GORDIAN_H_

#include <ctime>
#include <list>
#include <vector>

#include "./include/armadillo"
#include "EasyBMP.h"

#define lim_K 5  // limitation of cells' nbr for each block
#define PINTOTAL 16
#define wv 1  // weight
#define ratio_H 1.05
#define ratio_L 0.95

using namespace std;
using namespace arma;

typedef struct {
    int pos_x;
    int pos_y;
} PinPos_Def;

typedef struct {
    vector<int> netIDs;
    int pinpos[PINTOTAL][2];  // ALL PINS FOR ONE CELL(0,1)
    int netnbr;
    int shuffledID;
    int cellID;       // id for the cell in the calculation
    int cellID_Org;   // original ID for the cell
    int cellsize[2];  //(2,2)
    int pinnbr;       // record nbr of pin for this cell
    float cellsizesquar;
    float cellpos[2];  //(0,1) F, center position of this cell
    int region;        // belong to which region, 1~n
    char fixflag;      // 1: fixed, 0: moveable
    char tag;          // for partition purpose
    char reserve[2];
} Cell_Def;

typedef struct {
    vector<Cell_Def *> cellInd;  // index of cell in the net [CELLTOTAL]
    vector<int *> pinInd;  // index of pin of one cell, pin pos for this net
    float netpos[2];
    int cellnbr;  // nbr of cell in this net
    int netID;
} Net_Def;

typedef struct {
    vector<float> C_row;
} C_Matrix_Def;
typedef struct {
    vector<float> dxy;
    int original_ID;  // record the original id
    int new_ID;       // for calculate, shuffle
} Dxy_Def;
typedef struct {
    int original;
    int newplace;
} PosRow_Def;
typedef struct {
    vector<PosRow_Def> row_pos;
    int cellnbr;
} PosShuffle_Record_Def;
typedef struct {
    list<int> cellID_org;
    int cellnbr;
    //	int n;//for counting only
} MatrixInfo_Def;
typedef struct {
    vector<int> cellsum;  // the total nbr of cell in one typical region
    vector<float> x;
    vector<float> y;
    vector<float> edge_topX;
    vector<float> edge_botX;
    vector<float> edge_topY;
    vector<float> edge_botY;
    vector<int> cutdir_flag;  // 0:compare X-----1:compare Y
    int region_cnt;           // for counting purpose
    //	vector<float> regionsize;
    // vector<int> regionID;
    // int region_nbr;
    //	vector<int>cutcheck_flag;//0: no cut for this region; 1: cut are needed
} Region_Def;
class Gordian {
   public:
    Gordian();
    void processFunc();

   private:
    char netlist_read();
    char getheader(string line);
    char getbody(string line);
    char set_up_obj();
    int getdatanbr(string line, string::size_type pos);
    void reCalCellID();
    void structCreat();
    void shuffle_();
    void beforShuffle();
    void calNetpos();
    float calNetlenght();
    int partition(vector<MatrixInfo_Def> rowinfo_A, mat inputx, mat inputy);
    float ratioCal(vector<MatrixInfo_Def> rowinfo_A, int reg, float *max_x,
                   float *max_y, float *peak_ratio, char *max_dir);
    float ratioCal_2(vector<MatrixInfo_Def> rowinfo_A, int reg, float *max_x,
                     float *max_y, float *peak_ratio, char *max_dir);
    void moveCells(vector<MatrixInfo_Def> rowinfo_A, int reg,
                   float *region_w_new, float *region_w_old, float *sqare_new,
                   float *sqare_old);
    void matrixCal(mat input_dxy, mat C, mat input_D, mat inv_D, mat input_B,
                   mat input_Z, mat tranZ, mat *output_xy,
                   int xory);  // xory:0 x; 1 y
    void plotBMP(int *fileID);
    void plot_a_Cell(Cell_Def cell, BMP *img);
    void plot_a_Net(Net_Def net, BMP *img);
    void plot_edges(BMP *img);
    int region_nbr;      // record the total nbr of the subregions
    unsigned int N_cnt;  // current N(?)
    unsigned int C_cnt;  // cell #
    unsigned int Num_Cells;
    unsigned int Num_Mcells;
    unsigned int Num_Fcells;
    unsigned int Num_Nets;
    unsigned int W;
    unsigned int H;
    float parti_Step;
    clock_t timer[3];
    Region_Def Region_Pos;
    vector<Cell_Def> Cells;  // all the cell data [CELLTOTAL]
    vector<Net_Def> Nets;    // netlist data  [NETTOTAL]  ;
    vector<float> d_x;       //[CELLTOTAL]
    vector<float> d_y;       //[CELLTOTAL]
    mat C_matrix;
};

#endif
