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
                std::cout << "omni case in robot-spec\n";
                std::map<part, cost> partCosts;

                //initialize each part to have their own part cost in the map
                for (int i = 0; i < numberOfParts; i++)
                {
                    partCosts[i] = partAssemblySprocketCosts.at(i);
                }

                //after intitializing the map, we will iterate through the instructions and update each parts cost
                for (int j = 0; j < numberOfParts; j++)
                {
                    if (assemblySpecification[j].size() != 0)
                    {
                        //this is an intermediate part, so we need to get the cost of some number of previous parts, and add it to this parts cost

                        //for each part this part depends on
                        for (int k = 0; k < assemblySpecification[j].size(); k++)
                        {
                            //add the partCost of that part to the current intermediate part
                            partCosts[j] += partCosts[assemblySpecification[j].at(k)];
                        }
                    }
                }

                return partCosts[numberOfParts-1];
            }
            break;
        case ROBOTOMATON:
            {
                std::cout << "robo case in robot-spec\n";
                std::map<part, cost> partCosts;
                std::vector<std::pair<cost, numPreviousStages>> steps = getRobotomatonSteps();
                int numSteps = this->numberOfStagesInConstruction;
                // std::cout << "steps vector: \n";
                // for(auto it = steps.begin(); it != steps.end(); it++){
                //     std::cout << "cost: " << it->first << " numPrevParts: " << it->second <<std::endl;
                // }

                //for the number of instructions
                for (int i = 0; i < numSteps; i++)
                {
                    //set the cost of each part to its "by itself" cost
                    partCosts[i] = steps.at(i).first;

                    //if this part has dependencies on previous parts
                    if (steps.at(i).second != 0)
                    {

                        //UNSURE IF CORRECT
                        //iterate through the previous j steps and add their
                        int j = i - steps.at(i).second;
                        for (j; j < i; j++)
                        {
                            partCosts[i] += partCosts[j];
                        }
                    }
                }
                std::cout << partCosts[numSteps-1] << "\n";
                return partCosts[numSteps-1];
            }
            break;
    }
    return 0;

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
    case ROBOTOMATON:
        {
            std::cout << "Robot Name: " <<
                this->getName() << std::endl;
            std::cout << "Number of Construction Stages: " <<
                this->getNumberOfConstructionStages() << std::endl;
            std::cout << "Construction Stages:\n ";
            for(auto it = this->robotomatonSpecification.begin(); it != this->robotomatonSpecification.end() ; it++){
                std::cout << "\tCost: " << it->first << " NumPrevStages: " << it->second;
                std::cout << std::endl;
            }
        }
    }
    return;
}
