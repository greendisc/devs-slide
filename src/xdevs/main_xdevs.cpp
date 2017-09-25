#include "../../lib/xdevs/xdevs/core//simulation/Coordinator.h"

#include "DevsSlide.h"
#include <fstream>
#include <ctype.h>

bool existeArchivo(const std::string fichero);

int main(int argc, char *argv[]) {
	if(argc != 6){
		std::cout << "Argumentos incorrectos. El formato correcto es: ./devsslide OutputFile JobFile WeatherFile ConfigurationFile initialTime." << std::endl;
		return -1;
	}else if(!existeArchivo(argv[2])){
		std::cout << "No existe el archivo de Jobs: " << argv[2] << std::endl;
		return -1;
	}else if(!existeArchivo(argv[3])){
		std::cout << "No existe el archivo de Weather: " << argv[3] << std::endl;
		return -1;
	}else if (!existeArchivo(argv[4])){
		std::cout << "No existe el archivo de configuración: " << argv[4] << std::endl;
		return -1;
	}
	bool check=false;
	for(unsigned int i=0; i < strlen(argv[5]); i++){
		if(!isdigit(argv[5][i]))
			check=true;
	}
	if(check){
		std::cout << argv[5] << " no es un número." << std::endl;
		return -1;
	}else{
		DevsSlide dataCenter(argv[1],argv[2],argv[3], argv[4], (double) atoi(argv[5]));
		Coordinator coordinator(&dataCenter);
		coordinator.initialize();
		coordinator.simulate((long int)100000000);
		coordinator.exit();
		return 0;
	}
} 

bool existeArchivo(const std::string fichero){

	std::ifstream fich(fichero);
	if(fich.is_open()){
		fich.close();
		return true;
	}else{
		return false;
	}
}
