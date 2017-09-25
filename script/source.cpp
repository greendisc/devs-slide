#include <iostream>
#include <fstream>
#include <string>
using namespace std;

bool getNextAllocParam(ifstream& allocFile, string& ircName, string&  serverName, string&  numThreads) {
	if (!allocFile.eof()) {
		std::getline(allocFile, ircName, ',');
		std::getline(allocFile, serverName, ',');
		std::getline(allocFile, numThreads);
		if (ircName.size()>0 && serverName.size()>0 && numThreads.size()>0) {
			ircName = ircName.substr(2, ircName.size() - 3);
			serverName = serverName.substr(1, serverName.size() - 2);
			numThreads = numThreads.substr(0, numThreads.size() - 1);
			return true;
		}
	}
	return false;
}

bool getNextJobParam(std::ifstream& paramFilePath, std::string& numCores, std::string& cpuPower, std::string& memPower) {
	if (!paramFilePath.eof()) {
		std::getline(paramFilePath, numCores, ',');
		std::getline(paramFilePath, cpuPower, ',');
		std::getline(paramFilePath, memPower, ',');
		if (numCores.size()>0 && cpuPower.size()>0) {
			numCores = numCores.substr(1, numCores.size() - 1);
			return true;
		}
	}
	return false;
}

int main2() {
	/*
		std::getline(jobsFile, typeAsString, ';');
		std::getline(jobsFile, timeAsString, ';');
		std::getline(jobsFile, idAsString, ';');
		std::getline(jobsFile, ircAsString, ';');
		std::getline(jobsFile, rackAsString, ';');
		std::getline(jobsFile, serverAsString, ';');
		std::getline(jobsFile, numThreadsAsString, ';');
		std::getline(jobsFile, numCoresAsString, ';');
		std::getline(jobsFile, cpuPowerAsString, ';');
		std::getline(jobsFile, memPowerAsString);*/
	ifstream jobsFile("joblogger.txt");
	ofstream newJobsFile("jobsFileMedDC.txt");
	std::string typeAsString;
	std::string timeAsString;
	std::string idAsString;
	do {
		std::getline(jobsFile, typeAsString, ';');
		if (typeAsString != "exit") {
			std::getline(jobsFile, timeAsString, ';');
			std::getline(jobsFile, idAsString, '\n');
			std::cout << timeAsString << std::endl;
			newJobsFile << typeAsString << ';' << timeAsString << ';' << idAsString ;
			ifstream allocFile("alloc/alloc_out_" + idAsString+ ".txt");
			ifstream paramFile("params/param_job_" + idAsString + ".txt");
			std::string ircAsString;
			std::string rackAsString;
			std::string serverAsString;
			std::string numThreadsAsString;
			getNextAllocParam(allocFile, ircAsString, serverAsString, numThreadsAsString);
			string ir = ircAsString.substr(2, ircAsString.size()-1);
			int r = stoi(ir);
			string server = serverAsString.substr(1, serverAsString.size() - 1);
			int s = stoi(server);
			int q;
			if (s % 36 > 18 || s % 36 ==0) q = 0; else q = 1;
			rackAsString = "rack" + to_string((r * 2) - q );
			newJobsFile << ';' << ircAsString << ';' << rackAsString << ';' << serverAsString << ';' << numThreadsAsString << ';';
			std::string numCoresAsString;
			std::string cpuPowerAsString;
			std::string memPowerAsString;
			getNextJobParam(paramFile, numCoresAsString, cpuPowerAsString, memPowerAsString);
			newJobsFile << numCoresAsString << ';' << cpuPowerAsString << ';' << memPowerAsString << std::endl;
			allocFile.close();
			paramFile.close();
		}
	} while (typeAsString != "exit");
	newJobsFile.close();
	jobsFile.close();
	return 0;
}
