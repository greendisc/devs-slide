/*
 * main_temp.cpp
 *
 *  Created on: 15 de jul. de 2016
 *      Author: jlrisco
 */

#include "../xdevs/DevsJobsOffline.h"

int main() {
	std::string jobsFilePath = "../../../../Borrar/slurm-allocation-data/joblogger-default-slurm.txt";
	std::string newJobsFilePath = "../../../../Borrar/slurm-allocation-data/JobLogger.txt";
	DevsJobsOffline jobs("JOBS", jobsFilePath);
	DevsJobsOffline::JobEntry* currentJobEntry = jobs.getNextEntry();
	// We need several fields:
	// IRC name, from alloc_out_<id>.txt
	// Server name, from alloc_out_<id>.txt
	// Rack name, taken from layout
	// Num threads, from alloc_out_<id>.txt
	// Num cores, from param_job_<id>.txt
	// CPU power, from param_job_<id>.txt
	// Mem power, from param_job_<id>.txt
	std::string ircName, serverName, rackName, numCores, cpuPower, memPower;
	std::ifstream allocFile;
	std::ifstream paramFile;
	while(currentJobEntry!=0) {
		std::string allocFilePath = "../../../../Borrar/slurm-allocation-data/files-default-slurm/alloc_out_" + std::to_string(currentJobEntry->id) + ".txt";
		std::string paramFilePath = "../../../../Borrar/slurm-allocation-data/files-default-slurm/param_job_" + std::to_string(currentJobEntry->id) + ".txt";
		allocFile.open(allocFilePath);
		paramFile.open(paramFilePath);
		allocFile.close();
		paramFile.close();
		delete currentJobEntry;
		currentJobEntry = jobs.getNextEntry();
	}
	jobs.exit();
	return 0;
}
