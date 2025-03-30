#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cstdio>
#include <cmath>
#include <ctime>
#include "EasyBMP_Geometry.h"
#include "EasyBMP.h"
#include "Gordian.h"
#include "./include/armadillo"

using namespace std;
using namespace arma;

Gordian::Gordian()
{
	N_cnt = 0;
	C_cnt = 0;
	region_nbr = 1;
	//Region_Pos.cut_flag = 0;
}
/*
return -1: file open fail
*/
char Gordian::netlist_read()
{
	printf("input the name of netlist file.(include file name extension)\n");
	string netlistfilename;
	cin >> netlistfilename;
	ifstream netlist_file(netlistfilename);
	if (!netlist_file.is_open())
	{
		printf("file open failed!\n");
		return -1;//error
	}
	string line;
	while (getline(netlist_file, line))
	{
		char headertmp;
		if ((headertmp=getheader(line))<1)
		{
			if (headertmp == 0){
				break;
			}
			else
				while (1);//error todo
		}
	}
	structCreat();
	while (getline(netlist_file, line))
	{
		getbody(line);
	}
	netlist_file.close();
}
char Gordian::getheader(string line)
{
	string::size_type pos;
	pos = line.find("Num_Cells=");
	if (pos != string::npos)
	{
		Num_Cells = getdatanbr(line,pos+10);
		return 1;
	}
	pos = line.find("Num_Mcells=");
	if (pos != string::npos)
	{
		Num_Mcells = getdatanbr(line, pos + 11);
		return 1;
	}
	pos = line.find("Num_Fcells=");
	if (pos != string::npos)
	{
		Num_Fcells = getdatanbr(line, pos + 11);
		return 1;
	}
	pos = line.find("Num_Nets=");
	if (pos != string::npos)
	{
		Num_Nets = getdatanbr(line, pos + 9);
		return 1;
	}
	pos = line.find("W=");
	if (pos != string::npos)
	{
		W = getdatanbr(line, pos + 2);
		return 1;
	}
	pos = line.find("H=");
	if (pos != string::npos)
	{
		H = getdatanbr(line, pos + 2);
		return 0;
	}
	printf("header error!\n");
	return -1;//error
}
//*
char Gordian::getbody(string line)
{
	switch (line[0])
	{
	case 'C': C_cnt = getdatanbr(line, 2);//get cell nbr
		Cells.at(C_cnt).cellID = C_cnt;//get id
		Cells.at(C_cnt).cellID_Org = C_cnt;//get id
		Nets.at(N_cnt).cellInd.push_back(&Cells[C_cnt]);//[Nets[N_cnt].cellnbr] = ;
		Cells.at(C_cnt).netIDs.push_back(N_cnt);//record nets connect to the cell
		string::size_type pos;
		pos = line.find('(',2);
		float sizew, sizeh;
		sizew=Cells.at(C_cnt).cellsize[0] = getdatanbr(line, pos + 1);
		pos = line.find(',', pos + 1);
		sizeh=Cells.at(C_cnt).cellsize[1] = getdatanbr(line, pos + 1);
		Cells.at(C_cnt).cellsizesquar = sizew*sizeh;
		int dx, dy;
		pos = line.find('(', pos+2);
		dx = getdatanbr(line, pos + 1);
		pos = line.find(',', pos + 1);
		dy = getdatanbr(line, pos + 1);
		pos = line.find('F');
		if (pos == string::npos)
		{
			Cells.at(C_cnt).fixflag = 0;//moveable
		}
		else
		{
			Cells.at(C_cnt).fixflag = 1;//fixed
			Cells.at(C_cnt).cellpos[0] = getdatanbr(line, pos + 2);
			pos = line.find(',', pos + 2);
			Cells.at(C_cnt).cellpos[1] = getdatanbr(line, pos + 1);
		}
		int i;
		for (i = 0; i <= Cells.at(C_cnt).pinnbr; i++)
		{
			if ((dx == Cells.at(C_cnt).pinpos[i][0]) && (dy == Cells.at(C_cnt).pinpos[i][1]))
			{
				Nets.at(N_cnt).pinInd.push_back(Cells.at(C_cnt).pinpos[i]);//get index of pin of the cell for this net [N_cnt]
				break;
			}
			if (i == Cells.at(C_cnt).pinnbr)
			{
				Cells.at(C_cnt).pinpos[i][0] = dx;
				Cells.at(C_cnt).pinpos[i][1] = dy;
				Nets[N_cnt].pinInd.push_back(Cells.at(C_cnt).pinpos[i]);
				break;
			}
		}
		Cells.at(C_cnt).pinnbr++;
		Cells.at(C_cnt).netnbr++;
		Nets.at(N_cnt).cellnbr++;
		break;
	case 'N': N_cnt = getdatanbr(line, 2);
		Nets.at(N_cnt).netID = N_cnt;
		break;
	default:printf("netlist file error!\n");//todo, impossible
		return -1;
	}
	return 1;
}
//*/
int Gordian::getdatanbr(string line, string::size_type pos)
{
	string::size_type tmp;
	string str;
	str.clear();
	for (tmp = pos; tmp < line.size(); tmp++)
	{
		if ((line[tmp] >= '0') && (line[tmp] <= '9')||(line[tmp]=='-'))
			str += line[tmp];
		else
			break;
	}
	return stoi(str);
}
void Gordian::structCreat()
{
	/*build Cells and Nets structure*/
	int it;
	Nets.resize(Num_Nets + 1);
	for (it = 0; it <= Num_Nets; it++)
	{
		Nets.at(it).cellInd.clear();
		Nets.at(it).pinInd.clear();
		Nets.at(it).cellnbr = 0;
		Nets.at(it).netID = 0;
	}
	Cells.resize(Num_Cells);
	for (it = 0; it < Num_Cells; it++)
	{
		Cells.at(it).cellID = 0;
		Cells.at(it).cellID_Org = 0;
		Cells.at(it).cellpos[0] = H/2;
		Cells.at(it).cellpos[1] = W/2;
		Cells.at(it).fixflag = 0;
		Cells.at(it).netIDs.clear();
		Cells.at(it).netnbr = 0;
		Cells.at(it).pinnbr = 0;
		Cells.at(it).cellsize[0] = 0;
		Cells.at(it).cellsize[1] = 0;
		Cells.at(it).cellsizesquar = 0;
		Cells.at(it).region = 0;//initial to first region,ie. the original region for the first level
		Cells.at(it).tag = -1;
		memset(Cells.at(it).pinpos, 0, sizeof(Cells.at(it).pinpos));
	}
}
void Gordian::reCalCellID()
{
	unsigned int i;
	int cnt = 0;
	for (i = 0; i < Num_Cells; i++)
	{
		if (Cells.at(i).fixflag == 1)
		{
			cnt++;
		}
		else
		{
			Cells.at(i).cellID -= cnt;
		}
	}
}

char Gordian::set_up_obj()
{
	int i, j, k;
	C_matrix.set_size(Num_Mcells, Num_Mcells);
	C_matrix.zeros();
	d_x.assign(Num_Mcells,0);//push_back(0);
	d_y.assign(Num_Mcells,0);//push_back(0);
	float e;
	int v;
	for (v = 1; v <= Num_Nets; v++)
	{
		e = 2.0 / (float)Nets.at(v).cellnbr;//wv *
		int u,c;
		for (j = 0; j < Nets.at(v).cellnbr; j++)
		{
			u = Nets.at(v).cellInd.at(j)->cellID_Org;//

			if (Cells.at(u).fixflag == 1)//moveable check, fixed
				continue;
			else
				c = Cells.at(u).cellID;//for calculate
			C_matrix(c,c) += e*(Nets.at(v).cellnbr - 1);//.at(c).C_row.at(c)
			d_x.at(c) += Nets[v].pinInd.at(j)[0] * e*(Nets.at(v).cellnbr - 1);
			d_y.at(c) += Nets[v].pinInd.at(j)[1] * e*(Nets.at(v).cellnbr - 1);
			int landa,u_landa,c2;
			for (landa = 0; landa < Nets.at(v).cellnbr; landa++)
			{
				u_landa = Nets.at(v).cellInd.at(landa)->cellID_Org;//[landa]
				if (u_landa == u)
					continue;
				else
					c2 = Cells.at(u_landa).cellID;
				if (Cells.at(u_landa).fixflag == 0)//moveable check, moveable
				{
					C_matrix(c,c2) -= e;//[u][u_landa].at(c).C_row.at(c2)
					d_x.at(c) -= (float)Nets.at(v).pinInd.at(landa)[0] * e;
					d_y.at(c) -= (float)Nets.at(v).pinInd.at(landa)[1] * e;
				}
				else//fixed
				{
					d_x.at(c) -= e*(float)(Cells.at(u_landa).cellpos[0] + Nets.at(v).pinInd.at(landa)[0]);
					d_y.at(c) -= e*(float)(Cells.at(u_landa).cellpos[1] + Nets.at(v).pinInd.at(landa)[1]);
				}
			}
		}
	}
//	C_matrix.save("C_Matrix.txt", raw_ascii);
	return 1;
}

void Gordian::shuffle_()
{
	mat dx_cal(Num_Mcells,1);
	mat dy_cal(Num_Mcells,1);
	mat Cmatrix_cal;//create a temp C matrix for calculation purpose
	Cmatrix_cal.set_size(Num_Mcells, Num_Mcells);
	mat Cmatrix_tmp;//create temp matrix
	Cmatrix_tmp.set_size(Num_Mcells, Num_Mcells);
	list<int>::iterator it;
	int fileId = 0;
	int partition_flag = -1;
	/*loop begin*/
	while (1)
	{
		vector<MatrixInfo_Def>rowinfo_A;// for recording info of matrix A for future cal A,dx,C matrix
		rowinfo_A.resize(region_nbr);
		for (int i = 0; i < region_nbr; i++)//initial rowinfo
		{//i means nbr of row
			rowinfo_A.at(i).cellnbr = 0;
		}
		for (int i = 0; i < Num_Cells; i++)//get info for matrix A
		{
			if (Cells[i].fixflag == 1)
				continue;
			it = rowinfo_A.at(Cells[i].region).cellID_org.begin();//get the iterator of first element
			if (rowinfo_A.at(Cells[i].region).cellID_org.size() == 0)
			{
				rowinfo_A.at(Cells[i].region).cellID_org.push_back(i);
			}
			else
			{
				if (Cells.at(i).cellsizesquar > Cells.at(*it).cellsizesquar)
					rowinfo_A.at(Cells[i].region).cellID_org.insert(it, i);
				else
					rowinfo_A.at(Cells[i].region).cellID_org.push_back(i);
			}
			rowinfo_A.at(Cells[i].region).cellnbr++;
		}
		mat A;//create A matrix
		A.set_size(region_nbr, Num_Mcells);
		A.zeros();//fill A with '0'
		int colpos = 0;//to accumulate the pos change in the previous rows
		for (int i = 0; i < region_nbr; i++)//bulid shuffled A matrix,dx,dy,C matrix
		{	//build A matrix
			if (i>0)
				colpos += rowinfo_A.at(i - 1).cellnbr-1;
			it = rowinfo_A.at(i).cellID_org.begin();//get the iterator of first element
			A(i, i) = Cells[*it].cellsizesquar;
			dx_cal(i, 0) = d_x.at(Cells[*it].cellID);
			dy_cal(i, 0) = d_y.at(Cells[*it].cellID);
			Cmatrix_tmp.row(i) = C_matrix.row(Cells[*it].cellID);//ROW shuffle first
			for (int j = 0; j < (rowinfo_A.at(i).cellnbr - 1); j++)
			{
				it++;
				A(i, j + colpos + region_nbr) = Cells[*it].cellsizesquar;
				dx_cal(j + colpos + region_nbr, 0) = d_x.at(Cells[*it].cellID);
				dy_cal(j + colpos + region_nbr, 0) = d_y.at(Cells[*it].cellID);
				Cmatrix_tmp.row(j + colpos + region_nbr) = C_matrix.row(Cells[*it].cellID);
			}
		}
		colpos = 0;
		for (int i = 0; i < region_nbr; i++)//Cmatrix_cal column shuffle
		{
			if (i>0)
				colpos += rowinfo_A.at(i - 1).cellnbr-1;
			it = rowinfo_A.at(i).cellID_org.begin();//get the iterator of first element
			Cmatrix_cal.col(i) = Cmatrix_tmp.col(Cells[*it].cellID);//column shuffle first
			for (int j = 0; j < (rowinfo_A.at(i).cellnbr - 1); j++)
			{
				it++;
				Cmatrix_cal.col(j + colpos + region_nbr) = Cmatrix_tmp.col(Cells[*it].cellID);
			}
		}
		mat tmp;
		tmp = sum(A, 1);//get weight for each row vector
		for (int i = 0; i < region_nbr; i++)//to get the weighted matrix A
		{
			A.row(i) = (1 / tmp.row(i)(0))*A.row(i);
		}
		mat I, D, B, Z, ZT, x, y, invD, tranZ;//get all the matrix
		D = A.submat(0, 0, region_nbr - 1, region_nbr - 1);// .submat(first_row, first_column, last_row, last_column)
		B = A.submat(0, region_nbr, region_nbr - 1, Num_Mcells - 1);
		I = eye(Num_Mcells - region_nbr, Num_Mcells - region_nbr);
		invD = inv(D);
		Z = join_cols(-invD*B, I);//same for x,y
		tranZ = trans(Z);
		matrixCal(dx_cal, Cmatrix_cal, D, invD, B, Z, tranZ, &x, 0);
		matrixCal(dy_cal, Cmatrix_cal, D, invD, B, Z, tranZ, &y, 1);
		calNetpos();
		fileId++;
		if (partition_flag == 0)
		{
			plotBMP(&fileId);
			printf("Overall iterations: %d\n", fileId-1);
			printf("Overall wire length is %.3f(include fixed cells)\n",calNetlenght());//cal length of nets
			printf("Calculation end!\n");
			break;
		}
		plotBMP(&fileId);
		partition_flag = partition(rowinfo_A, x, y);
		if (partition_flag == 0)
		{
			printf("partition finish!\n");
		}
//		x.save("x.txt", raw_ascii);
//		y.save("y.txt", raw_ascii);
	}
	/*loop end*/
}
void Gordian::matrixCal(mat input_dxy, mat C, mat input_D, mat inv_D, mat input_B, mat input_Z, mat tranZ, mat *output_xy, int xory)//xory:0 x; 1 y
{
	mat ZT, c, cT, xy0, xyi, xy, uxy(region_nbr, 1), zeromat(Num_Mcells - region_nbr, 1);
	zeromat.zeros();
	for (int i = 0; i < region_nbr; i++)
	{
		if (xory==0)
			uxy(i, 0) = Region_Pos.x.at(i);
		else
			uxy(i, 0) = Region_Pos.y.at(i);
	}
	xy0 = join_cols(inv_D*uxy, zeromat);
	cT = trans(C*xy0 + input_dxy)*input_Z;
	c = trans(cT);
	xyi = -inv((tranZ*C*input_Z))*c;
	*output_xy = input_Z*xyi + xy0;
}
//return status of partition: >=1 -> more partition; =0 -> no more partition
int Gordian::partition(vector<MatrixInfo_Def> rowinfo_A, mat inputx, mat inputy)
{
	list<int>::iterator it;
	int colpos = 0;
	for (int i = 0; i < region_nbr; i++)//bulid shuffled A matrix,dx,dy,C matrix
	{	//get info back & assign coordinates to Cells
		if (i>0)
			colpos += rowinfo_A.at(i - 1).cellnbr-1;
		it = rowinfo_A.at(i).cellID_org.begin();//get the iterator of first element
		Cells[*it].cellpos[0] = inputx(i, 0);
		Cells[*it].cellpos[1] = inputy(i, 0);
		for (int j = 0; j < (rowinfo_A.at(i).cellnbr - 1); j++)
		{
			it++;
			Cells[*it].cellpos[0] = inputx(j + colpos + region_nbr, 0);
			Cells[*it].cellpos[1] = inputy(j + colpos + region_nbr, 0);
		}
	}
		parti_Step *= 2;
		int reg,reg_left=0;
		for (reg = 0; reg < region_nbr; reg++)
		{
			if (Region_Pos.cellsum.at(reg) <= lim_K)//less than lim_K threshold , no cut for this region
				continue;
			reg_left++;
			Region_Pos.region_cnt++;//could not be cleared! always grow up!
			if (Region_Pos.cellsum.size() < (Region_Pos.region_cnt + 1))
				Region_Pos.cellsum.push_back(0);
			else
				Region_Pos.cellsum.at(Region_Pos.region_cnt) = 0;
			float d_edgex, d_edgey;
			d_edgex = Region_Pos.edge_topX.at(reg) - Region_Pos.edge_botX.at(reg);
			d_edgey = Region_Pos.edge_topY.at(reg) - Region_Pos.edge_botY.at(reg);
			if (d_edgex >= d_edgey)
				Region_Pos.cutdir_flag.push_back(0);
			else
				Region_Pos.cutdir_flag.push_back(1);
			parti_Step *= 2;
			float max_x = 0, max_y = 0, peak_ratio = 0;
			char max_dir = 0;
			float coe = 1;
			float ratio;
			unsigned char coeflag=0x00;
			while (1)
			{
		//		if (Region_Pos.cellsum.at(reg) > 200)
		//			ratio = ratioCal(rowinfo_A, reg, &max_x, &max_y, &peak_ratio,&max_dir);
	//			else
					ratio = ratioCal_2(rowinfo_A, reg, &max_x, &max_y, &peak_ratio, &max_dir);
				if ((ratio < ratio_L)||(ratio>ratio_H))//predefined threshold
				{//move cutline closer to 0
					float sign,val;
					char preflag;
					preflag = coeflag;
					if ((ratio - 1) < 0)
					{
						sign = -1;
						coeflag |= 0x01;
						preflag |= 0x01;
					}
					else
					{
						sign = 1;
						coeflag |= 0x10;
						preflag |= 0x10;
					}
					val = sign*(ratio - 1);
					if (coeflag == 0x11)
					{
						coe /= 2.0f;//decrease

						coeflag = preflag;
					}
					if (coe < 0.001)
					{
						coe = 1;
					}
					if (Region_Pos.cutdir_flag.back())
					{
						Region_Pos.y.at(reg) += sign*parti_Step*coe*val;
					}
					else
					{
						Region_Pos.x.at(reg) += sign*parti_Step*coe*val;
					}
				}
				else
				{
					if (Region_Pos.cutdir_flag.back())
					{
						Region_Pos.edge_botX.push_back(Region_Pos.edge_botX.at(reg));
						Region_Pos.edge_topX.push_back(Region_Pos.edge_topX.at(reg));
						Region_Pos.edge_topY.push_back(Region_Pos.edge_topY.at(reg));
						Region_Pos.edge_botY.push_back(Region_Pos.y.at(reg));
						Region_Pos.edge_topY.at(reg) = Region_Pos.y.at(reg);
						Region_Pos.x.push_back(Region_Pos.x.at(reg));//x remain unchanged
						Region_Pos.y.push_back((Region_Pos.edge_topY.at(Region_Pos.region_cnt) + Region_Pos.y.at(reg)) / 2);//new center for new region
						Region_Pos.y.at(reg) = (Region_Pos.y.at(reg) + Region_Pos.edge_botY.at(reg)) / 2;//new center for old region
						break;
					}
					else
					{
						Region_Pos.edge_botX.push_back(Region_Pos.x.at(reg));
						Region_Pos.edge_topX.push_back(Region_Pos.edge_topX.at(reg));
						Region_Pos.edge_topY.push_back(Region_Pos.edge_topY.at(reg));
						Region_Pos.edge_botY.push_back(Region_Pos.edge_botY.at(reg));
						Region_Pos.edge_topX.at(reg) = Region_Pos.x.at(reg);
						Region_Pos.y.push_back(Region_Pos.y.at(reg));//y remain unchanged
						Region_Pos.x.push_back((Region_Pos.edge_topX.at(Region_Pos.region_cnt) + Region_Pos.x.at(reg)) / 2);//new center for new region
						Region_Pos.x.at(reg) = (Region_Pos.x.at(reg) + Region_Pos.edge_botX.at(reg)) / 2;//new center for old region
						break;
					}
				}
			}
		}
//		Region_Pos.cutdir_flag ^= 1;
		region_nbr = Region_Pos.region_cnt + 1;
	return reg_left;
}
void Gordian::moveCells(vector<MatrixInfo_Def> rowinfo_A, int reg, float *region_w_new, float *region_w_old, float *sqare_new, float *sqare_old)
{
	list<int>::iterator it;
	Region_Pos.cellsum.at(Region_Pos.region_cnt) = 0;
	Region_Pos.cellsum.at(reg) = 0;
	*sqare_new = 0; *sqare_old = 0;
	*region_w_new = 0; *region_w_old = 0;
	it = rowinfo_A.at(reg).cellID_org.begin();
	for (int j = 0; j < rowinfo_A.at(reg).cellnbr; j++)
	{
		switch (Region_Pos.cutdir_flag.back())
		{
		case 0://cut X
			if (Cells[*it].cellpos[0] > Region_Pos.x.at(reg))
			{
				Cells[*it].region = Region_Pos.region_cnt;//new region
				Region_Pos.cellsum.at(Region_Pos.region_cnt)++;//increase cellsum for new region
				*sqare_new += Cells[*it].cellsizesquar;
			}
			else//belong to old region
			{
				Cells[*it].region = reg;//old region
				Region_Pos.cellsum.at(reg)++;
				*sqare_old += Cells[*it].cellsizesquar;
			}
			break;
		case 1://cut Y
			if (Cells[*it].cellpos[1] > Region_Pos.y.at(reg))//
			{
				Cells[*it].region = Region_Pos.region_cnt;//new region
				Region_Pos.cellsum.at(Region_Pos.region_cnt)++;//increase cellsum for new region
				*sqare_new += Cells[*it].cellsizesquar;
			}
			else//belong to old region
			{
				Cells[*it].region = reg;//old region
				*sqare_old += Cells[*it].cellsizesquar;
				Region_Pos.cellsum.at(reg)++;
			}
			break;
		default:while (1);//impossible
		}
		it++;
	}
	if (Region_Pos.cutdir_flag.back())
	{
		*region_w_old = Region_Pos.y.at(reg) - Region_Pos.edge_botY.at(reg);
		*region_w_new = Region_Pos.edge_topY.at(reg) - Region_Pos.y.at(reg);
	}
	else
	{
		*region_w_old = Region_Pos.x.at(reg) - Region_Pos.edge_botX.at(reg);
		*region_w_new = Region_Pos.edge_topX.at(reg) - Region_Pos.x.at(reg);
	}
}
float Gordian::ratioCal_2(vector<MatrixInfo_Def> rowinfo_A, int reg, float *max_x, float *max_y, float *peak_ratio, char *max_dir)
{
	/*loop*/
	float sqare_new = 0, sqare_old = 0;
	float region_w_new = 0, region_w_old = 0;//width of the region
	float ratio;
	float sign = 0;
	while (1)
	{
		if (sign == 0)
		{
			if (Region_Pos.cutdir_flag.back())
			{
				Region_Pos.y.at(reg) -= 0.5f;//*parti_Step
			}
			else
			{
				Region_Pos.x.at(reg) -= 0.5f;//*parti_Step
			}
		}
		moveCells(rowinfo_A, reg, &region_w_new, &region_w_old, &sqare_new, &sqare_old);
		if (sign == 0)
		{
			if (sqare_old < sqare_new)
				sign = 1;
			else if (sqare_old > sqare_new)
				sign = -1;
			else{ratio = 1;break;}
		}
		/*limit condition*/
		if ((region_w_old <= 2) || (region_w_new <= 2))//area limit
			*max_dir = 1;
		else if ((sqare_new < 4) || (sqare_old < 4))//cell nbr limit
			*max_dir = 1;
		else
			ratio = (region_w_old*sqare_new) / (sqare_old *region_w_new);
		if (*max_dir == 1)
		{
			ratio = 1;
			if (Region_Pos.cutdir_flag.back())
				Region_Pos.y.at(reg) = *max_y;
			else
				Region_Pos.x.at(reg) = *max_x;
//			printf("%.2f\n", *peak_ratio);//check
			moveCells(rowinfo_A, reg, &region_w_new, &region_w_old, &sqare_new, &sqare_old);
			break;/*difference between cal_1*/
		}
		if (abs(ratio - 1) < abs(*peak_ratio - 1))
		{
			*peak_ratio = ratio;
			if (Region_Pos.cutdir_flag.back())
				*max_y = Region_Pos.y.at(reg);
			else
				*max_x = Region_Pos.x.at(reg);
		}
		if (Region_Pos.cutdir_flag.back())
		{
			Region_Pos.y.at(reg) += sign / Region_Pos.cellsum.at(reg);//*parti_Step
		}
		else
		{
			Region_Pos.x.at(reg) += sign / Region_Pos.cellsum.at(reg);//*parti_Step
		}
	}
	return 1;//return the peak point val and set ratio as 1 to end the check
}
float Gordian::ratioCal(vector<MatrixInfo_Def> rowinfo_A, int reg, float *max_x, float *max_y, float *peak_ratio, char *max_dir)
{
//	float max_x = 0, max_y = 0, peak_ratio = 0;
//	char max_dir = 0;
//	float coe = 1;
	float ratio;
//	list<int>::iterator it;
	Region_Pos.cellsum.at(Region_Pos.region_cnt) = 0;
	Region_Pos.cellsum.at(reg) = 0;
	float sqare_new = 0, sqare_old = 0;
	float region_w_new = 0, region_w_old = 0;//width of the region
	moveCells(rowinfo_A, reg, &region_w_new, &region_w_old, &sqare_new, &sqare_old);
	/*limit condition*/
	if ((region_w_old <= 2) || (region_w_new <= 2))//area limit
		*max_dir = 1;
	else if ((sqare_new < 4) || (sqare_old < 4))//cell nbr limit
		*max_dir = 1;
	else
		ratio = (region_w_old*sqare_new) / (sqare_old *region_w_new);
	if (*max_dir == 1)
	{
		ratio = 1;
		if (Region_Pos.cutdir_flag.back())
			Region_Pos.y.at(reg) = *max_y;
		else
			Region_Pos.x.at(reg) = *max_x;
		moveCells(rowinfo_A, reg, &region_w_new, &region_w_old, &sqare_new, &sqare_old);
	}
	if (abs(ratio - 1)<abs(*peak_ratio - 1))
	{
		*peak_ratio = ratio;
		if (Region_Pos.cutdir_flag.back())
			*max_y = Region_Pos.y.at(reg);
		else
			*max_x = Region_Pos.x.at(reg);
	}
	return ratio;
}
void Gordian::plotBMP(int *fileID)
{
	BMP image;
	RGBApixel color;//black
	color.Blue = 0;
	color.Green = 0;
	color.Red = 0;
	unsigned int hig, wid;
	hig = H * 10+400;
	wid = W * 10+400;
	image.SetSize(hig, wid);
//	DrawFastLine(image, 100, 100, 100, hig-100, color);
//	DrawFastLine(image, 100, 100, wid-100, 100, color);
//	DrawFastLine(image, wid-100, hig-100, wid-100, 100, color);
//	DrawFastLine(image, wid-100, hig-100, 100, hig-100, color);
	for (int i = 0; i < Num_Cells; i++)
	{
		plot_a_Cell(Cells.at(i), &image);
	}
	for (int i = 0; i < Num_Nets; i++)
	{
//		plot_a_Net(Nets.at(i), &image);//uncomm to have connected plots
	}
	plot_edges(&image);
	char filename[32];
	sprintf(filename, "Partition_plot_%d.bmp", *fileID);
	image.WriteToFile(filename);
}
void Gordian::plot_a_Cell(Cell_Def cell, BMP *img)
{
	RGBApixel color;//black
	color.Blue = 0;
	color.Green = 0;
	color.Red = 255;
	int x0, x1, y0, y1;
	x0 = (int)(10.f * (cell.cellpos[1] - cell.cellsize[1] / 2)) + 200;
	x1 = (int)(10.f * (cell.cellpos[1] + cell.cellsize[1] / 2)) + 200;
	y0 = (int)(10.f * (cell.cellpos[0] - cell.cellsize[0] / 2)) + 200;
	y1 = (int)(10.f * (cell.cellpos[0] + cell.cellsize[0] / 2)) + 200;
	DrawFastLine(*img, x0, y0, x1, y0, color);
	DrawFastLine(*img, x0, y0, x0, y1, color);
	DrawFastLine(*img, x1, y1, x0, y1, color);
	DrawFastLine(*img, x1, y1, x1, y0, color);
}
void Gordian::plot_a_Net(Net_Def net, BMP *img)
{
	RGBApixel color;//green
	color.Blue = 0;
	color.Green = 255;
	color.Red = 0;
	int x0, x1, y0, y1;
	for (int i = 0; i < net.cellnbr; i++)
	{
		x0 = (int)(net.netpos[1] * 10.f) + 200;
		y0 = (int)(net.netpos[0] * 10.f) + 200;
		x1 = (int)((net.cellInd.at(i)->cellpos[1] + net.pinInd.at(i)[1])*10.f) + 200;
		y1 = (int)((net.cellInd.at(i)->cellpos[0] + net.pinInd.at(i)[0])*10.f) + 200;
		DrawFastLine(*img, x0, y0, x1, y1, color);
	}
}
void Gordian::plot_edges(BMP* img)
{
	RGBApixel color;//black
	color.Blue = 0;
	color.Green = 0;
	color.Red = 0;
	int x0, x1, y0, y1;
	for (int i = 0; i <= Region_Pos.region_cnt; i++)
	{
		x0 = (int)(10.f * (Region_Pos.edge_botY.at(i))) + 200;
		x1 = (int)(10.f * (Region_Pos.edge_topY.at(i))) + 200;
		y0 = (int)(10.f * (Region_Pos.edge_botX.at(i))) + 200;
		y1 = (int)(10.f * (Region_Pos.edge_topX.at(i))) + 200;
		DrawFastLine(*img, x0, y0, x1, y0, color);
		DrawFastLine(*img, x0, y0, x0, y1, color);
		DrawFastLine(*img, x1, y1, x0, y1, color);
		DrawFastLine(*img, x1, y1, x1, y0, color);
	}
}
void Gordian::calNetpos()
{
	for (int i = 0; i < Num_Nets; i++)
	{
		Nets[i].netpos[0] = 0;
		Nets[i].netpos[1] = 0;
		for (int j = 0; j < Nets[i].cellnbr; j++)
		{
			Nets[i].netpos[0] += Nets[i].cellInd.at(j)->cellpos[0] + Nets[i].pinInd.at(j)[0];
			Nets[i].netpos[1] += Nets[i].cellInd.at(j)->cellpos[1] + Nets[i].pinInd.at(j)[1];
		}
		Nets[i].netpos[0] /= Nets[i].cellnbr;
		Nets[i].netpos[1] /= Nets[i].cellnbr;
	}
}
float Gordian::calNetlenght()
{
	float netlen = 0;
	for (int i = 0; i < Num_Nets; i++)
	{
		float x1, y1, x0, y0;
		x0 = Nets.at(i).netpos[0];
		y0 = Nets.at(i).netpos[1];
		for (int j = 0; j < Nets.at(i).cellnbr; j++)
		{
			x1 = Nets.at(i).cellInd.at(j)->cellpos[0] + Nets.at(i).pinInd.at(j)[0];
			y1 = Nets.at(i).cellInd.at(j)->cellpos[1] + Nets.at(i).pinInd.at(j)[1];
			netlen += sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));
		}
	}
	return netlen;
}
void Gordian::beforShuffle()
{
	Region_Pos.x.push_back(H / 2);//init region center
	Region_Pos.y.push_back(W / 2);//init region center
	Region_Pos.cellsum.push_back(Num_Mcells);
	Region_Pos.region_cnt = 0;//for counting purpose
	Region_Pos.edge_topX.push_back(H);
	Region_Pos.edge_botX.push_back(0);
	Region_Pos.edge_topY.push_back(W);
	Region_Pos.edge_botY.push_back(0);
	parti_Step = 0.001;
}

void Gordian::processFunc()
{
	netlist_read();
	reCalCellID();
	set_up_obj();
	timer[0] = clock();
	beforShuffle();
	shuffle_();
	timer[0] = clock() - timer[0];
	printf("Time for Gordian iterations: %.2f\n", ((float)timer[0]) / CLOCKS_PER_SEC);
}
