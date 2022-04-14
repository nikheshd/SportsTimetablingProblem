#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include "ilcplex/ilocplex.h"
using namespace std;

int main() {
	int n = 6;
	int m = 2 * (n - 1);
	string gamemode = "P";

	IloEnv env;
	IloModel Model(env);
	IloArray<IloArray<IloBoolVarArray>> x(env, n);


	for (int i = 0; i < n; i++)
	{
		x[i] = IloArray<IloBoolVarArray>(env, n);
		for (int j = 0; j < n; j++)
		{
			x[i][j] = IloBoolVarArray(env, m);
		}
	}

	for (int i = 0; i < n; i++)
	{
		for (int k = 0; k < m; k++)
		{
			IloExpr B1_lhs(env);
			for (int j = 0; j < n; j++)
			{
				if (i == j) {
					continue;
				}
				B1_lhs += x[i][j][k] + x[j][i][k];
			}
			Model.add(B1_lhs == 1);
		}
	}

	//gamemode: NULL
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (i == j) {
				continue;
			}
			IloExpr B2_lhs(env);
			for (int k = 0; k < m; k++)
			{
				B2_lhs += x[i][j][k];
			}
			Model.add(B2_lhs == 1);
		}
	}

	//gamemode: Phased
	if (gamemode == "P") {
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (i == j) {
					continue;
				}
				IloExpr B2_lhs(env);
				for (int k = 0; k < n - 1; k++)
				{
					B2_lhs += x[i][j][k] + x[j][i][k];
				}
				Model.add(B2_lhs == 1);
			}
		}
	}

	for (int i = 0; i < n; i++)
	{
		for (int k = 0; k < m; k++)
		{
			Model.add(x[i][i][k] == 0);
		}
	}

	/*
	<CA1 max="0" min="0" mode="H" penalty="1" slots="5" teams="4" type="SOFT"/>
	<CA1 max="1" min="0" mode="H" penalty="1" slots="2;1" teams="5" type="SOFT"/>
	<CA1 max="1" min="0" mode="H" penalty="1" slots="9;7;1" teams="1" type="SOFT"/>*/
	IloExpr ca1_1(env), ca1_2(env), ca1_3(env);
	IloNumVar yca1_1(env, 0, IloInfinity, ILOINT), yca1_2(env, 0, IloInfinity, ILOINT), yca1_3(env, 0, IloInfinity, ILOINT);
	for (int j = 0; j < n; j++)
	{
		ca1_1 += x[4][j][5];
		ca1_2 += x[5][j][2] + x[5][j][1];
		ca1_3 += x[1][j][9] + x[1][j][7] + x[1][j][1];
	}
	Model.add(ca1_1 <= yca1_1);
	Model.add(ca1_2 <= 1 + yca1_2);
	Model.add(ca1_3 <= 1 + yca1_3);

	/*<CA2 teams1 = "0" min = "0" max = "1" mode1 = "HA" mode2 = "GLOBAL" penalty = "1" teams2 = "1;2"
		slots = "0;1;2" type = "SOFT" / >*/
	IloExpr ca2(env);
	IloNumVar yca2(env, 0, IloInfinity, ILOINT);
	ca2 += x[0][1][0] + x[0][2][0] + x[0][1][1] + x[0][2][1] + x[0][1][2] + x[0][2][2];
	Model.add(ca2 <= 1 + yca2);

	/*CA3
	<CA3 intp = "4" max = "2" min = "0" mode1 = "A" mode2 = "SLOTS" penalty = "5" teams1 = "2" teams2 = "0;3;5;4;1" type = "SOFT" / > */
	IloNumVar yca3(env, 0, IloInfinity, ILOINT);
	for (int k = 0; k < m-4; k++)
	{
		IloExpr d1(env);
		for (int i = k; i < k+4; i++)
		{
			d1 += x[2][0][i] + x[2][3][i] + x[2][5][i] + x[2][4][i] + x[2][1][i];
		}
		Model.add(d1 <= 2 + yca3);
	}

	/*<CA4 teams1="0;1" max="3" mode1="H" teams2="2,3" mode2="GLOBAL"
	slots ="0;1" type="SOFT"/>*/

	IloExpr Objectivefn(env);
	Objectivefn += yca1_1 + yca1_2 + yca1_3 + yca2 + 4*yca3;

	Model.add(IloMinimize(env, Objectivefn));
	Objectivefn.end();
	IloCplex cplex(Model);
	cplex.setOut(env.getNullStream());
	cplex.solve();
	cout << "Total penalty: " << cplex.getObjValue() << endl;
	for (int k = 0; k < m; k++)
	{
		cout << "Slot " << k << ": ";
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (cplex.getValue(x[i][j][k]) != 0) {
					cout << "(" << i << ", " << j << "); ";
				}
			}
		}
		cout << "\n";
	}
	return 0;
}