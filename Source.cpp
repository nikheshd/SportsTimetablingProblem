#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "tinyxml2.h"
#include "ilcplex/ilocplex.h"
#include <chrono>
using namespace std;
using namespace tinyxml2;
using namespace std::chrono;

#pragma region Structs and Functions
struct CA1 {
	string type;
	string model;
	int min;
	int max;
	int penalty;
	vector<int>slots;
	vector<int>teams;
};

struct CA2
{
	string type;
	int min;
	int max;
	int penalty;
	vector<int>teams1;
	vector<int>teams2;
	vector<int>slots;
	string model;
};

struct CA3
{
	string type;
	int min;
	int max;
	int intp;
	int penalty;
	vector<int>teams1;
	vector<int>teams2;
	string mode1;
	string mode2;
};

struct CA4
{
	string type;
	int min;
	int max;
	int penalty;
	vector<int>teams1;
	vector<int>teams2;
	vector<int>slots;
	string mode1;
	string mode2;
};

struct GA1
{
	string type;
	int min;
	int max;
	int penalty;
	vector<pair<int, int>>meetings;
	vector<int>slots;


};

struct BR1
{
	string type;
	int intp;
	int penalty;
	vector<int>teams;
	vector<int>slots;
	string mode2;
};
struct BR2
{
	string type;
	int intp;
	int penalty;
	vector<int>teams;
	vector<int>slots;
	string mode;
};

struct FA1
{
	string type;
	int intp;
	int penalty;
	vector<int>slots;
	string mode;
	vector<int>teams;
};

struct SE1
{
	string type;
	int min;
	int penalty;
	vector<int>teams;
};

vector<int> stov(const char* s)
{
	//convert string of type "a1;a2;a3;a4;a5" to vector {a1,a2,a3,a4,a4,a5}
	vector<int>ans;
	int it = 0;
	while (s[it] != '\0')
	{
		int a = (int)(s[it] - '0');
		it++;
		while (s[it] != '\0' && s[it] != ';')
		{
			int b = (int)(s[it] - '0');
			a = 10 * a + b;
			it++;
		}
		ans.push_back(a);
		if (s[it] == ';')
			it++;
	}
	return ans;
}
vector<pair<int, int>>stov2(const char* m)
{
	//convert string(of type "a1,b1;a2,b2;a3,b3") to vector{{a1,b1},{a2,b2},{a3,b3}} 
	vector<pair<int, int>>ans;
	int i = 0;
	while (m[i] != '\0')
	{
		pair<int, int>p;
		int a = (int)(m[i] - '0');
		i++;
		while (m[i] != '\0' && m[i] != ',')
		{
			int at = (int)(m[i] - '0');
			a = 10 * a + at;
			i++;
		}
		//m[i]==','
		i++;
		int b = (int)(m[i] - '0');
		i++;
		while (m[i] != '\0' && m[i] != ';')
		{
			int bt = (int)(m[i] - '0');
			b = 10 * b + bt;
			i++;
		}
		p.first = a;
		p.second = b;
		ans.push_back(p);
		if (m[i] == ';')
			i++;
	}
	return ans;
}
#pragma endregion

int main() {
	int n = 0;
	string InstanceName;
	string GameMode;
	vector<CA1>constraints1;
	vector<CA2>constraints2;
	vector<CA3>constraints3;
	vector<CA4>constraints4;
	vector<GA1>gameconstraints1;
	vector<BR1>breakconstraints1;
	vector<BR2>breakconstraints2;
	vector<FA1>fairnessconstraints1;
	vector<SE1>seperationconstraints1;
	try
	{
		XMLDocument doc;
		doc.LoadFile("ITC2021_Test5.xml");
	#pragma region Read Data
		XMLElement* root = doc.RootElement();
		if (root) 
		{
			XMLElement* instance_name = root->FirstChildElement("MetaData")->FirstChildElement("InstanceName");
			if (instance_name) {
				InstanceName = instance_name->GetText();
			}
			else
			{
				cerr << "error: instancename not found\n";
			}
		}
		if (root)
		{
			XMLElement* teams = root->FirstChildElement("Resources")->FirstChildElement("Teams");
			if (teams)
			{
				XMLElement* team = teams->FirstChildElement("team");
				while (team)
				{
					n++;
					team = team->NextSiblingElement("team");
				}
				//cout << "n=" << n << '\n';
			}
			else
			{
				cerr << "Error: Teams not found\n";
			}
		}
		else
		{
			cerr << "Error :Unable to Load xml file" << "\n";
		}


		if (root)
		{
			XMLElement* mode = root->FirstChildElement("Structure")->FirstChildElement("Format")->FirstChildElement("gameMode");
			if (mode)
			{

				if ((mode->GetText())[0] == 'P')
				{
					//phased constraint holds true
					//each team play every other team exactly once in first half of the tournament
					//sum over k =1 to k =n-1 (x[i][j][k]+x[j][i][k]) ==1 for evry i,j i!=j
					GameMode = "P";
				}
			}
			else
			{
				cerr << "error: gamemode not found\n";
			}
		}

		if (root)
		{
			XMLElement* constraints = root->FirstChildElement("Constraints");
			if (constraints)
			{
				XMLElement* capacityc = constraints->FirstChildElement("CapacityConstraints");
				XMLElement* gamec = constraints->FirstChildElement("GameConstraints");
				XMLElement* breakc = constraints->FirstChildElement("BreakConstraints");
				XMLElement* fairnessc = constraints->FirstChildElement("FairnessConstraints");
				XMLElement* separationc = constraints->FirstChildElement("SeparationConstraints");


				if (capacityc)
				{
					XMLElement* ca1 = capacityc->FirstChildElement("CA1");
					// <CA1 teams="0"  max="0" mode="H" slots="0" type="HARD"/>
					while (ca1)
					{
						const char* type = ca1->Attribute("type");
						int min, max;
						int penalty;
						const char* team = ca1->Attribute("teams");
						const char* s = ca1->Attribute("slots");
						const char* mode = ca1->Attribute("mode");
						vector<int> slots = stov(s);
						vector<int>teams = stov(team);
						ca1->QueryAttribute("penalty", &penalty);
						ca1->QueryAttribute("min", &min);
						ca1->QueryAttribute("max", &max);
						constraints1.push_back({ type,mode,min,max,penalty,slots,teams });
						ca1 = ca1->NextSiblingElement("CA1");
					}
					XMLElement* ca2 = capacityc->FirstChildElement("CA2");

					while (ca2)
					{
						const char* type = ca2->Attribute("type");
						int min, max;
						int penalty;
						const char* s = ca2->Attribute("slots");
						const char* mode = ca2->Attribute("mode1");
						const char* t1 = ca2->Attribute("teams1");
						const char* t2 = ca2->Attribute("teams2");
						vector<int>teams1 = stov(t1);
						vector<int>teams2 = stov(t2);
						vector<int> slots = stov(s);
						ca2->QueryAttribute("penalty", &penalty);
						ca2->QueryAttribute("min", &min);
						ca2->QueryAttribute("max", &max);
						constraints2.push_back({ type,min,max,penalty,teams1,teams2,slots,mode });
						ca2 = ca2->NextSiblingElement("CA2");
					}
					XMLElement* ca3 = capacityc->FirstChildElement("CA3");
					while (ca3)
					{
						const char* type = ca3->Attribute("type");
						int  min, max, intp;
						int penalty;
						const char* t1 = ca3->Attribute("teams1");
						const char* t2 = ca3->Attribute("teams2");
						const char* mode1 = ca3->Attribute("mode1");
						const char* mode2 = ca3->Attribute("mode2");
						vector<int> teams2 = stov(t2);
						vector<int> teams1 = stov(t1);

						ca3->QueryAttribute("penalty", &penalty);
						ca3->QueryAttribute("min", &min);
						ca3->QueryAttribute("max", &max);
						ca3->QueryAttribute("intp", &intp);
						constraints3.push_back({ type,min,max,intp,penalty,teams1,teams2,mode1,mode2 });
						ca3 = ca3->NextSiblingElement("CA3");
					}
					XMLElement* ca4 = capacityc->FirstChildElement("CA4");

					while (ca4)
					{
						const char* type = ca4->Attribute("type");
						int  min, max;
						int penalty;
						const char* t1 = ca4->Attribute("teams1");
						const char* t2 = ca4->Attribute("teams2");
						const char* s = ca4->Attribute("slots");
						const char* mode1 = ca4->Attribute("mode1");
						const char* mode2 = ca4->Attribute("mode2");
						vector<int>teams1 = stov(t1);
						vector<int> teams2 = stov(t2);
						vector<int>slots = stov(s);
						ca4->QueryAttribute("penalty", &penalty);
						ca4->QueryAttribute("min", &min);
						ca4->QueryAttribute("max", &max);
						constraints4.push_back({ type,min,max,penalty,teams1,teams2,slots,mode1,mode2 });
						ca4 = ca4->NextSiblingElement("CA4");
					}
				}
				else
				{
					cerr << "error:capacity constraints not found\n";
				}

				if (gamec)
				{
					XMLElement* ga1 = gamec->FirstChildElement("GA1");
					while (ga1)
					{
						const char* type = ga1->Attribute("type");
						int  min, max;
						int penalty;
						const char* s = ga1->Attribute("slots");
						const char* m = ga1->Attribute("meetings");
						vector<pair<int, int>>meetings = stov2(m);
						vector<int>slots = stov(s);
						ga1->QueryAttribute("penalty", &penalty);
						ga1->QueryAttribute("min", &min);
						ga1->QueryAttribute("max", &max);
						gameconstraints1.push_back({ type,min,max,penalty,meetings,slots });
						ga1 = ga1->NextSiblingElement("GA1");
					}
				}
				else
				{
					cerr << "error:game constraints not found\n";
				}

				if (breakc)
				{
					XMLElement* br1 = breakc->FirstChildElement("BR1");

					while (br1)
					{
						const char* type = br1->Attribute("type");
						int intp;
						int penalty;
						const char* s = br1->Attribute("slots");
						const char* t = br1->Attribute("teams");
						const char* mode = br1->Attribute("mode2");
						vector<int>teams = stov(t);
						vector<int>slots = stov(s);
						br1->QueryAttribute("penalty", &penalty);
						br1->QueryAttribute("intp", &intp);
						breakconstraints1.push_back({ type,intp,penalty,teams,slots,mode });
						br1 = br1->NextSiblingElement("BR1");
					}
					XMLElement* br2 = breakc->FirstChildElement("BR2");

					while (br2)
					{
						const char* type = br2->Attribute("type");
						int   intp;
						int penalty;
						const char* s = br2->Attribute("slots");
						const char* mode = br2->Attribute("homeMode");
						const char* t = br2->Attribute("teams");
						vector<int>slots = stov(s);
						vector<int>teams = stov(t);
						br2->QueryAttribute("penalty", &penalty);
						br2->QueryAttribute("intp", &intp);
						breakconstraints2.push_back({ type,intp,penalty,teams,slots,mode });
						br2 = br2->NextSiblingElement("BR2");
					}
				}
				else
				{
					cerr << "error:break constraints not found\n";
				}
				if (fairnessc)
				{
					XMLElement* fa2 = fairnessc->FirstChildElement("FA2");
					while (fa2)
					{
						const char* type = fa2->Attribute("type");
						int   intp;
						int penalty;
						const char* s = fa2->Attribute("slots");
						const char* mode = fa2->Attribute("mode");
						const char* t = fa2->Attribute("teams");
						vector<int>slots = stov(s);
						vector<int>teams = stov(t);
						fa2->QueryAttribute("penalty", &penalty);
						fa2->QueryAttribute("intp", &intp);
						fairnessconstraints1.push_back({ type,intp,penalty,slots,mode,teams });
						fa2 = fairnessc->NextSiblingElement("FA2");
					}
				}
				else
				{
					cerr << "error:fairness constraints not found\n";
				}

				if (separationc)
				{
					XMLElement* se1 = separationc->FirstChildElement("SE1");
					while (se1)
					{
						const char* type = se1->Attribute("type");
						int   min;
						int penalty;


						const char* t = se1->Attribute("teams");

						vector<int>teams = stov(t);
						se1->QueryAttribute("penalty", &penalty);
						se1->QueryAttribute("min", &min);
						min++;
						seperationconstraints1.push_back({ type,min,penalty,teams });
						se1 = separationc->NextSiblingElement("SE1");
					}
				}
				else
				{
					cerr << "error:separation constraints not found\n";
				}

			}
			else
			{
				cerr << "Error:Constraints not found\n";
			}

		}


	}
	catch (...) {
		cerr << " ERROR" << endl;
		throw;
	}
	//cout << constraints1.size() << endl;
	#pragma endregion

	auto start = high_resolution_clock::now();
	int m = 2 * (n - 1); //no. of time slots available
	IloEnv env;
	IloModel Model(env);
	IloArray<IloArray<IloBoolVarArray>> x(env, n);  //decision variables

	//decision variables
	for (int i = 0; i < n; i++)
	{
		x[i] = IloArray<IloBoolVarArray>(env, n);
		for (int j = 0; j < n; j++)
		{
			x[i][j] = IloBoolVarArray(env, m);
		}
	}

	//common constraint 1
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
			B1_lhs.end();
		}
	}

	//common constraint 2
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
			B2_lhs.end();
		}
	}

	//gamemode: Phased constraint
	if (GameMode == "P") {
		for (int i = 0; i < n; i++)
		{
			for (int j = i+1; j < n; j++)
			{
				IloExpr B2_lhs(env);
				for (int k = 0; k < n - 1; k++)
				{
					B2_lhs += x[i][j][k] + x[j][i][k];
				}
				Model.add(B2_lhs == 1);
				B2_lhs.end();
			}
		}
	}

	//common constraint 3
	for (int i = 0; i < n; i++)
	{
		for (int k = 0; k < m; k++)
		{
			Model.add(x[i][i][k] == 0);
		}
	}

	IloExpr Objectivefn(env);  //objective function

	//CA1 constraints
	IloNumVarArray y_ca1(env, constraints1.size(), 0, IloInfinity, ILOINT);
	for (int i = 0; i < constraints1.size(); i++)
	{
		IloExpr CA1_expr(env);
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < constraints1[i].slots.size(); k++)
			{
				if(constraints1[i].model=="H"){ 
					CA1_expr += x[constraints1[i].teams[0]][j][constraints1[i].slots[k]]; 
				}
				else if(constraints1[i].model == "A"){ 
					CA1_expr += x[j][constraints1[i].teams[0]][constraints1[i].slots[k]]; 
				}
			}
		}
		Model.add(CA1_expr <= constraints1[i].max + y_ca1[i]);
		CA1_expr.end();
		if (constraints1[i].type == "HARD") {
			Model.add(y_ca1[i] == 0);
		}
		Objectivefn += constraints1[i].penalty * y_ca1[i];
	}

	//CA2 constraints
	IloNumVarArray y_ca2(env, constraints2.size(), 0, IloInfinity, ILOINT);
	for (int i = 0; i < constraints2.size(); i++)
	{
		IloExpr CA2_expr(env);
		for (int j = 0; j < constraints2[i].teams2.size(); j++)
		{
			for (int k = 0; k < constraints2[i].slots.size(); k++)
			{
				if (constraints2[i].model == "H") {
					CA2_expr += x[constraints2[i].teams1[0]][constraints2[i].teams2[j]][constraints2[i].slots[k]];
				}
				else if (constraints2[i].model == "A") {
					CA2_expr += x[constraints2[i].teams2[j]][constraints2[i].teams1[0]][constraints2[i].slots[k]];
				}
				else if (constraints2[i].model == "HA") {
					CA2_expr += x[constraints2[i].teams1[0]][constraints2[i].teams2[j]][constraints2[i].slots[k]] + x[constraints2[i].teams2[j]][constraints2[i].teams1[0]][constraints2[i].slots[k]];
				}
			}
		}
		Model.add(CA2_expr <= constraints2[i].max + y_ca2[i]);
		CA2_expr.end();
		if (constraints2[i].type == "HARD") {
			Model.add(y_ca2[i] == 0);
		}
		Objectivefn += constraints2[i].penalty * y_ca2[i];
	}

	//CA3
	IloNumVarArray y(env, 0, 0, IloInfinity, ILOINT);
	for (int i = 0; i < constraints3.size(); i++)
	{
		//cout << constraints3[i].min << " " << constraints3[i].max << " " << constraints3[i].penalty << " " << constraints3[i].intp<<" "<< constraints3[i].mode1 << endl;
		for (int j = 0; j < constraints3[i].teams1.size(); j++)
		{
			for (int k = 0; k <= m-constraints3[i].intp; k++)
			{
				IloExpr c(env);
				for (int l = 0; l < constraints3[i].teams2.size(); l++)
				{
					for (int m = k; m < k+ constraints3[i].intp; m++)
					{
						if (constraints3[i].mode1 == "H") {
							c += x[constraints3[i].teams1[j]][constraints3[i].teams2[l]][m];
						}
						else if (constraints3[i].mode1 == "A") {
							c += x[constraints3[i].teams2[l]][constraints3[i].teams1[j]][m];
						}
						else if (constraints3[i].mode1 == "HA") {
							c += x[constraints3[i].teams1[j]][constraints3[i].teams2[l]][m] + x[constraints3[i].teams2[l]][constraints3[i].teams1[j]][m];
						}
					}
				}
				if (constraints3[i].type == "HARD") {
					Model.add(c <= constraints3[i].max);
					Model.add(c >= constraints3[i].min);
				}
				else if (constraints3[i].type == "SOFT") {
					y.add(IloNumVar(env, 0, IloInfinity, ILOINT));
					Model.add(c - y[y.getSize() - 1] <= constraints3[i].max);
					Objectivefn += constraints3[i].penalty * y[y.getSize() - 1];
				}
			}
		}
	}

	/*//CA3 constraints
	IloArray<IloArray<IloNumVarArray>> y_ca3(env, constraints3.size());
	for (int a = 0; a < constraints3.size(); a++)
	{
		y_ca3[a] = IloArray<IloNumVarArray>(env, constraints3[a].teams1.size());
		for (int i = 0; i < constraints3[a].teams1.size(); i++)
		{
			y_ca3[a][i] = IloNumVarArray(env, m - constraints3[a].intp, 0, IloInfinity, ILOINT);
			for (int k = 0; k <= m - constraints3[a].intp; k++)
			{
				IloExpr CA3_expr(env);
				for (int j = 0; j < constraints3[a].teams2.size(); j)
				{
					for (int l = k; l < k + constraints3[a].intp; l++)
					{
						if (constraints3[a].mode1 == "H") {
							CA3_expr += x[constraints3[a].teams1[i]][constraints3[a].teams2[j]][l];
						}
						else if (constraints3[a].mode1 == "A") {
							CA3_expr += x[constraints3[a].teams2[j]][constraints3[a].teams1[i]][l];
						}
						else if (constraints3[a].mode1 == "HA") {
							CA3_expr += x[constraints3[a].teams1[i]][constraints3[a].teams2[j]][l] + x[constraints3[a].teams2[j]][constraints3[a].teams1[i]][l];
						}
					}
				}
				Model.add(CA3_expr <= constraints3[a].max + y_ca3[a][i][k]);
				CA3_expr.end();
				if (constraints3[a].type == "HARD") {
					Model.add(y_ca3[a][i][k] == 0);
				}
				Objectivefn += constraints3[a].penalty * y_ca3[a][i][k];
			}
		}
	}*/
	

	//CA4 constraints
	IloArray<IloNumVarArray> y_ca4(env, constraints4.size());
	for (int i = 0; i < constraints4.size(); i++)
	{
		if (constraints4[i].mode2 == "GLOBAL") {
			y_ca4[i] = IloNumVarArray(env, 1, 0, IloInfinity, ILOINT);
			IloExpr CA4_expr(env);
			for (int j = 0; j < constraints4[i].teams1.size(); j++)
			{
				for (int k = 0; k < constraints4[i].teams2.size(); k++)
				{
					for (int l = 0; l < constraints4[i].slots.size(); l++)
					{
						if (constraints4[i].mode1 == "H") {
							CA4_expr += x[constraints4[i].teams1[j]][constraints4[i].teams2[k]][constraints4[i].slots[l]];
						}
						else if (constraints4[i].mode1 == "A") {
							CA4_expr += x[constraints4[i].teams2[k]][constraints4[i].teams1[j]][constraints4[i].slots[l]];
						}
						else if (constraints4[i].mode1 == "HA") {
							CA4_expr += x[constraints4[i].teams1[j]][constraints4[i].teams2[k]][constraints4[i].slots[l]] + x[constraints4[i].teams2[k]][constraints4[i].teams1[j]][constraints4[i].slots[l]];
						}
					}
				}
			}
			Model.add(CA4_expr <= constraints4[i].max + y_ca4[i][0]);
			CA4_expr.end();
			if (constraints4[i].type == "HARD") {
				Model.add(y_ca4[i][0] == 0);
			}
			Objectivefn += constraints4[i].penalty * y_ca4[i][0];
		}
		else if (constraints4[i].mode2 == "EVERY") {
			y_ca4[i] = IloNumVarArray(env, constraints4[i].slots.size(), 0, IloInfinity, ILOINT);
			for (int l = 0; l < constraints4[i].slots.size(); l++)
			{
				IloExpr CA4_expr(env);
				for (int j = 0; j < constraints4[i].teams1.size(); j++)
				{
					for (int k = 0; k < constraints4[i].teams2.size(); k++)
					{
						if (constraints4[i].mode1 == "H") {
							CA4_expr += x[constraints4[i].teams1[j]][constraints4[i].teams2[k]][constraints4[i].slots[l]];
						}
						else if (constraints4[i].mode1 == "A") {
							CA4_expr += x[constraints4[i].teams2[k]][constraints4[i].teams1[j]][constraints4[i].slots[l]];
						}
						else if (constraints4[i].mode1 == "HA") {
							CA4_expr += x[constraints4[i].teams1[j]][constraints4[i].teams2[k]][constraints4[i].slots[l]] + x[constraints4[i].teams2[k]][constraints4[i].teams1[j]][constraints4[i].slots[l]];
						}

					}
				}
				Model.add(CA4_expr <= constraints4[i].max + y_ca4[i][l]);
				CA4_expr.end();
				if (constraints4[i].type == "HARD") {
					Model.add(y_ca4[i][l] == 0);
				}
				Objectivefn += constraints4[i].penalty * y_ca4[i][l];
			}
		}
	}

	//GA1 constraints
	IloNumVarArray y_ga1(env, gameconstraints1.size(), 0, IloInfinity, ILOINT);
	for (int i = 0; i < gameconstraints1.size(); i++)
	{
		IloExpr GA1_expr(env);
		for (int j = 0; j < gameconstraints1[i].meetings.size(); j++)
		{
			for (int k = 0; k < gameconstraints1[i].slots.size(); k++)
			{
				GA1_expr += x[gameconstraints1[i].meetings[j].first][gameconstraints1[i].meetings[j].second][gameconstraints1[i].slots[k]];
			}
		}
		Model.add(GA1_expr + y_ga1[i] >= gameconstraints1[i].min);
		Model.add(GA1_expr <= gameconstraints1[i].max + y_ga1[i]);
		GA1_expr.end();
		if (gameconstraints1[i].type == "HARD") {
			Model.add(y_ga1[i] == 0);
		}
		Objectivefn += gameconstraints1[i].penalty * (y_ga1[i]);
	}

	//BR1 constraints
	IloNumVarArray y_br1(env, breakconstraints1.size(), 0, IloInfinity, ILOINT);
	IloArray<IloBoolVarArray> H_br1(env, breakconstraints1.size());
	IloArray<IloBoolVarArray> A_br1(env, breakconstraints1.size());
	for (int i = 0; i < breakconstraints1.size(); i++)
	{
		IloExpr BR1_expr(env);
		H_br1[i] = IloBoolVarArray(env, breakconstraints1[i].slots.size());
		A_br1[i] = IloBoolVarArray(env, breakconstraints1[i].slots.size());
		for (int k = 0; k < breakconstraints1[i].slots.size(); k++)
		{
			if (breakconstraints1[i].slots[k] == 0) {
				continue;
			}
			IloExpr BR1_expr1(env);
			IloExpr BR1_expr2(env);
			for (int j = 0; j < n; j++)
			{
				BR1_expr1 += (x[breakconstraints1[i].teams[0]][j][(breakconstraints1[i].slots[k]) - 1] + x[breakconstraints1[i].teams[0]][j][breakconstraints1[i].slots[k]]);
				BR1_expr2 += (x[j][breakconstraints1[i].teams[0]][(breakconstraints1[i].slots[k]) - 1] + x[j][breakconstraints1[i].teams[0]][breakconstraints1[i].slots[k]]);
			}
			Model.add(H_br1[i][k] >= BR1_expr1 - 1);
			Model.add(A_br1[i][k] >= BR1_expr2 - 1);
			if (breakconstraints1[i].mode2 == "H") {
				BR1_expr += H_br1[i][k];
			}
			else if (breakconstraints1[i].mode2 == "A") {
				BR1_expr += A_br1[i][k];
			}
			else if (breakconstraints1[i].mode2 == "HA") {
				BR1_expr += H_br1[i][k] + A_br1[i][k];
			}
			BR1_expr1.end();
			BR1_expr2.end();
		}
		Model.add(BR1_expr <= breakconstraints1[i].intp + y_br1[i]);
		BR1_expr.end();
		if (breakconstraints1[i].type == "HARD") {
			Model.add(y_br1[i] == 0);
		}
		Objectivefn += breakconstraints1[i].penalty * y_br1[i];
	}

	//BR2 constraints
	IloNumVarArray y_br2(env, breakconstraints2.size(), 0, IloInfinity, ILOINT);
	IloArray<IloArray<IloBoolVarArray>> H(env, breakconstraints2.size());
	IloArray<IloArray<IloBoolVarArray>> A(env, breakconstraints2.size());
	for (int i = 0; i < breakconstraints2.size(); i++)
	{
		IloExpr BR2_expr(env);
		H[i] = IloArray<IloBoolVarArray > (env, breakconstraints2[i].teams.size());
		A[i] = IloArray<IloBoolVarArray >(env, breakconstraints2[i].teams.size());
		for (int a = 0; a < breakconstraints2[i].teams.size(); a++)
		{
			H[i][a] = IloBoolVarArray(env, breakconstraints2[i].slots.size());
			A[i][a] = IloBoolVarArray(env, breakconstraints2[i].slots.size());
			for (int k = 0; k < breakconstraints2[i].slots.size(); k++)
			{
				if (breakconstraints2[i].slots[k] == 0) {
					Model.add(H[i][a][k] == 0);
					Model.add(A[i][a][k] == 0);
					continue;
				}
				IloExpr BR2_expr1(env);
				IloExpr BR2_expr2(env);
				for (int j = 0; j < n; j++)
				{
					BR2_expr1 += (x[breakconstraints2[i].teams[a]][j][(breakconstraints2[i].slots[k]) - 1] + x[breakconstraints2[i].teams[a]][j][breakconstraints2[i].slots[k]]);
					BR2_expr2 += (x[j][breakconstraints2[i].teams[a]][(breakconstraints2[i].slots[k]) - 1] + x[j][breakconstraints2[i].teams[a]][breakconstraints2[i].slots[k]]);
				}
				Model.add(BR2_expr1-1 <= H[i][a][k]);
				Model.add(BR2_expr2-1 <= A[i][a][k]);
				BR2_expr += H[i][a][k] + A[i][a][k];
				BR2_expr1.end();
				BR2_expr2.end();
			}
		}
		Model.add(BR2_expr <= breakconstraints2[i].intp + y_br2[i]);
		BR2_expr.end();
		if (breakconstraints2[i].type == "HARD") {
			Model.add(y_br2[i] == 0);
		}
		Objectivefn += breakconstraints2[i].penalty * y_br2[i];
	}

	//FA1 constraints
	IloNumVarArray y_fa1(env, fairnessconstraints1.size(), 0, IloInfinity, ILOINT);
	for (int i = 0; i < fairnessconstraints1.size(); i++)
	{
		for (int a = 0; a < fairnessconstraints1[i].teams.size()-1; a++)
		{
			for (int b = a+1; b < fairnessconstraints1[i].teams.size(); b++)
			{
				IloExpr FA1_expr(env);
				for (int k = 0; k < fairnessconstraints1[i].slots.size(); k++)
				{
					for (int j = 0; j < n; j++)
					{
						FA1_expr += x[fairnessconstraints1[i].teams[a]][j][fairnessconstraints1[i].slots[k]] - x[fairnessconstraints1[i].teams[b]][j][fairnessconstraints1[i].slots[k]];
					}
				}
				Model.add(FA1_expr <= fairnessconstraints1[i].intp + y_fa1[i]);
				Model.add(-FA1_expr <= fairnessconstraints1[i].intp + y_fa1[i]);
				FA1_expr.end();
			}
		}
		if (fairnessconstraints1[i].type == "HARD") {
			Model.add(y_fa1[i] == 0);
		}
		Objectivefn += fairnessconstraints1[i].penalty * y_fa1[i];
	}

	//SE1 constraints
	IloNumVarArray y_se1(env, 0, 0, IloInfinity, ILOINT);
	IloBoolVarArray y2(env, 0);
	for (int a = 0; a < seperationconstraints1.size(); a++)
	{
		for (int i = 0; i < seperationconstraints1[a].teams.size(); i++)
		{
			for (int j = i+1; j < seperationconstraints1[a].teams.size(); j++)
			{
				IloExpr c(env);
				for (int k = 0; k < m; k++)
				{
					c += k * x[seperationconstraints1[a].teams[i]][seperationconstraints1[a].teams[j]][k] - k * x[seperationconstraints1[a].teams[j]][seperationconstraints1[a].teams[i]][k];
				}
				if (seperationconstraints1[a].type == "HARD") {
					y2.add(IloBoolVar(env));
					Model.add(seperationconstraints1[a].min - c <= INT_MAX * y2[y2.getSize() - 1]);
					Model.add(seperationconstraints1[a].min + c <= INT_MAX * (1-y2[y2.getSize() - 1]));
				}
				else if(seperationconstraints1[a].type == "SOFT") {
					y2.add(IloBoolVar(env));
					y_se1.add(IloNumVar(env, 0, IloInfinity, ILOINT));
					//if c>0
					Model.add(c <= INT_MAX * (1-y2[y2.getSize() - 1]));
					Model.add(seperationconstraints1[a].min - y_se1[y_se1.getSize() - 1] - c <= INT_MAX * y2[y2.getSize() - 1]);
					//if c<0
					y2.add(IloBoolVar(env));
					Model.add(-c <= INT_MAX * (1 - y2[y2.getSize() - 1]));
					Model.add(seperationconstraints1[a].min - y_se1[y_se1.getSize() - 1] + c <= INT_MAX * y2[y2.getSize() - 1]);
					Objectivefn += seperationconstraints1[a].penalty * y_se1[y_se1.getSize() - 1];
				}
			}
		}
	}
	
	/*//SE1 constraints
	vector<SE1> new_seperationconstraints1;
	for (int i = 0; i < seperationconstraints1.size(); i++)
	{
		for (int j = 0; j < seperationconstraints1[i].teams.size()-1; j++)
		{
			for (int k = j+1; k < seperationconstraints1[i].teams.size(); k++)
			{
				new_seperationconstraints1.push_back({seperationconstraints1[i].type,seperationconstraints1[i].min,seperationconstraints1[i].penalty,{seperationconstraints1[i].teams[j],seperationconstraints1[i].teams[k]}});
			}
		}
	}
	IloNumVarArray y_se1(env, new_seperationconstraints1.size(), 0, IloInfinity, ILOINT);
	IloBoolVarArray y2_se1(env, 2*new_seperationconstraints1.size());
	for (int i = 0; i < new_seperationconstraints1.size(); i++)
	{
		IloExpr SE1_expr(env);
		for (int k = 0; k < m; k++)
		{
			SE1_expr += k * x[new_seperationconstraints1[i].teams[0]][new_seperationconstraints1[i].teams[1]][k] - k * x[new_seperationconstraints1[i].teams[1]][new_seperationconstraints1[i].teams[0]][k];
		}
		//if SE1_expr>=0
		Model.add(SE1_expr <=INT_MAX*(1-y2_se1[2*i]));
		Model.add(new_seperationconstraints1[i].min -y_se1[i]-SE1_expr <=  INT_MAX*y2_se1[2*i]);
		//if SE1_expr<=0
		Model.add(-SE1_expr <= INT_MAX * (1 - y2_se1[2 * i + 1]));
		Model.add(new_seperationconstraints1[i].min - y_se1[i] - SE1_expr <= INT_MAX * (1 - y2_se1[2 * i + 1]));
		SE1_expr.end();
		if (new_seperationconstraints1[i].type == "HARD") {
			Model.add(y_se1[2*i] == 0);
			Model.add(y_se1[2 * i + 1] == 0);
		}
		Objectivefn += new_seperationconstraints1[i].penalty * (y_se1[i]);
	}*/
	
	
	//solving the LP by minimising objective function
	Model.add(IloMinimize(env, Objectivefn));
	Objectivefn.end();
	IloCplex cplex(Model);
	cplex.setOut(env.getNullStream());
	cplex.solve();
	auto stop = high_resolution_clock::now();
	ofstream outfile("output.txt");
	outfile << "Solution to " << InstanceName << ": \n" << endl;
	outfile << "Total penalty: " << cplex.getObjValue() << "\n" << endl;
	cout << cplex.getObjValue() << endl;
	for (int k = 0; k < m; k++)
	{
		outfile << "Slot " << k << ": ";
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (cplex.getValue(x[i][j][k]) != 0) {
					outfile << "(" << i << ", " << j << "); ";
				}
			}
		}
		outfile << "\n";
	}
	auto duration = duration_cast<microseconds>(stop - start);
	outfile << "\nTime taken: "<< duration.count() << " microseconds" << endl;
	return 0;
}