#include "robot-spec.hpp"
#include <iostream>

RobotSpecification::RobotSpecification(){
    this->robotName = "";
    this->assemblySpecification = std::map<part, std::vector<part>>();
    this->partAssemblySprocketCosts = std::vector<cost>();
    this->numberOfParts = 0;
    this->numberOfAssemblyDependencies = 0;
}

RobotSpecification::~RobotSpecification(){
    this->robotName.clear();
    for(auto it = this->assemblySpecification.begin();
        it != this->assemblySpecification.end(); it++){
            it->second.clear();//clear every vector in the map
        }
    this->assemblySpecification.clear();
    this->partAssemblySprocketCosts.clear();
}

cost RobotSpecification::getTotalCostForPart(part whichPart){
    //return 0; //TODO: Implement this function

    switch(this->type){
        case OMNIDROID:
            {
                std::map<part, cost> partCosts;
            }
            break;
        case ROBOTAMATA:
            {

            }
            break;
    }

}

void RobotSpecification::print(){
    switch(this->type){

    case OMNIDROID:
        {
            std::cout << "Robot Name: " <<
                this->getName() << std::endl;

            std::cout << "Number of Parts: " << this->numberOfParts << std::endl;
            std::cout << "Number of Assembly Dependencies: " << this->numberOfAssemblyDependencies << std::endl;
            std::cout << "Assembly Instructions:" << std::endl;

            for(auto it = this->assemblySpecification.begin();
                it != this->assemblySpecification.end(); it++){
                std::cout << "\tPart " << it->first << " requires parts ";
                for(auto itt = it->second.begin(); itt != it->second.end(); itt++){
                    std::cout << *itt << ", ";
                }
                std::cout << std::endl;
            }

            std::cout << "Part Costs:" << std::endl;
            int i = 0;
            for(auto it = this->partAssemblySprocketCosts.begin();
                it != this->partAssemblySprocketCosts.end();
                it++){
                std::cout << "Part " << i << " requires " << *it << " parts." << std::endl;
                i++;
            }
        }
        break;
    case ROBOTAMATON:
        {
            std::cout << "Robot Name: " <<
                this->getName() << std::endl;
            std::cout << "Number of Construction Stages: " <<
                this->getNumberOfConstructionStages() << std::endl;
        }
    }
    return;
}
