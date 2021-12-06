#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>   

#include "cfop.h"
#include "lbl.h"
#include "roux.h"
#include "petrus.h"
#include "zz.h"

#include "tinyxml2.h"
#include "time.h"

using namespace grcube3;
using namespace tinyxml2;

enum class Methods { NONE, LBL, CFOP, ROUX, PETRUS, ZZ };
enum class Collections { NONE, OLL, PLL, _1LLL, COLL, CMLL, EPLL, OCLL, ZBLL };
enum class Orientations 
{ 
	ALL,
	Ux, Dx, Fx, Bx, Rx, Lx,
	UD, FB, RL,
	UF, UR, UB, UL,
	DF, DR, DB, DL,
	FU, FR, FD, FL,
	BU, BR, BD, BL,
	RU, RF, RD, RB,
	LU, LF, LD, LB,
};

static std::string o_strings[] = 
{
	"All",
	"U layer up", "D layer up", "F layer up", "B layer up", "R layer up", "L layer up",
	"U & D layers up", "F & B layers up", "R & L layers up",
	"UF", "UR", "UB", "UL",
	"DF", "DR", "DB", "DL",
	"FU", "FR", "FD", "FL",
	"BU", "BR", "BD", "BL",
	"RU", "RF", "RD", "RB",
	"LU", "LF", "LD", "LB",
};

// Command line parameters
static struct 
{
	Methods Method = Methods::NONE;
	Collections Collect = Collections::NONE;
	Orientations Orient = Orientations::ALL;
	Metrics Metric = Metrics::STM;

	uint NumSolves = 1u, // Number of random solves to perform
		 LengthScramble = 20u, // Scramble length
		 NumInsp = 1u, // Number of inspections per orientation
		 Depth1 = 6u, // Primary depth
		 Depth2 = 6u; // Secondary depth

	int Threads = 0; // Number of threads to use in the search (0 = all cores, -1 = no threads)

	bool Best = false, Debug = false, Regrip = false, Cancellations = false, OneLookL6E = false, Times = false;
} Parameters;

void ProcessParameters(int, char* []); // Process command line parameters

std::string GetParametersSummary();

std::string GetExternalFilesCheck();

void ProcessOrientations(std::vector<Lyr>&, const Orientations);
void ProcessOrientations(std::vector<Spn>&, const Orientations);

void sLBL(const Algorithm&, std::ofstream&);
void sCFOP(const Algorithm&, std::ofstream&);
void sRoux(const Algorithm&, std::ofstream&);
void sPetrus(const Algorithm&, std::ofstream&);
void sZZ(const Algorithm&, std::ofstream&);

  // ********************************************************************************* //
 // ********** Main function ******************************************************** //
// ********************************************************************************* //

int main(int argc, char* argv[])
{
	// Process command line parameters
	ProcessParameters(argc, argv);
	
	std::cout << GetParametersSummary();

	if (Parameters.Method == Methods::NONE)
	{
		std::ofstream outfile;
		outfile.open("scrambles.txt", std::ofstream::app);
		for (uint n = 1u; n <= Parameters.NumSolves; n++) outfile << Algorithm(Parameters.LengthScramble).ToString() << "\n";
		outfile.close();
		std::cout << "Done!" << std::endl;
		return 0;
	}

	std::vector<Algorithm> Scrambles;

	std::ifstream infile("scrambles.txt");

	if (infile)
	{
		std::cout << "Scrambles file detected, using it for solves..." << std::endl;
		std::string line;
		while (std::getline(infile, line)) Scrambles.push_back(Algorithm(line));
		infile.close();
	}
	else
	{
		std::cout << "Generating " << Parameters.NumSolves << " random scrambles..." << std::flush;
		for (uint n = 1u; n <= Parameters.NumSolves; n++) Scrambles.push_back(Algorithm(Parameters.LengthScramble));
		std::cout << "Done!" << std::endl;
	}

	// Open log file
	std::ofstream f_log;
	f_log.open("LOG.txt", std::ofstream::app);
	
#ifdef __linux__
    
	auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");

	f_log << "----------------------------------------\n" << argv[0] << " executed at " << oss.str() << "\n";
#else
    char currenttime[100];
	std::time_t start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	ctime_s(currenttime, 100, &start_time);
	f_log << "----------------------------------------\n"  << argv[0] << " executed at " << currenttime << "\n";
#endif
	f_log << GetExternalFilesCheck();
	f_log << "\n";
	f_log << GetParametersSummary();
	f_log << "\n";
	// Main loop
	uint n = 0u;
	for (const auto& Scramble : Scrambles)
	{
		std::cout << "Solve " << ++n << " of " << Scrambles.size() << std::endl;

		switch (Parameters.Method)
		{
		case Methods::LBL: sLBL(Scramble, f_log); break;
		case Methods::CFOP: sCFOP(Scramble, f_log); break;
		case Methods::ROUX: sRoux(Scramble, f_log); break;
		case Methods::PETRUS: sPetrus(Scramble, f_log); break;
		case Methods::ZZ: sZZ(Scramble, f_log); break;
		default: break;
		}
	}

	f_log.close();
}

  // ********************************************************************************* //
 // ********** Functions ************************************************************ //
// ********************************************************************************* //

void ProcessParameters(int argc, char* argv[])
{
	for (int p = 1; p < argc; p++)
	{
		std::string Par = argv[p], SubPar;

		if (Par.find("LBL") != std::string::npos || Par.find("lbl") != std::string::npos || Par.find("Lbl") != std::string::npos)
		{
			Parameters.Method = Methods::LBL;
			std::cout << "Selected LBL method (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("CFOP") != std::string::npos || Par.find("cfop") != std::string::npos || Par.find("Cfop") != std::string::npos)
		{
			Parameters.Method = Methods::CFOP;
			std::cout << "Selected CFOP method (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("ROUX") != std::string::npos || Par.find("roux") != std::string::npos || Par.find("Roux") != std::string::npos)
		{
			Parameters.Method = Methods::ROUX;
			std::cout << "Selected Roux method (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("PETRUS") != std::string::npos || Par.find("petrus") != std::string::npos || Par.find("Petrus") != std::string::npos)
		{
			Parameters.Method = Methods::PETRUS;
			std::cout << "Selected Petrus method (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("ZZ") != std::string::npos || Par.find("zz") != std::string::npos || Par.find("Zz") != std::string::npos)
		{
			Parameters.Method = Methods::ZZ;
			std::cout << "Selected ZZ method (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("1LLL") != std::string::npos || Par.find("1lll") != std::string::npos)
		{
			Parameters.Collect = Collections::_1LLL;
			std::cout << "Selected 1LLL algorithms collection (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("ZBLL") != std::string::npos || Par.find("zbll") != std::string::npos || Par.find("Zbll") != std::string::npos)
		{
			Parameters.Collect = Collections::ZBLL;
			std::cout << "Selected ZBLL algorithms collection (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("CMLL") != std::string::npos || Par.find("cmll") != std::string::npos || Par.find("Cmll") != std::string::npos)
		{
			Parameters.Collect = Collections::CMLL;
			std::cout << "Selected CMLL algorithms collection (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("COLL") != std::string::npos || Par.find("coll") != std::string::npos || Par.find("Coll") != std::string::npos)
		{
			Parameters.Collect = Collections::COLL;
			std::cout << "Selected COLL algorithms collection (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("OCLL") != std::string::npos || Par.find("ocll") != std::string::npos || Par.find("Ocll") != std::string::npos)
		{
			Parameters.Collect = Collections::OCLL;
			std::cout << "Selected OCLL algorithms collection (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("EPLL") != std::string::npos || Par.find("epll") != std::string::npos || Par.find("Epll") != std::string::npos)
		{
			Parameters.Collect = Collections::EPLL;
			std::cout << "Selected EPLL algorithms collection (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("OLL") != std::string::npos || Par.find("oll") != std::string::npos || Par.find("Oll") != std::string::npos)
		{
			Parameters.Collect = Collections::OLL;
			std::cout << "Selected OLL algorithms collection (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("PLL") != std::string::npos || Par.find("pll") != std::string::npos || Par.find("Pll") != std::string::npos)
		{
			Parameters.Collect = Collections::PLL;
			std::cout << "Selected PLL algorithms collection (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("BEST") != std::string::npos || Par.find("best") != std::string::npos || Par.find("Best") != std::string::npos)
		{
			Parameters.Best = true;
			std::cout << "Show best solve (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("REGRIP") != std::string::npos || Par.find("regrip") != std::string::npos || Par.find("Regrip") != std::string::npos)
		{
			Parameters.Regrip = true;
			std::cout << "Regrip enabled (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("CANC") != std::string::npos || Par.find("Canc") != std::string::npos || Par.find("canc") != std::string::npos)
		{
			Parameters.Cancellations = true;
			std::cout << "Cancellations enabled (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("L6E") != std::string::npos || Par.find("L6e") != std::string::npos || Par.find("l6e") != std::string::npos)
		{
			Parameters.OneLookL6E = true;
			std::cout << "Roux L6E in one look (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("QSTM") != std::string::npos || Par.find("Qstm") != std::string::npos || Par.find("qstm") != std::string::npos)
		{
			Parameters.Metric = Metrics::QSTM;
			std::cout << "Using QSTM metric (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("STM") != std::string::npos || Par.find("Stm") != std::string::npos || Par.find("stm") != std::string::npos)
		{
			Parameters.Metric = Metrics::STM;
			std::cout << "Using STM metric (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("1.5HTM") != std::string::npos || Par.find("1.5Htm") != std::string::npos || Par.find("1.5htm") != std::string::npos)
		{
			Parameters.Metric = Metrics::HTM15;
			std::cout << "Using 1.5HTM metric (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("HTM") != std::string::npos || Par.find("Htm") != std::string::npos || Par.find("htm") != std::string::npos)
		{
			Parameters.Metric = Metrics::HTM;
			std::cout << "Using HTM metric (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("QTM") != std::string::npos || Par.find("Qtm") != std::string::npos || Par.find("qtm") != std::string::npos)
		{
			Parameters.Metric = Metrics::QTM;
			std::cout << "Using QTM metric (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("ETM") != std::string::npos || Par.find("Etm") != std::string::npos || Par.find("etm") != std::string::npos)
		{
			Parameters.Metric = Metrics::ETM;
			std::cout << "Using ETM metric (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("ATM") != std::string::npos || Par.find("Atm") != std::string::npos || Par.find("atm") != std::string::npos)
		{
			Parameters.Metric = Metrics::ATM;
			std::cout << "Using ATM metric (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("PTM") != std::string::npos || Par.find("Ptm") != std::string::npos || Par.find("ptm") != std::string::npos)
		{
			Parameters.Metric = Metrics::PTM;
			std::cout << "Using PTM metric (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("OBTM") != std::string::npos || Par.find("Obtm") != std::string::npos || Par.find("obtm") != std::string::npos)
		{
			Parameters.Metric = Metrics::OBTM;
			std::cout << "Using OBTM metric (parameter '" << Par << "')" << std::endl;
			continue;
		}
		
		if (Par.find("TIME") != std::string::npos || Par.find("time") != std::string::npos || Par.find("Time") != std::string::npos)
		{
			Parameters.Times = true;
			std::cout << "Show times (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.find("DEBUG") != std::string::npos || Par.find("debug") != std::string::npos || Par.find("Debug") != std::string::npos)
		{
			Parameters.Debug = true;
			std::cout << "Debug enabled (parameter '" << Par << "')" << std::endl;
			continue;
		}

		if (Par.size() <= 2u || Par[0] != '-')
		{
			std::cout << "Unknown parameter '" << Par << "'" << std::endl;
			continue;
		}

		switch (Par[1])
		{
		case 'n': case 'N': // Number of solves
			SubPar = Par.substr(2);
			Parameters.NumSolves = std::stoi(SubPar);
			if (Parameters.NumSolves == 0u) Parameters.NumSolves = 1u;
			else if (Parameters.NumSolves > 10000u) Parameters.NumSolves = 10000u;
			std::cout << "Number of solves set to " << Parameters.NumSolves << " (parameter '" << Par << "')" << std::endl;
			break;

		case 'l': case 'L':// Scramble length
			SubPar = Par.substr(2);
			Parameters.LengthScramble = std::stoi(SubPar);
			if (Parameters.LengthScramble < 4u) Parameters.LengthScramble = 4u;
			else if (Parameters.LengthScramble > 100u) Parameters.LengthScramble = 100u;
			std::cout << "Scramble length set to " << Parameters.LengthScramble << " (parameter '" << Par << "')" << std::endl;
			break;

		case 'i': case 'I':// Number of inspections per orientation
			SubPar = Par.substr(2);
			Parameters.NumInsp = std::stoi(SubPar);
			if (Parameters.NumInsp < 1u) Parameters.NumInsp = 1u;
			else if (Parameters.NumInsp > 100u) Parameters.NumInsp = 100u;
			std::cout << "Number of inspections set to " << Parameters.NumInsp << " (parameter '" << Par << "')" << std::endl;
			break;

		case 't': case 'T':// Number of threads
			SubPar = Par.substr(2);
			Parameters.Threads = std::stoi(SubPar);
			if (Parameters.Threads < -1) Parameters.Threads = -1;
			else if (Parameters.Threads > 128) Parameters.Threads = 128;
			if (Parameters.Threads == -1) std::cout << "No threads (parameter '" << Par << "')" << std::endl;
			else if (Parameters.Threads == 0) std::cout << "All avaliable threads (parameter '" << Par << "')" << std::endl;
			else std::cout << "Number of threads set to " << Parameters.Threads << " (parameter '" << Par << "')" << std::endl;
			break;

		case 'd': case 'D':// Primary depth
			SubPar = Par.substr(2);
			Parameters.Depth1 = std::stoi(SubPar);
			if (Parameters.Depth1 < 4u) Parameters.Depth1 = 4u;
			else if (Parameters.Depth1 > 10u) Parameters.Depth1 = 10u;
			std::cout << "Primary depth set to " << Parameters.Depth1 << " (parameter '" << Par << "')" << std::endl;
			break;

		case 's': case 'S':// Secondary depth
			SubPar = Par.substr(2);
			Parameters.Depth2 = std::stoi(SubPar);
			if (Parameters.Depth2 < 4u) Parameters.Depth2 = 4u;
			else if (Parameters.Depth2 > 15u) Parameters.Depth2 = 15u;
			std::cout << "Secondary depth set to " << Parameters.Depth2 << " (parameter '" << Par << "')" << std::endl;
			break;

		case 'o': case 'O': // Orientations to search
			SubPar = Par.substr(2);
			if (SubPar.length() < 1u) std::cout << "Unknown orientation '" << Par << "'" << std::endl;
			else if (SubPar.length() < 2u)
			{
				switch (SubPar[0])
				{
				case 'U': Parameters.Orient = Orientations::Ux; break;
				case 'D': Parameters.Orient = Orientations::Dx; break;
				case 'F': Parameters.Orient = Orientations::Fx; break;
				case 'B': Parameters.Orient = Orientations::Bx; break;
				case 'R': Parameters.Orient = Orientations::Rx; break;
				case 'L': Parameters.Orient = Orientations::Lx; break;
				default: std::cout << "Unknown orientation '" << Par << "'" << std::endl;
				}
				std::cout << "Orientation search defined (parameter '" << Par << "')" << std::endl;
			}
			else
			{
				switch (SubPar[0])
				{
				case 'U':
					switch (SubPar[1])
					{
					case 'F': Parameters.Orient = Orientations::UF; break;
					case 'B': Parameters.Orient = Orientations::UB; break;
					case 'R': Parameters.Orient = Orientations::UR; break;
					case 'L': Parameters.Orient = Orientations::UL; break;
					case 'D': Parameters.Orient = Orientations::UD; break;
					default: Parameters.Orient = Orientations::Ux; break;
					}
					break;

				case 'D':
					switch (SubPar[1])
					{
					case 'F': Parameters.Orient = Orientations::DF; break;
					case 'B': Parameters.Orient = Orientations::DB; break;
					case 'R': Parameters.Orient = Orientations::DR; break;
					case 'L': Parameters.Orient = Orientations::DL; break;
					default: Parameters.Orient = Orientations::Dx; break;
					}
					break;

				case 'F':
					switch (SubPar[1])
					{
					case 'U': Parameters.Orient = Orientations::FU; break;
					case 'D': Parameters.Orient = Orientations::FD; break;
					case 'R': Parameters.Orient = Orientations::FR; break;
					case 'L': Parameters.Orient = Orientations::FL; break;
					case 'B': Parameters.Orient = Orientations::FB; break;
					default: Parameters.Orient = Orientations::Fx; break;
					}
					break;

				case 'B':
					switch (SubPar[1])
					{
					case 'U': Parameters.Orient = Orientations::BU; break;
					case 'D': Parameters.Orient = Orientations::BD; break;
					case 'R': Parameters.Orient = Orientations::BR; break;
					case 'L': Parameters.Orient = Orientations::BL; break;
					default: Parameters.Orient = Orientations::Bx; break;
					}
					break;

				case 'R':
					switch (SubPar[1])
					{
					case 'U': Parameters.Orient = Orientations::RU; break;
					case 'D': Parameters.Orient = Orientations::RD; break;
					case 'F': Parameters.Orient = Orientations::RF; break;
					case 'B': Parameters.Orient = Orientations::RB; break;
					case 'L': Parameters.Orient = Orientations::RL; break;
					default: Parameters.Orient = Orientations::Rx; break;
					}
					break;

				case 'L':
					switch (SubPar[1])
					{
					case 'U': Parameters.Orient = Orientations::LU; break;
					case 'D': Parameters.Orient = Orientations::LD; break;
					case 'F': Parameters.Orient = Orientations::LF; break;
					case 'B': Parameters.Orient = Orientations::LB; break;
					default: Parameters.Orient = Orientations::Lx; break;
					}
					break;

				default: Parameters.Orient = Orientations::ALL; break;
				}
				std::cout << "Orientation search defined (parameter '" << Par << "')" << std::endl;
			}
			break;

		default: 
			std::cout << "Unknown parameter '" << Par << "'" << std::endl; 
			break;
		}
	}
}

void ProcessOrientations(std::vector<Lyr>& CrossLayers, const Orientations Orient)
{
	if (Orient == Orientations::ALL ||
		Orient == Orientations::Ux ||
		Orient == Orientations::UD ||
		Orient == Orientations::UF ||
		Orient == Orientations::UR ||
		Orient == Orientations::UB ||
		Orient == Orientations::UL) CrossLayers.push_back(Lyr::D);

	if (Orient == Orientations::ALL ||
		Orient == Orientations::Dx ||
		Orient == Orientations::UD ||
		Orient == Orientations::DF ||
		Orient == Orientations::DR ||
		Orient == Orientations::DB ||
		Orient == Orientations::DL)	CrossLayers.push_back(Lyr::U);

	if (Orient == Orientations::ALL ||
		Orient == Orientations::Fx ||
		Orient == Orientations::FB ||
		Orient == Orientations::FU ||
		Orient == Orientations::FR ||
		Orient == Orientations::FD ||
		Orient == Orientations::FL)	CrossLayers.push_back(Lyr::B);

	if (Orient == Orientations::ALL ||
		Orient == Orientations::Bx ||
		Orient == Orientations::FB ||
		Orient == Orientations::BU ||
		Orient == Orientations::BR ||
		Orient == Orientations::BD ||
		Orient == Orientations::BL)	CrossLayers.push_back(Lyr::F);

	if (Orient == Orientations::ALL ||
		Orient == Orientations::Rx ||
		Orient == Orientations::RL ||
		Orient == Orientations::RU ||
		Orient == Orientations::RF ||
		Orient == Orientations::RD ||
		Orient == Orientations::RB)	CrossLayers.push_back(Lyr::L);

	if (Orient == Orientations::ALL ||
		Orient == Orientations::Lx ||
		Orient == Orientations::RL ||
		Orient == Orientations::LU ||
		Orient == Orientations::LF ||
		Orient == Orientations::LD ||
		Orient == Orientations::LB)	CrossLayers.push_back(Lyr::R);
}

void ProcessOrientations(std::vector<Spn>& SearchSpins, const Orientations Orient)
{
	SearchSpins.clear();

	switch (Orient)
	{
	case Orientations::ALL: for (int s = 0; s < 24; s++) SearchSpins.push_back(static_cast<Spn>(s)); break;

	case Orientations::Ux: SearchSpins.push_back(Spn::UF); SearchSpins.push_back(Spn::UB);
		SearchSpins.push_back(Spn::UR); SearchSpins.push_back(Spn::UL); break;
	case Orientations::Dx: SearchSpins.push_back(Spn::DF); SearchSpins.push_back(Spn::DB);
		SearchSpins.push_back(Spn::DR); SearchSpins.push_back(Spn::DL); break;
	case Orientations::Fx: SearchSpins.push_back(Spn::FU); SearchSpins.push_back(Spn::FD);
		SearchSpins.push_back(Spn::FR); SearchSpins.push_back(Spn::FL); break;
	case Orientations::Bx: SearchSpins.push_back(Spn::BU); SearchSpins.push_back(Spn::BD);
		SearchSpins.push_back(Spn::BR); SearchSpins.push_back(Spn::BL); break;
	case Orientations::Rx: SearchSpins.push_back(Spn::RU); SearchSpins.push_back(Spn::RD);
		SearchSpins.push_back(Spn::RF); SearchSpins.push_back(Spn::RB); break;
	case Orientations::Lx: SearchSpins.push_back(Spn::LU); SearchSpins.push_back(Spn::LD);
		SearchSpins.push_back(Spn::LF); SearchSpins.push_back(Spn::LB); break;

	case Orientations::UD: SearchSpins.push_back(Spn::UF); SearchSpins.push_back(Spn::UB);
		SearchSpins.push_back(Spn::UR); SearchSpins.push_back(Spn::UL);
		SearchSpins.push_back(Spn::DF); SearchSpins.push_back(Spn::DB);
		SearchSpins.push_back(Spn::DR); SearchSpins.push_back(Spn::DL); break;
	case Orientations::FB: SearchSpins.push_back(Spn::FU); SearchSpins.push_back(Spn::FD);
		SearchSpins.push_back(Spn::FR); SearchSpins.push_back(Spn::FL);
		SearchSpins.push_back(Spn::BU); SearchSpins.push_back(Spn::BD);
		SearchSpins.push_back(Spn::BR); SearchSpins.push_back(Spn::BL); break;
	case Orientations::RL: SearchSpins.push_back(Spn::RU); SearchSpins.push_back(Spn::RD);
		SearchSpins.push_back(Spn::RF); SearchSpins.push_back(Spn::RB);
		SearchSpins.push_back(Spn::LU); SearchSpins.push_back(Spn::LD);
		SearchSpins.push_back(Spn::LF); SearchSpins.push_back(Spn::LB); break;

	default: SearchSpins.push_back(static_cast<Spn>(static_cast<int>(Parameters.Orient) - static_cast<int>(Orientations::UF))); break;
	}
}

Lyr ProcessOrientations(const Orientations Orient)
{
	Lyr FirstLayer = Lyr::D;

	if (Parameters.Orient == Orientations::ALL || Parameters.Orient == Orientations::Ux || Parameters.Orient == Orientations::UD ||
		Parameters.Orient == Orientations::UF || Parameters.Orient == Orientations::UR ||
		Parameters.Orient == Orientations::UB || Parameters.Orient == Orientations::UL)	FirstLayer = Lyr::D;
	else if (Parameters.Orient == Orientations::Dx ||
		Parameters.Orient == Orientations::DF || Parameters.Orient == Orientations::DR ||
		Parameters.Orient == Orientations::DB || Parameters.Orient == Orientations::DL)	FirstLayer = Lyr::U;
	else if (Parameters.Orient == Orientations::Fx || Parameters.Orient == Orientations::FB ||
		Parameters.Orient == Orientations::FU || Parameters.Orient == Orientations::FR ||
		Parameters.Orient == Orientations::FD || Parameters.Orient == Orientations::FL)	FirstLayer = Lyr::B;
	else if (Parameters.Orient == Orientations::Bx ||
		Parameters.Orient == Orientations::BU || Parameters.Orient == Orientations::BR ||
		Parameters.Orient == Orientations::BD || Parameters.Orient == Orientations::BL)	FirstLayer = Lyr::F;
	else if (Parameters.Orient == Orientations::Rx || Parameters.Orient == Orientations::RL ||
		Parameters.Orient == Orientations::RU || Parameters.Orient == Orientations::RF ||
		Parameters.Orient == Orientations::RD || Parameters.Orient == Orientations::RB)	FirstLayer = Lyr::L;
	else if (Parameters.Orient == Orientations::Lx ||
		Parameters.Orient == Orientations::LU || Parameters.Orient == Orientations::LF ||
		Parameters.Orient == Orientations::LD || Parameters.Orient == Orientations::LB)	FirstLayer = Lyr::R;

	return FirstLayer;
}

void sLBL(const Algorithm& Scramble, std::ofstream& flog)
{
	LBL SearchLBL(Scramble, Parameters.Threads);
	
	SearchLBL.SetMetric(Parameters.Metric);

	Lyr FirstLayer = ProcessOrientations(Parameters.Orient);

	std::cout << "Searching cross... " << std::flush;
	if (!SearchLBL.SearchFLCross(FirstLayer))
	{
		std::cout << "\n" << "LBL: No cross found!" << std::endl;
		return;
	}

	std::cout << "first layer... " << std::flush;
	SearchLBL.SearchFLCorners();

	std::cout << "second layer... " << std::flush;
	SearchLBL.SearchSLEdges();

	std::cout << "last layer... " << std::flush;
	SearchLBL.SearchLLCross1();
	SearchLBL.SearchLLCross2();
	SearchLBL.SearchLLCorners1();
	SearchLBL.SearchLLCorners2();

	std::string Report = SearchLBL.GetReport();
	if (Parameters.Times)
	{
		Report += "\n";
		Report += SearchLBL.GetTimeReport();
	}
	flog << Report;
	std::cout << "Done!" << "\n\n" << Report << std::endl;
}

void sCFOP(const Algorithm& Scramble, std::ofstream& flog)
{
	CFOP SearchCFOP(Scramble, Parameters.Threads);

	std::vector<Lyr> CrossLayers;
	ProcessOrientations(CrossLayers, Parameters.Orient);
	SearchCFOP.SetCrossLayers(CrossLayers);
	SearchCFOP.SetMetric(Parameters.Metric);

	std::cout << "Searching CFOP crosses... " << std::flush;

	if (!SearchCFOP.SearchCrosses(Parameters.Depth1, Parameters.NumInsp))
	{
		std::cout << "\n" << "No CFOP crosses found!" << std::endl;
		return;
	}

	std::cout << "F2L... " << std::flush;
	SearchCFOP.SearchF2L();

	if (Parameters.Collect == Collections::_1LLL)
	{
		std::cout << "1LLL... " << std::flush;
		SearchCFOP.Search1LLL();
	}
	else if (Parameters.Collect == Collections::ZBLL)
	{
		std::cout << "EO... " << std::flush;
		SearchCFOP.SearchEOLL();
		std::cout << "ZBLL... " << std::flush;
		SearchCFOP.SearchZBLL();
	}
	else // Default
	{
		std::cout << "OLL... " << std::flush;
		SearchCFOP.SearchOLL();
		std::cout << "PLL... " << std::flush;
		SearchCFOP.SearchPLL();
	}

	if (Parameters.Regrip) SearchCFOP.SetRegrips();

	flog << SearchCFOP.GetReport(Parameters.Cancellations, true); // Debug
	if (Parameters.Best) flog << SearchCFOP.GetBestReport(Parameters.Cancellations);
	flog << "\n" << SearchCFOP.GetTimeReport();

	std::cout << "Done!" << "\n\n" << SearchCFOP.GetReport(Parameters.Cancellations, Parameters.Debug) << std::endl;

	if (Parameters.Best) std::cout << "\nBest solve - " << SearchCFOP.GetBestReport(Parameters.Cancellations) << std::endl; 
	
	if (Parameters.Times) std::cout << SearchCFOP.GetTimeReport() << std::endl; 
}

void sRoux(const Algorithm& Scramble, std::ofstream& flog)
{
	Roux SearchRoux(Scramble, Parameters.Threads);

	std::vector<Spn> SearchSpins;

	ProcessOrientations(SearchSpins, Parameters.Orient);

	SearchRoux.SetSearchSpins(SearchSpins);
	
	SearchRoux.SetMetric(Parameters.Metric);

	std::cout << "Searching Roux first blocks... " << std::flush;
	if (!SearchRoux.SearchFirstBlock(Parameters.Depth1, Parameters.NumInsp))
	{
		std::cout << "\n" << "No Roux first blocks found!" << std::endl;
		return;
	}

	std::cout << "second blocks... " << std::flush;
	SearchRoux.SearchSecondBlocksFirstSquare(Parameters.Depth2);
	SearchRoux.SearchSecondBlocksSecondSquare(4u);

	if (Parameters.Collect == Collections::COLL)
	{
		std::cout << "COLL... " << std::flush;
		SearchRoux.SearchCOLL();
	}
	else
	{
		std::cout << "CMLL... " << std::flush;
		SearchRoux.SearchCMLL();
	}

	std::cout << "L6E... " << std::flush;
	if (!Parameters.OneLookL6E)
	{
		SearchRoux.SearchL6EO(10u);
		SearchRoux.SearchL6E2E(12u);
	}
	SearchRoux.SearchL6E(15u);

	if (Parameters.Regrip) SearchRoux.SetRegrips();

	flog << SearchRoux.GetReport(Parameters.Cancellations, true); // Debug
	if (Parameters.Best) flog << SearchRoux.GetBestReport(Parameters.Cancellations);
	flog << "\n" << SearchRoux.GetTimeReport();
	
	std::cout << "Done!" << "\n\n" << SearchRoux.GetReport(Parameters.Cancellations, Parameters.Debug) << std::endl;

	if (Parameters.Best) std::cout << "\nBest solve - " << SearchRoux.GetBestReport(Parameters.Cancellations) << std::endl;
	
	if (Parameters.Times) std::cout << SearchRoux.GetTimeReport() << std::endl; 
}

void sPetrus(const Algorithm& Scramble, std::ofstream& flog)
{
	Petrus SearchPetrus(Scramble, Parameters.Threads);

	std::vector<Spn> SearchSpins;

	ProcessOrientations(SearchSpins, Parameters.Orient);

	SearchPetrus.SetSearchSpins(SearchSpins);
	
	SearchPetrus.SetMetric(Parameters.Metric);

	std::cout << "Searching Petrus blocks... " << std::flush;
	if (!SearchPetrus.SearchBlock(Parameters.Depth1, Parameters.NumInsp))
	{
		std::cout << "\n" << "No Petrus blocks!" << std::endl;
		return;
	}

	std::cout << "expanding blocks... " << std::flush;
	SearchPetrus.SearchExpandedBlock();

	std::cout << "EO... " << std::flush;
	SearchPetrus.SearchEO();

	std::cout << "F2L... " << std::flush;
	SearchPetrus.SearchF2L();

	if (Parameters.Collect == Collections::OCLL || Parameters.Collect == Collections::PLL)
	{
		std::cout << "OCLL... " << std::flush;
		SearchPetrus.SearchOCLL();

		std::cout << "PLL... " << std::flush;
		SearchPetrus.SearchPLL();
	}
	else if (Parameters.Collect == Collections::COLL || Parameters.Collect == Collections::EPLL)
	{
		std::cout << "COLL... " << std::flush;
		SearchPetrus.SearchCOLL();

		std::cout << "EPLL... " << std::flush;
		SearchPetrus.SearchEPLL();
	}
	else
	{
		std::cout << "ZBLL... " << std::flush;
		SearchPetrus.SearchZBLL();
	}

	if (Parameters.Regrip) SearchPetrus.SetRegrips();

	flog << SearchPetrus.GetReport(Parameters.Cancellations, true); // Debug
	if (Parameters.Best) flog << SearchPetrus.GetBestReport(Parameters.Cancellations);
	flog << "\n" << SearchPetrus.GetTimeReport();

	std::cout << "Done!" << "\n\n" << SearchPetrus.GetReport(Parameters.Cancellations, Parameters.Debug) << std::endl;

	if (Parameters.Best) std::cout << "\nBest solve - " << SearchPetrus.GetBestReport(Parameters.Cancellations) << std::endl;
	
	if (Parameters.Times) std::cout << SearchPetrus.GetTimeReport() << std::endl; 
}

void sZZ(const Algorithm& Scramble, std::ofstream& flog)
{
	ZZ SearchZZ(Scramble, Parameters.Threads);
	
	std::vector<Spn> SearchSpins;

	ProcessOrientations(SearchSpins, Parameters.Orient);

	SearchZZ.SetSearchSpins(SearchSpins);
	
	SearchZZ.SetMetric(Parameters.Metric);
	
	std::cout << "Searching EOX... " << std::flush;
	if (!SearchZZ.SearchEOX(Parameters.Depth1, Parameters.NumInsp))
	{
		std::cout << "\n" << "No EO Lines found!" << std::endl;
		return;
	}

	std::cout << "F2L... " << std::flush;
	SearchZZ.SearchF2L();

	if (Parameters.Collect == Collections::OCLL || Parameters.Collect == Collections::PLL)
	{
		std::cout << "OCLL... " << std::flush;
		SearchZZ.SearchOCLL();

		std::cout << "PLL... " << std::flush;
		SearchZZ.SearchPLL();
	}
	else if (Parameters.Collect == Collections::COLL || Parameters.Collect == Collections::EPLL)
	{
		std::cout << "COLL... " << std::flush;
		SearchZZ.SearchCOLL();

		std::cout << "EPLL... " << std::flush;
		SearchZZ.SearchEPLL();
	}
	else
	{
		std::cout << "ZBLL... " << std::flush;
		SearchZZ.SearchZBLL();
	}

	if (Parameters.Regrip) SearchZZ.SetRegrips();
	
	flog << SearchZZ.GetReport(Parameters.Cancellations, true); // Debug
	if (Parameters.Best) flog << SearchZZ.GetBestReport(Parameters.Cancellations);
	flog << "\n" << SearchZZ.GetTimeReport();

	std::cout << "Done!" << "\n\n" << SearchZZ.GetReport(Parameters.Cancellations, Parameters.Debug) << std::endl;

	if (Parameters.Best) std::cout << "\nBest solve - " << SearchZZ.GetBestReport(Parameters.Cancellations) << std::endl;
	
	if (Parameters.Times) std::cout << SearchZZ.GetTimeReport() << std::endl; 
}

std::string GetParametersSummary()
{
	std::ostringstream oss;

	switch (Parameters.Method)
	{
		case Methods::LBL:
			oss << "Parameters summary:\n";
			oss << "\tSelected method: Layer-By-Layer (LBL)\n";
			oss << "\tOrientation (first layer): " << Cube::GetLayerChar(ProcessOrientations(Parameters.Orient)) << "\n";
			oss << "\tMetric: " << Algorithm::GetMetricString(Parameters.Metric) << "\n";
			if (Parameters.Times) oss << "\tSearch times will be shown\n";
			oss << "\tCores: " << (Parameters.Threads == 0 ? "All\n" : std::to_string(Parameters.Threads)) << std::flush;
			break;
		case Methods::CFOP:
			oss << "Parameters summary:\n";
			oss << "\tSelected method: CFOP\n";
			oss << "\tOrientation(s): " << o_strings[static_cast<int>(Parameters.Orient)] << "\n";
			oss << "\tNumber of inspections per orientation: " << Parameters.NumInsp << "\n";
			oss << "\tMetric: " << Algorithm::GetMetricString(Parameters.Metric) << "\n";
			if (Parameters.Collect == Collections::NONE) oss << "\tUsing default algset for CFOP: OLL + PLL\n";
			else if (Parameters.Collect == Collections::OLL || Parameters.Collect == Collections::PLL) oss << "\tLast layer in two looks: OLL + PLL\n";
			else if (Parameters.Collect == Collections::_1LLL) oss << "\tLast layer in one look: 1LLL\n";
			else if (Parameters.Collect == Collections::ZBLL) oss << "\tLast layer in two looks: EO + ZBLL\n";
			else oss << "\tInvalid algset for CFOP, using OLL + PLL instead\n";
			oss << "\tSearch depth: " << Parameters.Depth1 << "\n";
			if (Parameters.Best) oss << "\tBest solve will be shown\n";
			if (Parameters.Regrip) oss << "\tRegrips will be added\n";
			if (Parameters.Cancellations) oss << "\tCancellations will be applied\n";
			if (Parameters.Times) oss << "\tSearch times will be shown\n";
			oss << "\tCores: " << (Parameters.Threads == 0 ? "All\n" : std::to_string(Parameters.Threads)) << std::flush;
			break;
		case Methods::ROUX:
			oss << "Parameters summary:\n";
			oss << "\tSelected method: Roux\n";
			oss << "\tOrientation(s): " << o_strings[static_cast<int>(Parameters.Orient)] << "\n";
			oss << "\tNumber of inspections per orientation: " << Parameters.NumInsp << "\n";
			oss << "\tSelected metric: " << Algorithm::GetMetricString(Parameters.Metric) << "\n";
			if (Parameters.Collect == Collections::NONE) oss << "\tUsing default algset for Roux: CMLL\n";
			else if (Parameters.Collect == Collections::CMLL) oss << "\tLast layer corners will be solved with CMLL algorithms\n";
			else if (Parameters.Collect == Collections::COLL) oss << "\tLast layer corners will be solved with COLL algorithms\n";
			else oss << "\tInvalid algset for Roux corners, using CMLL instead\n";
			if (Parameters.OneLookL6E) oss << "\tLast six edges will be solved in one look\n";
			else oss << "\tLast six edges will be solved in three steps\n";
			oss << "\tFirst block search depth: " << Parameters.Depth1 << "\n";
			oss << "\tSecond block search depth: " << Parameters.Depth2 << "\n";
			if (Parameters.Best) oss << "\tBest solve will be shown\n";
			if (Parameters.Regrip) oss << "\tRegrips will be added\n";
			if (Parameters.Cancellations) oss << "\tCancellations will be applied\n";
			if (Parameters.Times) oss << "\tSearch times will be shown\n";
			oss << "\tCores: " << (Parameters.Threads == 0 ? "All\n" : std::to_string(Parameters.Threads)) << std::flush;
			break;
		case Methods::PETRUS:
			oss << "Parameters summary:\n";
			oss << "\tSelected method: Petrus\n";
			oss << "\tOrientation(s): " << o_strings[static_cast<int>(Parameters.Orient)] << "\n";
			oss << "\tNumber of inspections per orientation: " << Parameters.NumInsp << "\n";
			oss << "\tSelected metric: " << Algorithm::GetMetricString(Parameters.Metric) << "\n";
			if (Parameters.Collect == Collections::NONE) oss << "\tUsing default algset for Petrus: ZBLL\n";
			else if (Parameters.Collect == Collections::COLL || Parameters.Collect == Collections::EPLL) oss << "\tLast layer in two looks: COLL + EPLL\n";
			else if (Parameters.Collect == Collections::OCLL || Parameters.Collect == Collections::PLL) oss << "\tLast layer in two looks: OCLL + PLL\n";
			else if (Parameters.Collect == Collections::ZBLL) oss << "\tLast layer in one look: ZBLL\n";
			else oss << "\tInvalid algset for Petrus, using ZBLL instead\n";
			oss << "\tBlock search depth: " << Parameters.Depth1 << "\n";
			oss << "\tF2L search depth: " << Parameters.Depth2 << "\n";
			if (Parameters.Best) oss << "\tBest solve will be shown\n";
			if (Parameters.Regrip) oss << "\tRegrips will be added\n";
			if (Parameters.Cancellations) oss << "\tCancellations will be applied\n";
			if (Parameters.Times) oss << "\tSearch times will be shown\n";
			oss << "\tCores: " << (Parameters.Threads == 0 ? "All\n" : std::to_string(Parameters.Threads)) << std::flush;
			break;
		case Methods::ZZ:
			oss << "Parameters summary:\n";
			oss << "\tSelected method: ZZ\n";
			oss << "\tOrientation(s): " << o_strings[static_cast<int>(Parameters.Orient)] << "\n";
			oss << "\tNumber of inspections per orientation: " << Parameters.NumInsp << "\n";
			oss << "\tSelected metric: " << Algorithm::GetMetricString(Parameters.Metric) << "\n";
			if (Parameters.Collect == Collections::NONE) oss << "\tUsing default algset for ZZ: ZBLL\n";
			else if (Parameters.Collect == Collections::COLL || Parameters.Collect == Collections::EPLL) oss << "\tLast layer in two looks: COLL + EPLL\n";
			else if (Parameters.Collect == Collections::OCLL || Parameters.Collect == Collections::PLL) oss << "\tLast layer in two looks: OCLL + PLL\n";
			else if (Parameters.Collect == Collections::ZBLL) oss << "\tLast layer in one look: ZBLL\n";
			else oss << "\tInvalid algset for Petrus, using ZBLL instead\n";
			oss << "\tEOX search depth: " << Parameters.Depth1 << "\n";
			if (Parameters.Best) oss << "\tBest solve will be shown\n";
			if (Parameters.Regrip) oss << "\tRegrips will be added\n";
			if (Parameters.Cancellations) oss << "\tCancellations will be applied\n";
			if (Parameters.Times) oss << "\tSearch times will be shown\n";
			oss << "\tCores: " << (Parameters.Threads == 0 ? "All\n" : std::to_string(Parameters.Threads)) << std::flush;
			break;
		default:
			oss << "No selected method, appending " << Parameters.NumSolves << " random scramble(s)";
			oss << " of " << Parameters.LengthScramble << " movements to 'scrambles.txt' file..." << std::flush;
			break;
	}
	return oss.str();
}

std::string GetExternalFilesCheck()
{
	std::ostringstream oss;
	
	oss << "Number of 3-movements cancellations loaded: " << std::to_string(Algorithm::GetCancellation3Size() >> 1) << "\n";
	oss << "Number of 2-movements cancellations loaded: " << std::to_string(Algorithm::GetCancellation2Size() >> 1) << "\n";
	oss << "OLL algorithms loaded: " << std::to_string(Cube::OLL_Algorithms.GetCasesNumber()) << "\n";
	oss << "PLL algorithms loaded: " << std::to_string(Cube::PLL_Algorithms.GetCasesNumber()) << "\n";
	oss << "1LLL algorithms loaded: " << std::to_string(Cube::Algorithms_1LLL.GetCasesNumber()) << "\n";
	oss << "ZBLL algorithms loaded: " << std::to_string(Cube::ZBLL_Algorithms.GetCasesNumber()) << "\n";
	oss << "COLL algorithms loaded: " << std::to_string(Cube::COLL_Algorithms.GetCasesNumber()) << "\n";
	oss << "EPLL algorithms loaded: " << std::to_string(Cube::EPLL_Algorithms.GetCasesNumber()) << "\n";
	oss << "OCLL algorithms loaded: " << std::to_string(Cube::OCLL_Algorithms.GetCasesNumber()) << "\n";
	oss << "CMLL algorithms loaded: " << std::to_string(Cube::CMLL_Algorithms.GetCasesNumber()) << "\n";
	
	return oss.str();
}
